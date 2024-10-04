# TODO(Barach): This shouldn't require absolute path (CURDIR).

# Common library includes
ALLINC += $(CURDIR)/common/include

# ChibiOS comilation
include common/make/chibios.mk

# Board file generation
include common/make/board.mk

# Clangd compilation flag generation
include common/make/clangd.mk

# Board programming
include common/make/openocd.mk