################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../shell/drv_led.c \
../shell/drv_uart2.c \
../shell/shell.c 

OBJS += \
./shell/drv_led.o \
./shell/drv_uart2.o \
./shell/shell.o 

C_DEPS += \
./shell/drv_led.d \
./shell/drv_uart2.d \
./shell/shell.d 


# Each subdirectory must supply rules for building sources it contributes
shell/%.o shell/%.su shell/%.cyclo: ../shell/%.c shell/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/jeanp/Documents/ENSEA/3A/Systemes_electroniques_dans_l_automobile/2425_ESE_TP_autoradio_THOMAR_BOULOT/STM/shell" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-shell

clean-shell:
	-$(RM) ./shell/drv_led.cyclo ./shell/drv_led.d ./shell/drv_led.o ./shell/drv_led.su ./shell/drv_uart2.cyclo ./shell/drv_uart2.d ./shell/drv_uart2.o ./shell/drv_uart2.su ./shell/shell.cyclo ./shell/shell.d ./shell/shell.o ./shell/shell.su

.PHONY: clean-shell

