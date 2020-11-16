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

void Field::generateFromFile(const string& path) {
    ifstream file;
    file.open(path, ios::in);
    cout << "Reading file " << path << "\n";
    if (file.is_open()) {
        string line;
        getline(file, line);
        int fieldSize = getSize(line);
        for (int i = 0; i < fieldSize; i++) {
            vector<CellContent> row (fieldSize, Empty);
            map.push_back(row);
        }
        int currLine = 0;
        char delim = ' ';
        while (getline(file, line)) {
            cout << currLine << '\n';
            if (currLine < fieldSize) {
                // GameField representation
                cout << line << '\n';
                string rowConfig = line.substr(0, line.find(delim));
                cout << rowConfig << '\n';
                cout << line.substr(line.find(delim)) << '\n';
                int rowNumber = stoi(line.substr(line.find(delim)));
                rowNumbers.push_back(rowNumber);
                cout << rowConfig << '\n';
                for (int i = 0; i < fieldSize; i++) {
                    if (rowConfig[i] == '.') {
                        map[currLine][i] = Empty;
                    } else if (rowConfig[i] == 'T') {
                        map[currLine][i] = Tree;
                    } else {
                        throw "Bad File Format (Cell is neither Tree nor Empty)";
                    }
                }
            } else if (currLine == fieldSize) {
                // Column Number definitions
                for (int i = 0; i < fieldSize * 2; i+=2) {
                    int colNumber = stoi(line.substr(i, line.find(' ')));
                    colNumbers.push_back(colNumber);
                    cout << "Col Number: " << colNumber << '\n';
                }
            }
            cout << line << "\n";
            currLine++;
        }
        printField();
    } else {
        cout << "Could not open file." << '\n';
    }
}

int Field::getSize(const string& firstLine) {
    vector<string> info;
    istringstream iss(firstLine);
    for (string l; iss >> l; )
        info.push_back(l);
    cout << info[0] << "\n";
    return stoi(info[0]);
}

void Field::printField() {
    for (auto & i : map) {
        for (auto & j : i) {
            switch (j) {
                case Tent: cout<<"^";
                    break;
                case Tree: cout << "T";
                break;
                case Empty: cout << ".";
                break;
                default: throw "Bad map config";
            }
        }
        cout << '\n';
    }
}