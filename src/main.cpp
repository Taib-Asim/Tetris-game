#include <SFML/Graphics.hpp>

const int ROWS=20;
const int COLS=10;
const float CELL=30;
const float Board_x=20;   //^ using float because SFML humse pixel coordinates float me expect krta setsize() setposition()
const float Board_y=20;

const unsigned Window_W=520;  //? ye cols pehle confuse krdeta thora
const unsigned Window_H=640;  //* just like above

const int Start_R=0;
const int Start_C=3;

const int Start_Level=1;
const int Start_Delay=30;
const int Speed_change=5;
const int Min_Delay=5;
const int Lines_per_level=10;
const int Line_points[5]={0,100,300,500,800};  

// task 1
void handleInput(sf::RenderWindow & window)
{
    while ( const std::optional event = window.pollEvent() )
    {
        if ( event->is<sf::Event::Closed>() )
            window.close();
    }
}
void updateGame()
{

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
