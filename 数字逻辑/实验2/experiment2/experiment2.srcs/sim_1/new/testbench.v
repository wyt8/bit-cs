`timescale 1ns / 1ps

module testbench();
    reg [3:0] moduleInput = 4'b0; // 作为模块输入
    wire [2:0] moduleOutput; // 作为模块输出
    integer i;
    initial begin
        // 每隔10ns将输入加一
        for(i = 0; i < 16; i = i + 1) begin
            # 10;
            moduleInput = moduleInput + 1'b1;
        end
        // 所有输入都验证完后停止仿真
        $stop;
    end
    
    sqrt sqrt(moduleInput, moduleOutput);
endmodule
