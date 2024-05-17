#include <bits/stdc++.h>
using namespace std;
typedef int ll;


// custom funtion for sorting
// swap occurs if true is returned
bool swapCmp(int &right, int &left){
    cout << left << " " << right << "\n";
    return left > right;
}

// for priority queue
// swap occurs if true is returned
class swapCmp{
public:
    bool operator()(int &up, int &down){
        return down < up;
    }
};

// last element is the top of the heap
// so up is right and down is left
class swapCmp{
public:
    bool operator()(int &right, int &left){
        return left < right;
    }
};



// comparing the element with target in lower_bound
bool greatCmp(pair<ll,ll> elem, pair<ll,ll> target){
    return target.second > elem.second;
}