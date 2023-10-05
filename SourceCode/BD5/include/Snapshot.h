/**
 * @file    Snapshot.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Class representing a snapshot of Geometry contained in the BD5 at a specific time
 * @version 0.1
 * @date    2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include "Object.h"

namespace BD5 {

/**
 * @brief Class representing a snapshot of the Geometry contained in the BD5 at a specific time
 * 
 */
class Snapshot
{
public:
    /**
     * @brief Construct a new Snapshot object
     * 
     * @param in_Time   Time of the snapshot
     * @param in_Object A vector of geometric objects
     */
    Snapshot(float in_Time, const std::vector<BD5::Object>& in_Object);
    /**
     * @brief Construct a new Snapshot object
     * 
     */
    Snapshot() {};
    /**
     * @brief   Time of the snapshot
     * 
     * @return float    Time of the snapshot
     */
    float Time() const;
    /**
     * @brief   Get all the objects as a std::vector 
     * 
     * @return const std::vector<BD5::Object>&  Vector of objects
     */
    const std::vector<BD5::Object>& GetObjects() const;
    /**
     * @brief Get the object at index
     * 
     * @param index     Index pointed in the vector
     * @return const BD5::Object&   A reference to the object
     */
    const BD5::Object& GetObjectAt(int index) const;
private:
    float time = 0.0;
    std::vector<BD5::Object> objects;
};

}