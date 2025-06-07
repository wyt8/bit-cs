`timescale 1ns / 1ps
module counter(
    input clk,                //时钟
    input reset,              //复位
    input run,                //开始
    input stop,               //停止
    output reg timeout,       //是否超时
    output reg min,           //分
    output reg [7:0] sec,     //秒
    output reg [7:0] milisec,  //毫秒 
    output [1:0] s
    );
    //分别为不同的状态赋不同的值
    parameter state_initial =  2'b00 ;
    parameter state_run     =  2'b01 ;
    parameter state_stop    =  2'b10 ;
    parameter state_timeout =  2'b11 ;
    //声明
    reg [1:0] state = state_initial, next_state = state_initial;
   assign  s =state;
    //是否复位
    always @ (posedge clk or negedge reset)
    begin
        //复位则initial
        if(!reset) begin
            state <= state_initial;
        end
        //不复位
        else begin
            state <= next_state;
        end
    end
    
    //计时10ms
    //10ms的时钟次数
    parameter ms_10 = 1000000; 
    //记录时钟次数
    reg [31:0] count = 0;
    //记录是否到了10ms
    wire flag;
    assign flag = count == ms_10;
    
    //count计数
    always @ (posedge clk)
    begin
        case(state)
            state_initial : begin
                count <= 0;
                timeout <= 1'b0;
            end
            
            state_run : begin
                if(count < ms_10)
                    count <= count + 1;
                else begin
                    count <= 0;
                end
            end
            
            state_stop : begin
                count <= count;
            end
            
            state_timeout : begin
                count <= 0;
                timeout <= 1'b1;
            end
        endcase
    end
    
    //状态转移
    always @ (*)
    begin
        case(state)
            state_initial : begin
                //默认状态转移到计时状态
                if(run == 1'b1) begin
                    next_state = state_run;
                end
                //默认状态不变
                else begin
                    next_state = state_initial;
                end
            end
        
            state_run : begin
                //转移到暂停状态
                if(stop == 1'b1) begin
                    next_state = state_stop;
                end
                //转移到超时状态
                else if (min == 1'b1 && sec == 'h59 && milisec == 'h99 && flag ) begin
                    next_state = state_timeout;
                end
                //计时状态不变
                else begin
                    next_state = state_run;
                end
            end
        
            state_stop : begin
                //转移到计时状态
                if(run == 1'b1)
                    next_state = state_run;
                //暂停状态不变
                else 
                    next_state = state_stop;
            end
        
            state_timeout : begin
                //一直保持timeout状态，直到被reset（此时next_state自动转为initial）
                next_state = state_timeout;
            end
        endcase
    end
    
    //毫秒处理
    always @ (posedge clk or negedge reset) begin
        if(!reset) begin
            milisec <= 'b0;
        end
        else if(flag) begin
            //990ms归0
            if(milisec == 'h99) begin
                milisec <= 'h00;
            end
            //个位为9进位
            else if(milisec[3:0] == 'h9)begin
                milisec[3:0] <= 'h0;
                milisec[7:4]<= milisec[7:4] + 1'b1;
            end
            //个位加1
            else begin
                milisec[3:0] <= milisec[3:0] + 1'b1;
            end
        end
    end
    
    //秒处理
    always @ (posedge clk or negedge reset) begin
        if(!reset) begin
            sec <='b0;
        end
        //1000ms时second加1
        else if (flag && milisec == 'h99) begin
            //60s归0
            if(sec == 'h59)begin
                sec <= 'h00;
            end
            //个位10s进1
            else if (sec[3:0]=='h9) begin
                sec[3:0] <='b0;
                sec[7:4] <= sec[7:4]+1'b1;
            end
            //个位加1
            else begin
                sec[3:0] <= sec[3:0] + 1'b1;
            end
        end
        else begin
            sec <= sec;
        end
    end
    
    //分处理
    always @ (posedge clk or negedge reset) begin
        if(!reset) begin
            min <= 1'b0;
        end
        //60s
        else if(flag && sec == 'h59 && milisec=='h99) begin
            //min为0
            if(min == 1'b0) begin
                min <= 1'b1;
            end
            //min为1，timeout
            else begin
                min <= min; 
            end
        end
    end
    
endmodule

