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
    static vector<int> getSize(const string& firstLine);
    vector<vector<CellContent>> map;
    vector<int> rowNumbers;
    vector<int> colNumbers;
    void setClearRows();
    void setClearCols();
    void blockFieldsWithoutTree();
    int emptyFieldsInRow(int row);
    int emptyFieldsInCol(int col);
    void solveRows();
    void solveCols();
    void blockTentRadius();
    void solveRowsAndCols();
    void blockTreeWithTentRadius();
    void blockRadiusTent(int r, int c);
    void blockRadiusTree(int r, int c);
    bool checkRadiusFor(CellContent cellContent, int r, int c);
    static size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);

};

#endif //SAT_FIELD_H
