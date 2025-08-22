ifndef AM4096_MK
define AM4096_MK
1
endef

# Include the module's common dependencies
include common/src/peripherals/interface/eeprom.mk

# Add the module's source file to the compilation
CSRC += common/src/peripherals/i2c/am4096.c

endif # AM4096_MK