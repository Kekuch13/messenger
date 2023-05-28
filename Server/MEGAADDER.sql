drop table if exists users cascade;
drop table if exists dialogs cascade;
drop table if exists messages cascade;

create table users
(
    username varchar(50) primary key,
    password varchar(70) not null
);

create table dialogs
(
    id serial primary key,
    username_1 varchar(50),
    username_2 varchar(50),
    CONSTRAINT user_1 FOREIGN KEY (username_1) REFERENCES users (username) ON DELETE CASCADE,
    CONSTRAINT user_2 FOREIGN KEY (username_2) REFERENCES users (username) ON DELETE CASCADE
);

create table messages
(
    id serial primary key,
    dialog_id int not null,
    author varchar(50) not null,
    date date default CURRENT_DATE not null,
    time time default CURRENT_TIME not null,
    text varchar(250) not null,
    CONSTRAINT dialog FOREIGN KEY (dialog_id) REFERENCES dialogs (id) ON DELETE CASCADE,
    CONSTRAINT username FOREIGN KEY (author) REFERENCES users (username) ON DELETE CASCADE
);

INSERT INTO users(username, password)
    VALUES ('anna', 'anna'),
           ('maria','maria'),
           ('ivan','ivan'),
           ('john','john'),
           ('donald','donald'),
           ('nick','nick');

INSERT INTO dialogs(username_1, username_2)
    VALUES ('anna', 'john'),
           ('donald', 'john'),
           ('ivan', 'anna'),
           ('nick', 'donald'),
           ('maria', 'john'),
           ('maria', 'nick'),
           ('nick', 'ivan'),
           ('donald', 'anna');