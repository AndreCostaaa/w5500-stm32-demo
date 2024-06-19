################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/ioLibrary_Driver/Internet/DHCP/subdir.mk
-include Drivers/ioLibrary_Driver/Ethernet/W5500/subdir.mk
-include Drivers/ioLibrary_Driver/Ethernet/W5300/subdir.mk
-include Drivers/ioLibrary_Driver/Ethernet/W5200/subdir.mk
-include Drivers/ioLibrary_Driver/Ethernet/W5100S/subdir.mk
-include Drivers/ioLibrary_Driver/Ethernet/W5100/subdir.mk
-include Drivers/ioLibrary_Driver/Ethernet/subdir.mk
-include Drivers/ioLibrary_Driver/Application/multicast/subdir.mk
-include Drivers/ioLibrary_Driver/Application/loopback/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := w5500-demo
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
w5500-demo.elf \

MAP_FILES += \
w5500-demo.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
w5500-demo.list \


# All Target
all: main-build

# Main-build Target
main-build: w5500-demo.elf secondary-outputs

# Tool invocations
w5500-demo.elf w5500-demo.map: $(OBJS) $(USER_OBJS) /home/andre/dev/viacam/calec/w5500-demo/STM32L476RGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "w5500-demo.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/home/andre/dev/viacam/calec/w5500-demo/STM32L476RGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="w5500-demo.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

w5500-demo.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "w5500-demo.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout w5500-demo.elf w5500-demo.list w5500-demo.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
