echo "Running install.sh"
sudo apt-get update
sudo apt-get upgrade -y
echo "Installing pkg-config"
sudo apt-get install pkg-config -y
echo "Installing git"
sudo apt-get install git -y
echo "Installing build-essential"
sudo apt-get install build-essential libtool autoconf unzip wget libssl-dev -y
echo "Installing cmake"
version=3.27
build=4
mkdir ~/temp
cd ~/temp
wget https://cmake.org/files/v$version/cmake-$version.$build.tar.gz
tar -xzvf cmake-$version.$build.tar.gz
cd cmake-$version.$build/
./bootstrap
make -j$(nproc)
sudo make install
cd ~
echo "Installing lapack and blas"
sudo apt-get install liblapack-dev libblas-dev -y
echo "Installing boost and armadillo"
sudo apt-get install libboost-dev libarmadillo-dev -y
echo "Installing mlpack from source"
cd ~/temp
git clone https://github.com/mlpack/mlpack
cd mlpack
cmake -S . -B build -DDOWNLOAD_DEPENDENCIES=ON -DDEBUG=ON
sudo cmake --build build --target install
cd ~
echo "Installing GSL"
sudo apt-get install libgsl-dev -y
echo "Installing HDF5"
sudo apt-get install libhdf5-dev -y
echo "Finished install.sh"