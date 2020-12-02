#include <iostream>
#include "Field.h"

int main() {
    Field field;
    field.generateFromFile("../inputs/tents-15x15-t2.txt");
    cout << "Solving...\n";
//    CellContent tree = Tree;
//    CellContent empty = Empty;
//    empty = tree;
//    tree = Blocked;
//    cout << empty << tree << '\n';
    bool sol = field.solve2();
    cout << "Solved!\n";
    cout << sol << '\n';
    field.printField();
//    std::cout << "Hello, World!" << std::endl;
    return 0;
}
