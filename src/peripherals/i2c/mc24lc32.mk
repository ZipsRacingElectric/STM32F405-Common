ifndef MC24LC32_MK
define MC24LC32_MK
1
endef

# Include the module's common dependencies
include common/src/peripherals/interface/eeprom.mk

# Add the module's source file to the compilation
CSRC += common/src/peripherals/i2c/mc24lc32.c

endif # MC24LC32_MK