#include<iostream>
using namespace std;
// 设Z18为模18整数加群，求元素的阶。
//首先明确单位元为0，然后判断元素的几次方幂为单位元
int main(){
    int n;
    cin>>n;
    int sum=0;
    int j=0;//j表示节阶
    do{
        sum+=n;
        j++;
        if(sum>=18) sum%=18;
    }while(sum!=0);
    cout<<j<<endl;
    system("pause");
    return 0;
}