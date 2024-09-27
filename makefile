# ChibiOS comilation
include common/chibios.mk

# Board file generation
include common/board.mk

# Clangd compilation flag generation
include common/clangd.mk

# Board programming
include common/openocd.mk