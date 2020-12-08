//
// Created by Marcel Baur on 15/11/2020.
//

#include "Field1D.h"
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include "Cell.h"

using namespace std;


void Field1D::generateFromFile(const string &path) {
    ifstream file;
    file.open(path, ios::in);
    if (file.is_open()) {
        string line;
        getline(file, line);
        vector<int> fieldSize = getSize(line);
        for (int i = 0; i < fieldSize[0]; i++) {
            vector<CellContent> row(fieldSize[1], Empty);
            map.push_back(row);
        }
        int currLine = 0;
        char delim = ' ';
        while (getline(file, line)) {
            if (currLine < fieldSize[0]) {
                // GameField representation
                string rowConfig = line.substr(0, line.find(delim));
                int rowNumber = stoi(line.substr(line.find(delim)));
                rowNumbers.push_back(rowNumber);
                for (int i = 0; i < fieldSize[1]; i++) {
                    if (rowConfig[i] == '.') {
                        map[currLine][i] = Empty;
                    } else if (rowConfig[i] == 'T') {
                        map[currLine][i] = Tree;
                    } else {
                        throw "Bad File Format (Cell is neither Tree nor Empty)";
                    }
                }
            } else if (currLine == fieldSize[0]) {
                // Column Number definitions
                try {
                    std::vector<std::string> v;
                    split(line, v, ' ');
                    for (auto &s : v) {
                        colNumbers.push_back(stoi(s));
                    }
                } catch (const exception &e) {
                    cout << "ERROR in colNumbers" << e.what();
                }
            }
            currLine++;
        }
        printField();
    } else {
        cout << "Could not open file." << '\n';
    }
}

vector<int> Field1D::getSize(const string &firstLine) {
    try {
        vector<string> info;
        istringstream iss(firstLine);
        for (string l; iss >> l;)
            info.push_back(l);
        int rows = stoi(info[0]);
        int cols = stoi(info[1]);
        vector<int> result = {rows, cols};
        return result;
    } catch (const exception &e) {
        cout << "ERROR in getSize";
    }
}

void Field1D::flattenVector(vector<vector<CellContent>> map) {
    mapCells.clear();

    for (int i = 0; i < map.size(); i++) {
        for (int j = 0; j < map[i].size(); j++) {
            mapCells.push_back(map[i][j]);
        }
    }
}

void Field1D::printMapCells(vector<CellContent> cells) {
    int cell = 0;
    int row = 0;
    for (auto &j : cells) {
        switch (j) {
            case Tent:
                cout << "^" << ' ';
                break;
            case Tree:
                cout << "T" << ' ';
                break;
            case Empty:
                cout << "." << ' ';
                break;
            case Blocked:
                cout << "o" << ' ';
                break;
            default:
                cout << "?" << ' ';
        }
        if (cell % colNumbers.size() == colNumbers.size() - 1) {
            cout << ' ' << rowNumbers[row];
            cout << '\n';
            row++;
        }
        cell++;
    }
    for (auto &c : colNumbers) cout << c << ' ';
    cout << '\n';
}

void Field1D::printField() {
    int cell = 0;
    int row = 0;
    for (auto &j : mapCells) {
        switch (j) {
            case Tent:
                cout << "^" << ' ';
                break;
            case Tree:
                cout << "T" << ' ';
                break;
            case Empty:
                cout << "." << ' ';
                break;
            case Blocked:
                cout << "o" << ' ';
                break;
            default:
                cout << "?" << ' ';
        }
        if (cell % colNumbers.size() == colNumbers.size() - 1) {
            cout << ' ' << rowNumbers[row];
            cout << '\n';
            row++;
        }
        cell++;
    }
    for (auto &c : colNumbers) cout << c << ' ';
    cout << '\n';
}


bool Field1D::solve() {
    flattenVector(map);
    blockFieldsWithoutTree();
    setClearCols();
    setClearRows();
    analyzeRowsAndCols();
    analyzeRowsAndCols();
    blockRadiusOfAllTents();

    ValidField1D next = findOpenField();
    if (next.result) {
        return solveRec(next.coord);
    } else {
        if (isDone()) return true;
        return false;
    }
}

bool Field1D::solveRec(int coord) {
    printField();
    vector<CellContent> deepCopy = saveMap();
    mapCells[coord] = Tent;
    bool isValid = assertValidMove(coord);
    if (isValid) {
        blockRadiusTent(coord);
        solveColRowForField(coord);
        ValidField1D next = findOpenField();
        if (next.result) {
            if (solveRec(next.coord)) {
                return true;
            } else {
                restoreMap(deepCopy);
                mapCells[coord] = Blocked;
                if (solveRec(next.coord)) {
                    return true;
                } else {
                    return false;
                }
            }
        } else {
            if (isDone()) return true;
            return false;
        }

    } else {
        mapCells[coord] = Blocked;
        ValidField1D next_alt = findOpenField();
        if (next_alt.result) {
            if (solveRec(next_alt.coord)) {
                return true;
            }
            return false;
        } else {
            if (isDone()) return true;
            return false;
        }
    }

}


void Field1D::analyzeRowsAndCols() {

    for (int row = 0; row < rowNumbers.size(); row++) {
        int freeFields = 0;
        int tents = 0;
        for (int coord = row * colNumbers.size(); coord < (row + 1) * colNumbers.size(); coord++) {
            if (mapCells[coord] == Empty) freeFields++;
            if (mapCells[coord] == Tent) tents++;
        }
        if (freeFields + tents == rowNumbers[row]) {
            for (int c = row * colNumbers.size(); c < (row + 1) * colNumbers.size(); c++) {
                if (mapCells[c] == Empty) mapCells[c] = Tent;
            }
        }
    }

    for (int column = 0; column < colNumbers.size(); column++) {
        int freeFields = 0;
        int tents = 0;
        for (int coord = column; coord <= mapCells.size() - (colNumbers.size() - column); coord += colNumbers.size()) {
            if (mapCells[coord] == Empty) freeFields++;
            if (mapCells[coord] == Tent) tents++;
        }
        if (freeFields + tents == colNumbers[column]) {
            for (int coord = column; coord <= mapCells.size() - (colNumbers.size() - column); coord += colNumbers.size()) {
                if (mapCells[coord] == Empty) mapCells[coord] = Tent;
            }
        }
    }
}

vector<CellContent> Field1D::saveMap() {
    vector<CellContent> deepCopy(mapCells.size(), Empty);
    for (int i = 0; i < mapCells.size(); i++) {
        deepCopy[i] = mapCells[i];
    }
    return deepCopy;
}


vector<CellContent> Field1D::restoreMap(vector<CellContent> deepCopy) {
    for (int r = 0; r < deepCopy.size(); r++) {
        mapCells[r] = deepCopy[r];
    }
    return deepCopy;
}


void Field1D::setClearRows() {
    for (int row = 0; row < rowNumbers.size(); row++) {
        if (rowNumbers[row] == 0) {
            for (int coord = row * colNumbers.size(); coord < (row + 1) * colNumbers.size(); coord++) {
                if (mapCells[coord] == Empty) mapCells[coord] = Blocked;
            }
        }
    }
}

void Field1D::setClearCols() {
    for (int column = 0; column < colNumbers.size(); column++) {
        if (colNumbers[column] == 0) {
            for (int coord = column;
                 coord <= mapCells.size() - (colNumbers.size() - column); coord += colNumbers.size()) {
                if (mapCells[coord] == Empty) mapCells[coord] = Blocked;
            }
        }
    }
}

void Field1D::blockFieldsWithoutTree() {
    printField();
    for (int coord = 0; coord < mapCells.size(); coord++) {
        if (mapCells[coord] == Empty) {
            if (coord == 0 &&
                mapCells[rightOf(coord)] != Tree &&
                mapCells[belowOf(coord)] != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord == mapCells.size() - 1 &&
                       mapCells[leftOf(coord)] != Tree &&
                       mapCells[aboveOf(coord)] != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord == colNumbers.size() - 1 &&
                       mapCells[leftOf(coord)] != Tree &&
                       mapCells[belowOf(coord)] != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord == mapCells.size() - (colNumbers.size() - 1) &&
                       mapCells[rightOf(coord)] != Tree &&
                       mapCells[aboveOf(coord)] != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord % colNumbers.size() == 0 &&
                       mapCells[aboveOf(coord)] != Tree &&
                       mapCells[rightOf(coord)] != Tree &&
                       mapCells[belowOf(coord)] != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord < colNumbers.size() &&
                       mapCells[leftOf(coord)] != Tree &&
                       mapCells[rightOf(coord)] != Tree &&
                       mapCells[belowOf(coord)] != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord % colNumbers.size() == colNumbers.size() - 1 && // colN sstatt rowN
                       mapCells[aboveOf(coord)] != Tree &&
                       mapCells[leftOf(coord)] != Tree &&
                       mapCells[belowOf(coord)] != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord >= (rowNumbers.size() - 1) * colNumbers.size() &&
                       mapCells[leftOf(coord)] != Tree &&
                       mapCells[rightOf(coord)] != Tree &&
                       mapCells[aboveOf(coord)] != Tree) {
                mapCells[coord] = Blocked;
            } else if (mapCells[aboveOf(coord)] != Tree &&
                       mapCells[leftOf(coord)] != Tree &&
                       mapCells[belowOf(coord)] != Tree &&
                       mapCells[rightOf(coord)] != Tree) {
                mapCells[coord] = Blocked;
            }
        }

    }
}

void Field1D::blockRadiusOfAllTents() {
    for (int r = 0; r < mapCells.size(); r++) {
        if (mapCells[r] == Tent) {
            blockRadiusTent(r);
        }
    }
}

void Field1D::blockRadiusTent(const int coord) {
    printField();
    if (coord < (rowNumbers.size() - 1) * colNumbers.size() && mapCells[belowOf(coord)] == Empty) {
        mapCells[belowOf(coord)] = Blocked;
    }
    if (coord >= colNumbers.size() - 1 && mapCells[aboveOf(coord)] == Empty) {
        mapCells[aboveOf(coord)] = Blocked;
    }
    if (coord % colNumbers.size() != colNumbers.size() - 1 && mapCells[rightOf(coord)] == Empty) {
        mapCells[rightOf(coord)] = Blocked;
    }
    if (coord % colNumbers.size() != 0 && mapCells[leftOf(coord)] == Empty) {
        mapCells[leftOf(coord)] = Blocked;
    }
    if (coord < mapCells.size() - (colNumbers.size() - 1) && coord % colNumbers.size() != colNumbers.size() - 1 &&
            mapCells[diaBelowRightOf(coord)] == Empty) {
        mapCells[diaBelowRightOf(coord)] = Blocked;
    }
    if (coord < mapCells.size() - (colNumbers.size() - 1) && coord % colNumbers.size() != 0 &&
            mapCells[diaBelowLeftOf(coord)] == Empty) {
        mapCells[diaBelowLeftOf(coord)] = Blocked;
    }
    if (coord >= colNumbers.size() - 1 && coord % colNumbers.size() != colNumbers.size() - 1 &&
            mapCells[diaUpperRightOf(coord)] == Empty) {
        mapCells[diaUpperRightOf(coord)] = Blocked;
    }
    if (coord >= colNumbers.size() - 1 && coord % colNumbers.size() != 0 && mapCells[diaUpperLeftOf(coord)] == Empty) {
        mapCells[diaUpperLeftOf(coord)] = Blocked;
    }
}


size_t Field1D::split(const std::string &txt, std::vector<std::string> &strs, char ch) {
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

    return strs.size();
}

vector<int> Field1D::getNeighbors(int coord) {
    int coordRight = coord + 1;
    int coordLeft = coord - 1;
    int coordAbove = coord - colNumbers.size();
    int coordBelow = coord + colNumbers.size();

    vector<int> neighbours;
    if (coord % (colNumbers.size() - 1) != colNumbers.size()) {
        neighbours.push_back(coordRight);
    }
    if (coord % colNumbers.size() != 0) {
        neighbours.push_back(coordLeft);
    }
    if (coord > colNumbers.size()) {
        neighbours.push_back(coordAbove);
    }
    if (coord <= mapCells.size() - colNumbers.size()) {
        neighbours.push_back(coordBelow);
    }
    return neighbours;
}

ValidField1D Field1D::findOpenField() {
    printField();
    for (int f = 0; f < mapCells.size(); f++) {
        if (mapCells[f] == Empty) {
            return ValidField1D{true, f};
        }
    }

    return ValidField1D{false, -1};

}

bool Field1D::isDone() {
    for (int r = 0; r < rowNumbers.size(); r++) {
        int tents = 0;
        for (int c = r * colNumbers.size(); c < (r + 1) * colNumbers.size(); c++) {
            if (mapCells[c] == Tent) tents++;
        }
        if (tents != rowNumbers[r]) {
            return false;
        }
    }

    for (int c = 0; c < colNumbers.size(); c++) {
        int tents = 0;
        for (int r = c; r <= mapCells.size() - (colNumbers.size() - c); r += colNumbers.size()) {
            if (mapCells[r] == Tent) tents++;
        }
        if (tents != colNumbers[c]) {
            return false;
        }
    }

    return true;



//    // Check rows
//    for (int r = 0; r < map.size(); r++) {
//        int tent = 0;
//        for (int c = 0; c < map[r].size(); c++) {
//            if (map[r][c] == Tent) tent++;
//        }
//        if (tent != rowNumbers[r]) return false;
//    }
//
//    // Check cols
//    for (int c = 0; c < map.size(); c++) {
//        int tent = 0;
//        for (int r = 0; r < map[0].size(); r++) {
//            if (map[r][c] == Tent) tent++;
//        }
//        if (tent != colNumbers[c]) return false;
//
//    }
//    return true;
}

bool Field1D::assertNoNeighbouringTents(int coord) {
    vector neighbors = getNeighbors(coord);
    for (auto &n : neighbors) {
        if (mapCells[n] == Tent) return false;
    }
    return true;
}

bool Field1D::assertValidMove(int coord) {
    return assertNoNeighbouringTents(coord) && assertValidSum(coord) && assertValidParity(coord);
}

bool Field1D::assertValidSum(int coord) {
    // Calculate row and col number for this particular cell's row/col
    int rowOfCell = coord / colNumbers.size();
    int colOfCell = coord % colNumbers.size();
    int rowNumber = rowNumbers[rowOfCell];
    int colNumber = colNumbers[colOfCell];

    // iterate row
    int rowSum = 0;
    for (int i = rowOfCell * colNumbers.size(); i < (rowOfCell + 1) * colNumbers.size(); i++) { // TODO check
        if (mapCells[i] == Tent) rowSum++;
    }
    if (rowSum > rowNumbers[rowOfCell]) {
        return false;
    }

    // iterate col
    int colSum = 0;
    for (int i = colOfCell; i <= colOfCell + colNumbers.size() * (rowNumbers.size() - 1); i += colNumbers.size()) {
        if (mapCells[i] == Tent) colSum++;
    }
    if (colSum > colNumbers[colOfCell]) return false;

    return true;
}

void Field1D::solveColRowForField(int coord) {

    int rowOfCell = coord / colNumbers.size();
    int colOfCell = coord % colNumbers.size();


    // Iterate over rows
    int tentsR = 0;
    int freeR = 0;
    for (int i = rowOfCell * colNumbers.size(); i < (rowOfCell + 1) * colNumbers.size(); i++) {
        if (mapCells[i] == Tent) tentsR++;
        if (mapCells[i] == Empty) freeR++;
    }

    if (freeR == rowNumbers[rowOfCell] - tentsR) {
        for (int i = rowOfCell * colNumbers.size(); i < (rowOfCell + 1) * colNumbers.size(); i++) {
            if (mapCells[i] == Empty) mapCells[i] = Tent;
        }
    }
    if (tentsR == rowNumbers[rowOfCell]) {
        for (int i = rowOfCell * colNumbers.size(); i < (rowOfCell + 1) * colNumbers.size(); i++) {
            if (mapCells[i] == Empty) mapCells[i] = Blocked;
        }
    }

    // Iterate over cols
    int tentsC = 0;
    int freeC = 0;
    for (int i = colOfCell; i <= mapCells.size() - (colNumbers.size() - colOfCell); i += colNumbers.size()) {
        if (mapCells[i] == Tent) tentsC++;
        if (mapCells[i] == Empty) freeC++;
    }
    if (tentsC + freeC == colNumbers[colOfCell]) {
        for (int i = colOfCell; i <= mapCells.size() - (colNumbers.size() - colOfCell); i += colNumbers.size()) {
            if (mapCells[i] == Empty) mapCells[i] = Tent;
        }
    }
    if (tentsC == colNumbers[colOfCell]) {
        for (int i = colOfCell; i <= mapCells.size() - (colNumbers.size() - colOfCell); i += colNumbers.size()) {
            if (mapCells[i] == Empty) mapCells[i] = Blocked;
        }
    }

}

int Field1D::countTreesRec(int coord, vector<int> *pred) {
    pred->push_back(coord);
    if (mapCells[coord] == Tree) {
        int parity = 1;
        for (auto &n : getNeighbors(coord)) {
            if (!containsNumber(pred, n)) parity += countTentsRec(n, pred);
        }
        return parity;
    }
    return 0;
}

int Field1D::countTentsRec(int coord, vector<int> *pred) {
    pred->push_back(coord);
    if (mapCells[coord] == Tent) {
        int parity = -1;
        for (auto &n : getNeighbors(coord)) {
            if (!containsNumber(pred, n)) parity += countTreesRec(n, pred);
        }
        return parity;
    }
    return 0;
}

bool Field1D::assertValidParity(int coord) {
    int parity = -1;
    vector<int> pred;
    pred.emplace_back(coord);
    for (auto &n : getNeighbors(coord)) {
        parity += countTreesRec(n, &pred);
    }
    if (parity >= 0) return true;
    return false;
}


bool Field1D::containsNumber(vector<int> *vec, int coord) {
    for (auto &n : *vec) {
        if (coord == n) return true;
    }
    return false;
}

int Field1D::aboveOf(int current) {
    return current - colNumbers.size();
}

int Field1D::belowOf(int current) {
    return current + colNumbers.size();
}

int Field1D::rightOf(int current) {
    return current + 1;
}

int Field1D::leftOf(int current) {
    return current - 1;
}

int Field1D::diaUpperRightOf(int current) {
    return current - colNumbers.size() + 1;
}

int Field1D::diaUpperLeftOf(int current) {
    return current - colNumbers.size() - 1;
}

int Field1D::diaBelowRightOf(int current) {
    return current + colNumbers.size() + 1;
}

int Field1D::diaBelowLeftOf(int current) {
    return current + colNumbers.size() - 1;
}


