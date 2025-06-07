`timescale 1ns / 1ps

module timer(
    input clk, // ʱ���ź�
    input reset, // �����ź�
    input run, // ������ʱ�ź� 
    input stop,  // ֹͣ��ʱ�ź�
    output [3:0] select_digit, // �����ѡ���ź�
    output [7:0] digit_num, // ѡ�е��������ʾ������
    output led1, // 1����
    output led2, // ��ʱ��
    output speaker // ������
    );
   
//    wire reset_de,run_de,stop_de;
//    //�����ź�����ģ��
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
   
    //��ʱģ��
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
   
   //չʾģ��
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
