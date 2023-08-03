#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <sstream>
#include <vector>
#include <stack>
#include <unordered_set>

using namespace std;
using std::ifstream;

struct state {
    bool accept;
    map<string, vector<int> > transitions;
};

vector<vector<string> > input;
vector<state> states;
vector<string> outputs;

void storeVals() {
    for (int i = 3; i < input.size(); i++) {
        state st;
        st.accept = false;
        
        for (int j = 2; j < input[i].size(); j += 2) {
            int num = stoi(input[i][j]);
            if (st.transitions.find(input[i][j - 1]) != st.transitions.end()) {
                st.transitions[input[i][j - 1]].push_back(num);
            } else {
                vector<int> toState;
                toState.push_back(num);
                st.transitions[input[i][j - 1]] = toState;
            }
        }
        states.push_back(st);
    }
    
    for (int i = 0; i < input[2].size(); i++) {
        states[stoi(input[2][i])].accept = true;
    }
}

void helper(string str, string curAnsw, int curState) {
    if (str.length() == 0) {
        outputs.push_back(curAnsw);
        return;
    }
    string c(1, str[0]);   
    if (states[curState].transitions.find(c) != states[curState].transitions.end()) {
        for (int i = 0; i < states[curState].transitions[c].size(); i++) { 
            if (states[states[curState].transitions[c][i]].accept) {
                curAnsw += 'Y';
            } else {
                curAnsw += 'N';
            }
            if (str.length() > 1) {
                helper(str.substr(1, str.length() - 1), curAnsw, states[curState].transitions[c][i]);
            } else {
                helper("", curAnsw, states[curState].transitions[c][i]);
            }
            curAnsw = curAnsw.substr(0, curAnsw.length() - 1);
        }
    } else {
        for (int i = 0; i < str.length(); i++) {
            curAnsw += 'N';
        }
        helper("", curAnsw, 0);
    }
}

string run() {
    string res = "";
    storeVals();
    string str = input[0][0];
    helper(str, "", 0);
    
    bool yes = false;
    for (int i = 0; i < outputs[0].length(); i++) {
        for (int j = 0; j < outputs.size(); j++) {
            if (outputs[j][i] == 'Y') {
                yes = true;
                res += 'Y';
                break;
            }
        }
        if (!yes) res += 'N';
        yes = false;
    }

    // for (int j = 0; j < outputs.size(); j++) {
    //     cout << outputs[j] << endl;
    // }
    
    return res;
}

int main() {

    string line;
    getline(std::cin, line); 
    while (!line.empty()) { 
        vector<string> tokens;
        stringstream ss(line); 
        string intermediate;
        while(getline(ss, intermediate, ' ')) {
            tokens.push_back(intermediate);
        }
        input.push_back(tokens);
        std::getline(std::cin, line); 
    }

    if (input.size() > 0) cout << run() << endl;

    
    return 0;
}