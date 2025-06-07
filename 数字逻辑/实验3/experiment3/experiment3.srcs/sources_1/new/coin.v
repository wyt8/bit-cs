`timescale 1ns / 1ps

module coin(CLK, RESET, X, Z);
    input CLK, RESET, X;
    output reg Z;
    // 声明当前状态和下一状态
    reg [1:0] state, next_state;
    // 声明4种状态
    parameter S0 = 2'b00, S1 = 2'b01, S2 = 2'b10, S3 = 2'b11;
    
    always @ (posedge CLK or posedge RESET) begin
        if(RESET)
            state <= S0;
        else
            state <= next_state;
    end
    
    // 下一状态与输入和当前状态有关
    always @ (X or state) begin
        case (state)
            S0: next_state = X ? S1 : S0;
            S1: next_state = X ? S2 : S1;
            S2: next_state = X ? S3 : S2;
            S3: next_state = X ? S0 : S3;
        endcase 
    end
    
    // 输出与当前状态和输入有关
    always @ (X or state) begin
        case (state)
            S0: Z = 1'b0;
            S1: Z = 1'b0;
            S2: Z = 1'b0;
            S3: Z = X ? 1'b1 : 1'b0;
        endcase 
    end
    
endmodule
