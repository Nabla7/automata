//
// Created by Pim Van den Bosch on 2023/03/06.
//

#ifndef CODE_DFA_H
#define CODE_DFA_H

#include <unordered_set>
#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class DFA {
public:
    DFA(const string& input);
    bool accepts(const string& input);
    string transition(const string current_state,
                      const string input);
    int print();

private:
    json json_dfa;
    json states;
    unordered_set<string> accepting_states;
    json transitions;
    string start_state;
};

#endif //CODE_DFA_H
