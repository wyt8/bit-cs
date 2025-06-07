/*
* 插入、删除、更新
*/
-- 插入两条课程记录
insert into course (cid, cname)
values ("123456789", "C++"), ("987654321", "Java");
-- 删除30分以下的选课记录
delete from sc
where sc.score < 30;
-- 修改60分以下的成绩为60
update sc set score = 60
where score < 60;

/*
* 视图
*/

create view best (sid, sname, score) -- 创建视图，该视图存放获得过99分和100分成绩的学生信息，该视图无法更新
    as select distinct student.sid, sname, score from student, sc
    where student.sid = sc.sid and score > 98;

select * from best;