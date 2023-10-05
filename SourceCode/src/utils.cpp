/**
 * @file    utils.cpp
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Auxiliar utils
 * @version 0.1
 * @date    2023-09-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "utils.h"

std::vector<float> ColorPalette::GetColorAt(int index) {
    std::vector<std::vector<float>> palette = {
        {1.0, 1.0, 1.0}, {1.0, 0.0, 0.0}, {1.0, 0.47, 0.47}, {1.0, 0.78, 0.78}, {1.0, 0.47, 0.0}, 
        {1.0, 0.78, 0.47}, {1.0, 0.90, 0.78}, {1.0, 0.94, 0.0}, {0.90, 1.0, 0.78}, {0.78, 1.0, 0.47}, 
        {0.47, 1.0, 0.0}, {0.78, 1.0, 0.78}, {0.47, 1.0, 0.47}, {0.0, 1.0, 0.0}, {0.78, 1.0, 0.90}, 
        {0.47, 1.0, 0.78}, {0.0, 1.0, 0.47}, {0.0, 1.0, 0.78}, {0.0, 1.0, 0.87}, {0.0, 1.0, 1.0}, 
        {0.0, 0.90, 1.0}, {0.47, 0.90, 1.0}, {0.47, 0.78, 1.0}, {0.78, 0.78, 1.0}, {0.47, 0.47, 1.0}, 
        {0.0, 0.0, 1.0}, {0.47, 0.0, 1.0}, {0.78, 0.0, 1.0}, {0.78, 0.47, 1.0}, {0.90, 0.78, 1.0}, 
        {0.87, 0.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 0.0, 0.87}, {1.0, 0.78, 0.90}, {1.0, 0.47, 0.78}, 
        {1.0, 0.0, 0.78}, {1.0, 0.0, 0.47}};

    std::size_t i = 0;
    std::size_t sIndex = static_cast<std::size_t>(index);
    i = (sIndex >= palette.size()) ? (sIndex % palette.size()) : sIndex;
    return palette.at(i);
}
