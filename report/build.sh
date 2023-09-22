folder=./build
if [ ! -d "$folder" ]; then
  mkdir -p $folder
fi
pdflatex -shell-escape -output-directory=build report.tex
bibtex build/report
pdflatex -shell-escape -output-directory=build report.tex
pdflatex -shell-escape -output-directory=build report.tex
mv build/report.pdf .
rm -rf build
qlmanage -p report.pdf