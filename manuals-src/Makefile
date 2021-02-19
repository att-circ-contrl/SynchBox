# USE SyncBox manual - Makefile
# Written by Christopher Thomas.


PROJNAME=synchbox
BASENAME=$(PROJNAME)-manual
BASEGUIDE=$(PROJNAME)-guide

# This doesn't evaluate backticks until the macro is expanded, but still works.
THISDATE=`date +%Y%m%d`

DOCBITS=\
	$(BASENAME)-intro.tex		\
	$(BASENAME)-commands.tex	\
	$(BASENAME)-periphs.tex		\
	$(BASENAME)-maint.tex		\
	$(BASENAME)-hardware.tex	\
	$(BASENAME)-software.tex	\
	$(BASENAME)-files.tex		\
	$(BASENAME)-brainamp.tex	\
	$(BASENAME).tex			\
	$(BASEGUIDE).tex

AUXBITS=\
	captures/capture-help.txt		\
	captures/capture-messages.txt		\
	captures/capture-status.txt


default: helpscreen


helpscreen:
	@echo ""
	@echo "Targets:   clean  all  aux"
	@echo ""


force: clean all

all: $(BASENAME).pdf $(BASEGUIDE).pdf

clean: tclean
	rm -f $(BASENAME).pdf $(BASEGUIDE).pdf

tclean:
	rm -f $(BASENAME).log $(BASENAME).aux $(BASENAME).toc
	rm -f $(BASEGUIDE).log $(BASEGUIDE).aux $(BASEGUIDE).toc
	rm -f texput.*

aux:
	./rebuild-auxlinks.sh


$(BASENAME).pdf: $(DOCBITS) $(AUXBITS)
	pdflatex $(BASENAME).tex
	pdflatex $(BASENAME).tex
	make tclean

$(BASEGUIDE).pdf: $(DOCBITS) $(AUXBITS)
	pdflatex $(BASEGUIDE).tex
	pdflatex $(BASEGUIDE).tex
	make tclean


# This is the end of the file.
