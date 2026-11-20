#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({400, 400}), "Team Sync Test");
    window.setFramerateLimit(60);

    sf::RectangleShape block({40.f, 40.f});
    block.setPosition({180.f, 180.f});
    block.setFillColor(sf::Color::Yellow); // Friend will change this!

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