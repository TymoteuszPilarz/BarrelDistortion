//
//  Application.cpp
//  BarrelDistortion
//
//  Created by Tymoteusz Pilarz on 14/01/2023.
//

#include <cstdint>
#include <array>

#include "Application.hpp"
#include "SDL_Exception.hpp"

void Application::surface_to_vector()
{
    std::vector<Color> row(surface->w);
    image.resize(surface->h, row);
    
    unsigned int bytes_per_pixel = surface->format->BytesPerPixel;
    
    for (unsigned int h = 0; h < surface->h; ++h)
    {
        for (unsigned int w = 0; w < surface->w; ++w)
        {
            uint32_t pixel{};
            uint8_t* pixel_ptr = reinterpret_cast<uint8_t*>(surface->pixels) + h * surface->pitch + w * bytes_per_pixel;
            
            switch(bytes_per_pixel)
            {
                case 1:
                    pixel = *pixel_ptr;
                    break;
                case 2:
                    pixel = *reinterpret_cast<uint16_t*>(pixel_ptr);
                    break;
                case 3:
                    if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    {
                        pixel = pixel_ptr[0] << 16 | pixel_ptr[1] << 8 | pixel_ptr[2];
                    }
                    else
                    {
                        pixel = pixel_ptr[0] | pixel_ptr[1] << 8 | pixel_ptr[2] << 16;
                    }
                    break;
                case 4:
                    pixel = *reinterpret_cast<uint32_t*>(pixel_ptr);
                    break;
            }
            
            Color pixel_color{};
            SDL_GetRGBA(pixel, surface->format, &pixel_color.red, &pixel_color.green, &pixel_color.blue, &pixel_color.alpha);
            
            image[h][w] = pixel_color;
        }
    }
}

void Application::vector_to_surface()
{
    unsigned int bytes_per_pixel = surface->format->BytesPerPixel;
    
    for (unsigned int h = 0; h < surface->h; ++h)
    {
        for (unsigned int w = 0; w < surface->w; ++w)
        {
            uint32_t pixel = SDL_MapRGBA(surface->format, image[h][w].red, image[h][w].green, image[h][w].blue, image[h][w].alpha);
            uint8_t* pixel_ptr = reinterpret_cast<uint8_t*>(surface->pixels) + h * surface->pitch + w * bytes_per_pixel;
            
            switch(bytes_per_pixel)
            {
                case 1:
                    *pixel_ptr = static_cast<uint8_t>(pixel);
                    break;
                case 2:
                    *reinterpret_cast<uint16_t*>(pixel_ptr) = static_cast<uint16_t>(pixel);
                    break;
                case 3:
                        pixel_ptr[0] = pixel >> 16;
                        pixel_ptr[1] = pixel >> 8 & 0x000000ff;
                        pixel_ptr[2] = pixel & 0x000000ff;
                    break;
                case 4:
                    *reinterpret_cast<uint32_t*>(pixel_ptr) = pixel;
                    break;
            }
        }
    }
}


std::tuple<arma::mat, arma::mat, arma::mat, arma::mat> Application::vector_to_matrices() const
{
    const auto height = image.size();
    const auto width = image[0].size();
    
    arma::mat red(height, width);
    arma::mat green(height, width);
    arma::mat blue(height, width);
    arma::mat alpha(height, width);
    
    for (unsigned int i = 0; i < height; ++i)
    {
        for (unsigned int j = 0; j < width; ++j)
        {
            red(i, j) = image[i][j].red;
            green(i, j) = image[i][j].green;
            blue(i, j) = image[i][j].blue;
            alpha(i, j) = image[i][j].alpha;
        }
    }
    
    return {red, green, blue, alpha};
}

void Application::matrices_to_vector(const arma::mat& red, const arma::mat& green, const arma::mat& blue, const arma::mat& alpha)
{
    const auto height = image.size();
    const auto width = image[0].size();
    
    for (unsigned int i = 0; i < height; ++i)
    {
        for (unsigned int j = 0; j < width; ++j)
        {
            image[i][j].red = static_cast<uint8_t>(red(i, j));
            image[i][j].green = static_cast<uint8_t>(green(i, j));
            image[i][j].blue = static_cast<uint8_t>(blue(i, j));
            image[i][j].alpha = static_cast<uint8_t>(alpha(i, j));
        }
    }
}

std::tuple<arma::mat, arma::mat> Application::meshgrid(const arma::vec& x, const arma::vec& y) const
{
    arma::mat X(y.size(), x.size());
    arma::mat Y(y.size(), x.size());
    
    for (unsigned int i = 0; i < y.size(); ++i)
    {
        for (unsigned int j = 0; j < x.size(); ++j)
        {
            X(i, j) = x(j);
            Y(i, j) = y(i);
        }
    }
    
    return {X, Y};
}

arma::mat Application::interpolation(const arma::mat& matrix, const arma::mat& r_norm, const double x_center, const double y_center) const
{
    const auto XI = arma::regspace(0, matrix.n_cols - 1);
    const auto YI = arma::regspace(0, matrix.n_rows - 1);
    
    arma::mat ret_matrix = matrix;
    
    for (int i = 0; i < matrix.n_rows; ++i)
    {
        arma::vec vec;
        arma::interp1(XI - x_center, matrix.row(i), (XI - x_center) % trans(r_norm.row(i)), vec);
        ret_matrix.row(i) = trans(vec);
    }
        
    for (int i = 0; i < ret_matrix.n_cols; ++i)
    {
        arma::vec vec;
        arma::interp1(YI - y_center, ret_matrix.col(i), (YI - y_center) % r_norm.col(i), vec);
        ret_matrix.col(i) = vec;
    }
    
    return ret_matrix;
}

Application::Application()
{
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        throw SDL_Exception("Failure during IMG initialization");
    }
}

Application::~Application()
{
    if (surface)
        SDL_FreeSurface(surface);
    IMG_Quit();
}

bool Application::add_distortion()
{
    if (image.size() == 0)
    {
        std::cerr << "Failed to process. Load the image file first" << std::endl;
        return false;
    }
    
    const auto height = image.size();
    const auto width = image[0].size();
    
    const auto cx = (width / 2) + 0.5;
    const auto cy = (height / 2) + 0.5;
    
    auto [red, green, blue, alpha] = vector_to_matrices();
    
    std::array a {1.06, -0.0002, 0.000005}; //example coefficients;
    
    const auto [X, Y] = meshgrid(arma::regspace(1, width),arma::regspace(1 ,height));
    const arma::mat r = arma::sqrt((X - cx) % (X - cx) + (Y - cy) % (Y - cy));
    const arma::mat R = a[0] * r + a[1] * (r % r) + a[2] * (r % r % r);
    const arma::mat r_norm = R / r;

    red = interpolation(red, r_norm, cx, cy);
    green = interpolation(green, r_norm, cx, cy);
    blue = interpolation(blue, r_norm, cx, cy);
    alpha = interpolation(alpha, r_norm, cx, cy);

    matrices_to_vector(red, green, blue, alpha);
    
    return true;
}

bool Application::correct_distortion()
{
    if (image.size() == 0)
    {
        std::cerr << "Failed to process. Load the image file first" << std::endl;
        return false;
    }
    
    const auto height = image.size();
    const auto width = image[0].size();
    
    const auto cx = (width / 2) + 0.5;
    const auto cy = (height / 2) + 0.5;
    
    auto [red, green, blue, alpha] = vector_to_matrices();
    
    std::array a {1.06, -0.0002, 0.000005}; //example coefficients
    
    const unsigned int n_step = 32;
    
    const auto [I, J] = meshgrid(arma::regspace(n_step, n_step, width - n_step), arma::regspace(n_step, n_step, height - n_step));
    
    const arma::mat r = arma::sqrt((I - cx) % (I - cx) + (J - cy) % (J - cy));
    const arma::mat R = a[0] * r + a[1] * (r % r) + a[2] * (r % r % r);
    
    const auto a_est = trans(polyfit(arma::sort(r.as_col()), arma::sort(R.as_col()), 3));
    
    const auto r_pol = arma::regspace(0, (width > height ? width : height) / 2);
    const auto a_inv = arma::polyfit(arma::polyval(a_est, r_pol), r_pol, 3);
    
    const auto [X, Y] = meshgrid(arma::regspace(1, width), arma::regspace(1, height));
    const arma::mat R_init = arma::sqrt((X-cx) % (X-cx) + (Y-cy) % (Y-cy));
    const arma::mat R_fixed = polyval(a_inv, R_init);
    const arma::mat R_norm = R_fixed / R_init;

    for (int i = 0; i < 2; i++)
    {
        red = interpolation(red, R_norm, cx, cy);
        green = interpolation(green, R_norm, cx, cy);
        blue = interpolation(blue, R_norm, cx, cy);
        alpha = interpolation(alpha, R_norm, cx, cy);
    }
    
    matrices_to_vector(red, green, blue, alpha);
    
    return true;
}

bool Application::load_file(const std::string& file_path)
{
    surface = IMG_Load(file_path.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load file: " << file_path << std::endl;
        return false;
    }
    
    surface_to_vector();
    
    return true;
}

bool Application::save_file(const std::string& file_path)
{
    vector_to_surface();
    
    if (IMG_SavePNG(surface, file_path.c_str()) == -1)
    {
        std::cerr << "Failed to save file: " << file_path << std::endl;
        return false;
    }
    
    return true;
}
