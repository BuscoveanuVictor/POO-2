#include <SFML/Graphics.hpp>
#include "Game.h"

int main()
{
   
    //sf::CircleShape shape(100.f);
    //shape.setFillColor(sf::Color::Green);

    Game joc(500,500);

    joc.play();

    return 0;
}