`timescale 1ns / 1ps

module timer_test();
    reg clk; // 时钟信号
    reg reset; // 重置信号
    reg run; // 继续计时信号 
    reg stop;  // 停止计时信号
    wire [3:0] select_digit; // 数码管选择信号
    wire [7:0] digit_num; // 选中的数码管显示的数字
    wire led1; // 1分钟
    wire led2; // 超时灯
    initial begin
        clk <= 1'b0;
        reset <= 1'b1;
        stop <= 1'b0;
        run <= 1'b0;
        # 100 reset <= 1'b0;
        # 500 reset <= 1'b1;
        # 100 run <= 1'b1;
        # 500 run <= 1'b0;
        
//        # 10000 $stop;
    end
    
    always # 5 clk = ~clk;
    
    timer timer(clk, reset, run, stop, select_digit, digit_num, led1, led2);
endmodule
