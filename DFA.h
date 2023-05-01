//
// Created by Pim Van den Bosch on 2023/03/06.
//

#ifndef CODE_DFA_H
#define CODE_DFA_H

#include <unordered_set>
#include <string>
#include "json.hpp"
#include <set>

using namespace std;
using json = nlohmann::json;

class DFA {
public:

    DFA();

    DFA(const string& input);

    //DFA(const json& input);

    void constructFromJson(const json& input);

    DFA(const DFA& dfa1, const DFA& dfa2, bool intersection);

    bool accepts(const string& input);

    string transition(const string current_state,
                      const string input);

    void print() const;

    bool printTable();



    string to_dot() const;

    DFA minimize();

    string mergeStateNames(const std::unordered_set<std::string>& partition);

    json json_dfa;
    string type;
    std::vector<string> alphabet;
    json states;
    json transitions;
    string start_state;
    unordered_set<string> accepting_states;


    string getStateName(const std::unordered_set<std::string>& states);
    DFA productAutomaton(DFA& other);
    bool operator==(DFA& other);
    bool isAccepting(const std::string& stateName);

};

#endif //CODE_DFA_H
