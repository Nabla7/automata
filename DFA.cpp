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


DFA::DFA(){
    type = "DFA";

}

// Constructor for creating DFA from a json file
DFA::DFA(const std::string& input) {
    ifstream json_file(input);
    json_file >> json_dfa;
    constructFromJson(json_dfa);
}

/*
// Constructor for creating DFA from a json object
DFA::DFA(const json& input) {
    json_dfa = input;
    constructFromJson(json_dfa);
}
*/

// Private method to construct DFA from a json object
void DFA::constructFromJson(const json& input) {
    //Type
    type = json_dfa["type"];

    // Alphabet
    // Convert the JSON array to a vector of strings
    for (const auto& symbol : json_dfa["alphabet"]) {
        alphabet.push_back(symbol.get<std::string>());
    }

    //States
    states = json_dfa["states"];

    for (json state : states) {

        //Start state
        if (state["starting"] == true){
            start_state = state["name"];
        }

        //Final states
        if (state["accepting"] == true){
            string name = state["name"];
            accepting_states.insert(name);
        }
    }

    // Transitions
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

void DFA::print() const {
    // Create a json object
    nlohmann::json j;

    // Set the type
    j["type"] = type;

    // Set the alphabet
    j["alphabet"] = alphabet;

    // Set the states
    for (const auto& state_name : states) {
        nlohmann::json state;
        state["name"] = state_name["name"];
        state["starting"] = state_name["starting"];
        state["accepting"] = state_name["accepting"];
        j["states"].push_back(state);
    }

    // Set the transitions
    for (const auto& triple : transitions) {
        nlohmann::json transition;
        transition["from"] = triple["from"];
        transition["input"] = triple["input"];
        transition["to"] = triple["to"];
        j["transitions"].push_back(transition);
    }

    // Convert the json object to a string with 2 spaces of indentation
    std::string json_str = j.dump(2);

    // Print the json string
    std::cout << json_str << std::endl;
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

DFA DFA::minimize() {
    // Step 1: Partition states into two sets, F and Q-F, where F is the set of accepting states, and Q is the set of all states
    std::unordered_set<std::string> non_accepting_states;
    for (json state : states) {
        if (!accepting_states.count(state["name"])) {
            non_accepting_states.insert(state["name"].get<std::string>());
        }
    }


    // Create a partition object to hold the partition sets
    std::vector<std::unordered_set<std::string>> partitions = { accepting_states, non_accepting_states };

    // Create a worklist and initialize it with the smaller of the two sets
    std::queue<std::unordered_set<std::string>> worklist;
    if (accepting_states.size() <= non_accepting_states.size()) {
        worklist.push(accepting_states);
    } else {
        worklist.push(non_accepting_states);
    }


    // Step 2: Refine the partition sets
    while (!worklist.empty()) {
        std::unordered_set<std::string> A = worklist.front();
        worklist.pop();

        for (std::string a : alphabet) {
            // Let X be the set of states for which a transition on a leads to a state in A
            std::unordered_set<std::string> X;
            for (json triple : transitions) {
                if (A.count(triple["to"]) && a == triple["input"]) {
                    X.insert(triple["from"].get<std::string>());
                }
            }

            // For each set Y in partitions for which X intersect Y is nonempty and Y - X is nonempty, do:
            std::vector<std::unordered_set<std::string>> new_partitions;
            std::unordered_set<std::string> worklist_Y;

            for (const auto& Y : partitions) {
                std::unordered_set<std::string> Y_intersect_X, Y_diff_X;

                for (std::string y : Y) {
                    if (X.count(y)) {
                        Y_intersect_X.insert(y);
                    } else {
                        Y_diff_X.insert(y);
                    }
                }

                if (!Y_intersect_X.empty() && !Y_diff_X.empty()) {
                    new_partitions.push_back(Y_intersect_X);
                    new_partitions.push_back(Y_diff_X);

                    if (worklist_Y.empty()) {
                        if (Y_intersect_X == Y) {
                            worklist_Y = Y_intersect_X;
                        } else if (Y_diff_X == Y) {
                            worklist_Y = Y_diff_X;
                        }
                    }
                } else {
                    new_partitions.push_back(Y);
                }
            }

            partitions = new_partitions;

            for (auto Y = partitions.begin(); Y != partitions.end(); ++Y) {
                std::unordered_set<std::string> Y_intersect_X, Y_diff_X;

                for (std::string y : *Y) {
                    if (X.count(y)) {
                        Y_intersect_X.insert(y);
                    } else {
                        Y_diff_X.insert(y);
                    }
                }

                if (!Y_intersect_X.empty() && !Y_diff_X.empty()) {
                    // Replace Y with the split sets
                    Y = partitions.erase(Y);
                    partitions.insert(Y, Y_intersect_X);
                    partitions.insert(Y, Y_diff_X);

                    // Push the smaller of the two new sets into the worklist
                    if (Y_intersect_X.size() <= Y_diff_X.size()) {
                        worklist.push(Y_intersect_X);
                    } else {
                        worklist.push(Y_diff_X);
                    }
                }
            }
        }
    }


    // Step 3: Create a new DFA based on the final partitions
    DFA mindfa;
    mindfa.alphabet = alphabet;
    // Create the new minimized DFA states based on the partitions
    for (const auto& partition : partitions) {
        std::string merged_name = "{";
        bool is_starting = false;
        bool is_accepting = false;
        for (const auto& state_name : partition) {
            merged_name += state_name + ", ";
            if (state_name == start_state) is_starting = true;
            if (accepting_states.count(state_name)) is_accepting = true;
        }
        merged_name = merged_name.substr(0, merged_name.length() - 2);  // remove the last comma and space
        merged_name += "}";

        json state;
        state["name"] = merged_name;
        state["starting"] = is_starting;
        state["accepting"] = is_accepting;
        mindfa.states.push_back(state);

        if (is_starting) mindfa.start_state = merged_name;
        if (is_accepting) mindfa.accepting_states.insert(merged_name);
    }

    // Create the new minimized DFA transitions based on the partitions
    std::map<std::pair<std::string, std::string>, std::string> transition_map;
    for (const auto& triple : transitions) {
        std::string from, to;
        for (const auto& partition : partitions) {
            if (partition.count(triple["from"])) {
                from = DFA::mergeStateNames(partition);
            }
            if (partition.count(triple["to"])) {
                to = DFA::mergeStateNames(partition);
            }
        }
        transition_map[{from, triple["input"].get<std::string>()}] = to;
    }

    // Convert the map back to the transition structure
        for (const auto& pair : transition_map) {
            json transition;
            transition["from"] = pair.first.first;
            transition["input"] = pair.first.second;
            transition["to"] = pair.second;
            mindfa.transitions.push_back(transition);
        }
    return mindfa;
}

// Helper function to merge state names in a partition into a string
std::string DFA::mergeStateNames(const std::unordered_set<std::string>& states) {
    // Convert the unordered_set to a vector
    std::vector<std::string> state_vector(states.begin(), states.end());

    // Sort the vector
    std::sort(state_vector.begin(), state_vector.end());

    // Merge the state names
    std::string merged_name = "{";
    for (const auto& state_name : state_vector) {
        merged_name += state_name + ", ";
    }

    merged_name = merged_name.substr(0, merged_name.length() - 2);  // remove the last comma and space
    merged_name += "}";

    return merged_name;
}

bool DFA::printTable() {
    // Create a 2D map to store the distinguishability matrix
    map<string, map<string, string>> distinguishabilityMatrix;

    // Initialize the distinguishability matrix with "-"
    for (json state1: states) {
        string stateName1 = state1["name"];
        for (json state2: states) {
            string stateName2 = state2["name"];
            if (stateName1 != stateName2) {
                distinguishabilityMatrix[stateName1][stateName2] = "-";
                distinguishabilityMatrix[stateName2][stateName1] = "-";
            }
        }
    }

    // Mark distinguishable states with "X"
    // If one state is accepting and the other is not, they are distinguishable
    for (json state1: states) {
        string stateName1 = state1["name"];
        for (json state2: states) {
            string stateName2 = state2["name"];
            if (stateName1 != stateName2) {
                if ((accepting_states.count(stateName1) && !accepting_states.count(stateName2)) ||
                    (!accepting_states.count(stateName1) && accepting_states.count(stateName2))) {
                    distinguishabilityMatrix[stateName1][stateName2] = "X";
                    distinguishabilityMatrix[stateName2][stateName1] = "X";
                }
            }
        }
    }

    // Keep refining the distinguishability matrix until no more changes are made
    bool changed = true;
    while (changed) {
        changed = false;

        // For each pair of distinct states
        for (json state1: states) {
            string stateName1 = state1["name"];
            for (json state2: states) {
                string stateName2 = state2["name"];
                if (stateName1 != stateName2 && distinguishabilityMatrix[stateName1][stateName2] != "X") {
                    // For each symbol in the alphabet
                    for (string a: alphabet) {
                        // Find the states we transition to
                        string transitionState1 = transition(stateName1, a);
                        string transitionState2 = transition(stateName2, a);

                        // If the states we transition to are distinguishable, then so are our original pair of states
                        if (transitionState1 != transitionState2 &&
                            distinguishabilityMatrix[transitionState1][transitionState2] == "X") {
                            distinguishabilityMatrix[stateName1][stateName2] = "X";
                            distinguishabilityMatrix[stateName2][stateName1] = "X";
                            changed = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Print the table in the required format
    // Sort the state names
    std::vector<std::string> stateNames;
    for (const auto &state: states) {
        stateNames.push_back(state["name"].get<std::string>());
    }
    std::sort(stateNames.begin(), stateNames.end());

    // Print the distinguishability matrix
    for (size_t i = 1; i < stateNames.size(); ++i) {
        // Print the row name
        std::cout << stateNames[i] << "   ";

        // Then print each cell in the row up to the diagonal
        for (size_t j = 0; j < i; ++j) {
            std::cout << distinguishabilityMatrix[stateNames[i]][stateNames[j]] << "   ";
        }
        std::cout << std::endl;
    }

    // Then print the column names
    std::cout << "    ";  // Start with an empty cell
    for (size_t i = 0; i < stateNames.size() - 1; ++i) {
        std::cout << stateNames[i] << "   ";
    }
    std::cout << std::endl;

    // Check if there are any indistinguishable pairs of states left
    for (const auto& row : distinguishabilityMatrix) {
        for (const auto& cell : row.second) {
            if (cell.second != "X") {
                // There are indistinguishable states, so the DFAs are not equivalent
                return false;
            }
        }
    }
    // All states are distinguishable, so the DFAs are equivalent
    return true;

}

// Helper function to find the state name from a set of states
std::string DFA::getStateName(const std::unordered_set<std::string>& states) {
    std::string stateName = "{";
    for (const auto& state : states) {
        stateName += state + ", ";
    }
    stateName = stateName.substr(0, stateName.length() - 2);  // remove the last comma and space
    stateName += "}";
    return stateName;
}

// Method to create a product automaton
DFA DFA::productAutomaton(DFA& other) {
    DFA product;

    // Create a queue to store the pairs of states to be processed
    std::queue<std::pair<std::string, std::string>> toProcess;

    // Insert the pair of start states into the queue
    toProcess.push({this->start_state, other.start_state});

    // Create a set to store the pairs of states that have been processed
    std::set<std::pair<std::string, std::string>> processed;

    // Process the pairs of states until the queue is empty
    while (!toProcess.empty()) {
        // Get the next pair of states to process
        auto [state1, state2] = toProcess.front();
        toProcess.pop();

        // Skip this pair of states if they've already been processed
        if (processed.count({state1, state2}) > 0) {
            continue;
        }

        // Mark this pair of states as processed
        processed.insert({state1, state2});

        // Create a new state
        json newState;
        newState["name"] = getStateName({state1, state2});
        newState["starting"] = (state1 == this->start_state) && (state2 == other.start_state);
        newState["accepting"] = isAccepting(state1) && other.isAccepting(state2);
        product.states.push_back(newState);

        if (newState["starting"]) product.start_state = newState["name"];
        if (newState["accepting"]) product.accepting_states.insert(newState["name"].get<std::string>());

        // Create the transitions for the new state
        for (const auto& transition1 : this->transitions) {
            for (const auto& transition2 : other.transitions) {
                if (transition1["from"] == state1 && transition2["from"] == state2) {
                    json newTransition;
                    newTransition["from"] = newState["name"];
                    newTransition["input"] = transition1["input"];
                    newTransition["to"] = getStateName({transition1["to"], transition2["to"]});
                    product.transitions.push_back(newTransition);

                    // If this pair of states hasn't been processed yet, add it to the queue
                    if (processed.count({transition1["to"], transition2["to"]}) == 0) {
                        toProcess.push({transition1["to"], transition2["to"]});
                    }
                }
            }
        }
    }
    ofstream dfa1_dot("dfa1.dot");
    dfa1_dot << product.to_dot();
    dfa1_dot.close();
    return product;
}

// Modified comparison method
bool DFA::operator==(DFA& other) {
    // Create the product automaton
    DFA product = productAutomaton(other);

    // Apply the table filling procedure
    bool result = product.printTable();

    // If the initial state is reachable from a final state, the DFA's are not equivalent
    if (!result) {
        return false;
    }

    // Otherwise, the DFA's are equivalent
    return true;
}

// Method to check if a state is accepting
bool DFA::isAccepting(const std::string& stateName) {
    for (const auto& state : this->json_dfa["states"]) {
        if (state["name"] == stateName) {
            return state["accepting"];
        }
    }

    // Return false if the state was not found
    return false;
}



















