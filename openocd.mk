flash: build/$(PROJECT).elf
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program build/$(PROJECT).elf verify reset exit"