# Eukleides manpages makefile
# Copyright (c) Christian Obrecht 2004-2010

include ../Config

MAN_PAGES := $(addsuffix .gz, $(wildcard *.1))

man: $(MAN_PAGES)

%.1.gz: %.1
	@gzip -c $< > $@

install: $(MAN_PAGES)
	@install -d $(MAN_DIR)
	@install -m 0644 $^ $(MAN_DIR)

uninstall:
	@$(RM) $(addprefix $(MAN_DIR)/, $(MAN_PAGES))

clean:
	@$(RM) $(MAN_PAGES)
