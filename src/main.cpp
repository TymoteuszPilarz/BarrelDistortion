//
//  main.cpp
//  BarrelDistortion
//
//  Created by Tymoteusz Pilarz on 14/01/2023.
//

#include <iostream>

#include "Application.hpp"
#include "SDL_Exception.hpp"

int main(int argc, const char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Wrong number of arguments (2 required)" << std::endl;
        return -1;
    }
    
    try
    {
        Application app;
        
        if (!app.load_file(argv[1]))
            return -1;
        
        if (std::string(argv[3]) == "distortion")
        {
            if (!app.add_distortion())
                return -1;
        }
        else if (std::string(argv[3]) == "correction")
        {
            if (!app.correct_distortion())
                return -1;
        }
        else
        {
            std::cerr << "Wrong function selected" << std::endl;
            return -1;
        }
        
        if (!app.save_file(argv[2]))
            return -1;
        
    }
    catch(const SDL_Exception& exception)
    {
        std::cerr << exception.what() << std::endl;
    }
}
