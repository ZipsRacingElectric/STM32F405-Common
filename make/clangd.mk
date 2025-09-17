CLANGD_FILE := $(BUILDDIR)/compile_commands.json

# On Windows, the PWD command needs passed through cygpath to convert into a native Windows path.
ifeq ($(OS),Windows_NT)
	# Windows_NT on XP, 2000, 7, Vista, 10...
	PWD_CMD := $$(cygpath -w $$PWD)
else
	# POSIX, Linux, etc.
	PWD_CMD := $$PWD
endif

# This generates the Clangd compile_commands.json file.
$(CLANGD_FILE): $(CSRC) $(ALLINC)
	rm -f $(BUILDDIR)/compile_commands.json;
	mkdir -p $(BUILDDIR);
	printf "[\n"													>> $(CLANGD_FILE);
	for c in $(CSRC); do \
		printf "\t{\n"												>> $(CLANGD_FILE); \
		printf "\t\t\"directory\": \"%q\",\n" $(PWD_CMD)			>> $(CLANGD_FILE); \
		printf "\t\t\"command\": \"arm-none-eabi-gcc "				>> $(CLANGD_FILE); \
		printf "%s" "--std=c99 $(USE_OPT)"							>> $(CLANGD_FILE); \
		for i in $(ALLINC); do \
			printf "%s" " -I$$i"									>> $(CLANGD_FILE); \
		done; \
		printf "\",\n"												>> $(CLANGD_FILE); \
		printf "\t\t\"file\": \"$$c\"\n"							>> $(CLANGD_FILE); \
		printf "\t},\n"												>> $(CLANGD_FILE); \
	done;
	printf "]\n"													>> $(CLANGD_FILE);

clangd-file: $(CLANGD_FILE)