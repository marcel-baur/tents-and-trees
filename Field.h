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
};

class Field {
//    vector<vector<int>> map;
public:
    void generateFromFile(const string& path);
    bool solve();
    bool solve2();
    void printField();

private:
    static vector<int> getSize(const string& firstLine);
    vector<vector<CellContent>> map;
    vector<int> rowNumbers;
    vector<int> colNumbers;
    int previousSolvedRows;
    int previousSolvedCols;
    int controlCheck;
    void setClearRows();
    void setClearCols();
    void blockFieldsWithoutTree();
    int emptyFieldsInRow(int row);
    int emptyFieldsInCol(int col);
    int tentsInRow(int row);
    int tentsInCol(int col);
    bool solveRows();
    bool solveCols();
    void blockTentRadius();
    int checkForCompletedRows();
    int checkForCompletedCols();
    void blockTreeWithTentRadius();
    void blockRadiusTent(int r, int c);
    void blockRadiusTree(int r, int c);
    bool checkRadiusFor(CellContent cellContent, int r, int c);
    void placeTentForSingularTree();
    bool isFieldTreeOrBlocked(CellContent cellContent);
    void checkTreeFieldForSingleTent(int r, int c);
    void eliminateFieldByNeighbor(int r, int c);
    void analyzeTents();
    bool solveRec(int r, int c);
    bool solverStep();
    bool assertValidState();
    bool isDone();
    vector<vector<CellContent>> saveMap();
    vector<tuple<int,int>> getNeighbors(int r, int c);
    ValidField findOpenField();
    static size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);
    bool checkForChange(int currentSolvedRows, int currentSolvedCols);

    vector<vector<CellContent>> restoreMap(vector<vector<CellContent>> deepCopy);
};

#endif //SAT_FIELD_H
