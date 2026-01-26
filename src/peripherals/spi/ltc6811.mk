ifndef LTC6811_MK
define LTC6811_MK
1
endef

# Include the module's dependencies
include common/src/peripherals/spi/ltc681x.mk

# Add the module's source file to the compilation
CSRC += common/src/peripherals/spi/ltc6811.c

endif # LTC6811_MK