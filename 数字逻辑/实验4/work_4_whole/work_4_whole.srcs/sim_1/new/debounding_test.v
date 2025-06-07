`timescale 1ns / 1ps

module debouncing_test(

    );
    // ʱ���ź�����ģ��
    reg CLK;
    always #5 CLK = ~CLK;

    // �ź�����
    reg key;
    wire key_bouncing;

    // ʵ���������Ե�ģ��
    debouncing dut (
        .CLK(CLK),
        .key(key),
        .key_bouncing(key_bouncing)
    );
    reg [0:15] test_sequence = 16'b0000_0010_0001_1111;
    integer i;
    // ��ʼ��
    initial begin
        // ʱ�ӳ�ʼ��
        CLK = 0;
        // key�źų�ʼ��
        key = 0;
        
        // ģ������
        for(i=0;i<16;i=i+1)
        begin
            key = test_sequence[i];
            # 1;
        end
        // ģ������һ��ʱ��

        // ��������
        $finish;
    end

endmodule
