//
// Created by Marcel Baur on 16/11/2020.
//

#ifndef SAT_CELL_H
#define SAT_CELL_H

enum CellContent {
    Tree = 0, Tent = 1, Empty = 2, Blocked = 3
};
class Cell {
private:
    CellContent content;
public:
    CellContent getContent();
    void setContent();
    int computeCell();
    int getRow();
    int getCol();
};


#endif //SAT_CELL_H
