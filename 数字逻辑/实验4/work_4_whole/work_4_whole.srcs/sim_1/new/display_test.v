`timescale 1ns / 1ps

module display_test(
    );
    reg clk;
    reg reset;
    reg [7:0] sec, milisec;
    reg onemin_flag;
    wire [3:0] select_digit;
    wire [7:0] digit_num;
    wire led1; // 1∑÷÷”µ∆
    wire led2; // ≥¨ ±µ∆
    
    initial begin
        reset <= 1'b1;
        clk <= 1'b0;
        # 10 reset <= 1'b0;
        sec <= 8'b0101_1001;
        milisec <= 8'b1001_1001;
        onemin_flag <= 1'b1;
        # 1000 $stop;
    end
    
    always # 10 clk = ~clk;
    
    display display(clk, reset, sec, milisec, onemin_flag, select_digit, digit_num, led1, led2);
    
endmodule
