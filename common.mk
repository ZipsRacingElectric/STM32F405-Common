# Common library includes
ALLINC += common/src/

# ChibiOS comilation
include common/make/chibios.mk

# Board file generation
include common/make/board.mk

# Clangd compilation flag generation
include common/make/clangd.mk