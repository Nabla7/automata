//
// Created by Pim Van den Bosch on 2023/03/06.
//

#include "DFA.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>


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

// New constructor for the product automaton
DFA::DFA(const DFA& dfa1, const DFA& dfa2, bool intersection) {
    // Initialize the product automaton states and transitions
    for (const auto& s1 : dfa1.states) {
        for (const auto& s2 : dfa2.states) {
            string product_state = s1["name"].get<string>() + s2["name"].get<string>();
            json product_state_json;
            product_state_json["name"] = product_state;
            product_state_json["starting"] = s1["starting"].get<bool>() && s2["starting"].get<bool>();
            product_state_json["accepting"] = intersection ? (s1["accepting"].get<bool>() && s2["accepting"].get<bool>()) : (s1["accepting"].get<bool>() || s2["accepting"].get<bool>());

            states.push_back(product_state_json);

            if (product_state_json["starting"]) {
                start_state = product_state;
            }

            if (product_state_json["accepting"]) {
                accepting_states.insert(product_state);
            }
        }
    }

    // Initialize the product automaton transitions
    for (const auto& t1 : dfa1.transitions) {
        for (const auto& t2 : dfa2.transitions) {
            if (t1["input"].get<string>() == t2["input"].get<string>()) {
                json product_transition_json;
                product_transition_json["from"] = t1["from"].get<string>() + "_" + t2["from"].get<string>();
                product_transition_json["input"] = t1["input"].get<string>();
                product_transition_json["to"] = t1["to"].get<string>() + "_" + t2["to"].get<string>();

                transitions.push_back(product_transition_json);
            }
        }
    }

    // Update the JSON object
    json_dfa["states"] = states;
    json_dfa["transitions"] = transitions;
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

std::string DFA::to_dot() const {
    std::ostringstream dot;

    dot << "digraph G {\n";
    dot << "    rankdir=LR;\n";
    dot << "    node [shape=circle];\n";
    dot << "    start [shape=none, label=\"\"];\n";

    for (const auto& state : states) {
        std::string name = state["name"].get<std::string>();
        if (state["accepting"].get<bool>()) {
            dot << "    " << name << " [shape=doublecircle];\n";
        }
    }

    for (const auto& transition : transitions) {
        std::string from = transition["from"].get<std::string>();
        std::string input = transition["input"].get<std::string>();
        std::string to = transition["to"].get<std::string>();

        // Check if the from/to state is a pair of states
        size_t underscore_pos = from.find("_");
        if (underscore_pos != std::string::npos) {
            // If it is a pair of states, split the name into two states
            std::string from1 = from.substr(0, underscore_pos);
            std::string from2 = from.substr(underscore_pos + 1);
            from = "(" + from1 + ", " + from2 + ")";
        }

        underscore_pos = to.find("_");
        if (underscore_pos != std::string::npos) {
            std::string to1 = to.substr(0, underscore_pos);
            std::string to2 = to.substr(underscore_pos + 1);
            to = "(" + to1 + ", " + to2 + ")";
        }

        dot << "    " << from << " -> " << to << " [label=\"" << input << "\"];\n";
    }

    dot << "    start -> " << start_state << ";\n";
    dot << "}\n";

    return dot.str();
}
