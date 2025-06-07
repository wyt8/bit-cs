`timescale 1ns / 1ps

module sqrt(i, o);
    input [3:0] i; // 4位BCD码作为输入
    output [2:0] o; // 3位BCD码作为输出
    
    // 将输入的4位赋给A、B、C、D；将X、Y、Z赋给输出的3位
    wire A, B, C, D, X, Y, Z, T1;
    assign A = i[3], B = i[2], C = i[1], D = i[0];
    assign o[2] = X, o[1] = Y, o[0] = Z;
    
    // A_, B_, C_, D_, T1_分别是A, B, C, D, T1取反
    wire A_, B_, C_, D_, T1_;
    not notA(A_, A), notB(B_, B), notC(C_, C), notD(D_, D), notT1(T1_, T1);
    // T1 = C + D
    or orT1(T1, C, D);
    // X = A·B·T1
    and andX(X, A, B, T1);

    // T2 = A'·B
    // T3 = A·B'
    // T4 = C·D
    // T5 = A·T1'
    wire T2, T3, T4, A_C, C_D, CD_;
    and andT2(T2, A_, B);
    and andT3(T3, A, B_);
    and andT4(T4, C, D);
    and andT5(T5, A, T1_);
    and CandD_(CD_, C, D_);
    and C_andD(C_D, C_, D);
    
    wire A_T4;
    and A_andT4(A_T4, A_, T4);
    // Y = T2+ T3 + A'·T4 + T5
    or orY(Y, T2, T3, A_T4, T5);
    
    // t1 = C·D' + C'·D
    wire T2T4, t1, B_t1;
    or ort1(t1, CD_, C_D);
    and B_andT(B_t1, t1, B_);
    and T2andT4(T2T4, T2, T4);
    // Z = T2·T4+ T5+ B'·t1 + T3
    or orZ(Z, T2T4, T5, B_t1, T3);
    
endmodule
