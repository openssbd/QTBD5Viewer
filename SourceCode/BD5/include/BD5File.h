/**
 * @file    BD5File.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Class for reading and describing a BD5 file
 * @version 0.1
 * @date    2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <string>
#include <memory>
#include <map>
#include "H5Cpp.h"
#include "ScaleUnit.h"
#include "DataSet.h"
#include "Group.h"
#include "Object.h"
#include "Snapshot.h"
#include "Logger.h"

namespace BD5{

/**
 * @brief   BD5 general settings
 * 
 */
struct Settings {
    std::string DATA = "/data";
    std::string SCALE_UNIT = "/data/scaleUnit";
    std::string OBJECT_DEF = "/data/objectDef";;
    std::string LOG_FILE  = "bd5Viewer.log";;
    bool BD5FILE_INFO_FLAG = true;
};

/**
 * @brief   The minimum and maximum coordinates of the geometry
 * 
 */
struct Boundaries {
    float maxX = 0.0;
    float maxY = 0.0;
    float maxZ = 0.0;
    float minX = 0.0;
    float minY = 0.0;
    float minZ = 0.0;
};

/**
 * @brief   Class for reading and describing a BD5 file
 * 
 */
class BD5File
{
public:
    /**
     * @brief   Construct a new BD5File object
     * 
     */
    explicit BD5File();
    /**
     * @brief   Construct a new BD5File object
     * 
     * @param f File path
     */
    explicit BD5File(const std::string& f);
    BD5File(const BD5File&) = delete;
    BD5File& operator=(const BD5File&) = delete;
    /**
     * @brief   Read the vector of BD5 snapshots from the file
     * 
     * @param f File path
     * @return std::vector<BD5::Snapshot>   Vector of BD5 snapshots 
     */
    std::vector<BD5::Snapshot> Read(const std::string& f);
    /**
     * @brief   Read the vector of BD5 snapshots from current stored file path
     * 
     * @return std::vector<BD5::Snapshot>   Vector of BD5 snapshots
     */
    std::vector<BD5::Snapshot> Read();
    /**
     * @brief   Read a dataset
     *  
     * @param setPath   Full path-like dataset name 
     * @return BD5::DataSet The BD5 dataset
     */
    BD5::DataSet ReadDataSet(const std::string& setPath);
    /**
     * @brief   Read a group
     * 
     * @param groupPath Full path-like group name
     * @return BD5::Group   The BD5 group
     */
    BD5::Group ReadGroup(const std::string& groupPath);
    /**
     * @brief   File path
     * 
     * @return * std::string    File path 
     */
    std::string Name() const;
    /**
     * @brief   Get the BD5 Scales units
     * 
     * @return BD5::ScaleUnit The BD5 Scales units
     */
    BD5::ScaleUnit Scales() const;
    /**
     * @brief Get the minimum and maximum coordinates of the geometry
     * 
     * @return Boundaries   The minimum and maximum coordinates of the geometry
     */
    Boundaries GetBoundaries() const;

private:
    void Open(const std::string& f);
    BD5::ScaleUnit GetScaleUnit(BD5::DataSet&);
    TypeDescriptor GetTypeDescriptor(const H5::CompType&);
    ElementType GetElementType(const H5::CompType&, int);
    BD5::ScaleUnit scales;
    H5::H5File file;
    BD5::Boundaries boundaries;
    Settings settings;
    Logger logger;    
};

}
