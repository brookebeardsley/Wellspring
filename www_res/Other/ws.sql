create database ws;

use ws;

create table sview(
  sviewid int unsigned auto_increment not null,
  sviewname varchar(150) not null unique,
  primary key (sviewid));

create table schedule(
  scheduleid int unsigned auto_increment not null,
  trainline enum('El','Metra','Amtrak') not null,
  routename varchar(150) not null,
  runnumber varchar(100) not null,
  operatorid varchar(100) not null,
  primary key (scheduleid));

create table sview_schedule(
  sviewid int unsigned not null,
  scheduleid int unsigned not null,
  foreign key (sviewid) references sview(sviewid),
  foreign key (scheduleid) references schedule(scheduleid),
  primary key (sviewid,scheduleid));

alter table sview type=InnoDB;
alter table schedule type=InnoDB;
alter table sview_schedule type=InnoDB;

grant select, insert, update on ws.sview to ws_cilbup@localhost identified by 'cilbup';
grant select, insert, update on ws.schedule to ws_cilbup@localhost identified by 'cilbup';
grant select, insert, update, delete on ws.sview_schedule to ws_cilbup@localhost identified by 'cilbup';
