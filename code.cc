#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n;
    vector<char> symboles;
    cin >> n;
    while (symboles.size() < n) {
        char x;
        cin >> x;
        symboles.push_back(x);
    }

    return 0;
}