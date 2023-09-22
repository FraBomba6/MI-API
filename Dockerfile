FROM rayproject/ray:latest
LABEL authors="frabomba6"
# Install dependencies running install.sh
COPY install.sh ./install.sh
RUN sudo chmod +x ./install.sh
RUN ./install.sh
RUN sudo rm ./install.sh
# Copy the project
COPY . ./small_project_cluster
WORKDIR ./small_project_cluster
# Build the project using cmake
RUN sudo cmake -S . -B build
RUN sudo cmake --build build -t attack