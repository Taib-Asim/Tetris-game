#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<ctime> 

using namespace std;


const int ROWS=20;
const int COLS=10;
const float CELL=30;
const float Board_x=20;   //^ using float because SFML uses pixel coordinates in floating point number
const float Board_y=20;

const unsigned Window_W=520;  
const unsigned Window_H=640;  

const int Start_R=0;
const int Start_C=3;

const int Start_Level=1;
const int Start_Delay=30;
const int Speed_change=5;
const int Min_Delay=5;
const int Lines_per_level=10;
const int Line_points[5]={0,100,300,500,800};  

//^ PDF method
const int NONE=0,ROTATE=1,DOWN=2,LEFT=3,RIGHT=4,DROP=5,ESC=6,ENTER=7,HOLD=8;
const int Menu=0,Playing=1,game_over=2;
int game_state=Menu;
int menu_choice=0;

bool noSave=false;

int pressedKey=0;

int board[ROWS][COLS]={0};

char pieces[7][4][4];
char shape[4][4] ;


sf::Color colors[8] = {
sf::Color(25, 25, 45),   // 0: empty cell
sf::Color::Cyan,         // 1: I
sf::Color::Yellow,       // 2: O
sf::Color(170, 90, 255), // 3: T (purple)
sf::Color::Green,        // 4: S
sf::Color::Red,          // 5: Z
sf::Color::Blue,         // 6: J
sf::Color(255, 150, 0) };// 7: L (orange)


int piece_row=Start_R;
int piece_col=Start_C;
int shadow_row=Start_R;
int shadow_col=Start_C;


int fall_timer=0;

int fall_speed=Start_Delay;
int piece;
int score=0;
int lines_Cleared_pLevel=0;

//^ task 6 new variables
int total_line=0;
int next_piece=0;
int held_piece=0;
int level=Start_Level;

//* Bonus Task variables
bool holding=false;
bool same_switch=false;

//^ Audio Initializing
sf::SoundBuffer selectBuffer;
sf::SoundBuffer blockBuffer;
sf::SoundBuffer overBuffer;

sf::Sound selectSound(selectBuffer);
sf::Sound blockSound(blockBuffer);
sf::Sound overSound(overBuffer);

sf::Music themeMusic;
sf::SoundBuffer clearBuffer;
sf::Sound clearSound(clearBuffer);


//^ functions declaring here
bool readFile();
void BoardDraw(sf::RenderWindow& window);
void PieceDraw(sf::RenderWindow& window);
void shadowDraw(sf::RenderWindow& window);
void showPieceError(sf::RenderWindow& window);
bool canPlace(char shape[][4], int row, int col);
void rotatePiece();
void movePiece();
void fallAuto();
void lockPiece();
void generate();
void drop();
void clearLines();
void update_score_level(int & linesCleared);
void drawInformation(sf::RenderWindow& window, sf::Font& font);
void resetGame();
void savegame();
void loadgame();
void removesave();
void holdPiece();
void handleInput(sf::RenderWindow& window);
void updateGame(sf::RenderWindow& window);
void drawMenu(sf::RenderWindow& window, sf::Font& font);
void drawGameOver(sf::RenderWindow& window, sf::Font& font);
void drawGame(sf::RenderWindow& window, sf::Font& font);

int n;

// task 1
void handleInput(sf::RenderWindow& window)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Left)
            {
                pressedKey = LEFT;
            }
            else if (key->code == sf::Keyboard::Key::Right)
            {
                pressedKey = RIGHT;
            }
            else if (key->code == sf::Keyboard::Key::Down)
            {
                pressedKey = DOWN;
            }
            else if (key->code == sf::Keyboard::Key::Space)
            {
                pressedKey = DROP;
            }
            else if (key->code == sf::Keyboard::Key::A)
            {
                pressedKey = LEFT;
            }
            else if (key->code == sf::Keyboard::Key::D)
            {
                pressedKey = RIGHT;
            }
            else if (key->code == sf::Keyboard::Key::S)
            {
                pressedKey = DOWN;
            }
            else if (key->code == sf::Keyboard::Key::Up)
            {
                pressedKey = ROTATE;
            }
            else if (key->code == sf::Keyboard::Key::W)
            {
                pressedKey = ROTATE;
            }
            else if (key->code == sf::Keyboard::Key::Escape)
            {
                pressedKey = ESC;
            }
            else if (key->code == sf::Keyboard::Key::Enter)
            {
                pressedKey = ENTER;
            }
            else if (key->code == sf::Keyboard::Key::C)
            {
                pressedKey = HOLD;
            }
        }
    }
}
void updateGame(sf::RenderWindow& window)
{
    switch (game_state)
    {
    case Menu:
        if (pressedKey == ROTATE)  //pressing up key to move up
        {
            menu_choice -= 1;
            if (menu_choice == -1)
                menu_choice = 2;
            selectSound.play();
        }
        else if (pressedKey == DOWN)
        {
            menu_choice += 1;
            if (menu_choice == 3)
                menu_choice = 0;
            selectSound.play();
        }
        else if (pressedKey == ENTER)
        {
            if (menu_choice == 0)  //selected new game
            {
                resetGame();
                themeMusic.stop();
                game_state = Playing;
                selectSound.play();
            }
            else if (menu_choice == 1)  //selected continue
            {
                selectSound.play();
                if (game_state == Playing)
                    themeMusic.stop();
                loadgame();
            }
            else if (menu_choice == 2)   //selecting exit
            {
                window.close();
            }
        }
        break;
    case Playing:
        if (pressedKey == ESC)  //go to menu
        {
            savegame();
            themeMusic.play();
            game_state = Menu;
        }
        else
        {
            movePiece();
            fallAuto();
        }
        break;
    case game_over:
        if (pressedKey == ENTER)  //move to menu
        {
            removesave();
            menu_choice = 0;
            game_state = Menu;
            themeMusic.play();
        }
        break;
    }
    pressedKey = NONE;
}

void drawMenu(sf::RenderWindow& window, sf::Font& font)
{
    string options[3] = { "New Game", "Continue", "Exit" };

    sf::Text title(font, "TETRIS", 48);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition({ 175.f, 110.f });
    window.draw(title);

    for (int i = 0; i < 3; i++)
    {
        sf::Text item(font, options[i], 28);
        item.setFillColor(i == menu_choice ? sf::Color::Yellow : sf::Color::White);
        item.setPosition({ 185.f, 250.f + i * 55.f });
        window.draw(item);
    }
    if (noSave == true)
    {
        sf::Text msg(font, "No saved game", 22);
        msg.setFillColor(sf::Color::Red);
        msg.setPosition({ 180.f, 430.f });
        window.draw(msg);
    }
}

void drawGameOver(sf::RenderWindow& window, sf::Font& font)
{
    sf::Text over(font, "GAME OVER", 42);
    over.setFillColor(sf::Color::Red);
    over.setPosition({ 140.f, 190.f });
    window.draw(over);

    sf::Text finalScore(font, "Final Score: " + to_string(score), 26);
    finalScore.setFillColor(sf::Color::White);
    finalScore.setPosition({ 160.f, 265.f });
    window.draw(finalScore);

    sf::Text prompt(font, "Press Enter", 24);
    prompt.setFillColor(sf::Color::Yellow);
    prompt.setPosition({ 195.f, 330.f });
    window.draw(prompt);
}

void drawGame(sf::RenderWindow& window, sf::Font& font)
{
    window.clear(sf::Color(15, 15, 30));
    switch (game_state)
    {
    case Menu:
        drawMenu(window, font);
        break;
    case Playing:
        BoardDraw(window);
        drawInformation(window, font);
        shadowDraw(window);
        PieceDraw(window);
        break;
    case game_over:
        drawGameOver(window, font);
        break;
    }
    window.display();
}
// task 1

//* task 2 
bool readFile()
{
    ifstream fin("pieces.txt");
    if(!fin)  //Checking
    {
        cout<<"File Not opened";
        return false;
    }
    fin>> n ; //size
    for(int k=0;k<n;k++)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                fin >> pieces[k][i][j];
            } 
        }    
    }
    fin.close();
    return true;
}
void BoardDraw(sf::RenderWindow & window)
{
    sf::RectangleShape block({CELL - 2, CELL - 2});
    
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            block.setFillColor(colors[board[r][c]]);
            block.setPosition({Board_x + c * CELL + 1, Board_y + r * CELL + 1});
            window.draw(block);
        }
    }
    sf::RectangleShape border({ COLS * CELL,ROWS*CELL});
    border.setPosition({ Board_x,Board_y});
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(2.0f);
    border.setOutlineColor(sf::Color::White);
    window.draw(border);
}

void PieceDraw(sf::RenderWindow& window)
{
    sf::RectangleShape block({ CELL-2,CELL-2});
    block.setFillColor(colors[piece+1]);
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if(shape[r][c] == '#')
            {
                int board_r=piece_row+r;
                int board_c=piece_col+c;
                if(board_r>=0 && board_r<ROWS && board_c >=0 && board_c<COLS)
                {
                    block.setPosition({ Board_x + board_c *CELL +1 ,Board_y +board_r*CELL+1});
                    window.draw(block);
                }
            }
        }
        
    }
    
}
void shadowDraw(sf::RenderWindow& window)
{
    shadow_col = piece_col;
    shadow_row = piece_row;
    while (canPlace(shape, shadow_row + 1, shadow_col)) 
    {
        shadow_row++; //bringing shadow to minimum possible height
    }

    sf::Color col = colors[piece + 1];
    col.a = 170; 

    sf::Color fillCol = colors[piece + 1];
    fillCol.a = 45; 

    float x0 = 1.0f;            
    float y0 = 1.0f;            
    float side = CELL - 2.0f;   
    float thick = 2.5f;         
    float dash = 10.0f;         

    sf::RectangleShape fillBox({side, side});
    fillBox.setFillColor(fillCol);

    sf::RectangleShape dashH({dash, thick});
    sf::RectangleShape dashV({thick, dash});
    dashH.setFillColor(col);
    dashV.setFillColor(col);

    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if (shape[r][c] == '#')
            {
                int board_r = shadow_row + r;
                int board_c = shadow_col + c;
                if (board_r >= 0 && board_r < ROWS && board_c >= 0 && board_c < COLS)
                {
                    float px = Board_x + board_c * CELL + x0;
                    float py = Board_y + board_r * CELL + y0;
                    
                    fillBox.setPosition({px, py});
                    window.draw(fillBox);
                    
                    dashH.setPosition({px, py});
                    window.draw(dashH);
                    dashH.setPosition({px + side - dash, py});
                    window.draw(dashH);
                   
                    dashH.setPosition({px, py + side - thick});
                    window.draw(dashH);
                    dashH.setPosition({px + side - dash, py + side - thick});
                    window.draw(dashH);

                    dashV.setPosition({px, py});
                    window.draw(dashV);
                    dashV.setPosition({px, py + side - dash});
                    window.draw(dashV);

                    dashV.setPosition({px + side - thick, py});
                    window.draw(dashV);
                    dashV.setPosition({px + side - thick, py + side - dash});
                    window.draw(dashV);
                }
            }
        }
    }
}
void showPieceError(sf::RenderWindow & window) //To show error screen
{
    sf::Font font;
    if (!font.openFromFile("arial.ttf") && !font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        cout << "Font not opened" << endl;
        return;
    }

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color(15, 15, 30));

        sf::Text title(font, "ERROR", 42);
        title.setFillColor(sf::Color::Red);
        title.setPosition({185.f, 220.f});
        window.draw(title);

        sf::Text line1(font, "Could not load pieces.txt", 24);
        line1.setFillColor(sf::Color::White);
        line1.setPosition({126.f, 290.f});
        window.draw(line1);

        sf::Text line2(font, "Put pieces.txt in the project folder and restart.", 18);
        line2.setFillColor(sf::Color::Yellow);
        line2.setPosition({82.f, 330.f});
        window.draw(line2);

        window.display();
    }
}
//^ task 2 
//* task 3 

bool canPlace(char shape[][4],int row,int col) 
{
    int boardRow,boardCol;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if(shape[i][j] == '#')
            {
                boardRow=i+row;
                boardCol=j+col;
                if(boardRow>=ROWS || boardCol<0 || boardCol>=COLS || board[boardRow][boardCol]!=0 )
                    return false;
            }
        }   
    }
    return true;
}
void rotatePiece()  //Clockwise rotate
{
    char nShape[4][4];
    for(int i=0;i<4;i++)  //Storing in temp then rotating
    {
        for(int j=0;j<4;j++)
        {
            nShape[j][i]=shape[i][j];
        }
    }
    
    for(int j=0;j<2;j++)
    {
        for(int i=0;i<4;i++)
        {
            char t=nShape[i][j];
            nShape[i][j]=nShape[i][3-j];
            nShape[i][3-j]=t;
        }
    }
    if(canPlace(nShape,piece_row,piece_col))
    {
        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 4; c++)
            {
                shape[r][c]=nShape[r][c];
            }
        }
    }
}

void movePiece()
{
    if(pressedKey==LEFT)
    {
        if(canPlace(shape,piece_row,piece_col-1))
            piece_col--;
    }
    else if(pressedKey==RIGHT)
    {
        if(canPlace(shape,piece_row,piece_col+1))
            piece_col++;
    }
    else if(pressedKey==DOWN)
    {
        if(canPlace(shape,piece_row+1,piece_col))
            piece_row++;
    }
    else if(pressedKey==ROTATE)
        rotatePiece();   
    else if(pressedKey==DROP)
    {
        drop();
    }
    else if(pressedKey==HOLD)
    {
        holdPiece();
    }
}  //^ task 3

//* task 4
void fallAuto() //Decreases piece height by comparing with fall speed
{
    fall_timer++;
    if(fall_timer>=fall_speed)
    {
        fall_timer=0;
        if(canPlace(shape,piece_row+1,piece_col))
            piece_row++;
        else
        {
            lockPiece();
            generate();
        }
    }
}

void lockPiece()
{
    for (int i = 0; i < 4; i++) //Storing in board 
    {
        for (int j = 0; j < 4; j++)
        {
            if(shape[i][j] == '#')
            {
              board[piece_row+i][piece_col+j]=piece+1;
            }
        }   
    }
    clearLines();
    blockSound.play();
    same_switch=false;
}
void generate()
{
    if(pressedKey!=HOLD) //only creating new piece when c is not pressed
    {
        piece=next_piece;
        next_piece=rand()%7;
    }
    else if(holding==false) //if c is pressed and nothing is in hold then create new piece
    {
        piece=next_piece;
        next_piece=rand()%7;
    }
    for(int r=0;r<4;r++)
    {
        for (int c = 0; c < 4; c++)
        {
            shape[r][c]=pieces[piece][r][c];
        }
    }
    piece_row=Start_R;
    piece_col=Start_C;
    if(canPlace(shape,piece_row,piece_col)==false)
    {
        game_state=game_over;
        overSound.play();
    }
    fall_timer=0; //resetting fall timer
}
void drop()
{
    while(canPlace(shape,piece_row+1,piece_col))
    {
        piece_row++;
    }
    lockPiece();
    generate();
}
//^ task 4

// task 5
void clearLines()
{
    int linesClear=0;
    for(int r=ROWS-1;r>=0;r--)
    {
        bool fullLine=true;
        for(int c=0;c<COLS;c++)
        {
            if(board[r][c]==0)
            {
                fullLine=false;
                break;
            }
        }
        if(fullLine)  //shift board rows down when line is cleared
        {
            linesClear++;
            for(int row=r;row>0;row--)
            {
                for(int col=0;col<COLS;col++)
                {
                    board[row][col]=board[row-1][col];
                }
            }
            for(int col=0;col<COLS;col++)  //for top row because above this array is out of bounds 
            {  
                board[0][col]=0;
            }
            r++;//for rechecking
        }
    }
    if (linesClear > 0)
    {
        clearSound.play();
    }
    update_score_level(linesClear);
}
//^ task 5

//* task 6 score level graphics
void update_score_level(int & linesCleared)
{
    score+=Line_points[linesCleared];

    if(linesCleared>0)  
    {
        lines_Cleared_pLevel+=linesCleared;
        total_line+=linesCleared;
        level=Start_Level + (total_line/Lines_per_level);
        if(lines_Cleared_pLevel>=Lines_per_level)  //changing fall speed per level
        {
            lines_Cleared_pLevel-=Lines_per_level;
            if(fall_speed>Min_Delay)  //check for minimum fall delay
                fall_speed-=Speed_change;
        }
    }
}
void drawInformation(sf::RenderWindow & window, sf::Font & font)
{
    float panelX = 350.f;
    float boxW = 4 * CELL + 10.f;
    float boxH = 3 * CELL + 16.f;

    sf::RectangleShape statBox({boxW, 64.f});
    statBox.setFillColor(sf::Color(25, 25, 45));
    statBox.setOutlineThickness(1.5f);
    statBox.setOutlineColor(sf::Color(70, 80, 120));

    statBox.setPosition({panelX, 20.f});
    window.draw(statBox);

    sf::Text scoreLabel(font, "SCORE", 16);
    scoreLabel.setFillColor(sf::Color(170, 180, 210));
    scoreLabel.setPosition({panelX + 12.f, 26.f});
    window.draw(scoreLabel);

    sf::Text scoreVal(font, to_string(score), 26);
    scoreVal.setFillColor(sf::Color::Yellow);
    scoreVal.setPosition({panelX + 12.f, 46.f});
    window.draw(scoreVal);

    statBox.setPosition({panelX, 100.f});
    window.draw(statBox);

    sf::Text linesLabel(font, "LINES", 16);
    linesLabel.setFillColor(sf::Color(170, 180, 210));
    linesLabel.setPosition({panelX + 12.f, 106.f});
    window.draw(linesLabel);

    sf::Text linesVal(font, to_string(total_line), 26);
    linesVal.setFillColor(sf::Color::Cyan);
    linesVal.setPosition({panelX + 12.f, 126.f});
    window.draw(linesVal);

    statBox.setPosition({panelX, 180.f});
    window.draw(statBox);

    sf::Text levelLabel(font, "LEVEL", 16);
    levelLabel.setFillColor(sf::Color(170, 180, 210));
    levelLabel.setPosition({panelX + 12.f, 186.f});
    window.draw(levelLabel);

    sf::Text levelVal(font, to_string(level), 26);
    levelVal.setFillColor(sf::Color::Green);
    levelVal.setPosition({panelX + 12.f, 206.f});
    window.draw(levelVal);

    sf::Text nextLabel(font, "NEXT", 18);
    nextLabel.setFillColor(sf::Color::White);
    nextLabel.setPosition({panelX + 4.f, 268.f});
    window.draw(nextLabel);

    sf::RectangleShape nextBox({boxW, boxH});
    nextBox.setPosition({panelX, 294.f});
    nextBox.setFillColor(sf::Color(25, 25, 45));
    nextBox.setOutlineThickness(2.0f);
    nextBox.setOutlineColor(sf::Color::Cyan);
    window.draw(nextBox);

    sf::RectangleShape block({CELL - 2, CELL - 2});
    block.setFillColor(colors[next_piece + 1]);
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if (pieces[next_piece][r][c] == '#')
            {
                block.setPosition({panelX + 5.f + c * CELL + 1, 292.f + r * CELL + 1});
                window.draw(block);
            }
        }
    }

    sf::Text holdLabel(font, "HOLD (C)", 18);
    holdLabel.setFillColor(sf::Color::White);
    holdLabel.setPosition({panelX + 4.f, 488.f});
    window.draw(holdLabel);

    sf::RectangleShape holdBox({boxW, boxH});
    holdBox.setPosition({panelX, 514.f});
    holdBox.setFillColor(sf::Color(25, 25, 45));
    holdBox.setOutlineThickness(2.0f);
    holdBox.setOutlineColor(same_switch ? sf::Color(110, 110, 140) : sf::Color::Yellow);
    window.draw(holdBox);

    if (holding == true)
    {
        sf::Color hCol = colors[held_piece + 1];
        if (same_switch)
            hCol.a = 120;

        block.setFillColor(hCol);
        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 4; c++)
            {
                if (pieces[held_piece][r][c] == '#')
                {
                    block.setPosition({panelX + 5.f + c * CELL + 1, 512.f + r * CELL + 1});
                    window.draw(block);
                }
            }
        }
    }
} //^ task 6 done

//* task 7 Menu and game states
void resetGame()
{
    for (int i = 0; i < ROWS; i++)  //Resetting board and all variables
    {
        for (int j = 0; j < COLS; j++)
        {
            board[i][j]=0;
        }
    }
    score=0;
    total_line=0;
    level=Start_Level;
    fall_speed=Start_Delay;
    lines_Cleared_pLevel=0;
    next_piece=rand()%7;
    holding=false;
    same_switch=false;
    noSave=false;
     generate();
} //^ task 7

//* task 8 saving game 
void savegame()
{
	ofstream fout("savegame.txt"); //Writting data in txt file according to pdf
	if (!fout)
		return;
	fout << score << " " << total_line << " " << level << "\n";
	fout << next_piece << "\n";
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			fout << board[r][c]; 
			if (c < COLS - 1)
				fout << " ";
		}
		fout << "\n";
	}
	fout.close();
}

void loadgame()
{
	ifstream fin("savegame.txt");  
	if (!fin)
	{
        noSave=true;
		return;
	}
    int t;
    int count=0;
    if(fin>>t)  //Checking whether first character is being read or not
    {
        score=t;
        count++;
    }
    if(count==0)  //meaning first character was not present
    {
    
        noSave=true;
        fin.close();
    }
    else
    {
        noSave=false;
        fin >> total_line >> level;
        fin >> piece;
        next_piece=rand()%7;
        for (int r = 0; r < ROWS; r++)
        {
            for (int c = 0; c < COLS; c++)
            {
                fin >> board[r][c];
            }
        }
        fin.close();
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                shape[i][j]=pieces[piece][i][j];
            }
        }
        if(level>=6)
            fall_speed=Min_Delay;
        else
        {
            fall_speed=Start_Delay - (level-1)*Min_Delay;
        }
        lines_Cleared_pLevel=total_line%Lines_per_level;
        holding=false;
        same_switch=false;
        piece_row = Start_R;
        piece_col = Start_C;
        game_state = Playing;
    }
}

void removesave()
{
	ofstream fout("savegame.txt");
	fout.close();
}
//^ task 8

//* bonus task

void holdPiece()
{
    if(same_switch==false)  //running only if same piece is not switched again before lock piece
    {
        if(holding==false)  //for first time
        {
            held_piece=piece;
            generate();
            holding=true;
        }
        else  //for other than first time, replace current piece and held piece
        {
            int t=piece;
            piece=held_piece;
            held_piece=t;
            generate();
        }   
    }
    same_switch=true;
}

int main()
{
	sf::RenderWindow window( sf::VideoMode( { Window_W, Window_H } ), "Tetris" );
    window.setFramerateLimit(60);
    srand(time(0));
    next_piece=rand()%7;
    if(!readFile())
    {
        showPieceError(window);
        return 1;
    }
    sf::Font font;
    if(!font.openFromFile("arial.ttf"))
    {
        cout<<"Font not opened"<<endl;
        return 1;
    }
    if (!selectBuffer.loadFromFile("selecting options.mpeg") ||
        !blockBuffer.loadFromFile("blocks.mpeg") ||
        !overBuffer.loadFromFile("gameover.mpeg") ||
        !clearBuffer.loadFromFile("line_clear.mpeg"))
    {
        cout << "Sound not opened" << endl;
        return 1;
    }

    if (!themeMusic.openFromFile("theme.mpeg"))
    {
        cout << "Theme music not opened" << endl;
        return 1;
    }
    themeMusic.setLooping(true);
    themeMusic.setVolume(40.f);
    themeMusic.play();
	while ( window.isOpen() )
	{
		handleInput(window);
        updateGame(window);
        drawGame(window,font);
	}
    return 1;
}