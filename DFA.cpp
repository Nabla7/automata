// Created by Pim Van den Bosch on 2023/03/06.

#include "DFA.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <queue>


using namespace std;
using json = nlohmann::json;

// Constructor for creating DFA from a json file
DFA::DFA(const std::string& input) {
    ifstream json_file(input);

    json_file >> json_dfa;

    type = json_dfa["type"];

    // Convert the JSON array to a vector of strings
    for (const auto& symbol : json_dfa["alphabet"]) {
        alphabet.push_back(symbol.get<std::string>());
    }

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
/*
 This code first calculates all possible product states and stores them in a map.
 It then iteratively adds reachable states to states and their corresponding
 transitions to transitions by processing the states in the order they become
 reachable, starting with the start state. This ensures that all and only reachable
 states and transitions are included in the product automaton.
 */

DFA::DFA(const DFA& dfa1, const DFA& dfa2, bool intersection) {
    // Create a map for reachable states
    map<string, json> product_states;
    map<string, bool> reachable;

    type = dfa1.type;
    alphabet = dfa1.alphabet;
    json_dfa["type"] = type;
    json_dfa["alphabet"] = alphabet;

    // Initialize the product automaton states and transitions
    for (const auto& s1 : dfa1.states) {
        for (const auto& s2 : dfa2.states) {
            string product_state = "(" + s1["name"].get<string>() + "," + s2["name"].get<string>() + ")";
            json product_state_json;
            product_state_json["name"] = product_state;
            product_state_json["starting"] = s1["starting"].get<bool>() && s2["starting"].get<bool>();
            product_state_json["accepting"] = intersection ? (s1["accepting"].get<bool>() && s2["accepting"].get<bool>()) : (s1["accepting"].get<bool>() || s2["accepting"].get<bool>());

            if (product_state_json["starting"]) {
                start_state = product_state;
                reachable[product_state] = true;
            }

            if (product_state_json["accepting"]) {
                accepting_states.insert(product_state);
            }

            // Save the state
            product_states[product_state] = product_state_json;
        }
    }

    // Initialize a queue with the start state
    std::queue<string> queue;
    queue.push(start_state);

    // Process states until the queue is empty
    while (!queue.empty()) {
        string current_state = queue.front();
        queue.pop();

        // Add the current state to states
        states.push_back(product_states[current_state]);

        // Add transitions from the current state and update reachable
        for (const auto& t1 : dfa1.transitions) {
            for (const auto& t2 : dfa2.transitions) {
                if (t1["input"].get<string>() == t2["input"].get<string>()) {
                    string from = "(" + t1["from"].get<string>() + "," + t2["from"].get<string>() + ")";
                    string to = "(" + t1["to"].get<string>() + "," + t2["to"].get<string>() + ")";
                    if (from == current_state) {
                        // Add transition
                        json product_transition_json;
                        product_transition_json["from"] = from;
                        product_transition_json["input"] = t1["input"].get<string>();
                        product_transition_json["to"] = to;
                        transitions.push_back(product_transition_json);

                        // Mark "to" state as// Mark "to" state as reachable and add it to the queue
                        if (reachable.find(to) == reachable.end()) {
                            reachable[to] = true;
                            queue.push(to);
                        }
                    }
                }
            }
        }
    }

    // Update the JSON object
    json_dfa["states"] = states;
    json_dfa["transitions"] = transitions;
}





// Print the DFA
int DFA::print() {
    cout << setw(4) << json_dfa << endl;
    return 0;
}

// Transition function
string DFA::transition(const string current_state, const string input) {
    string output;
    for (json triple : transitions) {
        string from = triple["from"];
        string input_ = triple["input"];

        if (from == current_state && input_ == input) {
            output = triple["to"];
        }
    }
    return output;
}

// Acceptance checking function
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
        std::string name = "\"" + state["name"].get<std::string>() + "\"";
        if (state["accepting"].get<bool>()) {
            dot << "    " << name << " [shape=doublecircle];\n";
        }
    }

    for (const auto& transition : transitions) {
        std::string from = "\"" + transition["from"].get<std::string>() + "\"";
        std::string input = transition["input"].get<std::string>();
        std::string to = "\"" + transition["to"].get<std::string>() + "\"";

        dot << "    " << from << " -> " << to << " [label=\"" << input << "\"];\n";
    }

    std::string start_state_str = "\"" + start_state + "\"";
    dot << "    start -> " << start_state_str << ";\n";
    dot << "}\n";

    return dot.str();
}

