#include<iostream>
#include<cstdio>
#include<cstring>
#include<vector>
#include<algorithm>
using namespace std;



int main(){
    int eleNum[30][2],eleCount=0;
    char relatation[30][2];
    char s[100];
    gets(s);
    for(int i=0;i<strlen(s);i++){
        if(s[i]>='a'&&s[i]<='z'){
            eleNum[s[i]-'a'][0]=1;
            eleNum[s[i]-'a'][1]=0;
            eleCount++;
        }
    }
    int n=0;
    char a,b;
    while(scanf("<%c,%c>",&a,&b)!=EOF){
        getchar();
        relatation[n][0]=a;
        relatation[n][1]=b;
        eleNum[a-'a'][1]++;
        eleNum[b-'a'][1]++;
        n++;
    }
    vector<char> min;
    vector<char> max;
    for(int i=0;i<n;i++){
        int j;
        for(j=0;j<n;j++){
            if(relatation[i][0]==relatation[j][1]){
                break;
            }
        }
        if(j==n){
            min.push_back(relatation[i][0]);
        }
    }
    for(int i=0;i<n;i++){
        int j;
        for(j=0;j<n;j++){
            if(relatation[i][1]==relatation[j][0]){
                break;
            }
        }
        if(j==n){
            max.push_back(relatation[i][1]);
        }
    }
    for(int i=0;i<eleCount;i++){
        if(eleNum[i][0]==1&&eleNum[i][1]==0){
            min.push_back(i+'a');
            max.push_back(i+'a');
        }
    }
    auto minEnd=unique(min.begin(),min.end());
    auto maxEnd=unique(max.begin(),max.end());
    sort(min.begin(),minEnd);
    sort(max.begin(),maxEnd);
    int i;
    for(i=0;i<minEnd-min.begin()-1;i++){
        cout<<min[i]<<",";
    }
    cout<<min[i]<<endl;
    for(i=0;i<maxEnd-max.begin()-1;i++){
        cout<<max[i]<<",";
    }
    cout<<max[i]<<endl;
    system("pause");
    return 0;
}