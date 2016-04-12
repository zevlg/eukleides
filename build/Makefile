# Eukleides binaries makefile
# Copyright (c) Christian Obrecht 2004-2010

include ../Config

BUILD_DIR = $(addsuffix _build,$(BINARY))
DEPEND = $(BUILD_DIR)/depend
COMMON_DIR = ../src/common
MAIN_DIR = ../src/$(BINARY)
LEX = flex
LFLAGS = -8
YACC = bison
YFLAGS = -d
CC = gcc
IFLAGS = -I$(COMMON_DIR) -I$(MAIN_DIR) -I$(BUILD_DIR) 
ifneq ($(strip $(LOCALES)),)
MOFLAGS = -DMO_DIR=\"$(MO_DIR)\" 
endif
CFLAGS = -Wall -D__$(BINARY)__ $(IFLAGS) $(MOFLAGS)
LDFLAGS = -lncurses -lreadline -lm
VPATH = $(COMMON_DIR):$(MAIN_DIR):$(BUILD_DIR)

$(BINARY):

$(BUILD_DIR)/parser.tab.h $(BUILD_DIR)/parser.tab.c: $(COMMON_DIR)/parser.y
	@echo "    Building parser"
	@$(YACC) $(YFLAGS) $< -o $@

$(BUILD_DIR)/lex.yy.c: $(COMMON_DIR)/lexer.l $(BUILD_DIR)/parser.tab.h
	@echo "    Building lexer"
	@$(LEX) $(LFLAGS) -o$@ $<

SOURCES := $(wildcard $(COMMON_DIR)/*.c) \
           $(wildcard $(MAIN_DIR)/*.c) \
           $(BUILD_DIR)/parser.tab.c \
           $(BUILD_DIR)/lex.yy.c

OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.c=.o)))
          
$(DEPEND): $(SOURCES)
	@echo "    Making dependencies"
	@$(CC) -MM $(IFLAGS) $^ > $@

$(BUILD_DIR)/%.o: %.c
	@echo "    Building $(notdir $@)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BINARY): $(OBJECTS)
	@echo "    Linking $(notdir $@)"
	@$(CC) $(LDFLAGS) $^ -o $@

ifneq ($(BINARY),)
-include $(DEPEND)
endif

install: $(BINARY)
	@echo "Installing $<"
	@install -s $< $(BIN_DIR)

uninstall:
	@$(RM) $(addprefix $(BIN_DIR)/,$(BINARIES))

clean:
	@$(RM) $(BINARIES) $(addsuffix _build/*,$(BINARIES))

