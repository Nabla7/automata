//
// Created by Pim Van den Bosch on 2023/03/06.
//

#include "DFA.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;
using json = nlohmann::json;


DFA::DFA(const std::string& input) {
    ifstream json_file(input);

    json_file >> json_dfa;

    states = json_dfa["states"];
    for (json state : states) {
        if (state["starting"] == true){
            start_state = state["name"];
        }
        if (state["accepting"] == true){
            string name = state["name"];
            accepting_states.insert(name);
        }
    }

    transitions = json_dfa["transitions"];
}

int DFA::print(){
    cout << setw(4) << json_dfa << endl;
    return 0;
}

string DFA::transition(const string current_state, const string input){
    string output;
     for (json triple : transitions){
         string from = triple["from"];
         string input_ = triple["input"];

         if (from == current_state &&
             input_ == input){
                output = triple["to"];
            }
     }
    return output;
}

bool DFA::accepts(const string& input) {
    string current_state = start_state;

    for (char input_symbol : input) {
        string input_(1, input_symbol);
        current_state = transition(current_state, input_);
    }
    return accepting_states.count(current_state) > 0;
}
