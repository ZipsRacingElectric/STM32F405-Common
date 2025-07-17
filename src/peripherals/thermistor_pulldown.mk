ifndef THERMISTOR_PULLDOWN_MK
define THERMISTOR_PULLDOWN_MK
1
endef

# Include the module's common dependencies
include common/src/controls/steinhart_hart.mk

# Add the module's source file to the compilation
CSRC += common/src/peripherals/thermistor_pulldown.c

endif # THERMISTOR_PULLDOWN_MK