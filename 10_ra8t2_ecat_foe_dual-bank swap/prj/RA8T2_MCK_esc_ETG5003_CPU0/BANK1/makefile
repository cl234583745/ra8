################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

-include makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/sio/subdir.mk
-include src/r_fw_up_ra/subdir.mk
-include src/ethercat/renesas/subdir.mk
-include src/ethercat/beckhoff/Src/subdir.mk
-include src/subdir.mk
-include ra_gen/subdir.mk
-include ra/fsp/src/rm_ethercat_ssc_port/subdir.mk
-include ra/fsp/src/r_sci_b_uart/subdir.mk
-include ra/fsp/src/r_mram/subdir.mk
-include ra/fsp/src/r_ioport/subdir.mk
-include ra/fsp/src/r_gpt/subdir.mk
-include ra/fsp/src/r_ethercat_phy/targets/ICS1894/subdir.mk
-include ra/fsp/src/r_ethercat_phy/subdir.mk
-include ra/fsp/src/bsp/mcu/ra8t2/subdir.mk
-include ra/fsp/src/bsp/mcu/all/subdir.mk
-include ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/subdir.mk
-include ra/board/ra8t2_mck/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(JMP_DEPS)),)
-include $(JMP_DEPS)
endif
ifneq ($(strip $(FSY_DEPS)),)
-include $(FSY_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
-include $(SRC_UPPER_DEPS)
endif
ifneq ($(strip $(P_UPPER_DEPS)),)
-include $(P_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(PP_UPPER_DEPS)),)
-include $(PP_UPPER_DEPS)
endif
ifneq ($(strip $(P_DEPS)),)
-include $(P_DEPS)
endif
ifneq ($(strip $(FSY_UPPER_DEPS)),)
-include $(FSY_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(JMP_UPPER_DEPS)),)
-include $(JMP_UPPER_DEPS)
endif
ifneq ($(strip $(PP_DEPS)),)
-include $(PP_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
-include $(SX_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
RA8T2_MCK_esc_ETG5003_CPU0.srec \

SECONDARY_SIZE += \
RA8T2_MCK_esc_ETG5003_CPU0.siz \


# All Target
all:
	$(MAKE) --no-print-directory pre-build
	$(MAKE) --no-print-directory main-build
	$(MAKE) --no-print-directory post-build

# Main-build Target
main-build: RA8T2_MCK_esc_ETG5003_CPU0.elf secondary-outputs

# Tool invocations
RA8T2_MCK_esc_ETG5003_CPU0.elf: $(OBJS) $(USER_OBJS) $(LINKER_SCRIPT)
	$(file > RA8T2_MCK_esc_ETG5003_CPU0.elf.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -T "fsp_bank1.ld" -nostartfiles -Xlinker --gc-sections -L "E:\\RS_workspace\\RA8T2_MCK_esc_ETG5003_CPU0/script" -Wl,-Map,"RA8T2_MCK_esc_ETG5003_CPU0.map" --specs=nano.specs --specs=nosys.specs -o "RA8T2_MCK_esc_ETG5003_CPU0.elf" -Wl,--start-group $(OBJS) $(USER_OBJS) $(LIBS) -Wl,--end-group)
	@echo Building target: $@ && arm-none-eabi-gcc @"RA8T2_MCK_esc_ETG5003_CPU0.elf.in"
RA8T2_MCK_esc_ETG5003_CPU0.srec: RA8T2_MCK_esc_ETG5003_CPU0.elf
	arm-none-eabi-objcopy -O srec "RA8T2_MCK_esc_ETG5003_CPU0.elf"  "RA8T2_MCK_esc_ETG5003_CPU0.srec"
RA8T2_MCK_esc_ETG5003_CPU0.siz: RA8T2_MCK_esc_ETG5003_CPU0.elf
	arm-none-eabi-size --format=berkeley "RA8T2_MCK_esc_ETG5003_CPU0.elf"
# Other Targets
clean:
	-$(file > clean_file.tmp,  $(JMP_DEPS)  $(FSY_DEPS)  $(C_UPPER_DEPS)  $(SECONDARY_SIZE)  $(SRC_UPPER_DEPS)  $(P_UPPER_DEPS)  $(S_DEPS)  $(PP_UPPER_DEPS)  $(P_DEPS)  $(FSY_UPPER_DEPS)  $(C_DEPS)  $(SRC_DEPS)  $(JMP_UPPER_DEPS)  $(PP_DEPS)  $(SX_DEPS)  $(ASM_UPPER_DEPS)  $(SX_UPPER_DEPS)  $(OBJS)  $(SECONDARY_FLASH)  $(ASM_DEPS)  $(SREC)  $(S_UPPER_DEPS)  $(MAP) )
	-@ xargs -P 2 -s 32000 -t -a clean_file.tmp $(RM)
	-@$(RM) clean_file.tmp
	-$(RM)  RA8T2_MCK_esc_ETG5003_CPU0.elf
	-@echo ' '

pre-build:
	-cmd /c del ECATFW_RA8T2_B01.efw
	-@echo ' '

post-build:
	-cmd /c copy /Y RA8T2_MCK_esc_ETG5003_CPU0.srec ECATFW_RA8T2_B01.efw
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build pre-build post-build

-include ../makefile.targets
