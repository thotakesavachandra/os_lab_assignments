#include<bits/stdc++.h>
using namespace std;
typedef long long ll;

#define INF 1e9

const int mod = 1e9+7;

int dp[201][201][201];

// i = number of 1s
// j = number of 2s
// k = number of 3s

int fillDp(int i, int j, int k){
	if(i<0 || j<0 || k<0) return -INF;

	if(dp[i][j][k] != -1) return dp[i][j][k];

	if(i==0 && j==0 && k==0){
		dp[i][j][k] = 0;
		return 0;
	}
	
	dp[i][j][k] = 0;

	int v1 = fillDp(i-1, j, k);
	int v2 = fillDp(i, j-1, k);
	int v3 = fillDp(i, j, k-1);

	if( (i+k)%2==0 && (j+k)%2==0 ) dp[i][j][k] = 1;

	int val = max(v1,max(v2,v3));
	dp[i][j][k] += max(val,0);

	return dp[i][j][k];
}

void solve(){
	int n = 200;
	for(int i=0;i<=n;i++){
		for(int j=0;j<=n;j++){
			for(int k=0;k<=n;k++) dp[i][j][k] = -1;
		}
	}

	int t;
	cin >> t;
	while(t--){
		int i,j,k,l;
		cin >> i >> j >> k >> l;
		int ans = l/2;
		ans += fillDp(i,j,k);
		cout << ans << "\n";
	}
}


int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(0);cout.tie(0);
    int ncases=1;
    // cin >> ncases;
    while(ncases--){
        solve();
    }
}
