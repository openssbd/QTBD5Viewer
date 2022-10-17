#include "TypeDescriptor.h"
#include <cstring>
#include <sstream>


using namespace std;
using namespace BD5;

int TypeElementDescriptor::Size() const
{
    return size;
}

int TypeElementDescriptor::Offset() const
{
    return offset;
}

ElementType TypeElementDescriptor::Type() const
{
    return type;
}


std::string TypeElementDescriptor::ExtractString(const std::vector<char>& dataBuffer)
{
    string result = "";
    try
    {
        std::vector<char> elementData = slice(dataBuffer, offset, (offset + size));
        switch (type)
        {
            case ElementType::Integer:
            
                {
                auto value = RawExtract<int>(dataBuffer, offset, size);
                result = std::to_string(value);
                break;
                }
            case ElementType::LongLong:
                {
                auto value = RawExtract<long long>(dataBuffer, offset, size);
                result = std::to_string(value);
                break;
                }            
            case ElementType::UnsignedInteger:
                {
                auto value = RawExtract<unsigned int>(dataBuffer, offset, size);
                result = std::to_string(value);
                break;
                }
            case ElementType::UnsignedLongLong:
                {
                auto value = RawExtract<unsigned long long>(dataBuffer, offset, size);
                result = std::to_string(value);
                break;
                }            
            case ElementType::Float:
                {
                auto value = RawExtract<float>(dataBuffer, offset, size);
                result = std::to_string(value);
                break;
                }
            case ElementType::Double:
                {
                auto value = RawExtract<double>(dataBuffer, offset, size);
                result = std::to_string(value);
                break;
                }                             
            case ElementType::String:
                {
                auto value = RawExtract<string>(dataBuffer, offset, size);
                result = value;
                break;
                }
            case ElementType::Undefined:
                {
                ostringstream ss;
                ss << __FILE__ << ":" << __func__ << "() " << "Element with an undefined type";
                throw new std::out_of_range( string(ss.str()) );
                break;
                }
        }
    } catch(...) 
    {
        throw;
    }

    return result;
}



TypeDescriptor::TypeDescriptor(const std::map<std::string, TypeElementDescriptor>& elem, int in_Size) :
    elements(elem), size(in_Size)
{

}

TypeElementDescriptor TypeDescriptor::GetElement(const std::string& name) const
{
    try
    {
        return elements.at(name);       
    }
    catch(const out_of_range& ex)
    {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "Element not found. With name " << name;
        throw new out_of_range(string(ss.str()));
        throw;
    }
}

vector<string> TypeDescriptor::GetElementsNames() const
{
    vector<string> names;
    for (auto& item: elements)
        names.push_back(item.first);    
    return names;
}

int TypeDescriptor::GetNumElements() const
{
    return elements.size();
}

int TypeDescriptor::Size() const
{
    return size;
}

int TypeDescriptor::ElementSize(const std::string& name) const
{
    try
    {
        auto element = GetElement(name);
        return element.Size();
    }
    catch(...)
    {
        throw;
    }
}

int TypeDescriptor::ElementOffset(const std::string& name) const
{
    try
    {
        auto element = GetElement(name);
        return element.Offset();        
    }
    catch(...)
    {
        throw;
    }
}

bool TypeDescriptor::ContainsElement(const std::string& name) const
{
    int n = elements.count(name);
    return n > 0;
}


ElementType TypeDescriptor::GetElementType(const std::string& name) const
{
    try
    {
        auto element = GetElement(name);
        return element.Type();        
    }
    catch(...)
    {
        throw;
    }
}

std::string TypeDescriptor::ExtractString(const std::string& name, const std::vector<char>& data)
{
    try
    {
        auto element = GetElement(name);
        return element.ExtractString(data);
    }
    catch(...)
    {
        throw;
    }
}
