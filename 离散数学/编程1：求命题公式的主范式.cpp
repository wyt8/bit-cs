#include<iostream>
#include<string>
#include<stack>
#include<algorithm>
#include<vector>
#include<cmath>
using namespace std;

class expression{
public:
    expression(string s){
        this->s=s;
    }
    int ans(){
        for(int i=0;i<s.length();++i){
            if(isopt(s[i])||s[i]=='('||s[i]==')'){
                if(s[i]=='(') opt.push('(');
                else if(s[i]==')'){
                    while(opt.top()!='('){
                        char temp=opt.top();
                        opt.pop();
                        if(temp=='!'){
                            int a=num.top();
                            num.pop();
                            num.push(calc(a,-1,temp));
                        }
                        else{
                            int a=num.top();
                            num.pop();
                            int b=num.top();
                            num.pop();
                            num.push(calc(b,a,temp));
                        }
                    }
                    opt.pop();
                } 
                else if(opt.empty()||priority(opt.top())<priority(s[i])){
                    opt.push(s[i]);
                }
                else if(priority(opt.top())>=priority(s[i])){
                    char temp=opt.top();
                    opt.pop();
                    if(temp=='!'){
                        int a=num.top();
                        num.pop();
                        num.push(calc(a,-1,temp));
                    }
                    else{
                        int a=num.top();
                        num.pop();
                        int b=num.top();
                        num.pop();
                        num.push(calc(a,b,temp));
                    }
                    opt.push(s[i]);
                }
            }
            else{
                num.push(s[i]-48);
            }
        }
        while(!opt.empty()){
            char temp=opt.top();
            opt.pop();
            if(temp=='!'){
                int a=num.top();
                num.pop();
                num.push(calc(a,-1,temp));
            }
            else{
                int a=num.top();
                num.pop();
                int b=num.top();
                num.pop();
                num.push(calc(b,a,temp));
            }
        }
        return num.top();
    }
private:
    string s;
    stack<char> opt;
    stack<int> num;
    bool isopt(char c){
        if(c=='!'||c=='&'||c=='|'||c=='-'||c=='+') return true;
        else return false;
    }
    int priority(char c){
        if(c=='!') return 6;
        else if(c==')') return 5;
        else if(c=='&') return 4;
        else if(c=='|') return 3;
        else if(c=='-') return 2;
        else if(c=='+') return 1;
        else if(c=='(') return 0;
    }
    int calc(int a,int b,char c){
        switch(c){
            case '&':
                if(a==1&&b==1) return 1;
                else return 0;
                break;
            case '|':
                if(a==0&&b==0) return 0;
                else return 1;
                break;
            case '-':
                if(a==1&&b==0) return 0;
                else return 1;
                break;
            case '+':
                if(a==1&&b==0||a==0&&b==1) return 0;
                else return 1;
                break;
            case '!':
                if(a==0) return 1;
                else return 0;
                break;
        }
    }
};

class principalNormalForm{
public:
    principalNormalForm(string s){
        this->s=s;
        numVariables=0;
        numTrueValue=0;
    }
    void solve(){
        countVariables();
        DFS(0);
        if(numTrueValue==0) cout<<"0";
        else 
        for(int i=0;i<numTrueValue;++i){
            cout<<"m"<<trueValue[i];
            if(i!=numTrueValue-1) cout<<" ∨ ";
        }
        cout<<" ; ";
        int numFalseValue=pow(2,numVariables)-numTrueValue;
        if(numFalseValue==0) cout<<"1";
        else
        for(int i=0;i<pow(2,numVariables);++i){
            int j;
            for(j=0;j<numTrueValue;j++){
                if(i==trueValue[j]) break;
            }
            if(j!=numTrueValue) continue;
            cout<<"M"<<i;
            if(numFalseValue>1) cout<<" ∧ ";
            numFalseValue--;
        }
        cout<<endl;
    }
private:
    string s;
    int numVariables;
    vector<string> variables;
    int tempTrueTable[15];
    int trueValue[1000];
    int numTrueValue;
    bool isopt(char c){
        if(c=='!'||c=='&'||c=='|'||c=='-'||c=='+')
            return true;
        else return false;
    }
    void countVariables();
    void DFS(int n);
};

void principalNormalForm::countVariables(){
    int len=s.length();
    string temp;
    int start=0,end=0;
    for(int i=0;i<len;++i){
        if(s[i]=='('||s[i]==')'||isopt(s[i])){
            if(temp.size()&&find(variables.begin(),variables.end(),temp)==variables.end()){
                variables.push_back(temp);
            }
            temp.clear();
            continue;
        }
        else{
            temp.push_back(s[i]);
        }
    }
    if(temp.size()&&find(variables.begin(),variables.end(),temp)==variables.end()){
        variables.push_back(temp);
    }
    temp.clear();
    sort(variables.begin(),variables.end());
    numVariables=variables.size();
}

void principalNormalForm::DFS(int n){
    if(n==numVariables){
        expression a(s);
        if(a.ans()==1){
            int value=0;
            for(int j=0;j<numVariables;j++){
                value=2*value+tempTrueTable[j];
            }
            trueValue[numTrueValue]=value;
            numTrueValue++;
        }
    }
    else{
        string temp=s;
        int pos;
        pos=s.find(variables[n]);
        while(pos!=-1){
            s.replace(pos,variables[n].length(),"0");
            pos=s.find(variables[n]);
        }
        tempTrueTable[n]=0;
        principalNormalForm::DFS(n+1);
        s=temp;
        pos=s.find(variables[n]);
        while(pos!=-1){
            s.replace(pos,variables[n].length(),"1");
            pos=s.find(variables[n]);
        }
        tempTrueTable[n]=1;
        principalNormalForm::DFS(n+1);
        s=temp;
    }
}

int main(){
    string s;
    cin>>s;
    principalNormalForm obj(s);
    obj.solve();
    system("pause");
    return 0;
}