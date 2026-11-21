#include <SFML/Graphics.hpp>

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

int fall_speed=Start_Delay;
int piece=2;

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
void updateGame()
{
   
    pressedKey=NONE;
}
void drawGame(sf::RenderWindow & window)
{
    window.clear();
 
	window.display();
}
// task 1

int main()
{
	sf::RenderWindow window( sf::VideoMode( { Window_W, Window_H } ), "Tetris" );
    window.setFramerateLimit(60);
 
    
	while ( window.isOpen() )
	{
		handleInput(window);
        updateGame();
        drawGame(window);
	}
}