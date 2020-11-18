#include <iostream>
#include "Field.h"

int main() {
    Field field;
    field.generateFromFile("../inputs/tents-8x8-t1.txt");
    cout << "Solving...\n";
    field.solve();
    cout << "Solved!\n";
    field.printField();
//    std::cout << "Hello, World!" << std::endl;
    return 0;
}