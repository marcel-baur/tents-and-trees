#include <iostream>
#include "Field.h"

int main() {
    Field field;
    field.generateFromFile("../inputs/tents-25x30-t.txt");
    cout << "Solving...\n";
    field.solve();
    cout << "Solved!\n";
    field.printField();
//    std::cout << "Hello, World!" << std::endl;
    return 0;
}
