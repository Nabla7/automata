#include "DFA.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "json.hpp"
#include "NFA.h"
using namespace std;
using json = nlohmann::json;




int main() {
    // Load the JSON file containing the test cases
    std::ifstream test_file("tests.json");
    json tests_json;
    test_file >> tests_json;
    test_file.close();

    // Iterate over each test case
    for (const auto& test : tests_json["tests"]) {
        // Extract the DFA from the test case
        std::string test_name = test["name"].get<std::string>();
        json dfa_json = test["dfa"];

        // Construct the DFA and minimize it
        DFA dfa(dfa_json);
        DFA mindfa = dfa.minimize();

        // Save the dot files
        std::ofstream dfa_dot("testdots/" + test_name + "_regular.dot");
        dfa_dot << dfa.to_dot();
        dfa_dot.close();

        std::ofstream mindfa_dot("testdots/" + test_name + "_minimized.dot");
        mindfa_dot << mindfa.to_dot();
        mindfa_dot.close();

        // Print the tables and check equivalence
        //dfa.printTable();
        //mindfa.print();
        //std::cout << boolalpha << (dfa == mindfa) << std::endl;
    }

    return 0;
}


/*
int main() {
    DFA dfa1("input-product-and1.json");
    DFA dfa2("input-product-and2.json");
    DFA product(dfa1,dfa2,true); // true betekent doorsnede, false betekent unie
    product.print();

    ofstream dfa1_dot("dfa1.dot");
    dfa1_dot << dfa1.to_dot();
    dfa1_dot.close();

    ofstream dfa2_dot("dfa2.dot");
    dfa2_dot << dfa2.to_dot();
    dfa2_dot.close();

    ofstream product_dot("product.dot");
    product_dot << product.to_dot();
    product_dot.close();

    return 0;
}
*/

/*
int main() {
    DFA expected("expected.json");
    DFA actual("actual.json");

    expected.print();
    actual.print();

    expected.to_dot();
    actual.to_dot();

    ofstream expected_dot("expected_dot.dot");
    expected_dot << expected.to_dot();
    expected_dot.close();

    ofstream actual_dot("actual_dot.dot");
    actual_dot << actual.to_dot();
    actual_dot.close();
}
*/

/*
int main() {
    DFA dfa1("input-product-and1.json");
    DFA dfa2("input-product-and2.json");
    DFA product(dfa1, dfa2, true); // true means intersection, false means union

    ofstream dfa1_dot("dfa1.dot");
    dfa1_dot << dfa1.to_dot();
    dfa1_dot.close();

    ofstream dfa2_dot("dfa2.dot");
    dfa2_dot << dfa2.to_dot();
    dfa2_dot.close();

    ofstream product_dot("product.dot");
    product_dot << product.to_dot();
    product_dot.close();

    product.print();

}
*/

/*
int main() {

    DFA dfa("tfa.json");
    dfa.printTable();

    ofstream dfa_dot("DFA.dot");
    dfa_dot << dfa.to_dot();
    dfa_dot.close();



    DFA mindfa = dfa.minimize();
    mindfa.printTable();

    ofstream mindfa_dot("DFAmin.dot");
    mindfa_dot << mindfa.to_dot();
    mindfa_dot.close();

}
 */

/*
int main() {
    DFA dfa("tfa.json");
    DFA mindfa = dfa.minimize();
    dfa.printTable();
    mindfa.print();
    cout << boolalpha << (dfa == mindfa) << endl;    // zijn ze equivalent? Zou hier zeker moeten. Dit wordt getest in de volgende vraag, maar hiermee kan je al eens proberen
    return 0;
}
*/

/*
int main() {
    NFA nfa("input-ssc1.json");
    nfa.toDFA().print();
    return 0;
}
 */