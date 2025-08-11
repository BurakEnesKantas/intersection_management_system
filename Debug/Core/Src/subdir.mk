################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/commands.c \
../Core/Src/gpio_config.c \
../Core/Src/main.c \
../Core/Src/modes.c \
../Core/Src/rtc.c \
../Core/Src/stm32g0xx_hal_msp.c \
../Core/Src/stm32g0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g0xx.c \
../Core/Src/timer_config.c \
../Core/Src/traffic_lights.c \
../Core/Src/uart_config.c 

OBJS += \
./Core/Src/commands.o \
./Core/Src/gpio_config.o \
./Core/Src/main.o \
./Core/Src/modes.o \
./Core/Src/rtc.o \
./Core/Src/stm32g0xx_hal_msp.o \
./Core/Src/stm32g0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g0xx.o \
./Core/Src/timer_config.o \
./Core/Src/traffic_lights.o \
./Core/Src/uart_config.o 

C_DEPS += \
./Core/Src/commands.d \
./Core/Src/gpio_config.d \
./Core/Src/main.d \
./Core/Src/modes.d \
./Core/Src/rtc.d \
./Core/Src/stm32g0xx_hal_msp.d \
./Core/Src/stm32g0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g0xx.d \
./Core/Src/timer_config.d \
./Core/Src/traffic_lights.d \
./Core/Src/uart_config.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G070xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/commands.cyclo ./Core/Src/commands.d ./Core/Src/commands.o ./Core/Src/commands.su ./Core/Src/gpio_config.cyclo ./Core/Src/gpio_config.d ./Core/Src/gpio_config.o ./Core/Src/gpio_config.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/modes.cyclo ./Core/Src/modes.d ./Core/Src/modes.o ./Core/Src/modes.su ./Core/Src/rtc.cyclo ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/rtc.su ./Core/Src/stm32g0xx_hal_msp.cyclo ./Core/Src/stm32g0xx_hal_msp.d ./Core/Src/stm32g0xx_hal_msp.o ./Core/Src/stm32g0xx_hal_msp.su ./Core/Src/stm32g0xx_it.cyclo ./Core/Src/stm32g0xx_it.d ./Core/Src/stm32g0xx_it.o ./Core/Src/stm32g0xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32g0xx.cyclo ./Core/Src/system_stm32g0xx.d ./Core/Src/system_stm32g0xx.o ./Core/Src/system_stm32g0xx.su ./Core/Src/timer_config.cyclo ./Core/Src/timer_config.d ./Core/Src/timer_config.o ./Core/Src/timer_config.su ./Core/Src/traffic_lights.cyclo ./Core/Src/traffic_lights.d ./Core/Src/traffic_lights.o ./Core/Src/traffic_lights.su ./Core/Src/uart_config.cyclo ./Core/Src/uart_config.d ./Core/Src/uart_config.o ./Core/Src/uart_config.su

.PHONY: clean-Core-2f-Src

