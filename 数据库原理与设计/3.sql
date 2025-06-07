-- /*
-- * 模式
-- */
-- -- mysql中的模式与数据库基本上是等价的，命令中的database可以与schema互换
-- create database study; -- 定义数据库
-- show databases; -- 查看数据库
-- use study; -- 使用数据库
-- drop database study; -- 删除数据库
-- show databases; -- 查看数据库


/*
* 基本表
*/

-- create database study; -- 定义数据库
-- -- 创建
-- create table study.student -- 在study数据库中创建student基本表
--     (sid char(10) not null unique, -- 列级约束
--     sname char(20),
--     sgender char(6) check(sgender in ('male', 'female')), -- 列级约束，使用了check约束
--     sage tinyint unsigned check(sage >= 0 and sage <= 100),
--     constraint c1 primary key(sid)); -- 表级约束，如果主键涉及多个属性，必须使用表级约束

-- create table study.course
--     (cid char(9) not null unique primary key,
--     cname char(20) not null);

-- create table study.sc
--     (sid char(10) references study.student(sid), -- 列级约束，使用了外键约束，不过不会生效
--     cid char(9),
--     score tinyint unsigned null,
--     check(score >= 0 and score <= 100), -- 表级约束，可以省略constraint <约束名>
--      foreign key(sid) references study.student(sid), -- 表级约束，使用了列级约束
--     foreign key(cid) references study.course(cid)); 

-- use study;
-- show tables;
-- describe student; -- 描述表的结构，describe可以简写为desc
-- describe course;
-- describe sc;
-- -- 修改
-- alter table study.student
--     add smajor varchar(20),
--     drop sgender,
--     modify sname char(15);
-- describe student;
-- -- 删除
-- drop table study.sc;
-- show tables;

-- /*
-- * 索引
-- */

-- create index i_sage -- 创建索引
--     on study.student(sage asc);
-- show index from study.student; -- 查看索引 
-- drop index i_sage on study.student; -- 删除索引
-- show index from study.student;

/*
* 视图
*/

create view male_age (sid, sname, sage) -- 创建视图
    as select sid, sname, sage from student
    with check option;

desc male_age; -- 查看视图定义

alter view male_age (sname, sage) -- 修改视图，注意select中的列要与视图中的列对应
    as select sname, sage from student
    with check option;

desc male_age;

drop view male_age; -- 删除视图
