# Eukleides locales makefile
# Copyright (c) Christian Obrecht 2004-2010

include ../../Config

MO_FILES := $(addsuffix .mo, $(LOCALES))

locales: $(MO_FILES)

%.mo: %.po
	@echo "    Building $@"
	@msgfmt -o $@ -v $< 2> /dev/null

install: $(LOCALES)

$(LOCALES): $(MO_FILES)
	@dir=$(MO_DIR)/$@/LC_MESSAGES && \
	install -d $$dir && \
	install -m 0644 $@.mo $$dir/eukleides.mo

PDF_FILES := $(addsuffix .pdf, $(LOCALES))
TEX_FILES := $(addsuffix .tex, $(LOCALES))

documentation: $(PDF_FILES)

$(PDF_FILES): $(TEX_FILES)

%.tex: %.po
	@perl extract.pl $< $@ $(PAPER)

%.pdf: %.tex
	@echo "    Building $@"
	@pdflatex -interaction=nonstopmode $< > /dev/null

install_doc: $(PDF_FILES)
	@install -d $(DOC_DIR)
	@install -m 0644 $^ $(DOC_DIR)

uninstall:
	@for f in $(LOCALES); \
	 do \
	     $(RM) $(MO_DIR)/$$f/LC_MESSAGES/eukleides.mo; \
	 done

pot: eukleides.pot

files:
	grep -R -l "(_(" ../src/ > $@

messages: files
	xgettext --no-location --omit-header -k_ -f $< -o $@
	printf "\nmsgid \"parse error\"\nmsgstr \"\"\n" >> $@

keywords: ../src/common/keywords.c
	xgettext --no-location -k__ $< -o $@
	printf "\n### End of keywords ###\n\n" >> $@

eukleides.pot: keywords messages
	cat $^ > $@

clean:
	@for f in $(LOCALES); do $(RM) $$f.{mo,tex,log,pdf}; done
	@$(RM) files keywords messages eukleides.pot
