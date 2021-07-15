##
#### SoC Build Settings Notice
# The following variables need be defined according to your SoC and Board
# BOARD: MUST, set this variable to one of the valid folder basename under $(NUCLEI_SDK_SOC)/Board/
# CORE: MUST, set this variable to one of SUPPORTED_CORES defined in $(NUCLEI_SDK_BUILD)/Makefile.core
# VARIANT: OPTIONAL, set this variable to consecutive characters better less than 8 chars, such as lite, v1, v2.
#          It can be used to define different variant of similar board
# NUCLEI_SDK_SOC_BOARD: MUST, set this variable to $(NUCLEI_SDK_SOC)/Board/$(BOARD), this means the
#                       base folder for selected BOARD
# NUCLEI_SDK_SOC_COMMON: MUST, set this variable to $(NUCLEI_SDK_SOC)/Common, this means the base
#                       folder for selected SOC
# OPENOCD_CFG: MUST, set this variable to openocd configuration file used for the Board or SoC
# LINKER_SCRIPT: MUST, set this variable to linker script file for selected board and download mode
# RISCV_ARCH: MUST, set this variable to the riscv arch you are using, see $(NUCLEI_SDK_BUILD)/Makefile.core
# RISCV_ABI: MUST, set this variable to the riscv abi you are using, see $(NUCLEI_SDK_BUILD)/Makefile.core
# If your SoC used a fixed BOARD, CORE or DOWNLOAD mode, please use override to define a fixed variable value, eg.
# override CORE := n205
# override DOWNLOAD := flashxip
# override BOARD := nuclei_fpga_eval
#### Source Code Management Notice
# you need to declare the following items:
# 1. C/ASM/CPP source code folders which need to be compiled
# 2. Some C/ASM/CPP source code files whihc need to be compiled
# You can use these makefile variables defined in $(NUCLEI_SDK_BUILD)/Makefile.files to achieve this, for details,
# please check https://doc.nucleisys.com/nuclei_sdk/develop/buildsystem.html#build-related-makefile-variables-used-only-in-application-makefile
##

##### Put your SoC build configurations below #####

BOARD ?= nuclei_fpga_eval
CORE ?= n307fd

ifeq ($(BOARD),hbird_eval)
$(warning BOARD hbird_eval is renamed to nuclei_fpga_eval since Nuclei SDK 0.3.1, please use BOARD=nuclei_fpga_eval now)
override BOARD := nuclei_fpga_eval
endif

NUCLEI_SDK_SOC_BOARD := $(NUCLEI_SDK_SOC)/Board/$(BOARD)
NUCLEI_SDK_SOC_COMMON := $(NUCLEI_SDK_SOC)/Common

OPENOCD_CFG ?= $(NUCLEI_SDK_SOC_BOARD)/openocd_demosoc.cfg
LINKER_SCRIPT ?= $(NUCLEI_SDK_SOC_BOARD)/Source/GCC/gcc_demosoc_$(DOWNLOAD).ld

# File existence check for OPENOCD_CFG and LINKER_SCRIPT
ifeq ($(wildcard $(OPENOCD_CFG)),)
$(error The openocd configuration file $(OPENOCD_CFG) for $(SOC) doesn't exist, please check!)
endif
ifeq ($(wildcard $(LINKER_SCRIPT)),)
$(error The link script file $(LINKER_SCRIPT) for $(SOC) doesn't exist, please check!)
endif

# Set RISCV_ARCH and RISCV_ABI
CORE_UPPER := $(call uc, $(CORE))
include $(NUCLEI_SDK_BUILD)/Makefile.core
# you can override SUPPORTED_CORES defined in Makefile.core to limit the COREs used in this SoC
# eg. override SUPPORTED_CORES := n305 n307

CORE_ARCH_ABI := $($(CORE_UPPER)_CORE_ARCH_ABI)
# Check whether CORE is in SUPPORTED_CORES
ifeq ($(filter $(CORE), $(SUPPORTED_CORES)),)
$(error Here we only support these cores: $(SUPPORTED_CORES))
endif
# Check whether CORE_ARCH_ABI is presented for CORE
ifneq ($(words $(CORE_ARCH_ABI)), 2)
$(error There is no coresponding ARCH_ABI setting for CORE $(CORE), please check Makefile.core)
endif

RISCV_ARCH ?= $(word 1, $(CORE_ARCH_ABI))
RISCV_ABI ?= $(word 2, $(CORE_ARCH_ABI))

##### Put your Source code Management configurations below #####

INCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Include

C_SRCDIRS += $(NUCLEI_SDK_SOC_COMMON)/Source $(NUCLEI_SDK_SOC_COMMON)/Source/Drivers \
		$(NUCLEI_SDK_SOC_COMMON)/Source/Stubs

ASM_SRCS += $(NUCLEI_SDK_SOC_COMMON)/Source/GCC/startup_demosoc.S \
		$(NUCLEI_SDK_SOC_COMMON)/Source/GCC/intexc_demosoc.S

# Add extra board related source files and header files
VALID_NUCLEI_SDK_SOC_BOARD := $(wildcard $(NUCLEI_SDK_SOC_BOARD))
ifneq ($(VALID_NUCLEI_SDK_SOC_BOARD),)
INCDIRS += $(VALID_NUCLEI_SDK_SOC_BOARD)/Include
C_SRCDIRS += $(VALID_NUCLEI_SDK_SOC_BOARD)/Source
endif
