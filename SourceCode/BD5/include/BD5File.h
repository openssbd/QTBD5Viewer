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
#include <limits>
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
    std::string OBJECT_DEF = "/data/objectDef";
    std::string TRACK_INFO = "/data/trackInfo";
    std::string LOG_FILE  = "bd5Viewer.log";
    bool BD5FILE_INFO_FLAG = true;
};

/**
 * @brief   The minimum and maximum coordinates of the geometry
 * 
 */
struct Boundaries {
    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();
    float maxZ = std::numeric_limits<float>::min();
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    void updateBoundaries(const Boundaries&);
};

/**
 * @brief   Description of a point in a track
 * 
 */
struct PointTrack {
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    int id = 0;
    std::string objID = "";
    std::string label = "";
    void print(void) {
        std::cout << " id " << id << " objID " << objID << " label " << label 
            << " x " << x << " " << x*0.105 << " y " << y << " " << y*0.105 << " z " << z << " " << z*0.5 << std::endl;        
    }
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
     * @brief   Get Object definitions
     * 
     * @return vector<string> A vector containing the object names
     */
    std::vector<std::string> ObjectsNames() const;
    /**
     * @brief Get the minimum and maximum coordinates of the geometry
     * 
     * @return Boundaries   The minimum and maximum coordinates of the geometry
     */
    Boundaries GetBoundaries() const;
    /**
     * @brief   Indicates if the dataset contains track info
     * 
     * @return true 
     * @return false 
     */
    bool HasTracks() const;
    /**
     * @brief   Get the a list of tracks described as PointTrack
     * 
     * @return std::vector<std::vector<PointTrack>> 
     */
    std::vector<std::vector<PointTrack>> GetTracks() const;

    /**
     * @brief   Get the labels corresponding to the time t
     * 
     */
    std::vector<std::vector<std::string>> GetLabelsAtTime(int);

private:
    void Open(const std::string& f);
    BD5::ScaleUnit GetScaleUnit(BD5::DataSet&);
    std::vector<std::string> GetObjNames(BD5::DataSet&);
    std::vector<std::pair<std::string, std::string>> GetRawTrackInfo(BD5::DataSet&);
    std::vector<std::vector<std::string>> MakeTrackPaths(const std::vector<std::pair<std::string, std::string>>&);
    std::vector<PointTrack> WriteTrackInfo(int, const std::vector<std::vector<EntityData>>&);
    std::vector<std::vector<PointTrack>> WriteTracksGeometry(const std::vector<std::vector<std::string>>&,
                                        const std::vector<PointTrack>&);
    std::vector<std::vector<PointTrack>> CreateTracks(const std::vector<BD5::Snapshot>&, const std::vector<std::vector<std::string>>&);
    TypeDescriptor GetTypeDescriptor(const H5::CompType&);
    ElementType GetElementType(const H5::CompType&, int);
    std::vector<std::vector<PointTrack>> tracks;
    BD5::ScaleUnit scales;
    std::vector<std::string> objNames;
    // Labels are ordered as: time_index, obj_index, label_index
    std::vector<std::vector<std::vector<std::string>>> labels;
    H5::H5File file;
    BD5::Boundaries boundaries;
    Settings settings;
    Logger logger;    
};

}
