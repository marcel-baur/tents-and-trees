//
// Created by Marcel Baur on 15/11/2020.
//

#ifndef SAT_FIELD_H
#define SAT_FIELD_H

#include <string>
#include <vector>
#include "Cell.h"


using namespace std;

class Field {
//    vector<vector<int>> map;
public:
    void generateFromFile(const string& path);
    void solve();
    void printField();

private:
    static int getSize(const string& firstLine);
    vector<vector<CellContent>> map;
};

#endif //SAT_FIELD_H
