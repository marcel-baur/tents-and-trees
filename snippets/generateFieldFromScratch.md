
    vector<vector<CellContent>> copy = saveMap();
    bool validMap = false;
    do {

        restoreMap(copy);
        for (int i = 0; i < amountOfTrees; i++) {
            bool tentPlaced = false;
            while(!tentPlaced) {
                int randRow = dis_rows(generator);
                int randCol = dis_cols(generator);
                if (map[randRow][randCol] != Empty ||
                    !neighborsContainField(randRow, randCol, Tree) ||
                    !assertNoNeighbouringTents(randRow, randCol)
                    ) {
                    continue;
                    cout << "Not valid \n";
                }
                map[randRow][randCol] = Tent;
                tentPlaced = true;
            }
        }

        rowNumbers.clear();
        colNumbers.clear();

        for (auto &row : map) {
            int trees = 0;
            for (auto &col : row) {
                if (col == Tent) trees++;
            }
            rowNumbers.push_back(trees);
        }

        for (int i = 0; i < c; i++) {
            int trees = 0;
            for (int j = 0; j < r; j++) {
                if (map[j][i] == Tent) trees++;
            }
            colNumbers.push_back(trees);
        }
        for (auto &row : map) {
            int trees = 0;
            for (auto &col : row) if (col == Empty) col = Blocked;

        }
        validMap = true;
        for (int i = 0; i < c; i++) {
            for (int j = 0; j < r; j++) {
                if(map[j][i] == Tent) validMap = validMap && assertValidParity(j, i);
            }
        }
        printField();
        cout << !validMap << !isDone() << '\n';

    } while (!isDone() || !validMap);
