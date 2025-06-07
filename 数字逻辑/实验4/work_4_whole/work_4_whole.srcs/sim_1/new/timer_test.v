`timescale 1ns / 1ps

module timer_test();
    reg clk; // ʱ���ź�
    reg reset; // �����ź�
    reg run; // ������ʱ�ź� 
    reg stop;  // ֹͣ��ʱ�ź�
    wire [3:0] select_digit; // �����ѡ���ź�
    wire [7:0] digit_num; // ѡ�е��������ʾ������
    wire led1; // 1����
    wire led2; // ��ʱ��
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
