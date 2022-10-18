#include <iostream>
#include "DataSet.h"
#include "utils.h"

using namespace std;
using namespace BD5;

std::string DataSet::FullName() const
{
    return fullName;
}

int DataSet::NumMembers() const
{
    return typeDescriptor.GetNumElements();
}

int DataSet::Rank() const
{
    return rank;
}

int DataSet::NumItems() const
{
    return numItems;
}

vector<string> DataSet::MembersNames()
{
    return typeDescriptor.GetElementsNames();
}

ElementType DataSet::GetElementType(const std::string & name) const
{
    try
    {
        return typeDescriptor.GetElementType(name);
    }
    catch(...)
    {
        throw;
    }
}

BD5::TypeDescriptor DataSet::GetTypeDescriptor() const
{
    return typeDescriptor;
}

bool DataSet::ContainsElement(const std::string& name)
{
    return typeDescriptor.ContainsElement(name);
}

vector<char> DataSet::GetDataAt(int index)
{
    if ( (index < 0) || (index > numItems) ) {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "Row index out of range " << index;
        throw new std::out_of_range(string(ss.str()));        
    }
    size_t from = typeDescriptor.Size() * index;
    size_t to = from + typeDescriptor.Size();
    if (to > dataBuffer->size()) {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "Row size out of range. Buffer size " << dataBuffer->size() << " request size " << to;
        throw new std::out_of_range(string(ss.str()));        
    }
    auto row = slice(*dataBuffer, from, to);
    return row;
}

std::string DataSet::ExtractStringAt(const std::string& name, int row)
{
    try
    {
        auto rowData = GetDataAt(row);
        return typeDescriptor.ExtractString(name, rowData);
    }
    catch(...)
    {
        throw;
    }
}


void DataSet::ShowBuffer() const
{
    cout << "vector size " << dataBuffer->size() << endl;
    for (auto& item: *dataBuffer)
    {
        cout << item;
    }
    cout << " End " << endl;
}

void DataSet::ShowElements() const
{
    vector<string> names = typeDescriptor.GetElementsNames();
    for (auto& item: names)
    {
        cout << item << " ";
        cout << " End " << endl;
    }
}