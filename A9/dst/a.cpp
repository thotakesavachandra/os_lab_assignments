#include<bits/stdc++.h>
using namespace std;
typedef long long ll;

#define INF 1e9

const int mod = 1e9+7;



void solve(){
    int n;
    cin >> n;
    string input;
    cin >> input;

    for(int i=0;i<n;i++){
        if(input[i]=='1') input[i] = '0';
        else input[i] = '1';
    }

    const int mod = 2;

    for(int k=n; k>=1; k--){
        int sum = 0;
        int flag = 0;
        vector<int> nop(n,0);
        for(int i=0; i<n; i++){
            if(i>=k){
                sum -= nop[i-k];
                sum = (sum+mod)%mod;
            }
            if(i<=n-k){
                if(sum+'0' != input[i]){
                    sum = (sum+1)%mod;
                    nop[i] = 1;
                }
            }
            if(sum+'0' != input[i]){
                flag = 1;
                break;
            }
        }
        if(flag==0){
            cout << k << "\n";
            return;
        }
    }
}


int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(0);cout.tie(0);
    int ncases=1;
    cin >> ncases;
    while(ncases--){
        solve();
    }
}
