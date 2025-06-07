`timescale 1ns / 1ps

module testbench();
    reg [3:0] moduleInput = 4'b0; // ��Ϊģ������
    wire [2:0] moduleOutput; // ��Ϊģ�����
    integer i;
    initial begin
        // ÿ��10ns�������һ
        for(i = 0; i < 16; i = i + 1) begin
            # 10;
            moduleInput = moduleInput + 1'b1;
        end
        // �������붼��֤���ֹͣ����
        $stop;
    end
    
    sqrt sqrt(moduleInput, moduleOutput);
endmodule
