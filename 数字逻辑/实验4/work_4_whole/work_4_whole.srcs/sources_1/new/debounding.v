`timescale 1ns / 1ps

module debouncing(
    input wire key, CLK,
    output reg key_bouncing
);
    parameter COUNTER_MAX = 50;             // 最大取50次样
    reg [31:0] cnt = 0;                          // 存储计时器
    reg key_pre;
//    reg Is_counting = 1'b0;
//    always @(posedge CLK)        // 准备开始计时
//    begin 
//        if(Is_counting == 1'b0)
//        begin
//            key_pre <= key;
//            cnt <= 0;
//            Is_counting <= 1'b1;
//        end
//        else begin
//            if(!(key_pre == key && cnt < COUNTER_MAX)) begin
//                if(key_pre != key) begin
//                    cnt <= 0;
//                    Is_counting <= 1'b0;
//                end 
//                else begin
//                    key_bouncing <= key_pre;
//                end
//            end 
//            else begin
//                cnt <= cnt + 1;
//            end
//        end
//    end    
    
    always @(posedge CLK) begin
        key_pre <= key;
    end
    
    wire key_change = 1'b0;
    
    assign key_change = (key != key_pre) ? 1'b1 : 1'b0;
    
    always @ (posedge CLK) begin
        if(key_change)
            cnt <= 0;
        else
            cnt <= cnt + 1;
    end
    
    always @(posedge CLK) begin
        if(cnt >= COUNTER_MAX)
             key_bouncing <= key_pre;
    end
    
endmodule
