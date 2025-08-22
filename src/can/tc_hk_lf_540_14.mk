ifndef TC_HK_LF_540_14_MK
define TC_HK_LF_540_14_MK
1
endef

# Include the module's common dependencies
include common/src/can/can_node.mk

# Add the module's source file to the compilation
CSRC += common/src/can/tc_hk_lf_540_14.c

endif # TC_HK_LF_540_14_MK