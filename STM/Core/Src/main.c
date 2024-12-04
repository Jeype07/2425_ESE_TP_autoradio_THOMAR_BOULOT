/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "i2c.h"
#include "sai.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "drv_uart2.h"
#include "drv_led.h"
#include "sgtl5000.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TASK_SHELL_STACK_DEPTH 512
#define TASK_SHELL_PRIORITY 1



#define NUM_LEDS 8 // Nombre de LEDs contrôlées
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
TaskHandle_t h_task_shell = NULL;

TaskHandle_t ChenillardTaskHandle = NULL;
volatile uint8_t chenillard_running = 0; // Flag pour le contrôle du chenillard

h_shell_t h_shell;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

	return ch;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		shell_uart_receive_irq_cb();	// C'est la fonction qui donne le sémaphore!
	}
}

int fonction(h_shell_t * h_shell, int argc, char ** argv)
{
	int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "Je suis une fonction bidon\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);

	return 0;
}

int addition(h_shell_t * h_shell, int argc, char ** argv)
{
	if (argc == 3)
	{
		int a, b;
		a = atoi(argv[1]);
		b = atoi(argv[2]);
		int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "%d + %d = %d\r\n", a, b, a+b);
		h_shell->drv.transmit(h_shell->print_buffer, size);

		return 0;
	}
	else
	{
		int size = snprintf (h_shell->print_buffer, BUFFER_SIZE, "Erreur, pas le bon nombre d'arguments\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return -1;
	}
}

int ledToggle(h_shell_t * h_shell, int argc, char ** argv)
{
	if (argc == 3)
	{

		int a = atoi(argv[1]);
		int b = atoi(argv[2]);
		if((a>=0 && a<=7)!=0){
			if(b==0){

				h_shell->drv.led(h_shell->led_num = a,0x12);
			}
			if(b==1){

				h_shell->drv.led(h_shell->led_num = a,0x13);
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

void task_chenillard(void *params) {
	h_shell_t *h_shell = (h_shell_t *)params;
	uint8_t current_led_a = 0;
	//uint8_t current_led_b = 4;
	printf("0\r\n");
	while (1) {
		h_shell->drv.led(h_shell->led_num = current_led_a,0x12);

		//h_shell->drv.led(h_shell->led_num = current_led_b,0x13);
		current_led_a = (current_led_a + 1) % NUM_LEDS; // Passer à la LED suivante
		//current_led_b = (current_led_b + 1) % NUM_LEDS;
		osDelay(500); // Délai entre deux LEDs (200 ms)

	}
}



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
			MCP23S17_Write(MCPGPIOA, 0xff);
			MCP23S17_Write(MCPGPIOB, 0xff);
		}
	}


	return 0;
}


void task_shell(void * unused)
{
	shell_init(&h_shell);
	shell_add(&h_shell, 'f', fonction, "Une fonction inutile");
	shell_add(&h_shell, 'a', addition, "Effectue une somme");
	shell_add(&h_shell, 'b', ledToggle, "Allumer une led");
	shell_add(&h_shell, 'c',startchenillard, "Lancer chenillard/Arreter chenillard");
	shell_run(&h_shell);	// boucle infinie
}


/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* Configure the peripherals common clocks */
	PeriphCommonClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_SPI3_Init();
	MX_I2C2_Init();
	MX_SAI2_Init();
	/* USER CODE BEGIN 2 */

	__HAL_SAI_ENABLE(&hsai_BlockA2);
	sglt5000_get_CHIP_ID_val();

	h_shell.drv.receive = drv_uart2_receive;
	h_shell.drv.transmit = drv_uart2_transmit;
	h_shell.drv.init = MCP23S17_Init;
	h_shell.drv.led = drv_led;


	if (xTaskCreate(task_shell, "Shell", TASK_SHELL_STACK_DEPTH, NULL, TASK_SHELL_PRIORITY, &h_task_shell) != pdPASS)
	{
		printf("Error creating task shell\r\n");
		Error_Handler();
	}

	vTaskStartScheduler();
	/* USER CODE END 2 */

	/* Call init function for freertos objects (in cmsis_os2.c) */
	MX_FREERTOS_Init();

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Initializes the peripherals clock
	 */
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
	PeriphClkInit.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI1;
	PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
	PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
	PeriphClkInit.PLLSAI1.PLLSAI1N = 13;
	PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV17;
	PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
