//
// Created by Marcel Baur on 15/11/2020.
//

#include "Field.h"
#include <fstream>
#include <vector>
#include <string>
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
                    for (int i = 0; i < line.size(); i += 2) {
                        string extraction = line.substr(i, line.find(' '));
                        if (extraction.size() > 1) {
                            i = i + (extraction.size() - 1);
                        }
                        int colNumber = stoi(extraction);
                        colNumbers.push_back(colNumber);
                    }
                } catch (const exception &e) {
                    cout << "ERROR in colNumbers";
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
    for (auto &i : map) {
        for (auto &j : i) {
            switch (j) {
                case Tent:
                    cout << "^";
                    break;
                case Tree:
                    cout << "T";
                    break;
                case Empty:
                    cout << ".";
                    break;
                case Blocked:
                    cout << "x";
                    break;
                default:
                    cout << "?";
            }
        }
        cout << '\n';
    }
}

void Field::solve() {
    setClearRows();
    setClearCols();
    blockFieldsWithoutTree();
    solveRows();
    solveCols();
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
                } else if (c == colNumbers.size() - 1 && r == rowNumbers.size() - 1 &&
                           map[r][c - 1] != Tree &&
                           map[r - 1][c] != Tree) {
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

void Field::solveRows() {
    for (int i = 0; i < map.size(); i++) {
        int rowNumber = rowNumbers[i];
        int emptyFields = emptyFieldsInRow(i);
        if (rowNumber == emptyFields && rowNumber != 0) {
            for (int j = 0; j < map[i].size(); j++) {
                if (map[i][j] == Empty) map[i][j] = Tent;
            }
        }
    }
}

void Field::solveCols() {
    for (int col = 0; col < map[0].size(); col++) {
        int colNumber = colNumbers[col];
        int emptyFields = emptyFieldsInCol(col);
        if (colNumber == emptyFields && colNumber != 0) {
            for (int j = 0; j < map.size(); j++) {
                if (map[j][col] == Empty) map[j][col] = Tent;
            }
        }
    }
}
