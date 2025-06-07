#include<iostream>
#include<cstdio>
#include<cstring>
#include<cmath>
using namespace std;


int main(){
    int M[100][100];
    int n=0,a,temp[1000],i=0;
    while(scanf("%d",&a)!=EOF){
        temp[i++]=a;
    }
    n=sqrt(i);
    for(int i=0;i<n;++i){
        for(int j=0;j<n;j++){
            M[i][j]=temp[i*n+j];
        }
    }
    for(int k=0;k<n;k++){
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                M[i][j]=M[i][j]+M[i][k]*M[k][j];
            }
        }
    }
    for(int i=0;i<n;i++){
        if(M[i][0]!=0){
                printf("1");
            }
            else printf("0");
        for(int j=1;j<n;j++){
            if(M[i][j]!=0){
                printf(" 1");
            }
            else printf(" 0");
        }
        printf("\n");
    }
    system("pause");
    return 0;
}