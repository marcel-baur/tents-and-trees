//
// Created by Marcel Baur on 15/11/2020.
//

#ifndef SAT_FIELD1D_H
#define SAT_FIELD1D_H

#include <string>
#include <vector>
#include "Cell.h"


using namespace std;

struct ValidField1D {
    bool result;
    int coord;
};

class Field1D {

public:
    void generateFromFile(const string& path);
    bool solve();
    void printField();

private:
    static vector<int> getSize(const string& firstLine);
    vector<vector<CellContent>> map;
    vector<int> rowNumbers;
    vector<int> colNumbers;

    //One Dimesional Vector
    vector<CellContent> mapCells;
    void flattenVector(vector<vector<CellContent>> map);
    int aboveOf(int current);
    int belowOf(int current);
    int rightOf(int current);
    int leftOf(int current);
    int diaUpperRightOf(int current);
    int diaUpperLeftOf(int current);
    int diaBelowRightOf(int current);
    int diaBelowLeftOf(int current);

    void setClearRows();
    void setClearCols();
    void blockFieldsWithoutTree();
    void blockRadiusOfAllTents();
    void blockRadiusTent(int coord);
    bool solveRec(int coord);
    bool isDone();
    void analyzeRowsAndCols();
    bool assertValidMove(int coord);
    bool assertValidSum(int coord);
    void solveColRowForField(int coord);
    bool assertValidParity(int coord);
    int countTreesRec(int coord, vector<int>* pred);
    int countTentsRec(int coord, vector<int>* pred);
    bool assertNoNeighbouringTents(int coord);
    vector<CellContent> saveMap();
    vector<int> getNeighbors(int coord);
    ValidField1D findOpenField();
    static size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);

    vector<CellContent> restoreMap(vector<CellContent> deepCopy);

    void printMapCells(vector<CellContent> cells);

    static bool containsNumber(vector<int>* vec, int coord);

    void print1DVector();
};

#endif //SAT_FIELD_H
