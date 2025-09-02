ifndef DEBUG_MK
define DEBUG_MK
1
endef

# ChibiOS extra includes
include $(CHIBIOS)/os/hal/lib/streams/streams.mk
UDEFS += -DCHPRINTF_USE_FLOAT=TRUE

# Add the module's source file to the compilation
CSRC += common/src/debug.c

endif # DEBUG_MK