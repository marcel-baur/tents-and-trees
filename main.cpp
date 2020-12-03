#include <iostream>
#include "Field.h"

int main() {
    Field field;
    field.generateFromFile("../inputs/tents-25x20-t.txt");
    cout << "Solving...\n";
    bool sol = field.solve2();
    field.printField();
    cout << "Solved!\n";
    cout << sol << '\n';
    return 0;
}
