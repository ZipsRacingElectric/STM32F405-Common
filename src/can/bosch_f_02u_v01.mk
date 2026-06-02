ifndef BOSCH_F_02U_V01_MK
define BOSCH_F_02U_V01_MK
1
endef

# Include the module's common dependencies
include common/src/can/can_node.mk

# Add the module's source file to the compilation
CSRC += common/src/can/bosch_f_02u_v01.c

endif # BOSCH_F_02U_V01_MK