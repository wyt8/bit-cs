`timescale 1ns / 1ps

module counter_test(

    );
    reg clk,reset,run,stop;
    
    wire timeout;
    wire min;
    wire [7:0] sec;
    wire [7:0] milisec;
    wire state;
    parameter period = 4;
    counter counter
    (
        .clk        (clk),
        .reset      (reset),
        .run        (run),
        .stop       (stop),
        .timeout    (timeout),
        .min        (min),
        .sec        (sec),
        .milisec    (milisec),
        .s(state)
    );
    
    always begin
        clk = 1'b0;
        #(period/2);
        clk = 1'b1;
        #(period/2);
    end
    
    initial begin
//        reset = 1'b0;
        run = 1'b0;
        stop = 1'b0;
        #(period/2);
        
//        reset = 1'b1;
//        run = 1'b1;
//        #(100*period);
//        run = 1'b0;
//        stop = 1'b1;
//        #(100*period);
//        stop = 1'b0;
//        #(10000*period);
//        run = 1'b1;
//        #(100*period);
//        run = 1'b0;
//        #(150000*period);
//            reset = 1'b1;
            # 10;
            run = 1'b1;
            # 10;
            run = 1'b0;
    end
endmodule
