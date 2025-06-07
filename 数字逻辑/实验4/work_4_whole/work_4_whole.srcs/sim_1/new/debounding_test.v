`timescale 1ns / 1ps

module debouncing_test(

    );
    // 时钟信号生成模块
    reg CLK;
    always #5 CLK = ~CLK;

    // 信号声明
    reg key;
    wire key_bouncing;

    // 实例化待测试的模块
    debouncing dut (
        .CLK(CLK),
        .key(key),
        .key_bouncing(key_bouncing)
    );
    reg [0:15] test_sequence = 16'b0000_0010_0001_1111;
    integer i;
    // 初始化
    initial begin
        // 时钟初始化
        CLK = 0;
        // key信号初始化
        key = 0;
        
        // 模拟输入
        for(i=0;i<16;i=i+1)
        begin
            key = test_sequence[i];
            # 1;
        end
        // 模拟运行一段时间

        // 结束仿真
        $finish;
    end

endmodule
