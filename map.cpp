#include <unordered_map>
#include <iostream>
using namespace std;

int main() {
    unordered_map<string, int> freq;
    freq["apple"] = 5;
    freq["banana"] = 3;
    
    auto it = freq.find("apple");  // search for "apple"
    for (auto it = freq.begin(); it != freq.end(); it++) {
    cout << it->first << " " << it->second << endl;
    }

    return 0;
}