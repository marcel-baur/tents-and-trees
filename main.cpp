#include <iostream>
#include "Field.h"

int main() {
    Field field;
    field.generateFromFile("../inputs/tents-15x15-e1.txt");
    cout << "Solving...\n";
    field.solve();
    cout << "Solved!\n";
    field.printField();
//    std::cout << "Hello, World!" << std::endl;
    return 0;
}
