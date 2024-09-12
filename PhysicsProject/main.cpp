#include "SFML/Graphics.hpp"
#include "box2d\box2D.h"
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    //Creates an SFML window at a desired resolution.
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML and Box2D");
    window.setFramerateLimit(60);     

    //This is the drawing section for SFML.
    while (window.isOpen())
    {
        //Clear the previous window. If we don't do this, we will draw on top of previous stuff.
        window.clear();

        //Window events:
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        //Finally, display the window.
        window.display();
    }

    return 0;
}