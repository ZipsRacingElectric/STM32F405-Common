ifndef AMK_INVERTER_MK
define AMK_INVERTER_MK
1
endef

# Include the module's common dependencies
include common/src/can/can_node.mk

# Add the module's source file to the compilation
CSRC += common/src/can/amk_inverter.c

endif # AMK_INVERTER_MK