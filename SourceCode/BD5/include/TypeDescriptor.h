/**
 * @file    TypeDescriptor.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   TypeDescritor and TypeElementDescriptor classes. Both classes describe a BD5 datatype structure. 
 *          A BD5 is represented as a Compound datatype containing smaller basic types. 
 *          These classes help to interpret and extract individual basic types components.
 * @version 0.1
 * @date    2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <vector>
#include "utils.h"


namespace BD5 {

/**
 * @brief BD5 types are based on HDF5 types. On HDF5 types many different interpretations are possible based on hardware architectures
 *        We narrow the HDF5 representations to a basic C++ data types available on desktop computers. 
 *        This enum help to classify the types read from BD5 file into basic C++ data types
 * 
 */
enum class ElementType 
{
    String,             // any size string (1-byte per character)
    Integer,            // 4-bytes int
    LongLong,           // 8-bytes int
    UnsignedInteger,    // 4-bytes unsigned int
    UnsignedLongLong,   // 8-bytes unsigned long long
    Float,              // 4-bytes float
    Double,             // 8-bytes double
    Undefined
};

/**
 * @brief Class representing a individual basic type contained into a TypeDescriptor
 *        A extraction from this basic type into a C++ data type is possible
 * 
 */
class TypeElementDescriptor
{
public:
    /**
     * @brief Construct a new Type Element Descriptor object
     * 
     * @param in_Size   Number of bytes of this type
     * @param in_Offset Start position inside the buffer of this type 
     * @param in_Type   Assigned most similar C++ data type for this element
     */
    explicit TypeElementDescriptor(int in_Size, int in_Offset, ElementType in_Type) :
        size(in_Size), offset(in_Offset), type(in_Type) {}
    /**
     * @brief Construct a new Type Element Descriptor object
     * 
     */
    explicit TypeElementDescriptor() {};
    /**
     * @brief  Size of the type in bytes
     * 
     * @return int  Size of the type in bytes 
     */
    int Size() const;
    /**
     * @brief   Start position inside the buffer of this type 
     * 
     * @return int  Start position inside the buffer of this type
     */
    int Offset() const;
    /**
     * @brief  Assigned most similar C++ data type for this element
     * 
     * @return ElementType Assigned most similar C++ data type for this element
     */
    ElementType Type() const;

    /**
     * @brief   Extract a element as a number represented as a C++ data type
     * 
     * @tparam T    A C++ data type such as int, unsigned int, long long, unsigned long long, float and double 
     * @param dataBuffer A vector of chars representing a buffer in memory
     * @throw std::out_of_range if try to interpret with a type not defined on enum ElementType
     * @return T    The number extracted from dataBuffer represented as the C++ type provided by T
     */
    template <typename T>
    T ExtractNumberAs(const std::vector<char>& dataBuffer)
    {
        T result = 0;
        try
        {
            std::vector<char> elementData = slice(dataBuffer, offset, (offset + size));
            switch (type)
            {
            case ElementType::Integer:
            {
                auto value = RawExtract<int>(dataBuffer, offset, size);
                result = static_cast<T>(value);
                break;
            }
            case ElementType::LongLong:
            {
                auto value = RawExtract<long long>(dataBuffer, offset, size);
                result = static_cast<T>(value);
                break;
            }
            case ElementType::UnsignedInteger:
            {
                auto value = RawExtract<unsigned int>(dataBuffer, offset, size);
                result = static_cast<T>(value);
                break;
            }
            case ElementType::UnsignedLongLong:
            {
                auto value = RawExtract<unsigned long long>(dataBuffer, offset, size);
                result = static_cast<T>(value);
                break;
            }
            case ElementType::Float:
            {
                auto value = RawExtract<float>(dataBuffer, offset, size);
                result = static_cast<T>(value);
                break;
            }
            case ElementType::Double:
            {
                auto value = RawExtract<double>(dataBuffer, offset, size);
                result = static_cast<T>(value);
                break;
            }
            case ElementType::String:
            {
                auto value = RawExtract<std::string>(dataBuffer, offset, size);
                if (std::is_same<T, int>::value)
                {
                    result = stoi(value);
                }
                if (std::is_same<T, long long>::value)
                {
                    result = stol(value);
                }
                if (std::is_same<T, unsigned int>::value)
                {
                    unsigned long uLValue = stoul(value);
                    result = static_cast<unsigned int>(uLValue);
                }
                if (std::is_same<T, unsigned long long>::value)
                {
                    result = stoull(value);
                }
                if (std::is_same<T, float>::value)
                {
                    result = stof(value);
                }
                if (std::is_same<T, double>::value)
                {
                    result = stod(value);
                }

                break;
            }
            case ElementType::Undefined:
                {
                std::ostringstream ss;
                ss << __FILE__ << ":" << __func__ << "() " << "Element with an undefined type";
                throw new std::out_of_range( std::string(ss.str()) );
                break;
                }
            }
        }
        catch (...)
        {
            throw;
        }

        return result;
    }

    /**
     * @brief   Extract a element as a std::string
     * 
     * @param dataBuffer    A vector of chars representing a buffer in memory
     * @return std::string  string extracted from dataBuffer
     */
    std::string ExtractString(const std::vector<char>& dataBuffer);

private:
    int size = 0;
    int offset = 0;
    ElementType type = ElementType::Undefined;
};

/**
 * @brief   Class representing a BD5 Compound type. A compound type is composed by several basic BD5 types
 *          Basic BD5 types are stored in a map (dictionary) of TypeElementDescritor with its key referenced with the element name
 * 
 */
class TypeDescriptor
{
public:
    /**
     * @brief Construct a new Type Descriptor object
     * 
     * @param elem      A map (dictionary) containing the basic element types
     * @param in_Size   Size of the data represented by this TypeDescriptor in the memory buffer
     */
    explicit TypeDescriptor(const std::map<std::string, TypeElementDescriptor>& elem, int in_Size);
    /**
     * @brief Get the basic elements names
     * 
     * @return std::vector<std::string> The names of all the basic elements
     */
    std::vector<std::string> GetElementsNames() const;
    /**
     * @brief   Get the number of basic elements
     * 
     * @return int  number of basic elements
     */
    int GetNumElements() const;
    /**
     * @brief   Get the size on bytes of the basic element with name 
     * 
     * @param name  Name of the basic element
     * @return int  Size on bytes
     */
    int ElementSize(const std::string& name) const;
    /**
     * @brief   Start position on the buffer for the basic element with name
     * 
     * @param name  Name of the basic element
     * @return int  Start position on the buffer
     */
    int ElementOffset(const std::string& name) const;
    /**
     * @brief   Check if the basic element with name exists
     * 
     * @param name  Name of the basic element
     * @return true 
     * @return false 
     */
    bool ContainsElement(const std::string& name) const;
    /**
     * @brief Get the most similar C++ type for the basic element with name
     * 
     * @param name  Name of the basic element
     * @return ElementType  Most similar C++ type for the basic element with name
     */
    ElementType GetElementType(const std::string& name) const;
    /**
     * @brief   Size of the overall Compound data described by this TypeDescriptor in bytes
     * 
     * @return int  Size in bytes of the data described by this TypeDescriptor
     */
    int Size() const;
    /**
     * @brief   Extract the basic element with name as a number represented as a C++ type T from a vector 
     *          representing a buffer in memory 
     * 
     * @tparam T    A C++ type such as int, unsigned int, long long, unsigned long long, float and double 
     * @param name  Name of the basic element
     * @param dataBuffer A vector of chars representing a buffer in memory
     * @return T    The number extracted represented as the C++ type provided by T
     */
    template<typename T>
    T ExtractNumberAs(const std::string& name, const std::vector<char>& dataBuffer)
    {
        try
        {
            auto element = GetElement(name);
            return element.ExtractNumberAs<T>(dataBuffer);
        }
        catch (...)
        {
            // std::cout << "Error reading element: " << name << std::endl;
            throw;
        }
    }

    /**
     * @brief   Extract the basic element with name as a string from a vector representing a buffer in memory 
     * 
     * @param name      Name of the basic element
     * @param dataBuffer A vector of chars representing a buffer in memory
     * @return std::string String extracted
     */
    std::string ExtractString(const std::string& name, const std::vector<char>& dataBuffer);

private:
    // Store a map with { key:descriptor_name, value:descriptor } 
    std::map<std::string, TypeElementDescriptor> elements;
    TypeElementDescriptor GetElement(const std::string&) const;
    int size = 0;
};

}