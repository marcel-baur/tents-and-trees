//
// Created by Marcel Baur on 15/11/2020.
//

#include "Field.h"
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include "Cell.h"

using namespace std;


void Field::generateFromFile(const string &path) {
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

vector<int> Field::getSize(const string &firstLine) {
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

void Field::printField() {
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

bool Field::solve() {
    blockFieldsWithoutTree();
    setClearCols();
    setClearRows();
    analyzeRowsAndCols();
    analyzeRowsAndCols();
    blockTentRadius();

    ValidField next = findOpenField();
    if (next.result) {
        return solveRec(std::get<0>(next.coord), std::get<1>(next.coord));
    } else {
        if (isDone()) return true;
        return false;
    }
}

bool Field::solveRec(int r, int c) {
    vector<vector<CellContent>> deepCopy = saveMap();
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

void Field::solveColRowForField(int r, int c) {
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

void Field::analyzeRowsAndCols() {
    for (int r = 0; r < rowNumbers.size(); r++) {
        int freeFields = 0;
        int tents = 0;
        for (int c = 0; c < colNumbers.size(); c++) {
            if (map[r][c] == Empty) freeFields++;
            if (map[r][c] == Tent) tents++;
        }
        if (freeFields + tents == rowNumbers[r]) {
            for (int c = 0; c < colNumbers.size(); c++) {
                if (map[r][c] == Empty) map[r][c] = Tent;
            }
        }
    }
    for (int c = 0; c < colNumbers.size(); c++) {
        int freeFields = 0;
        int tents = 0;
        for (int r = 0; r < rowNumbers.size(); r++) {
            if (map[r][c] == Empty) freeFields++;
            if (map[r][c] == Tent) tents++;
        }
        if (freeFields + tents == colNumbers[c]) {
            for (int r = 0; r < rowNumbers.size(); r++) {
                if (map[r][c] == Empty) map[r][c] = Tent;
            }
        }
    }
}

vector<vector<CellContent>> Field::saveMap() {
    vector<vector<CellContent>> deepCopy;
    for (int i = 0; i < rowNumbers.size(); i++) {
        vector<CellContent> row(colNumbers.size(), Empty);
        deepCopy.push_back(row);
    }
    for (int r = 0; r < map.size(); r++) {
        for (int c = 0; c < map[r].size(); c++) {
            deepCopy[r][c] = map[r][c];
        }
    }
    return deepCopy;
}


vector<vector<CellContent>> Field::restoreMap(vector<vector<CellContent>> deepCopy) {
    for (int r = 0; r < map.size(); r++) {
        for (int c = 0; c < map[r].size(); c++) {
            map[r][c] = deepCopy[r][c];
        }
    }
    return deepCopy;
}



void Field::setClearRows() {
    for (int i = 0; i < rowNumbers.size(); i++) {
        if (rowNumbers[i] == 0) {
            for (int j = 0; j < map[i].size(); j++) {
                if (map[i][j] == Empty) map[i][j] = Blocked;
            }
        }
    }
}

void Field::setClearCols() {
    for (int i = 0; i < colNumbers.size(); i++) {
        if (colNumbers[i] == 0) {
            for (auto & j : map) {
                if (j[i] == Empty) j[i] = Blocked;
            }
        }
    }
}

void Field::blockFieldsWithoutTree() {
    for (int r = 0; r < rowNumbers.size(); r++) {
        for (int c = 0; c < colNumbers.size(); c++) {
            if (map[r][c] == Empty) {
                if (c == 0 && r == 0 &&
                    map[r][c + 1] != Tree &&
                    map[r + 1][c] != Tree) {
                    map[r][c] = Blocked;
                } else if (c == colNumbers.size() - 1 && r == rowNumbers.size() - 1 &&
                           map[r][c - 1] != Tree &&
                           map[r - 1][c] != Tree) {
                    map[r][c] = Blocked;
                } else if (c == colNumbers.size() - 1 && r == 0 &&
                           map[r][c - 1] != Tree &&
                           map[r + 1][c] != Tree) {
                    map[r][c] = Blocked;
                } else if (c == 0 && r == rowNumbers.size() - 1 &&
                           map[r][c + 1] != Tree &&
                           map[r - 1][c] != Tree) {
                    map[r][c] = Blocked;
                } else if (c == 0 && r > 0 &&
                           map[r - 1][c] != Tree &&
                           map[r][c + 1] != Tree &&
                           map[r + 1][c] != Tree) {
                    map[r][c] = Blocked;
                } else if (r == 0 && c > 0 &&
                           map[r][c - 1] != Tree &&
                           map[r][c + 1] != Tree &&
                           map[r + 1][c] != Tree) {
                    map[r][c] = Blocked;
                } else if (c == colNumbers.size() - 1 && r < rowNumbers.size() - 1 && r > 0 &&
                           map[r - 1][c] != Tree &&
                           map[r][c - 1] != Tree &&
                           map[r + 1][c] != Tree) {
                    map[r][c] = Blocked;
                } else if (r == rowNumbers.size() - 1 && c < colNumbers.size() - 1 && c > 0 &&
                           map[r][c - 1] != Tree &&
                           map[r][c + 1] != Tree &&
                           map[r - 1][c] != Tree) {
                    map[r][c] = Blocked;
                } else if (r < rowNumbers.size() - 1 && r > 0 && c < colNumbers.size() - 1 && c > 0 &&
                           map[r - 1][c] != Tree &&
                           map[r][c - 1] != Tree &&
                           map[r + 1][c] != Tree &&
                           map[r][c + 1] != Tree) {
                    map[r][c] = Blocked;
                }
            }
        }
    }
}

void Field::blockTentRadius() {
    for (int r = 0; r < rowNumbers.size(); r++) {
        for (int c = 0; c < colNumbers.size(); c++) {
            if (map[r][c] == Tent) {
                blockRadiusTent(r, c);
            }
        }
    }
}

void Field::blockRadiusTent(const int r, const int c) {
    if (r + 1 < rowNumbers.size() && map[r + 1][c] == Empty) {
        map[r + 1][c] = Blocked;
    }
    if (r - 1 >= 0 && map[r - 1][c] == Empty) {
        map[r - 1][c] = Blocked;
    }
    if (c + 1 < colNumbers.size() && map[r][c + 1] == Empty) {
        map[r][c + 1] = Blocked;
    }
    if (c - 1 >= 0 && map[r][c - 1] == Empty) {
        map[r][c - 1] = Blocked;
    }
    if (r + 1 < rowNumbers.size() && c + 1 < colNumbers.size() && map[r + 1][c + 1] == Empty) {
        map[r + 1][c + 1] = Blocked;
    }
    if (r + 1 < rowNumbers.size() && c - 1 >= 0
        && map[r + 1][c - 1] == Empty) {
        map[r + 1][c - 1] = Blocked;
    }
    if (r - 1 >= 0 && c + 1 < colNumbers.size() && map[r - 1][c + 1] == Empty) {
        map[r - 1][c + 1] = Blocked;
    }
    if (r - 1 >= 0 && c - 1 >= 0 && map[r - 1][c - 1] == Empty) {
        map[r - 1][c - 1] = Blocked;
    }
}


size_t Field::split(const std::string &txt, std::vector<std::string> &strs, char ch) {
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

vector<tuple<int, int>> Field::getNeighbors(int r, int c) {
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

ValidField Field::findOpenField() {
    for (int r = 0; r < map.size(); r++) {
        for (int c = 0; c < map[r].size(); c++) {
            if (map[r][c] == Empty) return ValidField{true, tuple<int, int>(r, c)};
        }
    }
    return ValidField{false, tuple<int, int>(-1, -1)};
}

bool Field::isDone() {
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

bool Field::assertNoNeighbouringTents(int r, int c) {
    vector neighbors = getNeighbors(r, c);
    for (auto &n : neighbors) {
        if (map[std::get<0>(n)][std::get<1>(n)] == Tent) return false;
    }
    return true;
}

bool Field::assertValidMove(int r, int c) {
    return assertNoNeighbouringTents(r, c) && assertValidSum(r, c)&& assertValidParity(r, c);
}

bool Field::assertValidSum(int r, int c) {
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

int Field::countTreesRec(int r, int c, vector<tuple<int, int>>* pred) {
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

int Field::countTentsRec(int r, int c, vector<tuple<int, int>> *pred) {
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

bool Field::assertValidParity(int r, int c) {
    int parity = -1;
    vector<tuple<int, int>> pred;
    pred.emplace_back(r, c);
    for (auto &n : getNeighbors(r, c)) {
        parity += countTreesRec(std::get<0>(n), std::get<1>(n), &pred);
    }
    if (parity >= 0) return true;
    return false;
}


bool Field::containsTuple(vector<tuple<int, int>>* vec, tuple<int, int> tup) {
    for (auto &t : *vec) {
        if (std::get<0>(t) == std::get<0>(tup) && std::get<1>(t) == std::get<1>(tup)) return true;
    }
    return false;
}

