/**
 * @file    Group.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Class for reading and describing a BD5 Group
 * @version 0.1
 * @date    2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <string>
#include <vector>

namespace BD5 {

/**
 * @brief   Class for reading and describing a BD5 Group
 * 
 */
class Group
{
public:
    /**
     * @brief   Construct a new Group object
     * 
     * @param in_Name   Group full name 
     * @param in_Groups Vector of groups inside this group
     * @param in_Datasets   Vector of datasets inside this group
     */
    explicit Group(const std::string& in_Name, const std::vector<std::string>& in_Groups, const std::vector<std::string>& in_Datasets);
    /**
     * @brief   Full path-like (HDF5 format) group name
     * 
     * @return std::string  The full path-like group name
     */
    std::string FullName() const;
    /**
     * @brief   Vector of groups inside this group
     * 
     * @return std::vector<std::string>     Vector of groups
     */
    std::vector<std::string> Groups();
    /**
     * @brief   Vector of datasets inside this group
     * 
     * @return std::vector<std::string>     Vector of datasets
     */
    std::vector<std::string> Datasets();
private:
    std::string fullName = "";
    std::vector<std::string> groups;
    std::vector<std::string> datasets;
};

}