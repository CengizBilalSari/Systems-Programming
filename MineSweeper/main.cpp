/*
 * This main  file is created for the application of the  minesweeper game.
 * It utilizes  QApplication and  grid class.
 *@authors Cengiz Bilal Sarı , Berkay Buğra Gök
 *@since Date: 25.05.2024
 *
 */
#include <QApplication>
#include "Grid.h"
int main(int argc, char *argv[]) {
    //  create app, grid and put icon of the window, set its title and give necessary sizes.
    QApplication app(argc, argv);
    Grid grid;
    QIcon icon("C:\\Users\\Cengizhan\\Documents\\ColorGame\\mine.png");
    grid.setWindowIcon(icon);
    grid.setWindowTitle("Mine Sweeper");
    grid.setFixedSize(33*BOARD_WIDTH, 33 * BOARD_HEIGHT);
    grid.show();
    return app.exec();
}
