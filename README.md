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

## Запуск

Запускаем kafka и zookeeper в docker

docker/docker-compose up

Запускаем сервер

./server

Запускаем не менее одно сервиса-обработчика для разбора очереди

./kafka_server

## Идея хранения 

При публикации постов мы записываем основной пост в таблицу wall:

CREATE TABLE `wall` (

  `id` int NOT NULL AUTO_INCREMENT,

  `time` datetime NOT NULL,

  `login` varchar(256) NOT NULL,

  `message` varchar(1024) DEFAULT NULL,

  PRIMARY KEY (`id`),

  KEY `login` (`login`)

);


В момент публикации поста делаем следующее:

- пишем пост в таблицу wall

- для каждого подписчика отправляем сообщение в kafka

- сервисы, читающие очередь kafka пишут в таблицу wall_view сообщение, которые формируют стену пользователя

CREATE TABLE `wallview` (
 
  `id` int NOT NULL AUTO_INCREMENT,

  `wall_id` int NOT NULL,

  `login` varchar(256) NOT NULL,

  PRIMARY KEY (`id`),

  UNIQUE KEY `id_UNIQUE` (`id`),

  KEY `login_ind` (`login`)

);

При просмотре стены выбираются записи из wall_view.

