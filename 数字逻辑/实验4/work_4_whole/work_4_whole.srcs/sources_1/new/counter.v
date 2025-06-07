`timescale 1ns / 1ps
module counter(
    input clk,                //ʱ��
    input reset,              //��λ
    input run,                //��ʼ
    input stop,               //ֹͣ
    output reg timeout,       //�Ƿ�ʱ
    output reg min,           //��
    output reg [7:0] sec,     //��
    output reg [7:0] milisec,  //���� 
    output [1:0] s
    );
    //�ֱ�Ϊ��ͬ��״̬����ͬ��ֵ
    parameter state_initial =  2'b00 ;
    parameter state_run     =  2'b01 ;
    parameter state_stop    =  2'b10 ;
    parameter state_timeout =  2'b11 ;
    //����
    reg [1:0] state = state_initial, next_state = state_initial;
   assign  s =state;
    //�Ƿ�λ
    always @ (posedge clk or negedge reset)
    begin
        //��λ��initial
        if(!reset) begin
            state <= state_initial;
        end
        //����λ
        else begin
            state <= next_state;
        end
    end
    
    //��ʱ10ms
    //10ms��ʱ�Ӵ���
    parameter ms_10 = 1000000; 
    //��¼ʱ�Ӵ���
    reg [31:0] count = 0;
    //��¼�Ƿ���10ms
    wire flag;
    assign flag = count == ms_10;
    
    //count����
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
    
    //״̬ת��
    always @ (*)
    begin
        case(state)
            state_initial : begin
                //Ĭ��״̬ת�Ƶ���ʱ״̬
                if(run == 1'b1) begin
                    next_state = state_run;
                end
                //Ĭ��״̬����
                else begin
                    next_state = state_initial;
                end
            end
        
            state_run : begin
                //ת�Ƶ���ͣ״̬
                if(stop == 1'b1) begin
                    next_state = state_stop;
                end
                //ת�Ƶ���ʱ״̬
                else if (min == 1'b1 && sec == 'h59 && milisec == 'h99 && flag ) begin
                    next_state = state_timeout;
                end
                //��ʱ״̬����
                else begin
                    next_state = state_run;
                end
            end
        
            state_stop : begin
                //ת�Ƶ���ʱ״̬
                if(run == 1'b1)
                    next_state = state_run;
                //��ͣ״̬����
                else 
                    next_state = state_stop;
            end
        
            state_timeout : begin
                //һֱ����timeout״̬��ֱ����reset����ʱnext_state�Զ�תΪinitial��
                next_state = state_timeout;
            end
        endcase
    end
    
    //���봦��
    always @ (posedge clk or negedge reset) begin
        if(!reset) begin
            milisec <= 'b0;
        end
        else if(flag) begin
            //990ms��0
            if(milisec == 'h99) begin
                milisec <= 'h00;
            end
            //��λΪ9��λ
            else if(milisec[3:0] == 'h9)begin
                milisec[3:0] <= 'h0;
                milisec[7:4]<= milisec[7:4] + 1'b1;
            end
            //��λ��1
            else begin
                milisec[3:0] <= milisec[3:0] + 1'b1;
            end
        end
    end
    
    //�봦��
    always @ (posedge clk or negedge reset) begin
        if(!reset) begin
            sec <='b0;
        end
        //1000msʱsecond��1
        else if (flag && milisec == 'h99) begin
            //60s��0
            if(sec == 'h59)begin
                sec <= 'h00;
            end
            //��λ10s��1
            else if (sec[3:0]=='h9) begin
                sec[3:0] <='b0;
                sec[7:4] <= sec[7:4]+1'b1;
            end
            //��λ��1
            else begin
                sec[3:0] <= sec[3:0] + 1'b1;
            end
        end
        else begin
            sec <= sec;
        end
    end
    
    //�ִ���
    always @ (posedge clk or negedge reset) begin
        if(!reset) begin
            min <= 1'b0;
        end
        //60s
        else if(flag && sec == 'h59 && milisec=='h99) begin
            //minΪ0
            if(min == 1'b0) begin
                min <= 1'b1;
            end
            //minΪ1��timeout
            else begin
                min <= min; 
            end
        end
    end
    
endmodule

