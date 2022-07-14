create table item (id serial primary key, name varchar(255) NOT NULL, price numeric(9,2) NOT NULL, data json, created timestamp default NOW());
