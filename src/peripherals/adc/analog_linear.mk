ifndef ANALOG_LINEAR_MK
define ANALOG_LINEAR_MK
1
endef

# Include the module's common dependencies
include common/src/controls/lerp.mk

# Add the module's source file to the compilation
CSRC += common/src/peripherals/adc/analog_linear.c

endif # ANALOG_LINEAR_MK