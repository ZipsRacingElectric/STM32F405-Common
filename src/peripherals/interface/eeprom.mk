ifndef EEPROM_MK
define EEPROM_MK
1
endef

# Add the module's source file to the compilation
CSRC += common/src/peripherals/interface/eeprom.c

endif # EEPROM_MK