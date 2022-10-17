/**
 * @file    DataSet.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Class for reading and describing a BD5 dataset
 * @version 0.1
 * @date    2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "TypeDescriptor.h"

namespace BD5 {

/**
 * @brief   Class for reading and describing a BD5 dataset
 * 
 */
class DataSet
{
public:
    /**
     * @brief   Construct a new Data Set object
     * 
     * @param in_Name   Dataset full name
     * @param in_Rank   Dataset rank
     * @param in_NumItems   Number of items in dataset
     * @param descriptor    BD5 Type descriptor of the dataset
     * @param buffer    Unique pointer to a buffer represented as a vector of char
     */
    explicit DataSet(const std::string& in_Name, int in_Rank, int in_NumItems,
                BD5::TypeDescriptor descriptor, std::unique_ptr<std::vector<char> > buffer) :
                    fullName(in_Name), rank(in_Rank), numItems(in_NumItems), 
                    typeDescriptor(descriptor), dataBuffer(std::move(buffer)) {}
    DataSet(const DataSet&) = delete;
    DataSet& operator=(const DataSet&) = delete;
    /**
     * @brief   Full path-like (HDF5 format) dataset name
     * 
     * @return std::string  Full path-like dataset name
     */
    std::string FullName() const;
    /**
     * @brief   The number of members inside the BD5 Type descriptor
     * 
     * @return int  Number of BD5 Type descriptor members 
     */
    int NumMembers() const;
    /**
     * @brief   Rank of the dataset
     * 
     * @return int  Rank of the dataset
     */
    int Rank() const;
    /**
     * @brief   Dataset number of items
     * 
     * @return int  Dataset number of items
     */
    int NumItems() const;
    /**
     * @brief   Vector containing the name of the member inside the BD5 Type descriptor
     * 
     * @return std::vector<std::string>     Vector of members inside the BD5 Type descriptor
     */
    std::vector<std::string> MembersNames();
    /**
     * @brief   Get the most similar C++ data type to the BD5 data type with name
     * 
     * @param name  Name of the member of the BD5 Type descriptor
     * @return ElementType  The most similar C++ data type to the BD5 data type
     */
    ElementType GetElementType(const std::string& name) const;
    /**
     * @brief   Get the BD5 Type descriptor
     * 
     * @return BD5::TypeDescriptor  The BD5 Type descriptor
     */
    BD5::TypeDescriptor GetTypeDescriptor() const;
    /**
     * @brief   Check if the BD5 Type descriptor contains the member with name
     * 
     * @param name  Name to be searched in the BD5 Type descriptor
     * @return true 
     * @return false 
     */
    bool ContainsElement(const std::string& name);
    /**
     * @brief   Get the Data at index
     * 
     * @param index     Index to search
     * @return std::vector<char> 
     */
    std::vector<char> GetDataAt(int index);
    /**
     * @brief   Extract a number from the dataset 
     * 
     * @tparam T    A C++ data type such as int, unsigned int, long long, unsigned long long, float and double 
     * @param name  Name to be searched in the BD5 Type descriptor
     * @param index Index to be searched in the dataset
     * @return T    The number extracted from dataset represented as the C++ type provided by T
     */
    template <typename T>
    T ExtractNumberAsAt(const std::string& name, int index)
    {
        try
        {
            auto rowData = GetDataAt(index);
            return typeDescriptor.ExtractNumberAs<T>(name, rowData);
        }
        catch (...)
        {
            throw;
        }
    }
    /**
     * @brief   Extract a string from the dataset
     * 
     * @param name  Name to be searched in the BD5 Type descriptor
     * @param index Index to be searched in the dataset
     * @return std::string The string extracted from dataset
     */
    std::string ExtractStringAt(const std::string& name, int index); 

    // For debugging.
    void ShowBuffer() const;
    void ShowElements() const;
private:
    std::string fullName = "";
    int rank = 0;
    int numItems = 0;
    BD5::TypeDescriptor typeDescriptor;
    std::unique_ptr<std::vector<char> > dataBuffer;
};

}