/**
 * @file    utils.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   Utility functions for handling basic string operations and extracting data from memory buffer into C++ data types
 * @version 0.1
 * @date    2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <sstream>
#include <locale>
#include <vector>

/**
 * @brief   Create a slice subvector from provided vector
 * 
 * @tparam T    A C++ type. We use a char vector for representing a buffer in memory
 * @param v     A vector representing a buffer in memory
 * @param from  Start position of slice
 * @param to    End position of slice
 * @return std::vector<T>  The new sliced vector
 */
template<typename T>
std::vector<T> slice(std::vector<T> const &v, int from, int to)
{
    auto first = v.cbegin() + from;
    auto last = v.cbegin() + to;
    std::vector<T> vec(first, last);
    return vec;
}

/**
 * @brief   Extract a number represented as a C++ type T from a vector representing a buffer in memory 
 * 
 * @tparam T    A C++ type such as int, unsigned int, long long, unsigned long long, float and double 
 * @param dataBuffer A vector of chars representing a buffer in memory
 * @param offset    Start position on the buffer where the function perform the extraction
 * @param size      Size of the data to be extracted to convert into a number of type T
 * @note    Be carefull with the size you provide. If the size and the C++ type doesn't match the function throw a length_error exception
 * @throw   std::out_of_range when extract with parameters produce extraction out of vector range.
 * @throw   std::length_error when C++ data type size provided in T does not match with size parameter 
 * @return T    The number extracted represented as the C++ type provided by T
 */
template <typename T>
T RawExtract(const std::vector<char>& dataBuffer, int offset, int size)
{
    size_t total = offset + size;
    if (dataBuffer.size() < total)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "Out of buffer. Requested offset " << offset << " and size " << size << " on a buffer with size " << dataBuffer.size();
        throw new std::out_of_range( std::string(ss.str()) );
    }
    std::vector<char> elementData = slice(dataBuffer, offset, (offset + size));
    T value = 0;
    if (sizeof(value) != size)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "Type byte lengths does not match. C++ type " << sizeof(value) << " BD5 type " << size;
        throw new std::length_error( std::string(ss.str()) );
    }
    std::memcpy(&value, static_cast<void *>(elementData.data()), size);
    return value;
}

// Template specialization for extracting string.
/**
 * @brief Specialization for extracting string from a vector representing a buffer in memory 
 * 
 * @param dataBuffer A vector of chars representing a buffer in memory
 * @param offset    Start position on the buffer where the function perform the extraction
 * @param size      Size of the data to be extracted to convert into a string
 * @throw   std::out_of_range when extract with parameters produce extraction out of vector range.
 * @return  std::string String extracted
 */
template <>
inline std::string RawExtract<std::string>(const std::vector<char>& dataBuffer, int offset, int size)
{
    size_t total = offset + size;
    if (dataBuffer.size() < total)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "Out of buffer. Requested offset " << offset << " and size " << size << " on a buffer with size " << dataBuffer.size();
        throw new std::out_of_range( std::string(ss.str()) );
    }
    try
    {
        std::vector<char> elementData = slice(dataBuffer, offset, (offset + size));
        std::string value = std::string(elementData.data());
        return value;
    }
    catch(...)
    {
        throw;
    }
}


/**
 * @brief   String left trim
 * 
 * @param s String to be trim.
 */
static inline void ltrim(std::string &s) 
{
    s.erase(s.begin(), 
            std::find_if(s.begin(), 
                         s.end(), 
                         [](unsigned char ch) {
                            return !std::isspace(ch);
                         })
    );
}

/**
 * @brief   String right trim
 * 
 * @param s String to be trim
 */
static inline void rtrim(std::string &s) 
{
    s.erase(std::find_if(s.rbegin(),
                         s.rend(),
                         [](unsigned char ch) {
                            return !std::isspace(ch);   
                         }).base(), s.end());
}

/**
 * @brief   String both ends trim
 * 
 * @param s String to be trim
 */
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

/**
 * @brief   String left trim creating a new string as result
 * 
 * @param s String to be trim
 * @return std::string  The trim created string
 */
static inline std::string ltrim_copy(std::string s) 
{
    ltrim(s);
    return s;
}

/**
 * @brief   String right trim creating a new string as result
 * 
 * @param s String to be trim
 * @return std::string  The trim created string
 */
static inline std::string rtrim_copy(std::string s) {
    rtrim (s);
    return s;
}

/**
 * @brief   String both ends trim creating a new string as result
 * 
 * @param s String to be trim
 * @return std::string  The trim created string
 */
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

