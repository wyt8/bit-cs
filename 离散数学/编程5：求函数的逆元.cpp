#include<iostream>
using namespace std;
// 设A={x|x属于R, x不等于0,1}，在A上定义6个函数，
// f1(x)=x,  f2(x)=x^-1, f3(x)=1-x,
// f4(x)=(1-x)^-1, f5(x)=(x-1)x^-1, f6(x)= x(x-1)^-1，
// *运算为函数的复合运算， 求函数的逆元。
//首先明确元素是函数，运算符为函数复合；其次明确单位元为恒等函数，即为f1
//然后给出运算表
//    f1 f2 f3 f4 f5 f6
// f1 f1 f2 f3 f4 f5 f6 
// f2 f2 f1 f4 f3 f6 f5
// f3 f3 f5 f1 f6 f2 f4
// f4 f4 f6 f2 f5 f1 f3
// f5 f5 f3 f6 f1 f4 f2
// f6 f6 f4 f5 f2 f3 f1
int main(){
    //res中存储运算表
    int res[6][6]={{1,2,3,4,5,6},  
                 {2,1,4,3,6,5},  
                 {3,5,1,6,2,4},  
                 {4,6,2,5,1,3},  
                 {5,3,6,1,4,2},  
                 {6,4,5,2,3,1}}; 
    char c;
    int n;
    cin>>c>>n;
    for(int i=0;i<6;i++){
        if(res[n-1][i]==1){//找到了右逆元，判断其是否为左逆元
            if(res[i][n-1]==1){
                cout<<"f"<<i+1<<endl;
                system("pause");
                return 0;
            }
        }
    }
    cout<<"no result!"<<endl;
    system("pause");
    return 0;
}