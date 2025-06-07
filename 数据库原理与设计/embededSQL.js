const mysql = require('mysql2');
const db = mysql.createPool({
    host: 'localhost',
    port: '3306',
    user: 'root',
    password: 'WYT12345a.',
    database: 'study'
});

db.query('select 1;', (err, res) => { // 测试数据库连接是否成功
    if(err)
        return console.log(err.message);
    else
        return console.log(res, '数据库连接成功！');
});

// 插入
const data = [['1120210000', 'wyt', '男', 20], ['1120210964', '王英泰', '男', 18]]; 
const sqlStr = `insert into student 
(sid, sname, sgender, sage)
values ?;`;
db.query(sqlStr, [data], (err, res) => {
    if(err)
        return console.log(err.message);
    console.log('插入数据成功！插入', res.affectedRows, '行！');
});

// 查询
db.query('select * from student;', (err, res) => {
    if(err) 
        return console.log(err.message);
    console.log(res);
});

// 修改
const newData= {sid: '1120210000',sname: 'wangyingtai', sgender: '男', sage: 19}; 
const sqlStr1 = `update student set
sname = ?, sage = ?
where sid = ?; `;
db.query(sqlStr1, [newData.sname, newData.sage, newData.sid], (err, res) => {
    if(err)
        return console.log(err.message);
    console.log('更新数据成功！更新', res.affectedRows, '行！');
});

// 删除
const sqlStr2 = `delete from student
where sid = ?`;
db.query(sqlStr2, ["1120210964"], (err, res) => {
    if(err)
        return console.log(err.message);
    console.log('删除数据成功！删除', res.affectedRows, '行！');
});

