# Описание 


## Инсталяция
sudo apt-get install gcc

sudo apt-get install g++

sudo apt-get install cmake

sudo apt-get install libssl-dev

sudo apt-get install zlib1g-dev

sudo apt-get install librdkafka-dev

sudo apt-get install mysql-server

sudo apt-get install mysql-client

sudo apt-get install libmysqlclient-dev

sudo apt-get install libboost-all-dev

# Install poco

git clone -b master https://github.com/pocoproject/poco.git

cd poco

mkdir cmake-build

cd cmake-build

cmake ..

cmake --build . --config Release

sudo cmake --build . --target install

## Install gtest
sudo apt-get install libgtest-dev

cd /usr/src/gtest/

sudo cmake -DBUILD_SHARED_LIBS=ON

sudo make

sudo cp *.so /usr/lib




## Зависимости

