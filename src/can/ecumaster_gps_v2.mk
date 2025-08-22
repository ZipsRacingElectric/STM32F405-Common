ifndef ECUMASTER_GPS_V2_MK
define ECUMASTER_GPS_V2_MK
1
endef

# Include the module's common dependencies
include common/src/can/can_node.mk

# Add the module's source file to the compilation
CSRC += common/src/can/ecumaster_gps_v2.c

endif # ECUMASTER_GPS_V2_MK