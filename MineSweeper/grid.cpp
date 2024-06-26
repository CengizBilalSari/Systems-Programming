/*
 * Grid class consists of functions and variables to control and implement the game correctly.
 * This class provides functionality for initializing the game board, placing mines, handling user interactions,
 * and managing the game state.
 * Features:
 *   Initialization of the game board and mine placement.
 *   Handling left and right click event for revaling cells and placing flags.
 *   Hint to asist the player
 *   Displaying the current score and checking win/loss conditions
 *   Restart button
 *@authors Cengiz Bilal Sarı , Berkay Buğra Gök
 *@since Date: 25.05.2024
 *
 */
#include "Grid.h"
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include <QImage>
#include <QList>
#include <cstdlib>
#include <ctime>
#include <QMessageBox>
#include <queue>
#include <QMessageBox>
#include <QMouseEvent>
#include <QContextMenuEvent>

Grid::Grid(QWidget *parent) : QWidget(parent) {
    // Constructor : start the game with initial conditions,set up the UI
        initializeBoard();
        placeMines();
        initializeClickableList();
        initializeMineCount();
        // connect buttons and functions
        restartButton = new QPushButton("Restart");
        connect(restartButton, &QPushButton::clicked, this, &Grid::restartGame);
        hintButton = new QPushButton("Hint");
        connect(hintButton, &QPushButton::clicked, this, &Grid::giveHint);

        // put layouts and labels
        buttonLayout = new QHBoxLayout;
        scoreLabel = new QLabel("Score: 0"); // Initial score is 0
        buttonLayout->addWidget(scoreLabel);
        buttonLayout->addWidget(restartButton);
        buttonLayout->addWidget(hintButton);
        layout = new QGridLayout;
        mainLayout= new QVBoxLayout;
        mainLayout->addLayout(buttonLayout); // Add button layout above
        mainLayout->addLayout(layout);
        setLayout(mainLayout);

        // Load the PNG images
        buttonImage0 = QPixmap("./assets/0.png");
        QSize newSize = buttonImage0.size() * 2;
        buttonImage0 = QPixmap("./assets/0.png").scaled(newSize);
        buttonImage1 = QPixmap("./assets/1.png").scaled(newSize);
        buttonImage2 = QPixmap("./assets/2.png").scaled(newSize);
        buttonImage3 = QPixmap("./assets/3.png").scaled(newSize);
        buttonImage4 = QPixmap("./assets/4.png").scaled(newSize);
        buttonImage5 = QPixmap("./assets/5.png").scaled(newSize);
        buttonImage6 = QPixmap("./assets/6.png").scaled(newSize);
        buttonImage7 = QPixmap("./assets/7.png").scaled(newSize);
        buttonImage8 = QPixmap("./assets/8.png").scaled(newSize);
        buttonImageMine = QPixmap("./assets/mine.png").scaled(newSize);
        buttonImageFlag = QPixmap("./assets/flag.png").scaled(newSize);
        buttonImageHint = QPixmap("./assets/hint.png").scaled(newSize);
        buttonImageEmpty = QPixmap("./assets/empty.png").scaled(newSize);

        layout->setHorizontalSpacing(0);
        layout->setVerticalSpacing(5+ ceil(((1/BOARD_HEIGHT)*50)));


        for (int row = 0; row < BOARD_HEIGHT; ++row) {
            for (int col = 0; col < BOARD_WIDTH; ++col) {

                // Create a QPushButton and set its icon to the button image
                QPushButton *button = new QPushButton;
                button->setFixedSize(30,30);

                button->setIcon(buttonImageEmpty);
                // connect right and left click
                button->setContextMenuPolicy(Qt::CustomContextMenu);
                connect(button, &QPushButton::clicked, this, &Grid::handleButtonClick);
                connect(button, &QPushButton::customContextMenuRequested, this, &Grid::rightClicked);
                button->setIconSize(buttonImage0.size()); // Set the icon size to match the image size
                layout->addWidget(button, row, col); // Add the button to the layout
            }
        }
    }

    void Grid::initializeBoard() {
        // initialize the board with giving zeros to board and flag list, also giving -1 to lastHint to indicate the hint is not opened.
        lastHint[0]=-1;
        lastHint[1]=-1;
        for (int i = 0; i < BOARD_HEIGHT; i++) {
            for (int j = 0; j < BOARD_WIDTH; j++) {
                board[i][j] = 0;
                flagsList[i][j] = 0;
            }
        }
    }
    void Grid::placeMines() {
        // Set seed using current time
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        for (int i = 0; i < MINE_COUNT; i++) {
            int x = rand() % BOARD_HEIGHT;
            int y = rand() % BOARD_WIDTH;
            if (board[x][y] == 9) {
                i--;
            } else {
                board[x][y] = 9;
            }
        }
    }
    void Grid::initializeClickableList() {
        // initialize clickableList with 0,1 and 9, which means not clickable, clickable and there is mine.
        for (int i = 0; i < BOARD_HEIGHT; i++) {
            for (int j = 0; j < BOARD_WIDTH; j++) {
                if (board[i][j] != 9) isClickableList[i][j] = 1;
                else isClickableList[i][j] = 9;
            }
        }
    }
    void Grid::initializeMineCount() {
        // put mine counts to every places in the board
        for (int i = 0; i < BOARD_HEIGHT; i++) {
            for (int j = 0; j < BOARD_WIDTH; j++) {
                int mineCount = 0;
                if (board[i][j] == 9) {
                    continue;
                }
                for (int k = -1; k < 2; k++) {
                    for (int l = -1; l < 2; l++) {
                        if (i + k < 0 || i + k >= BOARD_HEIGHT || j + l < 0 || j + l >= BOARD_WIDTH) {
                            continue;
                        }
                        if (board[i + k][j + l] == 9) {
                            mineCount++;
                        }
                    }
                }
                board[i][j] = mineCount;
            }
        }
    }

    bool Grid::isMine(int x, int y) { // check if the cell contains a mine
        if (board[x][y] == 9) {
            return true;
        } else {
            return false;
        }
    }
    bool Grid::isClickable(int x, int y) { // check whether the cell is clickable or not
        if (isClickableList[x][y] !=0) {
            return true;
        } else {
            return false;
        }
    }
    int Grid:: keepingScore(){  // keep the score with revealed cell to show to player.
        int score=0;
        for(int i=0;i<BOARD_HEIGHT;i++){
            for(int j=0;j<BOARD_WIDTH;j++){
                if(isClickableList[i][j]==0)
                    score++;
            }
        }
        return score;
    }
    void Grid:: bfs(int x, int y) {
        // this is basic breadth-first search algorithm to reveal cells
        // it opens neighbor clickable cells if the revealed button's mine count is 0.
        std::queue<std::pair<int, int>> queue;
        queue.push({x, y});
        std::vector<std::vector<bool>> visited(BOARD_HEIGHT, std::vector<bool>(BOARD_WIDTH, false));
        while (!queue.empty()) {
            int currentX = queue.front().first;
            int currentY = queue.front().second;
            queue.pop();
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int newX = currentX + i;
                    int newY = currentY + j;
                    if (newX >= 0 && newX < BOARD_HEIGHT && newY >= 0 && newY < BOARD_WIDTH &&
                        isClickableList[newX][newY] == 1 && board[newX][newY] != 9) {
                        isClickableList[newX][newY] = 0;
                        if (board[newX][newY] ==0&&!visited[newX][newY]) {
                            QPushButton* button = getButtonAt(newX, newY);
                            button->setIcon(buttonImage0);
                            visited[newX][newY] = true;
                            queue.push({newX, newY});
                        }
                        else {
                            QPushButton* button = getButtonAt(newX, newY);
                            puttingImage(button,newX,newY);
                        }
                    }
                }
            }
        }
    }
;

void Grid::deleteArray(int** array,int j) {
    for (int i = 0; i < j; ++i) {
        delete[] array[i]; // Deallocate memory for each row
    }
    delete[] array; // Deallocate memory for the array itself
}

int** Grid::neighborCell(int**  array ,int currentX, int currentY ){  // calculate the cells which is clickable
    int count=0;
    // keep the coordinates of the neighbor cells if it is possible and logical to click it, other than that put -1.
    for (int i = 0; i < 8; ++i) {
        array[i] = new int[2]; // Allocate memory for each row
    }
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i==0 && j==0){
                continue;
            }
            int newX = currentX + i;
            int newY = currentY + j;
            if (newX >= 0 && newX < BOARD_HEIGHT && newY >= 0 && newY < BOARD_WIDTH && isClickableList[newX][newY] !=0){
                array[count][0]=newX;
                array[count][1]=newY;
            }else{
                array[count][0]=-1;
                array[count][1]=-1;
            }
            count++;
        }
    }
    return array;
}
int Grid::findCorrectPlace(int* correctLocation,int array[][BOARD_WIDTH],int array2[][BOARD_WIDTH],int currentX,int currentY){
    // find correct place to give hint to player

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int newX = currentX + i;
            int newY = currentY + j;
            if(newX >= 0 && newX < BOARD_HEIGHT && newY >= 0 && newY < BOARD_WIDTH&& array[newX][newY]!=9 && array2[newX][newY]==1){
                correctLocation[0]=newX;
                correctLocation[1]=newY;
                return 1;
            }
        }
    } return 0;
}
int Grid::numberOfClickableCell(int** array){
    int number=0;
    for(int i=0;i<8;i++){
        if(array[i][0]!=-1){
            number++;
        }
    }
    return number;
}
int Grid::mineNumber(int array[][BOARD_WIDTH],int currentX,int currentY){
    int number=0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int newX = currentX + i;
            int newY = currentY + j;

            if(array[newX][newY]==9&& newX >= 0 && newX < BOARD_HEIGHT && newY >= 0 && newY < BOARD_WIDTH){
                number++;
            }
        }
    } return number;
}

void Grid::rightClicked() {
    // if player click the right click and there is already o flag, remove it , if there is no flag already put the flag to given button.
    QPushButton *rightClickedButton = qobject_cast<QPushButton*>(sender());
    // Find the position of the clicked button in the layout
    int row = -1, col = -1;
    for (int i = 0; i < layout->rowCount(); ++i) {
        for (int j = 0; j < layout->columnCount(); ++j) {
            if (layout->itemAtPosition(i, j)->widget() == rightClickedButton) {
                row = i;
                col = j;
                break;
            }
        }
        if (row != -1 && col != -1)
            break;
    }
    if(isClickableList[row][col]!=0){
        if (flagsList[row][col] == 0) {
            rightClickedButton->setIcon(buttonImageFlag);
            flagsList[row][col] = 1;
        }
        else { // a flag is already at the coordinate
            rightClickedButton->setIcon(buttonImageEmpty);
            flagsList[row][col] = 0;
        }
    }

}
void Grid::puttingImage(QPushButton *button, int revealRow,int revealCol){
    if (board[revealRow][revealCol] == 1) {
        button->setIcon(buttonImage1);
    }
    else if (board[revealRow][revealCol] == 2) {
        button->setIcon(buttonImage2);
    }
    else if (board[revealRow][revealCol] == 3) {
        button->setIcon(buttonImage3);
    }
    else if (board[revealRow][revealCol] == 4) {
        button->setIcon(buttonImage4);
    }
    else if (board[revealRow][revealCol] == 5) {
        button->setIcon(buttonImage5);
    }
    else if (board[revealRow][revealCol] == 6) {
        button->setIcon(buttonImage6);
    }
    else if (board[revealRow][revealCol] == 7) {
        button->setIcon(buttonImage7);
    }
    else if (board[revealRow][revealCol] == 8) {
        button->setIcon(buttonImage8);

    }}



/*Give Hint:
     * Find the possible mines from the eyes of the player, logic is like this:
     * Look at the mine count at the cell, then explore its neighbors, if its mine count and clickable neighbors number are same,
     * then it means that player can ensure the places where there are mines.
     * Keep these places in the keepingMine array to utilize it at the second part.
     * After the first main for loop part, all mines from the eyes of the player determined then now we can find the correct place to hint
     * If the number of mine is equal to number on the board, then  try to find correct place which has to be clickable and the only places
     * that player can click in the neighbors area.
     *
     */
int Grid::giveHint(){
    //check if the hint button was clicked recently and if the hinted cell is not flagged
    if (lastHint[0] != -1 && flagsList[lastHint[0]][lastHint[1]]==0) {
        // if the hint is opened and any flag does not put there, reveal the cell

        int revealRow = lastHint[0];
        int revealCol = lastHint[1];

        //reveal the cell which is given as hint, set the lastHint list to [-1,-1]
        QPushButton *button = getButtonAt(revealRow,revealCol);

        if (board[revealRow][revealCol] == 9) {
            button->setIcon(buttonImageMine);
        }
        else if (board[revealRow][revealCol] == 0) {
            button->setIcon(buttonImage0);
            bfs(revealRow,revealCol);
        }
         isClickableList[revealRow][revealCol] = 0;
        int newScore = keepingScore();
        scoreLabel->setText("Score: " + QString::number(newScore));
        puttingImage(button,revealRow,revealCol);
        if (newScore == winningCondition) {
            winningGame(newScore);
        }

        lastHint[0] = -1;
        lastHint[1] = -1;
        return 1;

    }
    // if hint would not given already....
    int  keepingMine[BOARD_HEIGHT][BOARD_WIDTH];  //  find mines from the eyes of the player
    for(int i=0;i<BOARD_HEIGHT;i++){
        for(int j=0;j<BOARD_WIDTH;j++){
            keepingMine[i][j]=-1;
        }
    }

    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            if(isClickableList[row][col]==0 && board[row][col]!=0){
                int** neighborsArray= new int*[8];
                neighborCell(neighborsArray,row,col);
                int number= numberOfClickableCell(neighborsArray);
                if(number==board[row][col]){
                    for(int i=0;i<8;i++){
                        if(neighborsArray[i][0]!=-1){
                            keepingMine[neighborsArray[i][0]][neighborsArray[i][1]]=9;
                        }
                    }
                } deleteArray(neighborsArray,8);
            }
        }
    }
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            if(isClickableList[row][col]==0){
                int numberOfMine = mineNumber(&keepingMine[0],row,col);
                if(numberOfMine==board[row][col]){
                    int* correctPlace= new int[2];
                    int isItFind=findCorrectPlace(correctPlace,&keepingMine[0],&isClickableList[0],row,col);
                    if(isItFind==1){
                        //If the places found then give hint with changing the icon in the push button and
                        // put its coordinates to last hint array to open it if player click hint button again
                        lastHint[0] = correctPlace[0];
                        lastHint[1] = correctPlace[1];
                        flagsList[lastHint[0]][lastHint[1]]=0;
                        QPushButton *button = getButtonAt(correctPlace[0],correctPlace[1]);
                        button->setIcon(buttonImageHint);
                        return 1;
                    }
                    delete[] correctPlace;
                }
            }
        }
    }return 0;
}
void Grid::revealAll(){
    // this method is used at the end of the game whether it is loss or win.
    // It reveals the mines at the board according to board list and put mine icon.
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            isClickableList[row][col]=0;
            // Create a QPushButton and set its icon to the button image
            QPushButton *button = getButtonAt(row,col);
            if (board[row][col] == 9) {
                button->setIcon(buttonImageMine);
            }
        }
    }
}
void Grid::restartGame(){
    // if player wants to restart the game, then update board with new initial conditions.
    initializeBoard();
    placeMines();
    initializeClickableList();
    initializeMineCount();
    scoreLabel->setText("Score: " + QString::number(0));

    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {

            // Create a QPushButton and set its icon to the button image
            QPushButton *button = getButtonAt(row,col);
            button->setIcon(buttonImageEmpty);
        }
    }
}
void Grid::winningGame(int score){
    // if score equals to winning condition which is calculated at the beginning of the game according to mine count and board sizes,
    // give message to player with message box.
    if(score==winningCondition){
        revealAll();
        QMessageBox::about(this, "Mineswepper","You win!");
    }
}

void Grid::handleButtonClick() {
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());

    // Find the position of the clicked button in the layout
    int row = -1, col = -1;
    for (int i = 0; i < layout->rowCount(); ++i) {
        for (int j = 0; j < layout->columnCount(); ++j) {
            if (layout->itemAtPosition(i, j)->widget() == clickedButton) {
                row = i;
                col = j;
                break;
            }
        }
        if (row != -1 && col != -1)
            break;
    }
    // Retrieve the value at the corresponding position in the board array
    int value = board[row][col];
    // Implement the logic based on the retrieved value
    if (value == 0 && isClickableList[row][col]==1) {
        // if the mine count is 0 and  it is clickable, run bfs and update score, check whether the player win the game or not
        bfs(row, col);
        int currentScore=keepingScore();
        scoreLabel->setText("Score: " + QString::number(currentScore));
        winningGame(currentScore);
    } else if (value == 9 &&isClickableList[row][col]==9) {
        revealAll();
        QMessageBox::about(this, "MineSwepper","You lose!"); // Terminate the game
    } else {
        // Change the icon of the clicked button based on the value
        // Update the score and check the winning conditon
        switch (value) {
        case 1:
            if (isClickableList[row][col] == 1) {
                clickedButton->setIcon(buttonImage1);
                isClickableList[row][col] = 0;
                int currentScore=keepingScore();
                scoreLabel->setText("Score: " + QString::number(currentScore));
                winningGame(currentScore);

            }
            break;
        case 2:
            if (isClickableList[row][col] == 1) {
                clickedButton->setIcon(buttonImage2);
                isClickableList[row][col] = 0;
                int currentScore=keepingScore();
                scoreLabel->setText("Score: " + QString::number(currentScore));
                winningGame(currentScore);
            }
            break;
        case 3:
            if (isClickableList[row][col] == 1) {
                clickedButton->setIcon(buttonImage3);
                isClickableList[row][col] = 0;
                int currentScore=keepingScore();
                scoreLabel->setText("Score: " + QString::number(currentScore));
                winningGame(currentScore);
            }
            break;
        case 4:
            if (isClickableList[row][col] == 1) {
                clickedButton->setIcon(buttonImage4);
                isClickableList[row][col] = 0;
                int currentScore=keepingScore();
                scoreLabel->setText("Score: " + QString::number(currentScore));
                winningGame(currentScore);
            }
            break;
        case 5:
            if (isClickableList[row][col] == 1) {
                clickedButton->setIcon(buttonImage5);
                isClickableList[row][col] = 0;
                int currentScore=keepingScore();
                scoreLabel->setText("Score: " + QString::number(currentScore));
                winningGame(currentScore);
            }
            break;
        case 6:
            if (isClickableList[row][col] == 1) {
                clickedButton->setIcon(buttonImage6);
                isClickableList[row][col] = 0;
                int currentScore=keepingScore();
                scoreLabel->setText("Score: " + QString::number(currentScore));
                winningGame(currentScore);
            }
            break;
        case 7:
            if (isClickableList[row][col] == 1) {
                clickedButton->setIcon(buttonImage7);
                int currentScore=keepingScore();
                scoreLabel->setText("Score: " + QString::number(currentScore));
                winningGame(currentScore);
            }
            break;
        case 8:
            if (isClickableList[row][col] == 1) {
                clickedButton->setIcon(buttonImage8);
                isClickableList[row][col] = 0;
                int currentScore=keepingScore();
                scoreLabel->setText("Score: " + QString::number(currentScore));
                winningGame(currentScore);
            }

        default:
            break;
        }
    }
}
QPushButton* Grid::getButtonAt(int row, int col) {
    // finding button in the grid with given row and col
    for (int i = 0; i < layout->rowCount(); ++i) {
        for (int j = 0; j < layout->columnCount(); ++j) {
            QPushButton *button = qobject_cast<QPushButton*>(layout->itemAtPosition(i, j)->widget());
            if (button && layout->itemAtPosition(i, j) != nullptr) {
                if (i == row && j == col) {
                    return button;
                }
            }
        }
    }
    return nullptr; // Return null if button not found at given row and column
}
