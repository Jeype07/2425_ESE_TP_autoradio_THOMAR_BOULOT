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

