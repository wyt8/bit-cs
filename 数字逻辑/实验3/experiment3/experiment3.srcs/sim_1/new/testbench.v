`timescale 1ns / 1ps

module testbench();
    wire Z;
    reg clk = 1'b0, X, reset;
    reg [0:23] test_seq = 24'b1010_1010_1111_0011_1011_1101;
    integer i = 0;
    parameter period = 10;
    coin coin(clk, reset, X, Z);
    
    initial begin
        reset <= 1'b1;
        # (period/2);
        reset <= 1'b0;
        // ��test_seq���θ�������
        for(i = 0; i < 24; i = i + 1) begin
            X <= test_seq[i];
            # period;
        end
        X = 1'b0;
        # (period/2);
        $stop;
    end
  
    // ģ��ʱ���ź�
    always # (period/2) clk = ~clk;
    
endmodule
