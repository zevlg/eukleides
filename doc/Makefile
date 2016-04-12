# Eukleides documentation makefile
# Copyright (c) Christian Obrecht 2004-2010

include ../Config

MAKEHTML = makeinfo --no-validate --html
TEXI = eukleides.texi
CSS = eukleides.css

documentation: $(DOC)

info: eukleides.info.gz

eukleides.info: $(TEXI)
	@echo "    Building info pages"
	@makeinfo $<

eukleides.info.gz: eukleides.info
	@gzip -c $< > $@

pdf: eukleides.pdf

eukleides.pdf: $(TEXI)
	@echo "    Building manual in PDF"
ifeq ($(PAPER),A4)
	@texi2pdf --texinfo=@afourpaper $< > /dev/null
else
	@texi2pdf $< > /dev/null
endif

ifeq ($(HTML),split)
html: manual/index.html
else
html: eukleides.html
endif

manual/index.html: $(TEXI)
	@echo "    Building manual in HTML format"
	@$(MAKEHTML) --css-ref=$(CSS) --output=manual $< && cp $(CSS) manual

eukleides.html: $(TEXI)
	@echo "    Building manual in HTML format"
	@$(MAKEHTML) --no-split --no-headers --css-include=$(CSS) \
		    --output=eukleides.html $<

INST_TARG := $(addprefix install_, $(DOC))

install: $(INST_TARG)

install_info: eukleides.info.gz
	@echo "    Installing info pages"
	@install -d $(INFO_DIR)
	@ginstall-info --quiet $< $(INFO_DIR)/dir
	@install -m 0644 $< $(INFO_DIR)

install_pdf: pdf
	@echo "    Installing manual in PDF"
	@install -d $(DOC_DIR)
	@install -m 0644 eukleides.pdf $(DOC_DIR)

ifeq ($(HTML),split)
install_html: install_split
else
install_html: install_nosplit
endif

install_split: html
	@echo "    Installing manual in HTML format"
	@install -d $(DOC_DIR)
	@install -d $(DOC_DIR)/manual
	@install -m 0644 manual/* $(DOC_DIR)/manual

install_nosplit: html
	@echo "    Installing manual in HTML format"
	@install -d $(DOC_DIR)
	@install -m 0644 eukleides.html $(DOC_DIR)

uninstall:
	@$(RM) -r $(DOC_DIR)
ifneq ($(findstring info,$(DOC)),)
	@ginstall-info --quiet --remove eukleides $(INFO_DIR)/dir
	@$(RM) $(INFO_DIR)/eukleides.info.gz
endif

tidy:
	@$(RM) eukleides.{aux,cp,cps,fn,fns,ky,log,pg,toc,tp,vr}

clean: tidy
	@$(RM) eukleides.{info,info.gz,pdf,html} manual/*
