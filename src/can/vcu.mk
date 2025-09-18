ifndef VCU_MK
define VCU_MK
1
endef

# Include the module's common dependencies
include common/src/can/can_node.mk

# Add the module's source file to the compilation
CSRC += common/src/can/vcu.c

endif # VCU_MK