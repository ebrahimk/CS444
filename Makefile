TARGET="insert name of tex file here"


default: pdf

bib: bib
	bibtex ${TARGET}

dvi: ${TARGET}.tex

	latex ${TARGET}.tex

	latex $(TARGET).tex

ps: dvi
	dvips -R -Poutline -t letter ${TARGET}.dvi -o ${TARGET}.ps

pdf: ps
	ps2pdf ${TARGET}.ps

clean-dvi:
	rm -f ${TARGET}.aux
	rm -f ${TARGET}.log
	rm -f ${TARGET}.dvi
	rm -f ${TARGET}.out
	rm -f ${TARGET}.toc

clean-ps: clean-dvi
	rm -f ${TARGET}.ps

clean-pdf: clean-ps
	rm -f ${TARGET}.bbl
	rm -f ${TARGET}.blg
	rm -f ${TARGET}.pdf


clean: clean-pdf
