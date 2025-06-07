`timescale 1ns / 1ps

module display(clk, reset, sec, milisec, onemin_flag, time_out, select_digit, digit_num, led1, led2, speaker);
    input clk;
    input reset;
    input [7:0] sec, milisec;
    input onemin_flag; // 大于等于一分钟标志
    input time_out;
    output [3:0] select_digit;
    output [7:0] digit_num;
    output wire led1; // 1分钟灯
    output wire led2; // 超时灯
    output reg speaker = 1'b0;
    reg [3:0] showing_num;
    reg [3:0] showing_digit = 4'b0001;
    parameter period = 50000;
    parameter fperiod = 10000000;
    reg [31:0] counter = 0;
    reg [31:0] fcounter = 0;
    assign led1 = onemin_flag;
    assign led2 = time_out;

    always @ (posedge clk) begin
        if(time_out) begin
            if(fcounter < fperiod) begin
                speaker <= 1'b0;
                fcounter <= fcounter + 1;
            end
            else begin
                speaker <= 1'b1;
                fcounter = 0;
            end
        end
    end
    assign select_digit = showing_digit;
    always @ (posedge clk or negedge reset) begin
        if(counter >= period) begin
            if(!reset) begin
                case(showing_digit)
                    4'b0001: begin
                        showing_digit = 4'b1000;
                        showing_num = 4'b1010;
                        counter <= 0;
                    end
                    4'b0010: begin
                        showing_digit = 4'b0001;
                        showing_num = 4'b1100;
                        counter <= 0;
                    end
                    4'b0100: begin
                        showing_digit = 4'b0010;
                        showing_num = 4'b1011;
                        counter <= 0;
                    end
                    4'b1000: begin
                        showing_digit = 4'b0100;
                        showing_num = 4'b1011;
                        counter <= 0;
                    end 
                endcase
            end
            else if(time_out) begin
                case(showing_digit)
                    4'b0001: begin
                        showing_digit = 4'b1000;
                        showing_num = 4'b0000;
                        counter <= 0;
                    end
                    4'b0010: begin
                        showing_digit = 4'b0001;
                        showing_num = 4'b1110;
                        counter <= 0;
                    end
                    4'b0100: begin
                        showing_digit = 4'b0010;
                        showing_num = 4'b0111;
                        counter <= 0;
                    end
                    4'b1000: begin
                        showing_digit = 4'b0100;
                        showing_num = 4'b1101;
                        counter <= 0;
                    end 
                endcase
            end
            else begin
                case(showing_digit)
                    4'b0001: begin
                        showing_digit = 4'b1000;
                        showing_num = sec[7:4];
                        counter <= 0;
                    end
                    4'b0010: begin
                        showing_digit = 4'b0001;
                        showing_num = milisec[3:0];
                        counter <= 0;
                    end
                    4'b0100: begin      
                        showing_digit = 4'b0010;
                        showing_num = milisec[7:4];
                        counter <= 0;
                    end
                    4'b1000: begin
                        showing_digit = 4'b0100;
                        showing_num = sec[3:0];
                        counter <= 0;
                    end 
                endcase
            end
        end
        else begin
            counter <= counter + 1;
        end
    end
    num2led num2led(showing_num, digit_num, showing_digit);
endmodule

module num2led(num, led, showing_digit);
    input [3:0] num;
    input [3:0] showing_digit;
    output [7:0] led;
    // 10为[，11为二，12为],13为U，14为-
    assign led[0] = (num == 4'b0000 || num == 4'b0001 || num == 4'b1110
                        || num == 4'b0111 || num == 4'b1010 || num == 4'b1011 
                        || num == 4'b1100 || num == 4'b1101) ? 0 : 1;
    assign led[1] = (num == 4'b0001 || num == 4'b0010 || num == 4'b0011
                        || num == 4'b0111 || num == 4'b1011 || num == 4'b1100 
                        || num == 4'b1110) ? 0 : 1;
    assign led[2] = (num == 4'b0001 || num == 4'b0011 || num == 4'b0100
                        || num == 4'b0101 || num == 4'b0111 || num == 4'b1001
                        || num == 4'b1011 || num == 4'b1100 || num == 4'b1110) ? 0 : 1;
    assign led[3] = (num == 4'b0001 || num == 4'b0100 || num == 4'b0111
                        || num == 4'b1110) ? 0 : 1;
    assign led[4] = (num == 4'b0010 || num == 4'b1010 || num == 4'b1011 
                        || num == 4'b1110) ? 0 : 1;
    assign led[5] = (num == 4'b0101 || num == 4'b0110 || num == 4'b1010
                        || num == 4'b1011 || num == 4'b1110) ? 0 : 1;
    assign led[6] = (num == 4'b0001 || num == 4'b0100
                        || num == 4'b1101) ? 0 : 1;
    assign led[7] = showing_digit == 4'b0100;                   
endmodule
