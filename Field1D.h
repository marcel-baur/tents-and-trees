//
// Created by Marcel Baur on 15/11/2020.
//

#ifndef SAT_FIELD_H
#define SAT_FIELD_H

#include <string>
#include <vector>
#include "Cell.h"


using namespace std;

struct ValidField {
    bool result;
    tuple<int, int> coord;
    int coordVec;
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
    vector<int> coordNumbers;
    void flattenVector(vector<vector<CellContent>> map);
    CellContent aboveOf(int current);
    CellContent belowOf(int current);
    CellContent rightOf(int current);
    CellContent leftOf(int current);
    CellContent diaUpperRightOf(int current);
    CellContent diaUpperLeftOf(int current);
    CellContent diaBelowRightOf(int current);
    CellContent diaBelowLeftOf(int current);

    void setClearRows();
    void setClearCols();
    void blockFieldsWithoutTree();
    void blockRadiusOfAllTents();
    void blockRadiusTent(int r);
    bool solveRec(int r, int c);
    bool isDone();
    void analyzeRowsAndCols();
    bool assertValidMove(int r, int c);
    bool assertValidSum(int r, int c);
    void solveColRowForField(int r, int c);
    bool assertValidParity(int r, int c);
    int countTreesRec(int r, int c, vector<tuple<int, int>>* pred);
    int countTentsRec(int r, int c, vector<tuple<int, int>>* pred);
    bool assertNoNeighbouringTents(int r, int c);
    vector<CellContent> saveMap();
    vector<tuple<int,int>> getNeighbors(int r, int c);
    ValidField findOpenField();
    static size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);

    vector<CellContent> restoreMap(vector<CellContent> deepCopy);

    static bool containsTuple(vector<tuple<int, int>>* vec, tuple<int, int> tup);

};

#endif //SAT_FIELD_H
