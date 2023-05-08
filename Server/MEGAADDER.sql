drop table if exists users cascade;
drop table if exists dialogs cascade;
drop table if exists messeges cascade;

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

create table messeges
(
    id serial primary key,
    dialog_id int not null,
    username varchar(50) not null,
    time time default CURRENT_TIME not null,
    CONSTRAINT dialog FOREIGN KEY (dialog_id) REFERENCES dialogs (id) ON DELETE CASCADE,
    CONSTRAINT username FOREIGN KEY (username) REFERENCES users (username) ON DELETE CASCADE
);

INSERT INTO users(username, password)
    VALUES ('keke', 'keke'),
           ('lolo','lolo'),
           ('11111','11111'),
           ('john','john'),
           ('artist','artist'),
           ('nick','nick');