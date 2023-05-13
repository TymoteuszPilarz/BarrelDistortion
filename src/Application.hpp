//
//  Application.hpp
//  BarrelDistortion
//
//  Created by Tymoteusz Pilarz on 14/01/2023.
//

#ifndef Application_hpp
#define Application_hpp

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#pragma clang diagnostic pop

#include <string>
#include <vector>
#include <tuple>
#include <armadillo>

#include "Color.hpp"


class Application final
{
private:
    std::vector<std::vector<Color>> image;
    SDL_Surface* surface = nullptr;
    
    void surface_to_vector();
    void vector_to_surface();
    
    [[nodiscard]] std::tuple<arma::mat, arma::mat, arma::mat, arma::mat> vector_to_matrices() const;
    void matrices_to_vector(const arma::mat& red, const arma::mat& green, const arma::mat& blue, const arma::mat& alpha);

    [[nodiscard]] std::tuple<arma::mat, arma::mat> meshgrid(const arma::vec& x, const arma::vec& y) const;
    [[nodiscard]] arma::mat interpolation(const arma::mat& matrix, const arma::mat& r_norm, const double x_center, const double y_center) const;
    
public:
    Application();
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    bool add_distortion();
    bool correct_distortion();
    bool load_file(const std::string& file_path);
    bool save_file(const std::string& file_path);
};


#endif /* Application_hpp */
