################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

-include makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/coremark/subdir.mk
-include src/subdir.mk
-include ra_gen/subdir.mk
-include ra/fsp/src/r_sci_b_uart/subdir.mk
-include ra/fsp/src/r_ioport/subdir.mk
-include ra/fsp/src/bsp/mcu/ra8t2/subdir.mk
-include ra/fsp/src/bsp/mcu/all/subdir.mk
-include ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/subdir.mk
-include ra/board/ra8t2_mck/subdir.mk
-include ra/arm/CMSIS_6/subdir.mk
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
$(C_DEPS):$(SMS_ASM)
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
OBJCOPY += \
ra8t2_coremark_llvm.srec \

SECONDARY_SIZE += \
ra8t2_coremark_llvm.siz \


# All Target
all: ra8t2_coremark_llvm.elf secondary-outputs

# Tool invocations
ra8t2_coremark_llvm.elf: $(OBJS) $(USER_OBJS) $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS) $(LINKER_SCRIPT)
	$(file > ra8t2_coremark_llvm.elf.in,-mcpu=cortex-m85 -mthumb -mlittle-endian -mfloat-abi=hard -Ofast -ffunction-sections -fdata-sections -fno-strict-aliasing -fmessage-length=0 -funsigned-char -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Waggregate-return -Wno-parentheses-equality -Wfloat-equal -g3 -o "ra8t2_coremark_llvm.elf" $(OBJS) $(USER_OBJS) -T "fsp.lld" -L"E:\\RS_workspace\\ra8t2_coremark_llvm/script" -L"E:\\RS_workspace\\ra8t2_coremark_llvm\\Debug" -Wl,-Map=ra8t2_coremark_llvm.map -Wl,--start-group -lcrt0 -lc++abi  -Wl,--end-group -Wl,--gc-sections -Wl,--cref -Wl,-z,norelro -Wl,--icf=none -I"E:\\RS_workspace\\ra8t2_coremark_llvm\\ra_gen" -I"." -I"E:\\RS_workspace\\ra8t2_coremark_llvm\\ra_cfg\\fsp_cfg\\bsp" -I"E:\\RS_workspace\\ra8t2_coremark_llvm\\ra_cfg\\fsp_cfg" -I"E:\\RS_workspace\\ra8t2_coremark_llvm\\src" -I"E:\\RS_workspace\\ra8t2_coremark_llvm\\ra\\fsp\\inc" -I"E:\\RS_workspace\\ra8t2_coremark_llvm\\ra\\fsp\\inc\\api" -I"E:\\RS_workspace\\ra8t2_coremark_llvm\\ra\\fsp\\inc\\instances" -I"E:\\RS_workspace\\ra8t2_coremark_llvm\\ra\\arm\\CMSIS_6\\CMSIS\\Core\\Include")
	@echo Building target: $@ && clang  --target=arm-none-eabi @"ra8t2_coremark_llvm.elf.in"> ra8t2_coremark_llvm.cref
ra8t2_coremark_llvm.srec: ra8t2_coremark_llvm.elf
	llvm-objcopy "ra8t2_coremark_llvm.elf" -O srec  "ra8t2_coremark_llvm.srec"
ra8t2_coremark_llvm.siz: ra8t2_coremark_llvm.elf
	llvm-size --format=berkeley "ra8t2_coremark_llvm.elf"
# Other Targets
clean:
	-$(file > clean_file.tmp,  $(JMP_DEPS)  $(FSY_DEPS)  $(C_UPPER_DEPS)  $(SECONDARY_SIZE)  $(SRC_UPPER_DEPS)  $(P_UPPER_DEPS)  $(S_DEPS)  $(PP_UPPER_DEPS)  $(P_DEPS)  $(FSY_UPPER_DEPS)  $(C_DEPS)  $(CREF)  $(SRC_DEPS)  $(OBJCOPY)  $(JMP_UPPER_DEPS)  $(PP_DEPS)  $(SX_DEPS)  $(ASM_UPPER_DEPS)  $(OBJS)  $(SX_UPPER_DEPS)  $(ASM_DEPS)  $(MAP)  $(S_UPPER_DEPS) )
	-@ xargs -P 2 -s 32000 -t -a clean_file.tmp $(RM)
	-@$(RM) clean_file.tmp
	-$(RM)  ra8t2_coremark_llvm.elf
	-@echo ' '

secondary-outputs: $(OBJCOPY) $(SECONDARY_SIZE)

.PHONY: all clean dependents 

-include ../makefile.targets
