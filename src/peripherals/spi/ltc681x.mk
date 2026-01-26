ifndef LTC681X_MK
define LTC681X_MK
1
endef

# Add the module's source file to the compilation
CSRC += common/src/peripherals/spi/ltc681x.c
CSRC += common/src/peripherals/spi/ltc681x_internal.c

endif # LTC681X_MK