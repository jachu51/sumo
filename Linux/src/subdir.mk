################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/adc.cpp \
../src/buttons.cpp \
../src/ctrl.cpp \
../src/lcd.cpp \
../src/led.cpp \
../src/main.cpp \
../src/menu.cpp \
../src/motor.cpp \
../src/sys.cpp 

C_SRCS += \
../src/misc.c \
../src/stm32f10x_adc.c \
../src/stm32f10x_dma.c \
../src/stm32f10x_gpio.c \
../src/stm32f10x_rcc.c \
../src/stm32f10x_spi.c \
../src/stm32f10x_tim.c \
../src/stm32f10x_usart.c \
../src/system_stm32f10x.c 

S_UPPER_SRCS += \
../src/startup_stm32f10x_md.S 

OBJS += \
./src/adc.o \
./src/buttons.o \
./src/ctrl.o \
./src/lcd.o \
./src/led.o \
./src/main.o \
./src/menu.o \
./src/misc.o \
./src/motor.o \
./src/startup_stm32f10x_md.o \
./src/stm32f10x_adc.o \
./src/stm32f10x_dma.o \
./src/stm32f10x_gpio.o \
./src/stm32f10x_rcc.o \
./src/stm32f10x_spi.o \
./src/stm32f10x_tim.o \
./src/stm32f10x_usart.o \
./src/sys.o \
./src/system_stm32f10x.o 

C_DEPS += \
./src/misc.d \
./src/stm32f10x_adc.d \
./src/stm32f10x_dma.d \
./src/stm32f10x_gpio.d \
./src/stm32f10x_rcc.d \
./src/stm32f10x_spi.d \
./src/stm32f10x_tim.d \
./src/stm32f10x_usart.d \
./src/system_stm32f10x.d 

S_UPPER_DEPS += \
./src/startup_stm32f10x_md.d 

CPP_DEPS += \
./src/adc.d \
./src/buttons.d \
./src/ctrl.d \
./src/lcd.d \
./src/led.d \
./src/main.d \
./src/menu.d \
./src/motor.d \
./src/sys.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C++ Compiler'
	arm-none-eabi-g++ -I/home/jachu/elektronika/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport -I/home/jachu/elektronika/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc -I"/home/jachu/workspaceSTM/Sumo_STM/src" -O0 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall -std=gnu++0x -Wa,-adhlns="$@.lst" -fno-exceptions -fno-rtti -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C Compiler'
	arm-none-eabi-gcc -I/home/jachu/elektronika/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport -I/home/jachu/elektronika/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc -I"/home/jachu/workspaceSTM/Sumo_STM/src" -O0 -fsingle-precision-constant -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -I/home/jachu/elektronika/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport -I/home/jachu/elektronika/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc -I"/home/jachu/workspaceSTM/Sumo_STM/src" -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


