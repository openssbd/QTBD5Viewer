/**
 * @file    ScaleUnit.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Class to represent the Scales applied to the overall geometries contained in BD5 files
 * @version 0.1
 * @date    2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace BD5{

/**
 * @brief   enum helping to classify the scales availble
 * 
 */
enum class SpaceTime_t
{
    ZeroDimension,
    OneDimension,
    TwoDimension,
    ThreeDimension,
    ZeroDimensionAndTime,
    OneDimensionAndTime,
    TwoDimensionAndTime,
    ThreeDimensionAndTime,
    Undefined
};

/**
 * @brief   Class to represent the Scales applied to the overall geometries contained in BD5 files
 * 
 */
class ScaleUnit
{
public:
    /**
     * @brief   Define the object no contain any scale information
     * 
     * @param dimension     Must be "0D"
     */
    void Set0Dimension(const std::string& dimension);
    /**
     * @brief   Define the object having only 1 dimension scale information
     * 
     * @param dimension     Must be "1D"
     * @param x             Scale factor on X
     * @param sUnit         Dimensions scale units
     */
    void Set1Dimension(const std::string& dimension, const float& x, const std::string& sUnit);
    /**
     * @brief   Define the object having 2 dimensions scale information
     * 
     * @param dimension     Must be "2D"
     * @param x             Scale factor on X
     * @param y             Scale factor on Y
     * @param sUnit         Dimensions scale units 
     */
    void Set2Dimension(const std::string& dimension, const float& x, const float& y, const std::string& sUnit);
    /**
     * @brief   Define the object having 3 dimensions scale information  
     * 
     * @param dimension     Must be "3D"
     * @param x             Scale factor on X
     * @param y             Scale factor on Y
     * @param z             Scale factor on Z
     * @param sUnit         Dimensions scale units 
     */
    void Set3Dimension(const std::string& dimension, const float& x, const float& y, const float& z, const std::string& sUnit);
    /**
     * @brief   Define the object no contain dimension scales only time scale information
     * 
     * @param dimension     Must be "0D+T"
     * @param t             Scale factor on time
     * @param tUnit         Time scale unit
     */
    void Set0DimensionAndTime(const std::string& dimension, const float& t, const std::string& tUnit);
    /**
     * @brief   Define the object having only 1 dimension scale and time scale information
     * 
     * @param dimension     Must be "1D+T"
     * @param x             Scale factor on X
     * @param sUnit         Dimensions scale units 
     * @param t             Scale factor on time
     * @param tUnit         Time scale unit
     */
    void Set1DimensionAndTime(const std::string& dimension, const float& x, const std::string& sUnit, 
        const float& t, const std::string& tUnit);
    /**
     * @brief   Define the object having 2 dimensions scale and time scale information
     * 
     * @param dimension     Must be "2D+T"
     * @param x             Scale factor on X
     * @param y             Scale factor on Y
     * @param sUnit         Dimensions scale units 
     * @param t             Scale factor on time
     * @param tUnit         Time scale unit
     */
    void Set2DimensionAndTime(const std::string& dimension, const float& x, const float& y, const std::string& sUnit, 
        const float& t, const std::string& tUnit);
    /**
     * @brief   Define the object having 3 dimensions scale and time scale information
     * 
     * @param dimension    Must be "3D+T" 
     * @param x            Scale factor on X
     * @param y            Scale factor on Y
     * @param z            Scale factor on Z
     * @param sUnit        Dimensions scale units 
     * @param t            Scale factor on time
     * @param tUnit        Time scale unit
     */
    void Set3DimensionAndTime(const std::string& dimension, const float& x, const float& y, const float& z, const std::string& sUnit, 
        const float& t, const std::string& tUnit);
    /**
     * @brief   Dimension string
     * 
     * @return std::string  Dimension string
     */
    std::string Dimension() const;
    /**
     * @brief   Dimension scale factor on X 
     * 
     * @return float    Dimension scale factor on X
     * 
     */
    float XScale() const;
    /**
     * @brief   Dimension scale factor on Y
     * 
     * @return float    Dimension scale factor on Y
     */
    float YScale() const;
    /**
     * @brief   Dimension scale factor on Z
     * 
     * @return float    Dimension scale factor on Z
     */
    float ZScale() const;
    std::vector<float> ScaleAs2D() const;
    /**
     * @brief   Get the overall scale factors as a vector
     * 
     * @return std::vector<float>   Vector containing X Y Z dimensions scales factors
     */
    std::vector<float> ScaleAs3D() const;
    /**
     * @brief   Dimension units
     * 
     * @return std::string  Dimension units
     */
    std::string SUnit() const;
    /**
     * @brief   Time scale factor
     * 
     * @return float    Time scale factor
     */
    float TScale() const;
    /**
     * @brief   Time units
     * 
     * @return std::string  Time units 
     */
    std::string TUnit() const;
    /**
     * @brief   The type of scales contained in this object
     * 
     * @return SpaceTime_t  Type of scales
     */
    SpaceTime_t Type() const;
    /**
     * @brief   Convert the dimension element from DB5 file into a scale type handle by this class
     * 
     * @return SpaceTime_t  Type of scales
     */
    static SpaceTime_t GetSpaceTimeType(const std::string& text);
 private:
    std::string dimension = "";
    float xScale = 0.0;
    float yScale = 0.0;
    float zScale = 0.0;
    std::string sUnit = "";
    float tScale = 0.0;
    std::string tUnit = "";
    SpaceTime_t kindScaleUnit = SpaceTime_t::Undefined;
};

}