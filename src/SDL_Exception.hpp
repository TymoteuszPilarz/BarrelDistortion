//
//  SDL_Exception.hpp
//  BarrelDistortion
//
//  Created by Tymoteusz Pilarz on 14/01/2023.
//

#ifndef SDL_Exception_hpp
#define SDL_Exception_hpp

#include <exception>
#include <string>

class SDL_Exception : public std::exception
{
private:
    std::string msg;
    
public:
    explicit SDL_Exception(const std::string& msg)
    {
        this->msg = msg;
    }
    
    [[nodiscard]] const char* what() const noexcept override
    {
        return msg.c_str();
    }
};


#endif /* SDL_Exception_hpp */

