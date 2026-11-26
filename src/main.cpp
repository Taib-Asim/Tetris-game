#include <SFML/Graphics.hpp>
#include<iostream>
#include<fstream>
#include<cstdlib>

using namespace std;

const int ROWS = 20;
const int COLS = 10;
const float CELL = 30;
const float Board_x = 20;   //^ using float because SFML humse pixel coordinates float me expect krta setsize() setposition()
const float Board_y = 20;

const unsigned Window_W = 520;  //? ye cols pehle confuse krdeta thora
const unsigned Window_H = 640;  //* just like above

const int Start_R = 0;
const int Start_C = 3;

const int Start_Level = 1;
const int Start_Delay = 30;
const int Speed_change = 5;
const int Min_Delay = 5;
const int Lines_per_level = 10;
const int Line_points[5] = { 0,100,300,500,800 };

//^ by sir method 
const int NONE=0,ROTATE=1,DOWN=2,LEFT=3,RIGHT=4,DROP=5,ESC=6,ENTER=7;
const int Menu=0,Playing=1,game_over=2;
int gamestate=Menu;
int menuchoice=0;

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
int fall_timer=0;

int total_line = 0;     //New Variable added task 6
int next_piece = 0;
int level = Start_Level;
int lines_Cleared_pLevel = 0;
int score = 0;

int fall_speed=Start_Delay;
int piece=2;
int siz;

bool game_status=true;

//^ functions declaring here 
void movePiece();
void drop();      
bool canPlace(char shape[][4],int row,int col);
void generate();
void lockPiece();
void rotatePiece();
void fallAuto(); 
void clearLines();  
void update_score_level(int& linesCleared);

//* task 2 by Suhaib
bool readFile()
{
    ifstream fin("pieces.txt");
    if(!fin)
    {
        cout<<"File Not opened";
        return false;
    }
    fin>> siz ;
    for(int k=0;k<siz;k++)
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
//^ task 2 by Suhaib
bool canPlace(char shape[][4], int row, int col)
{
    int boardRow, boardCol;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (shape[i][j] == '#')
            {
                boardRow = i + row;
                boardCol = j + col;
                if (boardRow >= ROWS || boardCol < 0 || boardCol >= COLS || board[boardRow][boardCol] != 0)
                    return false;
            }
        }
    }
    return true;
}

void rotatePiece()
{
    char nShape[4][4];
    for(int i=0;i<4;i++)
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
    if (pressedKey == LEFT)
    {
        if (canPlace(shape, piece_row, piece_col - 1))
            piece_col--;
    }
    else if (pressedKey == RIGHT)
    {
        if (canPlace(shape, piece_row, piece_col + 1))
            piece_col++;
    }
    else if (pressedKey == DOWN)
    {
        if (canPlace(shape, piece_row + 1, piece_col))
            piece_row++;
    }
    else if(pressedKey==ROTATE)
        rotatePiece();   
    else if(pressedKey==DROP)
        drop();
}  //^ task 3

//* task 4

void fallAuto()
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
    for (int i = 0; i < 4; i++)
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
}
void generate()
{
    piece=next_piece;
    next_piece=rand()%7;
    
    for(int r=0;r<4;r++)
    {
        for (int c = 0; c < 4; c++)
        {
            shape[r][c]=pieces[piece][r][c];
        }
    }
    piece_row=Start_R;
    piece_col=Start_C;
    if (canPlace(shape, piece_row, piece_col) == false)
    {
        gamestate = game_over;
    }

    fall_timer=0;
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

//task 5
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
        if(fullLine)
        {
            linesClear++;
            for(int row=r;row>0;row--)
            {
                for(int col=0;col<COLS;col++)
                {
                    board[row][col]=board[row-1][col];
                }
            }
            for(int col=0;col<COLS;col++)//Ye top row ke liye kyun ke us ke uper access karne se array out of bounds ho rhi hai
            {
                board[0][col]=0;
            }
            r++;//Taake agli line jo neeche ayi us ko dobara check kr sake
        }
    }
    update_score_level(linesClear);
}
//^task 5

//task 6
void update_score_level(int& linesCleared)
{
    score += Line_points[linesCleared];

    if (linesCleared > 0)
    {
        lines_Cleared_pLevel += linesCleared;
        total_line += linesCleared;
        level = total_line / Lines_per_level;
        if (lines_Cleared_pLevel >= Lines_per_level)
        {
            lines_Cleared_pLevel -= Lines_per_level;
            if (fall_speed > Min_Delay)
                fall_speed -= Speed_change;
        }
    }
}
void drawInformation(sf::RenderWindow & window, sf::Font & font)
{
    sf::Text scoreLabel(font, "SCORE", 20);
    scoreLabel.setPosition({355.f, 40.f});
    window.draw(scoreLabel);

    sf::Text scoreVal(font, to_string(score), 26);
    scoreVal.setFillColor(sf::Color::Yellow);
    scoreVal.setPosition({355.f, 68.f});
    window.draw(scoreVal);

    sf::Text linesLabel(font, "LINES", 20);
    linesLabel.setPosition({355.f, 130.f});
    window.draw(linesLabel);

    sf::Text linesVal(font, to_string(total_line), 26);
    linesVal.setPosition({355.f, 158.f});
    window.draw(linesVal);

    sf::Text levelLabel(font, "LEVEL", 20);
    levelLabel.setPosition({355.f, 220.f});
    window.draw(levelLabel);

    sf::Text levelVal(font, to_string(level), 26);
    levelVal.setFillColor(sf::Color::Green);
    levelVal.setPosition({355.f, 248.f});
    window.draw(levelVal);

    sf::Text nextLabel(font, "NEXT", 20);
    nextLabel.setPosition({355.f, 320.f});
    window.draw(nextLabel);

    sf::RectangleShape box({4 * CELL + 10, 4 * CELL + 10});
    box.setPosition({350.f, 355.f});
    box.setFillColor(sf::Color(25, 25, 45));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color::White);
    window.draw(box);

    sf::RectangleShape block({CELL - 2, CELL - 2});
    block.setFillColor(colors[next_piece + 1]);
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if (pieces[next_piece][r][c] == '#')
            {
                block.setPosition({355.f + c * CELL + 1, 360.f + r * CELL + 1});
                window.draw(block);
            }
        }
    }

} //^ task 6 done

//* task 7 Menu and game states
void resetGame()
{
    for (int i = 0; i < ROWS; i++)
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
    generate();
} //^ task 7

//* task 8 saving game 
void savegame()
{
	ofstream fout("savegame.txt");
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
    if(fin>>t)
    {
        score=t;
        count++;
    }
    if(count==0)
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
        piece_row = Start_R;
        piece_col = Start_C;
        gamestate = Playing;
    }
}

void removesave()
{
	ofstream fout("savegame.txt");
	fout.close();
}
//^ task 8

// task 1
void handleInput(sf::RenderWindow & window)
{
    while ( const std::optional event = window.pollEvent() )
    {
        if ( event->is<sf::Event::Closed>() )
            window.close();
        else if(const auto* key= event ->getIf<sf::Event::KeyPressed>())
        {
            if(key->code== sf::Keyboard::Key::Left)
            {
                pressedKey=LEFT;
            }
            else if(key->code== sf::Keyboard::Key::Right)
            {
                pressedKey=RIGHT;
            }
            else if(key->code== sf::Keyboard::Key::Down)
            {
                pressedKey=DOWN;
            }
            else if(key->code== sf::Keyboard::Key::Space)
            {
                pressedKey=DROP;
            }
            else if(key->code== sf::Keyboard::Key::A)
            {
                pressedKey=LEFT;
            }
            else if(key->code== sf::Keyboard::Key::D)
            {
                pressedKey=RIGHT;
            }
            else if(key->code== sf::Keyboard::Key::S)
            {
                pressedKey=DOWN;
            }            
            else if(key->code== sf::Keyboard::Key::Up)
            {
                pressedKey=ROTATE;
            }
            else if(key->code== sf::Keyboard::Key::W)
            {
                pressedKey=ROTATE;
            }
            else if(key->code== sf::Keyboard::Key::Escape)
            {
                pressedKey=ESC;
            }
            else if(key->code== sf::Keyboard::Key::Enter)
            {
                pressedKey=ENTER;
            }
        }
    }
}
void updateGame(sf::RenderWindow & window)
{
    switch(gamestate)
    {
        case Menu:
            if(pressedKey==ROTATE)
            {
                menuchoice-=1;
                if(menuchoice==-1)
                    menuchoice=2;
            }
            else if(pressedKey==DOWN)
            {
                menuchoice+=1;
                if(menuchoice==3)
                    menuchoice=0;
            }
            else if(pressedKey==ENTER)
            {
                if(menuchoice==0)
                {
                    resetGame();
                    gamestate=Playing;
                }
                else if(menuchoice==1)
                {
                    loadgame();
                }
                else if(menuchoice==2)
                {
                    window.close();
                }
            }
            break;
        case Playing:
            if(pressedKey==ESC)
            {
                savegame();
                gamestate=Menu;
            }
            else
            {
                movePiece();
                fallAuto();
            }
            break;
        case game_over:
            if(pressedKey==ENTER)
            {
                removesave();
                menuchoice=0;
                gamestate=Menu;
            }
            break;
    }
    pressedKey=NONE;
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
        item.setFillColor(i == menuchoice ? sf::Color::Yellow : sf::Color::White);
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
    switch (gamestate)
    {
    case Menu:
        drawMenu(window, font);
        break;
    case Playing:
        BoardDraw(window);
        drawInformation(window, font);
        PieceDraw(window);
        break;
    case game_over:
        drawGameOver(window, font);
        break;
    }
    window.display();
}

// task 1


int main()
{
	sf::RenderWindow window( sf::VideoMode( { Window_W, Window_H } ), "Tetris" );
    window.setFramerateLimit(60);
    srand(time(0));
    next_piece=rand()%7;
    if(!readFile())
    {
        return 1;
    }
    sf::Font font;
    if(!font.openFromFile("arial.ttf"))
    {
        cout<<"Font not opened"<<endl;
        return 1;
    }
    generate();
	while ( window.isOpen() )
	{
		handleInput(window);
        updateGame(window);
        drawGame(window,font);
	}
    return 0;
}