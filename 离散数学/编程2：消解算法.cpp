#include<iostream> 
#include<string> 
#include<vector> 
#include<algorithm> 
using namespace std; 
 
class digestion{ 
public: 
    digestion(string s){ 
        this->s=s; 
    } 
    void solve(){ 
        init(); 
        bool ans=calc(); 
        if(ans==true) 
            cout<<"YES"<<endl; 
        else 
            cout<<"NO"<<endl; 
    } 
private: 
    string s; 
    vector<string> S0; 
    vector<string> S1; 
    vector<string> S2; 
    void init(); 
    bool calc(); 
    bool Res(string s1,string s2,string &res); 
}; 
 
void digestion::init(){ 
    int len=s.length(); 
    string temp; 
    for(int i=0;i<len;++i){ 
        if(s[i]=='('||s[i]==')'||s[i]=='&'){ 
            if(temp.size()){ 
                S1.push_back(temp); 
                temp.clear(); 
            } 
        } 
        else{ 
            temp.push_back(s[i]); 
        } 
    } 
    if(temp.size()){ 
        S1.push_back(temp); 
        temp.clear(); 
    } 
} 
 
bool digestion::calc(){ 
    do{ 
        for(int i=0;i<S0.size();++i){ 
            for(int j=0;j<S1.size();++j){ 
                string C; 
                if(Res(S0[i],S1[j],C)){ 
                    if(C.empty()) return false; 
                    if(C.size()&&find(S0.begin(),S0.end(),C)==S0.end()&&find(S1.begin(),S1.end(),C)==S1.end()){ 
                        S2.push_back(C); 
                    } 
                } 
            } 
        } 
        for(int i=0;i<S1.size();++i){ 
            for(int j=i+1;j<S1.size();++j){ 
                string C; 
                if(Res(S1[i],S1[j],C)){ 
                    if(C.empty()) return false; 
                    if(C.size()&&find(S0.begin(),S0.end(),C)==S0.end()&&find(S1.begin(),S1.end(),C)==S1.end()){ 
                        S2.push_back(C); 
                    } 
                } 
            } 
        } 
        if(S2.empty()){ 
            return true; 
        } 
        else{ 
            for(int i=0;i<S1.size();++i){ 
                S0.push_back(S1[i]); 
            } 
            S1=S2; 
            S2.clear(); 
        } 
    }while(true); 
} 
 
bool digestion::Res(string s1,string s2,string &res){ 
    int len=s1.length(); 
    string temp; 
    vector<string> s1_;//存放s1的文字 
    for(int i=0;i<len;++i){ 
        if(s1[i]=='|'){ 
            if(temp.size()){ 
                s1_.push_back(temp); 
                temp.clear(); 
            } 
        } 
        else{ 
            temp.push_back(s1[i]); 
        } 
    } 
    if(temp.size()){ 
        s1_.push_back(temp); 
        temp.clear(); 
    } 
    len=s2.length(); 
    vector<string> s2_;//存放s2的文字 
    for(int i=0;i<len;++i){ 
        if(s2[i]=='|'){ 
            if(temp.size()){ 
                s2_.push_back(temp); 
                temp.clear(); 
            } 
        } 
        else{ 
            temp.push_back(s2[i]); 
        } 
    } 
    if(temp.size()){ 
        s2_.push_back(temp); 
        temp.clear(); 
    } 
    int flagFirst=1; 
    for(int i=0;i<s1_.size();++i){ 
        for(int j=0;j<s2_.size();++j){ 
            if(s1_[i]=="!"+s2_[j]||"!"+s1_[i]==s2_[j]){ 
                for(int i_=0;i_<s1_.size();++i_){ 
                    if(i_==i) continue; 
                    if(flagFirst==1){ 
                        res=s1_[i_]; 
                        flagFirst=0; 
                    } 
                    else res=res+"|"+s1_[i_]; 
                } 
                for(int j_=0;j_<s2_.size();++j_){
                    //s2_[j_]!=s1_[i]可以与第一个式子中被消去的文字相同
                    if(s2_[j_]!=s1_[i]&&(j_==j||find(s1_.begin(),s1_.end(),s2_[j_])!=s1_.end())) continue; 
                    if(flagFirst==1){ 
                        res=s2_[j_]; 
                        flagFirst=0; 
                    } 
                    else res=res+"|"+s2_[j_]; 
                } 
                return true; 
            } 
        } 
    } 
    return false; 
} 
 
int main(){ 
    string s; 
    cin>>s; 
    digestion obj(s); 
    obj.solve(); 
    system("pause"); 
    return 0; 
} 