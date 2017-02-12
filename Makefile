PDFLATEX=pdflatex
LATEX=latex
BIBTEX=bibtex
DVI2PDF=dvips
PS2PDF=ps2pdf
UNAME:=$(shell uname -s)
ifeq ($(UNAME),Linux)
	VIEWER=xdg-open
endif
ifeq ($(UNAME), Darwin)
	VIEWER=open
endif

FILE_NAME=main

.PHONY: all, clean, view

all:
	#$(LATEX) $(FILE_NAME).tex
	$(PDFLATEX) $(FILE_NAME).tex
	$(BIBTEX) $(FILE_NAME).aux
	$(PDFLATEX) $(FILE_NAME).tex
	#$(PDFLATEX) $(FILE_NAME).tex
	#$(LATEX) $(FILE_NAME).tex
	#$(LATEX) $(FILE_NAME).tex
	#$(DVI2PDF) -t letter $(FILE_NAME).dvi
	#$(PS2PDF) $(FILE_NAME).ps

view: all
	$(VIEWER) $(FILE_NAME).pdf
	rm *.log *.aux *.out
	#rm *-eps-converted-to.pdf
	pdffonts $(FILE_NAME).pdf

clean:
	rm *.log *.aux *.dvi *.out *.ps *.bak
