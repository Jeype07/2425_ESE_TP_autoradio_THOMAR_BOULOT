# 2425_ESE_TP_autoradio_THOMAR_BOULOT

Ce TP est réalisé par Jean-Philippe THOMAR et Paul BOULOT sous la supervision de N. Papazoglou et L. Fiack, dans le contexte d'un cours sur les systèmes électroniques dans l'automobile.
Il est question ici de prendre en main et d'utiliser le CODEC Audio SGTL5000 dans le but de produire des effets audios.

## 1 Démarrage

## 2 Le GPIO Expander et le VU-Metre
### 2.1 Configuration
1. La référence du GPIO Expander est MCP23S17-E/SO.
2. On utilise le SPI 3.
3. Sur STM32CubeIDE, il faut configuer les paramètres suivants :  
#### Configurer les signaux suivants dans l'ioc
- VU_nRESET : PA0 (GPIO_Output)
- VU_SCK : PC10 (SPI3_SCK)
- VU_nCS : PB7 (GPIO_Output)
- VU_MOSI : PB5 (SPI3_MOSI)
- VU_MISO : PC11 (SPI3_MISO)

### 2.2 Tests

Driver pour les leds

Fonction d'initialisation du GPIO expander
```C
void MCP23S17_Init( void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); //reset High
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);	//CS High

	// Configurer tous les GPIO comme sorties
	MCP23S17_Write(IODIRA, 0x00);
	MCP23S17_Write(IODIRB, 0x00);

	// Eteindre toutes les led
	MCP23S17_Write(MCPGPIOA, 0xff);
	MCP23S17_Write(MCPGPIOB, 0xff);
}
```

Fonction d'écriture de registre du GPIO expander
```C
void MCP23S17_Write( uint8_t reg, uint8_t value) {
	uint8_t data[3] = {MCP23S17_ADDR_WRITE, reg, value};
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); // CS Low
	HAL_SPI_Transmit(&hspi3, data, 3, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);   // CS High
}
```

Code pour changer l'état d'une led en particulier
```C
int ledToggle(h_shell_t * h_shell, int argc, char ** argv)
{

	if (argc == 3)
	{

		int a = atoi(argv[1]);
		int b = atoi(argv[2]);
		if(a>=0 && a<=7){
			if(b==0){

				h_shell->drv.led(h_shell->led_num = a,MCPGPIOA);
			}
			if(b==1){

				h_shell->drv.led(h_shell->led_num = a,MCPGPIOB);
			}
			else{
				int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "Erreur, verifier la led ou le gpio\r\n");
				h_shell->drv.transmit(h_shell->print_buffer, size);
				return -2;
			}
		}
		return 0;
	}
	else
	{
		int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "Erreur, pas le bon nombre d'arguments\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return -1;
	}
}
```

Code pour avoir une tâche qui gère le chenillard.
```C
void task_chenillard(void *params) {
	h_shell_t *h_shell = (h_shell_t *)params;
	uint8_t current_led_a = 0;
	uint8_t current_led_b = 4;
	while (1) {

		h_shell->drv.led(h_shell->led_num = current_led_a,MCPGPIOA);
		h_shell->drv.led(h_shell->led_num = current_led_b,MCPGPIOB);
		current_led_a = (current_led_a + 1) % NUM_LEDS; // Passer à la LED suivante
		current_led_b = (current_led_b + 1) % NUM_LEDS;
		osDelay(200); // Délai entre deux LEDs (200 ms)
	}
}
```

Création de la tâche chenillard
```C
int startchenillard(h_shell_t * h_shell, int argc, char ** argv){

	chenillard_running = 1; // Activer le chenillard
	if (ChenillardTaskHandle == NULL) {
		// Créer la tâche si elle n'existe pas
		xTaskCreate(task_chenillard, "ChenillardTask", 128,(void *)h_shell, 2, &ChenillardTaskHandle);
	}
	else{
		chenillard_running = 0;
		if (ChenillardTaskHandle != NULL) {
			vTaskDelete(ChenillardTaskHandle); // Supprimer la tâche
			ChenillardTaskHandle = NULL;
		}
	}
	return 0;
}
```
On a mis une priorité de 2 au chenillard pour qu'il ne soit pas bloqué par le shell.
Le shell reprend son execution à la réception d'un caractère sur l'uart car il s'agit d'une interruption qui lui donne un sémaphore.

## Le CODEC Audio SGTL5000

### 3.1 Configuration préalables

1. Les pin utilisé pour l'I2C sont PB10 et PB11 et cela correspond à l'I2C 2

Fonction pour récupérer l'ID du CODEC
```C
int sglt5000_get_CHIP_ID_val(h_shell_t * h_shell, int argc, char ** argv){
	uint8_t pData[2];
	if (HAL_I2C_Mem_Read(&hi2c2, SGTL5000_ADDR, CHIP_ID,CHIP_ID_REG_SIZE, pData, sizeof(pData), HAL_MAX_DELAY)== HAL_OK){
		int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "CHIP_ID value : %d\r\n", pData[1]);
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return 0;
	}
	else{
		int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "Error\r\n");
				h_shell->drv.transmit(h_shell->print_buffer, size);

		return -1;
	}
}
```
