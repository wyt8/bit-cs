/*
* 存储过程
*/
use study;
delimiter // --重新定义换行符
create procedure if not exists the_percentage_of_age (in min smallint, in max smallint, out percentage int) -- 定义存储过程
begin
    declare 
    select @percentage=count(*) from student where student.sage >= min and student.sage <= max;
end//
delimiter ;

call the_percentage_of_age(20, 30, @num);

select @num;

show create procedure the_percentage_of_age; -- 删除存储过程

drop procedure the_percentage_of_age; -- 删除存储过程
 
