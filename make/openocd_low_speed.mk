# Copy the low-speed configuration into the build directory, where the debugger expects it to be.
# - The reason for doing this is to make the debugger unaware of whether we are using the standard or the low-speed
#   configuration.
$(BUILDDIR)/stm32f405.cfg: $(COMMONDIR)/stm32f405_low_speed.cfg
	mkdir -p $(BUILDDIR)
	cp $< $@

# Target for programming the board. 'Building' this target executes OpenOCD with the selected configuration.
flash: build/$(PROJECT).elf $(BUILDDIR)/stm32f405.cfg
	openocd -f interface/stlink.cfg -f $(BUILDDIR)/stm32f405.cfg -c "program build/$(PROJECT).elf verify reset exit"