BOARD_CONF := $(CONFDIR)/board.chcfg
BOARD_FILES := $(BOARDDIR)/board.h $(BOARDDIR)/board.c

$(BOARD_FILES) &: $(BOARD_CONF)
	mkdir -p $(BOARDDIR)
	fmpp --data-root=$(CONFDIR) -S $(CHIBIOS_SOURCE_PATH)/tools/ftl/processors/boards/stm32f4xx/templates		\
		--freemarker-links=lib:$(CHIBIOS_SOURCE_PATH)/tools/ftl/libs -O $(BOARDDIR) -D "doc1:xml(board.chcfg)"	\

board-files: $(BOARD_FILES)