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

int main()
{
    sf::RenderWindow window(sf::VideoMode({400, 400}), "Team Sync Test");
    window.setFramerateLimit(60);

    sf::RectangleShape block({40.f, 40.f});
    block.setPosition({180.f, 180.f});
    block.setFillColor(sf::Color::White); // Friend will change this!

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Left)  block.move({-20.f, 0.f});
                if (key->code == sf::Keyboard::Key::Right) block.move({20.f, 0.f});
                if (key->code == sf::Keyboard::Key::Up)    block.move({0.f, -20.f});
                if (key->code == sf::Keyboard::Key::Down)  block.move({0.f, 20.f});
            }
        }

        window.clear(sf::Color(30, 30, 30));
        window.draw(block);
        window.display();
    }
}