#include "ScaleUnit.h"

using namespace BD5;
using namespace std;

void ScaleUnit::Set0Dimension(const std::string& dimension)
{
    kindScaleUnit = SpaceTime_t::ZeroDimension;
    this->dimension = dimension;
}

void ScaleUnit::Set1Dimension(const std::string& dimension, const float& x, const std::string& sUnit)
{
    kindScaleUnit = SpaceTime_t::OneDimension;
    this->dimension = dimension;
    this->xScale = x;
    this->sUnit = sUnit;
}

void ScaleUnit::Set2Dimension(const std::string& dimension, const float& x, const float& y, const std::string& sUnit)
{
    kindScaleUnit = SpaceTime_t::TwoDimension;
    this->dimension = dimension;
    this->xScale = x;
    this->yScale = y;
    this->sUnit = sUnit;
}

void ScaleUnit::Set3Dimension(const std::string& dimension, const float& x, const float& y, const float& z, const std::string& sUnit)
{
    kindScaleUnit = SpaceTime_t::ThreeDimension;
    this->dimension = dimension;
    this->xScale = x;
    this->yScale = y;
    this->zScale = z;
    this->sUnit = sUnit;
}

void ScaleUnit::Set0DimensionAndTime(const std::string& dimension, const float& t, const std::string& tUnit)
{
    kindScaleUnit = SpaceTime_t::ZeroDimensionAndTime;
    this->dimension = dimension;
    this->tScale = t;
    this->tUnit = tUnit;
}

void ScaleUnit::Set1DimensionAndTime(const std::string& dimension, const float& x, const std::string& sUnit, 
    const float& t, const std::string& tUnit)
{
    kindScaleUnit = SpaceTime_t::OneDimensionAndTime;
    this->dimension = dimension;
    this->xScale = x;
    this->sUnit = sUnit;
    this->tScale = t;
    this->tUnit = tUnit;
}

void ScaleUnit::Set2DimensionAndTime(const std::string& dimension, const float& x, const float& y, const std::string& sUnit, 
    const float& t, const std::string& tUnit)
{
    kindScaleUnit = SpaceTime_t::TwoDimensionAndTime;
    this->dimension = dimension;
    this->xScale = x;
    this->yScale = y;
    this->sUnit = sUnit;
    this->tScale = t;
    this->tUnit = tUnit;
}

void ScaleUnit::Set3DimensionAndTime(const std::string& dimension, const float& x, const float& y, const float& z, const std::string& sUnit, 
    const float& t, const std::string& tUnit)
{
    kindScaleUnit = SpaceTime_t::ThreeDimensionAndTime;
    this->dimension = dimension;
    this->xScale = x;
    this->yScale = y;
    this->zScale = z;
    this->sUnit = sUnit;
    this->tScale = t;
    this->tUnit = tUnit;
}

std::string ScaleUnit::Dimension() const
{
    return dimension;
}

float ScaleUnit::XScale() const
{
    return xScale;
}

float ScaleUnit::YScale() const
{
    return yScale;
}

float ScaleUnit::ZScale() const
{
    return zScale;
}

std::vector<float> ScaleUnit::ScaleAs2D() const
{
    vector<float> point2D{ xScale, yScale };
    return point2D;
}

std::vector<float> ScaleUnit::ScaleAs3D() const
{
    vector<float> point3D{ xScale, yScale, zScale };
    return point3D;
}

std::string ScaleUnit::SUnit() const
{
    return sUnit;
}

float ScaleUnit::TScale() const
{
    return tScale;
}

std::string ScaleUnit::TUnit() const
{
    return tUnit;
}

SpaceTime_t ScaleUnit::Type() const
{
    return kindScaleUnit;
}

SpaceTime_t ScaleUnit::GetSpaceTimeType(const string& text)
{
    std::map<SpaceTime_t, std::string> types = { { SpaceTime_t::ZeroDimension, "0D" },
                                { SpaceTime_t::OneDimension, "1D" },
                                { SpaceTime_t::TwoDimension, "2D" },
                                { SpaceTime_t::ThreeDimension, "3D" },
                                { SpaceTime_t::ZeroDimensionAndTime, "0D+T" },
                                { SpaceTime_t::OneDimensionAndTime, "1D+T" },
                                { SpaceTime_t::TwoDimensionAndTime, "2D+T" },
                                { SpaceTime_t::ThreeDimensionAndTime, "3D+T" },
                                };
 
 
    for (auto& type: types)
    {
        if (text == type.second) {
            return type.first;
        }
    }
    return SpaceTime_t::Undefined;
}