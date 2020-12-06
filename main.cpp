#include <iostream>
#include "Field.h"
#include <chrono>

using namespace std::chrono;

int main() {
    Field genField;
    Field field;
    // GENERATOR
    genField.generate(10,10);

//    genField.solve();
//    cout << '\n';
//    cout << "Solution:" << '\n';
//
//    genField.printField();

    // SOLVER
//    field.generateFromFile("../inputs/tents-8x8-t1.txt");
    cout << "Solving...\n";
    int begin = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    bool sol = genField.solve();
    int end = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    cout << '\n';
    genField.printField();
    cout << "Solved!\n";
    cout << "Solution: " << sol << '\n';
    int time = end - begin;
    cout << time << '\n';



    return 0;
}
