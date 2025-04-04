ifndef PID_CONTROLLER_MK
define PID_CONTROLLER_MK
1
endef

# Add the module's source file to the compilation
CSRC += common/src/controls/pid_controller.c

endif # PID_CONTROLLER_MK