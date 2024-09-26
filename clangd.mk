CLANGD_FILE := $(BUILDDIR)/compile_commands.json

# This generates the Clangd compile_commands.json file.
$(CLANGD_FILE): $(ALLCSRC) $(ALLINC)
	rm -f $(BUILDDIR)/compile_commands.json;
	mkdir -p $(BUILDDIR);
	printf "[\n"													>> $(CLANGD_FILE);
	for c in $(ALLCSRC); do \
		printf "\t{\n"												>> $(CLANGD_FILE); \
		printf "\t\t\"directory\": \"$(CURDIR)/$(BUILDDIR)\",\n"	>> $(CLANGD_FILE); \
		printf "\t\t\"command\": \"arm-none-eabi-gcc"				>> $(CLANGD_FILE); \
		for i in $(ALLINC); do \
			printf "%s" " -I$$i"									>> $(CLANGD_FILE); \
		done; \
		printf "\",\n"												>> $(CLANGD_FILE); \
		printf "\t\t\"file\": \"$$c\"\n"							>> $(CLANGD_FILE); \
		printf "\t},\n"												>> $(CLANGD_FILE); \
	done;
	printf "]\n"													>> $(CLANGD_FILE);

clangd-file: $(CLANGD_FILE)