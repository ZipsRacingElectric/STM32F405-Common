ifndef CAN_THREAD_MK
define CAN_THREAD_MK
1
endef

# Include the module's common dependencies
include common/src/can/can_node.mk

# Add the module's source file to the compilation
CSRC += common/src/can/can_thread.c

endif # CAN_THREAD_MK