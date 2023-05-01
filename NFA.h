//
// Created by Pim Van den Bosch on 2023/05/01.
//

#ifndef CODE_NFA_H
#define CODE_NFA_H

#include <unordered_map>
#include <set>
#include "json.hpp"
#include "DFA.h"

using namespace std;
using json = nlohmann::json;

class NFA {
public:
    NFA(const string& input);

    // Function to convert NFA to DFA
    DFA toDFA();

private:
    void constructFromJson(const json& input);
    set<string> transition(const string& current_state, const string& input);


    // Helper functions
    bool isAcceptingState(const set<string>& nfa_states);
    string mergeStateNames(const std::set<std::string>& states);

    // Member variables
    json json_nfa;
    string type;
    vector<string> alphabet;
    json states;
    unordered_map<string, unordered_map<string, set<string>>> transitions;
    unordered_set<string> accepting_states;
    string start_state; // Added this line
};

#endif //CODE_NFA_H

