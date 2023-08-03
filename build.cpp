#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <sstream>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <unordered_set>

using namespace std;
using std::ifstream;

struct state {
    bool accept;
    map<char, vector<state> > transitions;
    bool isFirst;
    int index;
};

struct NFA {
    vector<state> states;
};

int curIdx = 0;
string input;
stack<NFA> nfaStack;
queue<char> regexQ;

int precedence(char op) {
    if (op == '|')
        return 1;
    if (op == '.')
        return 2;
    if (op == '*')
        return 3;
    return 0;
}

void regexToPostfix() {
    string output = "";
    int i = 0;
    stack<char> s;

    while (input[i] != '\0') {
        if (input[i] >= 'a' && input[i] <= 'z'|| input[i] >= '0'&& input[i] <= '9') {
            output += input[i];
            i++;
        }
        else if (input[i] == '(') {
            if (input[i + 1] == ')') {
                output += '#';
                i += 2;
            } else {
                s.push(input[i]);
                i++;
            }   
        }
        else if (input[i] == ')') {
            while(s.top() != '(') {
                output += s.top();
                s.pop();
            }
            s.pop();
            i++;
        }
        else {
            while (!s.empty() && precedence(input[i]) <= precedence(s.top())) {
                output += s.top();
                s.pop();
            }
            s.push(input[i]);
            i++;
        }
    }
    while (!s.empty()) {
        output += s.top();
        s.pop();
    }

    input = output;
    for (int i = 0; i < input.length(); i++) {
        regexQ.push(input[i]);
    }
}

void addConcat() {
    string output = "";
    int i = 0;

    while (input[i] != '\0') {
        output += input[i];
        if (input[i] != '(' && input[i] != '|' && (i + 1 < input.length() && input[i + 1] != '*')
                    && (i + 1 < input.length() && input[i + 1] != ')') && (i + 1 < input.length() && input[i + 1] != '|')) {
            output += '.';
        }
        i++;
    }
    input = output;
}

void handleSymbol(char c) {
    NFA nfa;
    state st1;
    st1.accept = false;
    st1.isFirst = true;
    st1.index = 0;

    state st2;
    st2.accept = true;
    st2.isFirst = false;
    st2.index = 1;
  
    st1.transitions[c].push_back(st2);
    
    nfa.states.push_back(st1); 
    nfa.states.push_back(st2);
    nfaStack.push(nfa);
}

void handleUnion() {
    NFA nfa;
     
    NFA nfa2 = nfaStack.top();
    nfaStack.pop();
    NFA nfa1 = nfaStack.top();
    nfaStack.pop();
    int indx = 0;
    for (int i = 0; i < nfa1.states.size(); i++) {
        if (nfa1.states[i].isFirst) {
            indx = i;
            break;
        }
    }
    for (int j = 0; j < nfa2.states.size(); j++) {
        nfa2.states[j].index += nfa1.states.size() - 1;
        if (nfa2.states[j].isFirst) {
            if (nfa2.states[j].accept) {
                nfa1.states[indx].accept = true;
            }
            for (auto pair : nfa2.states[j].transitions) {
                for (int k = 0; k < pair.second.size(); k++) {
                    pair.second[k].index += nfa1.states.size() - 1;
                    nfa1.states[indx].transitions[pair.first].push_back(pair.second[k]);
                }
            }
        } else {
            for (auto pair : nfa2.states[j].transitions) {
                vector<state> toStates = pair.second;
                nfa2.states[j].transitions[pair.first].clear();
                for (int k = 0; k < pair.second.size(); k++) {
                    toStates[k].index += nfa1.states.size() - 1;
                    nfa2.states[j].transitions[pair.first].push_back(toStates[k]);
                }
            }
        }
    }

    for (int i = 0; i < nfa1.states.size(); i++) {
        nfa.states.push_back(nfa1.states[i]);
    }
    for (int j = 0; j < nfa2.states.size(); j++) {
        if (!nfa2.states[j].isFirst) nfa.states.push_back(nfa2.states[j]);
    }
    nfaStack.push(nfa);

}

void handleStar() {
    NFA nfa;
    NFA nfa1 = nfaStack.top();
    nfaStack.pop();
    int indx = 0;
    for (int i = 0; i < nfa1.states.size(); i++) {
        if (nfa1.states[i].isFirst) {
            indx = i;
            break;
        }
    }
    
   for (int j = 0; j < nfa1.states.size(); j++) {
        if (nfa1.states[j].accept) {
            for (auto pair : nfa1.states[indx].transitions) {
                for (int k = 0; k < pair.second.size(); k++) {
                    nfa1.states[j].transitions[pair.first].push_back(pair.second[k]);
                }
            }
        }
        nfa.states.push_back(nfa1.states[j]);
    }
    nfa.states[indx].accept = true;
     
    nfaStack.push(nfa);

}

void handleConcat() {
    NFA nfa;
    
    NFA nfa2 = nfaStack.top();
    nfaStack.pop();
    NFA nfa1 = nfaStack.top();
    nfaStack.pop();
    vector<int> firstaccs;
    for (int i = 0; i < nfa1.states.size(); i++) {
        if (nfa1.states[i].accept) {
            nfa1.states[i].accept = false;
            firstaccs.push_back(i);
        } 
    }

    for (int j = 0; j < nfa2.states.size(); j++) {
        nfa2.states[j].index += nfa1.states.size() - 1;
        if (nfa2.states[j].isFirst) {
            if (nfa2.states[j].accept) {
                for (int i = 0; i < firstaccs.size(); i++) nfa1.states[firstaccs[i]].accept = true;
            }
            for (auto pair : nfa2.states[j].transitions) {
                vector<state> toStates = pair.second;
                for (int k = 0; k < pair.second.size(); k++) {
                    toStates[k].index += nfa1.states.size() - 1;
                    for (int i = 0; i < firstaccs.size(); i++) nfa1.states[firstaccs[i]].transitions[pair.first].push_back(toStates[k]);
                }
            }
        } else {
            for (auto pair : nfa2.states[j].transitions) {
                vector<state> toStates = pair.second;
                nfa2.states[j].transitions[pair.first].clear();
                for (int k = 0; k < pair.second.size(); k++) {
                    toStates[k].index += nfa1.states.size() - 1;
                    nfa2.states[j].transitions[pair.first].push_back(toStates[k]);
                }
            }
        }
    }

    for(int i=0; i<nfa1.states.size(); i++) nfa.states.push_back(nfa1.states[i]);
    for (int j = 0; j < nfa2.states.size(); j++) {
        if (!nfa2.states[j].isFirst)
            nfa.states.push_back(nfa2.states[j]);
    }
    nfaStack.push(nfa);
    
}

void handleEpsilon() {
    NFA nfa;
    state st;
    st.accept = true;
    st.isFirst = true;
    st.index = 0;
    nfa.states.push_back(st);
    nfaStack.push(nfa);
}

void printResults() {
    int n = nfaStack.top().states.size();
    int a = 0;
    int t = 0;
    vector<string> arrows(n);
    vector<int> accepts;
    for (int i = 0; i < nfaStack.top().states.size(); i++) {
        if (nfaStack.top().states[i].accept) {
            a++;
            accepts.push_back(nfaStack.top().states[i].index);
        }
        string trans = "";
        int numTrans = 0;
        for (auto pair : nfaStack.top().states[i].transitions) {
            numTrans += pair.second.size();
            for (int k = 0; k < pair.second.size(); k++) {
                string ch(1, pair.first);
                trans += ch + " " + to_string(pair.second[k].index) + " ";
                t++;
            }
        }
        arrows[i] = to_string(numTrans) + " " + trans;
    }
    cout << n << " " << a << " " << t << endl;
    for (int i = 0; i < accepts.size(); i++) cout << accepts[i] << " ";
    cout << endl;
    for (int i = 0; i < arrows.size(); i++) {
        cout << arrows[i] << endl;
    }
}

void build() {
    addConcat();
    regexToPostfix();
    while (!regexQ.empty()) {
        char c = regexQ.front();
        regexQ.pop();
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
            handleSymbol(c);
        }
        else if (c == '#') {
            handleEpsilon();
        }
        else if (c == '|') {
            handleUnion();
        }
        else if (c == '*') {
            handleStar();
        }
        else if (c == '.') {
            handleConcat();
        }
    }
    printResults();
    
}


int main() {
    cin >> input;
    build();
    return 0;
}
