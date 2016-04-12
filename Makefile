# Eukleides main makefile
# Copyright (c) Christian Obrecht 2004-2010

include Config

MAKEC = $(MAKE) --no-print-directory -C
LOC := $(strip $(LOCALES))
INSTALL_BINARIES := $(addsuffix .install,$(BINARIES))

all: bin loc doc man

bin: $(BINARIES)

$(BINARIES):
	@echo "Making $@"
	@export BINARY=$@ && $(MAKEC) build

loc:
ifneq ($(LOC),)
	@echo "Making locales"
	@$(MAKEC) contrib/po
endif

doc: loc_doc
	@echo "Making documentation"
	@$(MAKEC) doc

loc_doc:
ifneq ($(LOC),)
	@echo "Making locales documentation"
	@$(MAKEC) contrib/po documentation
endif

.PHONY: man

man:
	@echo "Compressing man pages"
	@$(MAKEC) man

install: $(INSTALL_BINARIES) install_scr install_tex install_loc install_doc \
         install_man install_exm

$(INSTALL_BINARIES):
	@export BINARY=$(basename $@) && $(MAKEC) build install

install_scr:
	@echo "Installing scripts"
	@install bash/* $(BIN_DIR)

install_tex:
	@echo "Installing TeX files"
	@install -d $(TEX_DIR)
	@install -m 0644 tex/* $(TEX_DIR)
	@mktexlsr 2> /dev/null

install_loc:
ifneq ($(LOC),)
	@echo "Installing locales"
	@$(MAKEC) contrib/po install
endif

install_doc:
	@echo "Installing documentation"
	@$(MAKEC) doc install
ifneq ($(LOC),)
	@$(MAKEC) contrib/po install_doc
endif

install_exm:
	@echo "Installing examples"
	@install -d $(EXM_DIR)
	@install -m 0644 contrib/examples/* $(EXM_DIR)

install_man:
	@echo "Installing man pages"
	@$(MAKEC) man install

uninstall: uninstall_bin uninstall_scr uninstall_tex uninstall_loc \
           uninstall_doc uninstall_man

uninstall_bin:
	@echo "Uninstalling binaries"
	@$(MAKEC) build uninstall

uninstall_scr:
	@echo "Uninstalling scripts"
	@$(RM) $(addprefix $(BIN_DIR)/,$(notdir $(wildcard bash/*)))

uninstall_tex:
	@echo "Uninstalling TeX files"
	@$(RM) -r $(TEX_DIR)
	@mktexlsr 2> /dev/null

uninstall_loc:
ifneq ($(LOC),)
	@echo "Uninstalling locales"
	@$(MAKEC) contrib/po uninstall
endif

uninstall_doc:
	@echo "Uninstalling documentation"
	@$(MAKEC) doc uninstall

uninstall_man:
	@echo "Uninstalling man pages"
	@$(MAKEC) man uninstall

clean: clean_bin clean_loc clean_doc clean_man

clean_bin:
	@echo "Cleaning binaries"
	@$(MAKEC) build clean

clean_loc:
ifneq ($(LOC),)
	@echo "Cleaning locales"
	@$(MAKEC) contrib/po clean
endif

clean_doc:
	@echo "Cleaning documentation"
	@$(MAKEC) doc clean

clean_man:
	@echo "Cleaning man pages"
	@$(MAKEC) man clean

