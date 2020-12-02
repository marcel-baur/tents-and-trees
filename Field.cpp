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
                    cout << "x" << ' ';
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

void Field::solve() {
    checkForCompletedRows();
    checkForCompletedCols();

    while ((rowNumbers.size() - 1 != checkForCompletedRows() && colNumbers.size() - 1 != checkForCompletedCols())) {
        setClearRows();
        setClearCols();
        blockFieldsWithoutTree();
        solveRows();
        solveCols();
        blockTentRadius();
        blockTreeWithTentRadius();
        checkForCompletedRows();
        checkForCompletedCols();
        placeTentForSingularTree();
        analyzeTents(); // TODO: consolidate into this function
        printField();

        // for debugging
        cout << "solved-cols: " << checkForCompletedCols() << " space " << '\n';
        cout << "solved-rows: " << checkForCompletedRows() << " space " << '\n';
    }
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
            for (int j = 0; j < map.size(); j++) {
                if (map[j][i] == Empty) map[j][i] = Blocked;
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

int Field::emptyFieldsInRow(int row) {
    int result = 0;
    for (int i = 0; i < map[row].size(); i++) {
        if (map[row][i] == Empty) {
            result++;
        }
    }
    return result;
}

int Field::emptyFieldsInCol(int col) {
    int result = 0;
    for (int i = 0; i < map.size(); i++) {
        if (map[i][col] == Empty) {
            result++;
        }
    }
    return result;
}


int Field::tentsInRow(int row) {
    int result = 0;
    for (int i = 0; i < map[row].size(); i++) {
        if (map[row][i] == Tent) {
            result++;
        }
    }
    return result;
}

int Field::tentsInCol(int col) {
    int result = 0;
    for (int i = 0; i < map.size(); i++) {
        if (map[i][col] == Tent) {
            result++;
        }
    }
    return result;
}


bool Field::solveRows() {
    bool changed = false;
    for (int i = 0; i < map.size(); i++) {
        int rowNumber = rowNumbers[i];
        int emptyFields = emptyFieldsInRow(i);
        int tents = tentsInRow(i);
        if (rowNumber - tents == emptyFields && rowNumber != 0) {
            for (int j = 0; j < map[i].size(); j++) {
                if (map[i][j] == Empty) map[i][j] = Tent;
                changed = true;
            }
        }
    }
    return changed;
}


bool Field::solveCols() {
    bool changed = false;
    for (int col = 0; col < map[0].size(); col++) {
        int colNumber = colNumbers[col];
        int emptyFields = emptyFieldsInCol(col);
        int tents = tentsInCol(col);
        // cout << tents << '\n';
        if (colNumber - tents == emptyFields && colNumber != 0) {
            for (int j = 0; j < map.size(); j++) {
                if (map[j][col] == Empty) map[j][col] = Tent;
                changed = true;
            }
        }
    }
    return changed;
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

void Field::blockTreeWithTentRadius() {
    for (int r = 0; r < rowNumbers.size(); r++) {
        for (int c = 0; c < colNumbers.size(); c++) {
            if (map[r][c] == Tree) {
                if (checkRadiusFor(Tent, r, c)) {
                    blockRadiusTree(r, c);
                }
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

void Field::blockRadiusTree(int r, int c) {
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
}

bool Field::checkRadiusFor(CellContent cellContent, int r, int c) {

    if (r + 1 < rowNumbers.size() && map[r + 1][c] == cellContent) {
        return true;
    } else if (r - 1 >= 0 && map[r - 1][c] == cellContent) {
        return true;
    } else if (c + 1 < colNumbers.size() && map[r][c + 1] == cellContent) {
        return true;
    } else if (c - 1 >= 0 && map[r][c - 1] == cellContent) {
        return true;
    } else {
        return false;
    }
}

int Field::checkForCompletedRows() {
    int completedRows = 0;
    for (int row = 0; row < map.size(); row++) {
        int rowNumber = rowNumbers[row];
        int tentCounter = 0;

        for (int col = 0; col < map[row].size(); col++) {
            if (map[row][col] == Tent) tentCounter++;

        }
        if (tentCounter == rowNumber) {
            for (int col = 0; col < map[row].size(); col++) {
                if (map[row][col] == Empty) {
                    map[row][col] = Blocked;
                }
            }

            completedRows++;
        }
    }
    return completedRows;
}

int Field::checkForCompletedCols() {
    int completedCols = 0;
    for (int col = 0; col < colNumbers.size(); col++) {
        int colNumber = colNumbers[col];
        int tentCounter = 0;

        for (int row = 0; row < rowNumbers.size(); row++) {
            if (map[row][col] == Tent) tentCounter++;
        }

        if (tentCounter == colNumber) {
            for (int row = 0; row < rowNumbers.size(); row++) {
                if (map[row][col] == Empty) {
                    map[row][col] = Blocked;
                }
            }
            completedCols++;
        }
    }
    return completedCols;
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

/**
 * Find trees that can only have tents in one single place and put the tent there
 */
void Field::placeTentForSingularTree() {
    for (int r = 0; r < map.size(); r++) {
        for (int c = 0; c < map[r].size(); c++) {
            if (map[r][c] != Tree) continue;
            checkTreeFieldForSingleTent(r, c);
        }
    }
}

bool Field::isFieldTreeOrBlocked(CellContent cellContent) {
    return cellContent == Tree || cellContent == Blocked;
}

void Field::checkTreeFieldForSingleTent(int r, int c) {
//    cout << r << c << '\n';
//    cout << rowNumbers.size() << colNumbers.size() << '\n';
    if (c == 0 && r == 0) {
        if (isFieldTreeOrBlocked(map[r + 1][c]) && map[r][c + 1] == Empty) map[r][c + 1] = Tent;
        if (isFieldTreeOrBlocked(map[r][c + 1]) && map[r + 1][c] == Empty) map[r + 1][c] = Tent;
    } else if (c + 1 == colNumbers.size() && r + 1 == rowNumbers.size()) {
        if (isFieldTreeOrBlocked(map[r - 1][c]) && map[r][c - 1] == Empty) map[r][c - 1] = Tent;
        if (isFieldTreeOrBlocked(map[r][c - 1]) && map[r - 1][c] == Empty) map[r - 1][c] = Tent;
    } else if (c == 0 && r + 1 == rowNumbers.size()) {
        cout << "hello";
        if (isFieldTreeOrBlocked(map[r - 1][c]) && map[r][c + 1] == Empty) map[r][c + 1] = Tent;
        if (isFieldTreeOrBlocked(map[r][c + 1]) && map[r - 1][c] == Empty) map[r - 1][c] = Tent;
    } else if (c + 1 == colNumbers.size() && r == 0) {
        if (isFieldTreeOrBlocked(map[r = 1][c]) && map[r][c - 1] == Empty) map[r][c - 1] = Tent;
        if (isFieldTreeOrBlocked(map[r][c - 1]) && map[r = 1][c] == Empty) map[r = 1][c] = Tent;
    } else if (c == 0 && r + 1 < rowNumbers.size()) {
        if (isFieldTreeOrBlocked(map[r + 1][c]) && isFieldTreeOrBlocked(map[r - 1][c]) && map[r][c + 1] == Empty)
            map[r][c + 1] = Tent;
        if (isFieldTreeOrBlocked(map[r][c + 1]) && isFieldTreeOrBlocked(map[r - 1][c]) && map[r + 1][c] == Empty)
            map[r + 1][c] = Tent;
        if (isFieldTreeOrBlocked(map[r + 1][c]) && isFieldTreeOrBlocked(map[r][c + 1]) && map[r - 1][c] == Empty)
            map[r - 1][c] = Tent;
    } else if (c + 1 < colNumbers.size() && r == 0) {
        cout << 'here?' << '\n';
        if (isFieldTreeOrBlocked(map[r + 1][c]) && isFieldTreeOrBlocked(map[r][c - 1]) && map[r][c + 1] == Empty)
            map[r][c + 1] = Tent;
        if (isFieldTreeOrBlocked(map[r][c + 1]) && isFieldTreeOrBlocked(map[r][c - 1]) && map[r + 1][c] == Empty)
            map[r + 1][c] = Tent;
        if (isFieldTreeOrBlocked(map[r + 1][c]) && isFieldTreeOrBlocked(map[r][c + 1]) && map[r][c - 1] == Empty)
            map[r][c - 1] = Tent;
    } else if (c + 1 < colNumbers.size() && r + 1 == rowNumbers.size()) {
        if (isFieldTreeOrBlocked(map[r - 1][c]) && isFieldTreeOrBlocked(map[r][c - 1]) && map[r][c + 1] == Empty)
            map[r][c + 1] = Tent;
        if (isFieldTreeOrBlocked(map[r][c + 1]) && isFieldTreeOrBlocked(map[r][c - 1]) && map[r - 1][c] == Empty)
            map[r - 1][c] = Tent;
        if (isFieldTreeOrBlocked(map[r - 1][c]) && isFieldTreeOrBlocked(map[r][c + 1]) && map[r][c - 1] == Empty)
            map[r][c - 1] = Tent;
    } else if (c + 1 == colNumbers.size() && r + 1 < rowNumbers.size()) {
        if (isFieldTreeOrBlocked(map[r + 1][c]) && isFieldTreeOrBlocked(map[r - 1][c]) && map[r][c - 1] == Empty)
            map[r][c - 1] = Tent;
        if (isFieldTreeOrBlocked(map[r][c - 1]) && isFieldTreeOrBlocked(map[r - 1][c]) && map[r + 1][c] == Empty)
            map[r + 1][c] = Tent;
        if (isFieldTreeOrBlocked(map[r + 1][c]) && isFieldTreeOrBlocked(map[r][c - 1]) && map[r - 1][c] == Empty)
            map[r - 1][c] = Tent;
    }  else {
        if (isFieldTreeOrBlocked(map[r + 1][c]) && isFieldTreeOrBlocked(map[r][c - 1]) &&
            isFieldTreeOrBlocked(map[r - 1][c]) && map[r][c + 1] == Empty)
            map[r][c + 1] = Tent;
        if (isFieldTreeOrBlocked(map[r][c + 1]) && isFieldTreeOrBlocked(map[r][c - 1]) &&
            isFieldTreeOrBlocked(map[r - 1][c]) && map[r + 1][c] == Empty)
            map[r + 1][c] = Tent;
        if (isFieldTreeOrBlocked(map[r + 1][c]) && isFieldTreeOrBlocked(map[r][c + 1]) &&
            isFieldTreeOrBlocked(map[r - 1][c]) && map[r][c - 1] == Empty)
            map[r][c - 1] = Tent;
        if (isFieldTreeOrBlocked(map[r + 1][c]) && isFieldTreeOrBlocked(map[r][c + 1]) &&
            isFieldTreeOrBlocked(map[r][c - 1]) && map[r - 1][c] == Empty)
            map[r - 1][c] = Tent;
    }
}

void Field::analyzeTents() {
    for (int r = 0; r < map.size(); r++) {
        for (int c = 0; c < map[r].size(); c++) {
            if (map[r][c] != Tree) continue;
            eliminateFieldByNeighbor(r, c);
        }
    }
}

void Field::eliminateFieldByNeighbor(int r, int c) {
    vector<tuple<int, int>> list = getNeighbors(r, c);
    vector<tuple<int, int>> emptyNeighbors;
    for (auto &elem : list) {
        if (map[std::get<0>(elem)][std::get<1>(elem)] == Empty) emptyNeighbors.push_back(elem);
    }

    int neighbors = emptyNeighbors.size();
    std::map<tuple<int, int>, int> countMap;

    for (auto &en : emptyNeighbors) {
        vector<tuple<int, int>> neighborList = getNeighbors(std::get<0>(en), std::get<1>(en));
        for (auto &nb : neighborList) {
            auto res = countMap.insert(std::pair<tuple<int, int>, int>(nb, 1));
            if (res.second == false) res.first->second++;
        }
    }

    for (auto & elem : countMap)
    {
        if (elem.second == neighbors)
        {
            tuple<int, int> coord = elem.first;
            if (map[std::get<0>(coord)][std::get<1>(coord)] == Empty) map[std::get<0>(coord)][std::get<1>(coord)] = Blocked;
        }
    }
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
        cout << "hello";
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
