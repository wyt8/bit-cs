-- /*
-- * 单表查询
-- */

-- -- 查询年龄小于10岁的男学生信息
-- select sid, sname, sage from student where sage < 5 and sgender = 'male';
-- -- 查询名字的倒数第二位为6且年龄小于10岁的学生信息
-- select * from student where sname like "%6_" and sage < 10;
-- -- 查询年龄为18或19的男学生信息
-- select * from student where sage in (18, 19) and sgender = 'male';
-- -- 查询年龄在20~30岁之间的学生人数
-- select sage, count(*) as "num_of_students" from student 
-- group by sage having sage <= 30 and sage >= 20 
-- order by sage asc; 

-- /*
-- * 连接查询
-- */
-- -- 查询所有课程中成绩为95~100、年龄在18~30岁之间的学生姓名，课程名和成绩
-- select sname, cname, score from student, course, sc
-- where student.sid = sc.sid and course.cid = sc.cid and score between 98 and 100
-- and sage between 18 and 30
-- order by score desc;

/* 
* 嵌套查询
*/

-- 查询选修了20门课以上的学习信息
select sid, sname, sage, sgender from student
where sid in (select sid from sc group by sid having count(*) > 20);
-- 查询成绩为100分的学生信息
select sname from student
where exists (select * from sc where student.sid = sc.sid and score = 100); 

-- /*
-- * 集合查询
-- */
-- -- 查询学号为0000000001和0000000002的选课信息
-- select * from sc where sid = "0000000001"
-- union
-- select * from sc where sid = "0000000002";
