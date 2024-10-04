# Common library compilation
ALLINC += $(CURDIR)/common/include
CSRC += common/src/mc24lc32.c

# ChibiOS comilation
include common/make/chibios.mk

# Board file generation
include common/make/board.mk

# Clangd compilation flag generation
include common/make/clangd.mk

# Board programming
include common/make/openocd.mk