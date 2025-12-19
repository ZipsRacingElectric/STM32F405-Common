ifndef DRIVER_INPUT_BOARD_MK
define DRIVER_INPUT_BOARD_MK
1
endef

# Include the module's common dependencies
include common/src/can/can_node.mk

# Add the module's source file to the compilation
CSRC += common/src/can/driver_input_board.c

endif # DRIVER_INPUT_BOARD_MK