################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
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

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
ATARI_MOUSE.lss \

FLASH_IMAGE += \
ATARI_MOUSE.hex \

EEPROM_IMAGE += \
ATARI_MOUSE.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: ATARI_MOUSE.elf secondary-outputs

# Tool invocations
ATARI_MOUSE.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,ATARI_MOUSE.map -mmcu=atmega8 -o "ATARI_MOUSE.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

ATARI_MOUSE.lss: ATARI_MOUSE.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S ATARI_MOUSE.elf  >"ATARI_MOUSE.lss"
	@echo 'Finished building: $@'
	@echo ' '

ATARI_MOUSE.hex: ATARI_MOUSE.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex ATARI_MOUSE.elf  "ATARI_MOUSE.hex"
	@echo 'Finished building: $@'
	@echo ' '

ATARI_MOUSE.eep: ATARI_MOUSE.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex ATARI_MOUSE.elf  "ATARI_MOUSE.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: ATARI_MOUSE.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 ATARI_MOUSE.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) ATARI_MOUSE.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
