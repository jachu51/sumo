################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include bin/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Sumo_STM.hex \

SECONDARY_LIST += \
Sumo_STM.lst \

SECONDARY_SIZE += \
Sumo_STM.siz \


# All Target
all: Sumo_STM.elf secondary-outputs

# Tool invocations
Sumo_STM.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Sourcery Linux GCC C++ Linker'
	arm-none-eabi-g++ -T"/home/jachu/workspaceSTM/Sumo_STM/linker/stm32f10x_flash.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,Sumo_STM.map -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -g3 -gdwarf-2 -o "Sumo_STM.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

Sumo_STM.hex: Sumo_STM.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex Sumo_STM.elf  "Sumo_STM.hex"
	@echo 'Finished building: $@'
	@echo ' '

Sumo_STM.lst: Sumo_STM.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Create Listing'
	arm-none-eabi-objdump -h -S Sumo_STM.elf > "Sumo_STM.lst"
	@echo 'Finished building: $@'
	@echo ' '

Sumo_STM.siz: Sumo_STM.elf
	@echo 'Invoking: ARM Sourcery Linux GNU Print Size'
	arm-none-eabi-size  --format=berkeley Sumo_STM.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(SECONDARY_FLASH)$(CXX_DEPS)$(S_UPPER_DEPS)$(SECONDARY_LIST)$(C++_DEPS)$(SECONDARY_SIZE)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS) Sumo_STM.elf
	-@echo ' '

post-build:
	-@echo 'Copy the generated ELF file to the "bin" folder.'
	-cp Sumo_STM.elf "/home/jachu/workspaceSTM/Sumo_STM/bin/"
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
