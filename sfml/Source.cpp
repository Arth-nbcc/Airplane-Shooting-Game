#include <SFML/Graphics.hpp>
#include "Application.h"
#include <iostream>
#include <stdexcept>

int main()
{
    Application app;

    try {
        app.run();

    }
    catch (std::exception& e)
    {
        std::cout << "\n\nException: " << e.what() << std::endl;
    }
}
