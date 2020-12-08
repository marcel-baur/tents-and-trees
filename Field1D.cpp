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

    for (int i = 0; i < map.size(); i++){
        for (int j = 0; j < map[i].size(); j++){
            mapCells.push_back(map[i][j]);
        }
    }
}

void Field1D::printField() {
    int row = 0;
    for (auto &i : map) {
        for (auto &j : i) {
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
        }
        cout << ' ' << rowNumbers[row];
        row++;
        cout << '\n';
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

    ValidField next = findOpenField();
    if (next.result) {
        return solveRec(std::get<0>(next.coord), std::get<1>(next.coord));
    } else {
        if (isDone()) return true;
        return false;
    }
}

bool Field1D::solveRec(int r, int c) {
    vector<CellContent> deepCopy = saveMap();
    map[r][c] = Tent;
    bool isValid = assertValidMove(r,c);
    if (isValid) {
        blockRadiusTent(r, c);
        solveColRowForField(r, c);
        ValidField next = findOpenField();
        if (next.result) {
            if (solveRec(std::get<0>(next.coord), std::get<1>(next.coord))) {
                return true;
            } else {
                restoreMap(deepCopy);
                map[r][c] = Blocked;
                if (solveRec(std::get<0>(next.coord), std::get<1>(next.coord))) {
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
        map[r][c] = Blocked;
        ValidField next_alt = findOpenField();
        if (next_alt.result) {
            if (solveRec(std::get<0>(next_alt.coord), std::get<1>(next_alt.coord))) {
                return true;
            }
            return false;
        } else {
            if (isDone()) return true;
            return false;
        }
    }

}

void Field1D::solveColRowForField(int r, int c) {
    int tentsR = 0;
    int freeR = 0;
    for (int col = 0; col < colNumbers.size(); col++) {
        if (map[r][col] == Tent) tentsR++;
        if (map[r][col] == Empty) freeR++;
    }

    if (freeR == rowNumbers[r] - tentsR) {
        for (int col = 0; col < colNumbers.size(); col++) {
            if (map[r][col] == Empty) map[r][col] = Tent;
        }
    }
    if (tentsR == rowNumbers[r]) {
        for (int col = 0; col < colNumbers.size(); col++) {
            if (map[r][col] == Empty) map[r][col] = Blocked;
        }
    }

    int tentsC = 0;
    int freeC = 0;
    for (int row = 0; row < rowNumbers.size(); row++) {
        if (map[row][c] == Tent) tentsC++;
        if (map[row][c] == Empty) freeC++;
    }
    if (tentsC + freeC == colNumbers[c]) {
        for (int row = 0; row < rowNumbers.size(); row++) {
            if (map[row][c] == Empty) map[row][c] = Tent;
        }
    }
    if (tentsC == colNumbers[c]) {
        for (int row = 0; row < rowNumbers.size(); row++) {
            if (map[row][c] == Empty) map[row][c] = Blocked;
        }
    }

}

void Field1D::analyzeRowsAndCols() {
    for (int r = 0; r < rowNumbers.size(); r++) {
        int freeFields = 0;
        int tents = 0;
        for (int c = r*colNumbers.size(); c < (r+1)*colNumbers.size(); c++) {
            if (mapCells[c] == Empty) freeFields++;
            if (mapCells[c] == Tent) tents++;
        }
        if (freeFields + tents == rowNumbers[r]) {
            for (int c = r*colNumbers.size(); c < (r+1)*colNumbers.size(); c++) {
                if (mapCells[c] == Empty) mapCells[c] = Tent;
            }
        }
    }
    for (int c = 0; c < colNumbers.size(); c++) {
        int freeFields = 0;
        int tents = 0;
        for (int r = c; r <= c + colNumbers.size() * c; r += rowNumbers.size()) {
            if (mapCells[r] == Empty) freeFields++;
            if (mapCells[r] == Tent) tents++;
        }
        if (freeFields + tents == colNumbers[c]) {
            for (int r = c; r <= c + colNumbers.size() * c; r += rowNumbers.size()) {
                if (mapCells[r] == Empty) mapCells[r] = Tent;
            }
        }
    }
}

vector<CellContent> Field1D::saveMap() {
    vector<CellContent> deepCopy;
    for (int i = 0; i < mapCells.size(); i++) {
        deepCopy.push_back(mapCells[i]);
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
    for (int r = 0; r < rowNumbers.size(); r++) {
        if (rowNumbers[r] == 0) {
            for (int c = r*colNumbers.size(); c < (r+1)*colNumbers.size(); c++) {
                if (mapCells[c] == Empty) mapCells[c] = Blocked;
            }
        }
    }
}

void Field1D::setClearCols() {
    for (int r = 0; r < colNumbers.size(); r++) {
        if (colNumbers[r] == 0) {
            for (int c = r*colNumbers.size(); c < (r+1)*colNumbers.size(); c++) {
                if (mapCells[c] == Empty) mapCells[c] = Blocked;
            }
        }
    }
}

void Field1D::blockFieldsWithoutTree() {
    for (int coord = 0; coord < mapCells.size(); coord++) {
        if (mapCells[coord] == Empty) {
            if (coord == 0 &&
                rightOf(coord) != Tree &&
                belowOf(coord) != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord == mapCells.size() - 1 &&
                       leftOf(coord) != Tree &&
                       aboveOf(coord) != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord == colNumbers.size() - 1 &&
                       leftOf(coord) != Tree &&
                       belowOf(coord) != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord == mapCells.size()-(colNumbers.size() - 1) &&
                       rightOf(coord) != Tree &&
                       aboveOf(coord) != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord %colNumbers.size() == 0 &&
                       aboveOf(coord) != Tree &&
                       rightOf(coord) != Tree &&
                       belowOf(coord) != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord < colNumbers.size() &&
                       leftOf(coord) != Tree &&
                       rightOf(coord) != Tree &&
                       belowOf(coord) != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord %rowNumbers.size() == colNumbers.size() - 1 &&
                       aboveOf(coord) != Tree &&
                       leftOf(coord) != Tree &&
                       belowOf(coord) != Tree) {
                mapCells[coord] = Blocked;
            } else if (coord >= (rowNumbers.size() - 1) * colNumbers.size() &&
                       leftOf(coord) != Tree &&
                       rightOf(coord) != Tree &&
                       aboveOf(coord) != Tree) {
                mapCells[coord] = Blocked;
            } else if (aboveOf(coord) != Tree &&
                       leftOf(coord) != Tree &&
                       belowOf(coord) != Tree &&
                       rightOf(coord) != Tree) {
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

void Field1D::blockRadiusTent(const int r) {
    if (r < (rowNumbers.size() - 1)*colNumbers.size() && belowOf(r) == Empty) {
        mapCells[belowOf(r)] = Blocked;
    }
    if (r > colNumbers.size() - 1 && aboveOf(r) == Empty) {
        mapCells[aboveOf(r)] = Blocked;
    }
    if (r %rowNumbers.size() != colNumbers.size() - 1  && rightOf(r) == Empty) {
        mapCells[rightOf(r)] = Blocked;
    }
    if (r %colNumbers.size() != 0 && leftOf(r) == Empty) {
        mapCells[leftOf(r)] = Blocked;
    }
    if (r < mapCells.size() - (colNumbers.size() - 1) && r %colNumbers.size() != colNumbers.size()-1  && diaBelowRightOf(r) == Empty) {
        mapCells[diaBelowRightOf(r)] = Blocked;
    }
    if (r < mapCells.size() - (colNumbers.size() - 1) && r %colNumbers.size() != 0 && diaBelowLeftOf(r) == Empty) {
        mapCells[diaBelowLeftOf(r)] = Blocked;
    }
    if (r >= colNumbers.size() - 1 && r %colNumbers.size() != colNumbers.size()-1 && diaUpperRightOf(r) == Empty) {
        mapCells[diaUpperRightOf(r)] = Blocked;
    }
    if (r >= colNumbers.size() - 1 && r %colNumbers.size() != 0 && diaUpperLeftOf(r) == Empty) {
        mapCells[diaUpperLeftOf(r)] = Blocked;
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

vector<tuple<int, int>> Field1D::getNeighbors(int r, int c) {
    if (c == 0 && r == 0) {
        tuple<int, int> right(r, c + 1);
        tuple<int, int> below(r + 1, c);
        vector<tuple<int, int>> result;
        result.push_back(right);
        result.push_back(below);
        return result;
    } else if (c + 1 == colNumbers.size() && r + 1 == rowNumbers.size()) {
        tuple<int, int> above(r - 1, c);
        tuple<int, int> left(r, c - 1);
        vector<tuple<int, int>> result;
        result.push_back(above);
        result.push_back(left);
        return result;
    } else if (c == 0 && r + 1 == rowNumbers.size()) {
        tuple<int, int> right(r, c + 1);
        tuple<int, int> above(r - 1, c);
        vector<tuple<int, int>> result;
        result.push_back(right);
        result.push_back(above);
        return result;
    } else if (c + 1 == colNumbers.size() && r == 0) {
        tuple<int, int> below(r + 1, c);
        tuple<int, int> left(r, c - 1);
        vector<tuple<int, int>> result;
        result.push_back(below);
        result.push_back(left);
        return result;
    } else if (c == 0 && r + 1 < rowNumbers.size()) {
        tuple<int, int> right(r, c + 1);
        tuple<int, int> below(r + 1, c);
        tuple<int, int> above(r - 1, c);
        vector<tuple<int, int>> result;
        result.push_back(right);
        result.push_back(below);
        result.push_back(above);
        return result;
    } else if (c + 1 < colNumbers.size() && r == 0) {
        tuple<int, int> right(r, c + 1);
        tuple<int, int> below(r + 1, c);
        tuple<int, int> left(r, c - 1);
        vector<tuple<int, int>> result;
        result.push_back(right);
        result.push_back(below);
        result.push_back(left);
        return result;
    } else if (c + 1 < colNumbers.size() && r + 1 == rowNumbers.size()) {
        tuple<int, int> right(r, c + 1);
        tuple<int, int> above(r - 1, c);
        tuple<int, int> left(r, c - 1);
        vector<tuple<int, int>> result;
        result.push_back(right);
        result.push_back(above);
        result.push_back(left);
        return result;
    } else if (c + 1 == colNumbers.size() && r + 1 < rowNumbers.size()) {
        tuple<int, int> below(r + 1, c);
        tuple<int, int> above(r - 1, c);
        tuple<int, int> left(r, c - 1);
        vector<tuple<int, int>> result;
        result.push_back(below);
        result.push_back(above);
        result.push_back(left);
        return result;
    } else {
        tuple<int, int> right(r, c + 1);
        tuple<int, int> below(r + 1, c);
        tuple<int, int> above(r - 1, c);
        tuple<int, int> left(r, c - 1);
        vector<tuple<int, int>> result;
        result.push_back(right);
        result.push_back(below);
        result.push_back(above);
        result.push_back(left);
        return result;
    }
}

ValidField Field1D::findOpenField() {
    for (int r = 0; r < map.size(); r++) {
        for (int c = 0; c < map[r].size(); c++) {
            if (map[r][c] == Empty) return ValidField{true, tuple<int, int>(r, c)};
        }
    }
    return ValidField{false, tuple<int, int>(-1, -1)};
}

bool Field1D::isDone() {
    for (int r = 0; r < map.size(); r++) {
        int tent = 0;
        for (int c = 0; c < map[r].size(); c++) {
            if (map[r][c] == Tent) tent++;
        }
        if (tent != rowNumbers[r]) return false;
    }
    for (int c = 0; c < map.size(); c++) {
        int tent = 0;
        for (int r = 0; r < map[0].size(); r++) {
            if (map[r][c] == Tent) tent++;
        }
        if (tent != colNumbers[c]) return false;

    }
    return true;
}

bool Field1D::assertNoNeighbouringTents(int r, int c) {
    vector neighbors = getNeighbors(r, c);
    for (auto &n : neighbors) {
        if (map[std::get<0>(n)][std::get<1>(n)] == Tent) return false;
    }
    return true;
}

bool Field1D::assertValidMove(int r, int c) {
    return assertNoNeighbouringTents(r, c) && assertValidSum(r, c)&& assertValidParity(r, c);
}

bool Field1D::assertValidSum(int r, int c) {
    int rowSum = 0;
    for (int col = 0; col < colNumbers.size(); col++) {
        if (map[r][col] == Tent) rowSum++;
    }
    if (rowSum > rowNumbers[r]) return false;

    int colSum = 0;
    for (int row = 0; row < rowNumbers.size(); row++) {
        if (map[row][c] == Tent) colSum++;
    }
    if (colSum > colNumbers[c]) return false;

    return true;
}

int Field1D::countTreesRec(int r, int c, vector<tuple<int, int>>* pred) {
    pred->push_back(tuple<int, int>(r, c));
    if (map[r][c] == Tree) {
        int parity = 1;
        for (auto &n : getNeighbors(r, c)) {
            if (!containsTuple(pred, n)) parity += countTentsRec(std::get<0>(n), std::get<1>(n), pred);
        }
        return parity;
    }
    return 0;
}

int Field1D::countTentsRec(int r, int c, vector<tuple<int, int>> *pred) {
    pred->push_back(tuple<int, int>(r, c));
    if (map[r][c] == Tent) {
        int parity = -1;
        for (auto &n : getNeighbors(r, c)) {
            if (!containsTuple(pred, n)) parity += countTreesRec(std::get<0>(n), std::get<1>(n), pred);
        }
        return parity;
    }
    return 0;
}

bool Field1D::assertValidParity(int r, int c) {
    int parity = -1;
    vector<tuple<int, int>> pred;
    pred.emplace_back(r, c);
    for (auto &n : getNeighbors(r, c)) {
        parity += countTreesRec(std::get<0>(n), std::get<1>(n), &pred);
    }
    if (parity >= 0) return true;
    return false;
}


bool Field1D::containsTuple(vector<tuple<int, int>>* vec, tuple<int, int> tup) {
    for (auto &t : *vec) {
        if (std::get<0>(t) == std::get<0>(tup) && std::get<1>(t) == std::get<1>(tup)) return true;
    }
    return false;
}

CellContent Field1D::aboveOf(int current){
    return mapCells[current - colNumbers.size()];
}

CellContent Field1D::belowOf(int current){
    return mapCells[current + colNumbers.size()];
}

CellContent Field1D::rightOf(int current){
    return mapCells[current + 1];
}

CellContent Field1D::leftOf(int current){
    return mapCells[current - 1];
}

CellContent Field1D::diaUpperRightOf(int current){
    return mapCells[current - colNumbers.size() + 1];
}

CellContent Field1D::diaUpperLeftOf(int current){
    return mapCells[current - colNumbers.size() - 1];
}

CellContent Field1D::diaBelowRightOf(int current){
    return mapCells[current + colNumbers.size() + 1];
}

CellContent Field1D::diaBelowLeftOf(int current){
    return mapCells[current + colNumbers.size() - 1];
}


