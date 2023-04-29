#include "DFA.h"
#include <fstream> // Include this header
#include <sstream> // Include this header

using namespace std;

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

