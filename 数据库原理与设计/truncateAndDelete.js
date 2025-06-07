const mysql = require('mysql2');
const db = mysql.createPool({
    host: 'localhost',
    port: '3306',
    user: 'root',
    password: 'WYT12345a.',
    database: 'study'
});

db.query('select 1;', (err, res) => { // 测试连接数据库是否成功
    if(err)
        return console.log(err.message);
    else
        return console.log(res, '数据库连接成功！');
});

const sqlStr = `insert into student 
(sid, sname, sgender, sage)
values ?;`

let data = [];

for(let i = 0; i < 1000000; i++) { // 构造要插入的数据
    data.push([addZerosBeforeNum(i, 10), '学生' + getRandomNum(0, 1000000), getRandomNum(0,1) ? 'male' : 'female', getRandomNum(0, 100)]);
}


db.query(sqlStr, [data], (err, res) => {
    if(err)
        return console.log(err.message);
    else
        return console.log('插入数据成功！插入', res.affectedRows, '行！');
});

function addZerosBeforeNum(i, digitNum) { // 数字前补0
    let nowDigit = 0;
    let num = i;
    if(num === 0) {
        nowDigit = 1;
    }
    else {
        while(num > 0) {
            num  = Math.floor(num / 10);
            nowDigit++;
        }
    }
    let temp = "";
    for(let i = 0; i < digitNum - nowDigit; i++) {
        temp += "0";
    }
    return temp + i;
}

function getRandomNum(min, max, digitNum) { // 返回[min, max]范围内的随机数
    let res = Math.floor(Math.random()*(max-min+1))+min;
    if(digitNum) {
        res = addZerosBeforeNum(res, digitNum);
    }
    return res;
}
