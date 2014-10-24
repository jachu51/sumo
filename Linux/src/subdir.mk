################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/exception.cpp \
../src/interrupt.cpp \
../src/main.cpp \
../src/startup.cpp \
../src/system_call.cpp 

OBJS += \
./src/exception.o \
./src/interrupt.o \
./src/main.o \
./src/startup.o \
./src/system_call.o 

CPP_DEPS += \
./src/exception.d \
./src/interrupt.d \
./src/main.d \
./src/startup.d \
./src/system_call.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -O2  -g -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


