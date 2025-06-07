`timescale 1ns / 1ps

module timer(
    input clk, // 时钟信号
    input reset, // 重置信号
    input run, // 继续计时信号 
    input stop,  // 停止计时信号
    output [3:0] select_digit, // 数码管选择信号
    output [7:0] digit_num, // 选中的数码管显示的数字
    output led1, // 1分钟
    output led2, // 超时灯
    output speaker // 蜂鸣器
    );
   
//    wire reset_de,run_de,stop_de;
//    //按键信号消抖模块
//    debouncing res_debouncing(
//        .key        (reset),
//        .CLK        (clk),
//        .key_bouncing    (reset_de)
//    );
//    debouncing run_debouncing(
//        .key        (run),
//        .CLK        (clk),
//        .key_bouncing    (run_de)
//    );
//    debouncing stop_debouncing(
//        .key        (stop),
//        .CLK        (clk),
//        .key_bouncing    (stop_de)
//    );
    
    wire  min;
    wire [7:0] sec;
    wire [7:0] milisec;
    wire timeout;
   
    //计时模块
    counter counter(
        .clk                (clk),
        .reset              (reset),
        .run                (run),
        .stop               (stop),
        .timeout            (timeout),
        .min                (min),
        .sec                (sec),
        .milisec            (milisec)
    );
   
   //展示模块
   display display(
        .clk                (clk),
        .reset              (reset),
        .sec                (sec),
        .milisec            (milisec),
        .onemin_flag        (min),
        .time_out            (timeout),   
        .select_digit       (select_digit),
        .digit_num          (digit_num),
        .led1               (led1),
        .led2               (led2),
        .speaker            (speaker)
   );
   
endmodule
