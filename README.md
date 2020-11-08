# Описание 


Для хранения диалогов между пользователями используется таблица:

CREATE TABLE `chat` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `time` datetime NOT NULL,
  `source_login` varchar(256) NOT NULL,
  `destination_login` varchar(256) NOT NULL,
  `message` varchar(1024) DEFAULT NULL,
  `readed` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `source` (`source_login`),
  KEY `destination` (`destination_login`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=latin1;

В ней храняться сообщения от пользователя к пользователю.

Делаем sharding на два сервера. Идея - сделать равномерное распределение по парам source_login - destination_login. Для этого конкатинируем логины и высчитываем hash (сумму символов). Записи с четными хэщами посылаем на первый сервер, с нечетными на второй.

В программе в SQL запросы добавляем комментарий с номером (' -- shardind:0' или ' -- sharding:1'). Если понадобится сможем сделать шардинг на сколько угодно серверов.

## Пример кода вставки

void insert()
        {
            std::string query = "INSERT INTO chat  (time,source_login,destination_login,message,readed) VALUES(";
            query += "FROM_UNIXTIME(" + std::to_string(time) + "),";
            query += "'" + source_login + "',";
            query += "'" + destination_login + "',";
            query += "'" + message + "',";
            query += "'" + std::to_string(readed) + "'";
            query += ") -- sharding:";
            query +=std::to_string(get_sharding_hint(source_login, destination_login));
            database::Database_MySQL::get().execute(query);
        }

где количество шардов вычисляется как 
static size_t get_sharding_hint(const std::string &lhv, const std::string &rhv)
        {
            size_t result{};
            for (auto c : lhv)
                result += c;
            for (auto c : rhv)
                result += c;
            return result % 2;
        }

# конфиг proxySQL

mysql_query_rules:
(
    {
        rule_id=1
        active=1
        match_pattern="-- sharding:0"
        destination_hostgroup=0
        apply=1
    },
    {
        rule_id=2
        active=1
        match_pattern="-- sharding:1"
        destination_hostgroup=1
        apply=1
    },
)

наверное одно из правил избыточное, но добавил "для красоты"