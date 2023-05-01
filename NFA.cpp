//
// Created by Pim Van den Bosch on 2023/05/01.
//

#include "NFA.h"
#include "DFA.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <queue>
#include <list>

using namespace std;
using json = nlohmann::json;

NFA::NFA(const string& input) {
    ifstream json_file(input);
    json_file >> json_nfa;
    constructFromJson(json_nfa);
}

void NFA::constructFromJson(const json& input) {
    //Type
    type = json_nfa["type"];

    // Alphabet
    for (const auto& symbol : json_nfa["alphabet"]) {
        alphabet.push_back(symbol.get<string>());
    }

    //States
    states = json_nfa["states"];

    // Transitions
    for (json triple : json_nfa["transitions"]) {
        string from = triple["from"];
        string input_ = triple["input"];
        string to = triple["to"];

        // If a transition for this state and input doesn't exist, create a new set
        if (transitions.count(from) == 0 || transitions[from].count(input_) == 0) {
            transitions[from][input_] = set<string>();
        }

        // Add the destination state to the transition set
        transitions[from][input_].insert(to);
    }

    // Start and accepting states
    for (json state : states) {
        string name = state["name"];
        bool is_starting = state["starting"];
        bool is_accepting = state["accepting"];

        if (is_starting) {
            start_state = name;
        }

        if (is_accepting) {
            accepting_states.insert(name);
        }
    }
}

set<string> NFA::transition(const string& current_state, const string& input) {
    // If a transition exists for this state and input, return it. Otherwise, return an empty set
    if (transitions.count(current_state) > 0 && transitions[current_state].count(input) > 0) {
        return transitions[current_state][input];
    } else {
        return set<string>();
    }
}

// Helper function to merge state names in a partition into a string
std::string NFA::mergeStateNames(const std::set<std::string>& states) {
    // Convert the set to a vector and sort in numerical order
    std::vector<int> state_vector;
    for (const auto& state_name : states) {
        state_vector.push_back(std::stoi(state_name));
    }
    std::sort(state_vector.begin(), state_vector.end());

    // Merge the state names
    std::string merged_name = "{";
    for (const auto& state_name : state_vector) {
        merged_name += std::to_string(state_name) + ",";
    }
    merged_name.pop_back();  // remove the last comma
    merged_name += "}";

    return merged_name;
}

DFA NFA::toDFA() {
    // Initialize DFA
    DFA dfa;
    dfa.alphabet = alphabet;  // transfer the alphabet to the DFA

    // Define a list to keep track of unprocessed DFA states (each DFA state is a set of NFA states)
    list<set<string>> unprocessed_states;

    // Define a map to keep track of the name of each DFA state
    unordered_map<string, string> dfa_state_names;

    // The start state of the DFA is the set of NFA states reachable from the NFA start state
    set<string> start_states = {start_state};

    // Convert the start_states set to a string
    string start_states_str = mergeStateNames(start_states);

    // Add the start state to the list of unprocessed states and the map of state names
    unprocessed_states.push_back(start_states);
    dfa_state_names[start_states_str] = start_states_str;

    // While there are unprocessed states
    while (!unprocessed_states.empty()) {
        // Pop a state from the list of unprocessed states
        set<string> current_nfa_states = unprocessed_states.front();
        unprocessed_states.pop_front();

        // Convert the current_nfa_states set to a string
        string current_nfa_states_str = mergeStateNames(current_nfa_states);

        // Create a new state in the DFA
        string dfa_state_name = dfa_state_names[current_nfa_states_str];
        json new_state = {
                {"name", dfa_state_name},
                {"starting", (current_nfa_states.size() == 1 && current_nfa_states.count(start_state) > 0)},
                {"accepting", isAcceptingState(current_nfa_states)}
        };
        dfa.states.push_back(new_state);

        // For each symbol in the alphabet
        for (string symbol : alphabet) {
            // Find the set of NFA states reachable from the current set of NFA states
            set<string> next_nfa_states;
            for (string nfa_state : current_nfa_states) {
                set<string> reachable_states = transitions[nfa_state][symbol];
                next_nfa_states.insert(reachable_states.begin(), reachable_states.end());
            }

            // Convert the next_nfa_states set to a string
            string next_nfa_states_str = mergeStateNames(next_nfa_states);

            // If this set of NFA states has not been encountered before, add it to the list of unprocessed states
            if (dfa_state_names.count(next_nfa_states_str) == 0) {
                unprocessed_states.push_back(next_nfa_states);
                dfa_state_names[next_nfa_states_str] = next_nfa_states_str;
            }

            // Add a transition in the DFA
            json new_transition = {
                    {"from", dfa_state_name},
                    {"to", dfa_state_names[next_nfa_states_str]},
                    {"input", symbol}
            };
            dfa.transitions.push_back(new_transition);
        }
    }

    // Sort the states and transitions
    sort(dfa.states.begin(), dfa.states.end(), [](const json &a, const json &b) { return a["name"] < b["name"]; });
    sort(dfa.transitions.begin(), dfa.transitions.end(), [](const json &a, const json &b) { return a["from"] < b["from"] || (a["from"] == b["from"] && a["input"] < b["input"]); });

    // Return the DFA
    return dfa;
}

bool NFA::isAcceptingState(const set<string>& nfa_states) {
    // A set of NFA states is accepting if it contains at least one accepting NFA state
    for (string state : nfa_states) {
        if (accepting_states.count(state) > 0) {
            return true;
        }
    }
    return false;
}






