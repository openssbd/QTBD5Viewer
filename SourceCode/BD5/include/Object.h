/**
 * @file    Object.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Class to represent an object (In a BD5 file an object is contained in a group. 
 *          And it has a list of datasets as components, we call to every component of this list as subobject). 
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
#include <utility>
#include "utils.h"

namespace BD5 {

/**
 * @brief   The geometric elements used in BD5 files
 * 
 */
enum class EntityType
{
    Point,
    Circle,
    Sphere,
    Line,
    Face,
    Undefined
};

/* 
    Insert the coordinates of the item in a map using
    the chars x, y, z and r (x-axis, y-axis, z-axis, radius)
    for example x = 0.1; y = 0.2; z = 0.1; r = 5.0 
*/

/**
 * @brief   Generic representation of the data describing the geometric elements
 *          Notice that we use a map (dictionary) to reduce the complexity of describe geometry
 *          with different parameters. For example for describing a 3D point 3 float data are required
 *          The application insert three components with x y z as keys. For a 3D sphere the center point
 *          and a radius is required. The application insert a 3 float with x y z as keys and an additional
 *          float with r as key. This concept is applied for every geometric element.
 * 
 */
struct EntityData {
    std::string ID = "";
    std::string Label = "";    
    std::map<char, float> Values;
    /**
     * @brief   Test checking if contains 2D data
     * 
     * @return  true 
     * @return  false 
     */
    bool Is2D() const;
    /**
     * @brief   Test checking if contains 3D data
     * 
     * @return  true 
     * @return  false 
     */
    bool Is3D() const;
    /**
     * @brief   Test checking if contains radius data.
     * 
     * @return  true 
     * @return  false 
     */
    bool HasRadius() const;
    /**
     * @brief   Search for a component describing the geometric element. The function search if the
     *          key exists.
     * 
     * @return  true 
     * @return  false 
     */
    bool HasElement(const char&) const;
    /**
     * @brief   The X component
     * 
     * @return  float   X component 
     */
    float X() const;
    /**
     * @brief   The Y component
     * 
     * @return float    Y component
     */
    float Y() const;
    /**
     * @brief   The Z component
     * 
     * @return float    Z component
     */
    float Z() const;
    /**
     * @brief   The radius component
     * 
     * @return float    Radius component
     */
    float Radius() const;
    /**
     * @brief   Get the object type of the geometric element
     * 
     * @return  Geometric type of the element 
     */
    static EntityType GetEntityType(const std::string&);
};

/**
 * @brief   Class to represent an object (In a BD5 file an object is contained in a group. 
 *          And it has a list of datasets as components, 
 *          we call to every component of this list as subobject).
 *          Entities are individual geometric elements that assembled form a subobject.
 *          All data is structured in Vectors. We have next vectors showing its relationship: <br>
 *              vector<pair<first::Entity, second::vector<<vector<Entity>>> <br>
 *          where: <br>
 *              first = BD5 object_type <br>
 *              second = a vector of vectors of Entities <br>
 *              vector of vector of Entities = entities separated by sID (help to determine connected lines and polygons) <br>
 *              vector of entities by sID = The entities corresponding to this sID <br>
 */
class Object
{
public:
    void InsertObjectData(EntityType, const std::vector<std::vector<EntityData>>&);
    const std::vector<std::pair<EntityType, std::vector<std::vector<EntityData>>>>& GetAllSubObjects() const;
    const std::vector<std::vector<EntityData>>& GetSubObject(int) const;
    EntityType EntityTypeAtSubObject(int) const;
    const std::vector<EntityData>& GetEntitiesAtSubObjectNoSID(int) const;
    const std::vector<EntityData>& GetEntitiesAtSubObjectBySID(int, int) const;
    int GetNumSubObjects() const;
private:
    // Entities are stored as [first:object_type, second:Entities_list]::[sID (line and poly)]::[list_entities]
    std::vector< std::pair<EntityType, std::vector<std::vector<EntityData>> > > data;
};

}