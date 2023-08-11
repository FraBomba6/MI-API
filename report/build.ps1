# Same as build.sh but for Windows

$folder = "./build"
if(!(Test-Path -Path $folder)){
    New-Item -ItemType Directory -Path $folder
}

pdflatex -shell-escape -output-directory=build report.tex
bibtex build/report
pdflatex -shell-escape -output-directory=build report.tex
pdflatex -shell-escape -output-directory=build report.tex

Move-Item -Force -Path build/report.pdf ./report.pdf