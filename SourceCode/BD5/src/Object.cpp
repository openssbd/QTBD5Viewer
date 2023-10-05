#include "Object.h"

using namespace std;
using namespace BD5;

bool EntityData::Is2D() const
{
    int XYZ = Values.count('x') + Values.count('y') + Values.count('z');
    return ( (XYZ == 2) && (Values.count('z') == 0) );
}

bool EntityData::Is3D() const
{
    return ( Values.count('x') + Values.count('y') + Values.count('z') == 3 );
}

bool EntityData::HasRadius() const
{
    return (Values.count('r') > 0);
}

bool EntityData::HasElement(const char& name) const
{
    return (Values.count(name) > 0);
}

float EntityData::X() const
{
    try
    {
        return Values.at('x');
    }
    catch(const out_of_range& ex)
    {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "X not found.";
        throw new std::out_of_range(string(ss.str()));
    }
}

float EntityData::Y() const
{
    try
    {
        return Values.at('y');
    }
    catch(const out_of_range& ex)
    {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "Y not found.";
        throw new std::out_of_range(string(ss.str()));
    }
}


float EntityData::Z() const
{
    try
    {
        return Values.at('z');
    }
    catch(const out_of_range& ex)
    {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "Z not found.";
        throw new std::out_of_range(string(ss.str()));
    }
}

float EntityData::Radius() const
{
    try
    {
        return Values.at('r');
    }
    catch(const out_of_range& ex)
    {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "Radius not found.";
        throw new std::out_of_range(string(ss.str()));

    }
}

EntityType EntityData::GetEntityType(const std::string& text)
{
    std::string entity = text;
    std::for_each(entity.begin(), entity.end(), 
        [](char& c)
            { c = ::tolower(c); });
    std::map<EntityType, std::string> figures = { {EntityType::Point, "point"}, 
                                        {EntityType::Circle, "circle"}, 
                                        {EntityType::Sphere, "sphere"}, 
                                        {EntityType::Line, "line"}, 
                                        {EntityType::Face, "face"} };
    for (auto& figure: figures)
    {
        if (text == figure.second)
            return figure.first;
    }
    return EntityType::Undefined;
}


void Object::InsertObjectData(EntityType type, const vector<vector<EntityData>>& objData)
{
    try
    {
        data.push_back(make_pair(type, objData));
    }
    catch(const bad_alloc& ex)
    {
        cout << "No space for allocate a new object " << endl;
        std::cerr << ex.what() << '\n';
    }
}



const vector< pair<EntityType, vector<vector<EntityData>>>>& Object::GetAllSubObjects() const
{
    return data;
}

int Object::GetNumSubObjects() const 
{
    return data.size();
}

const vector<vector<EntityData>>& Object::GetSubObject(int index) const
{
    try
    {
        return data.at(index).second;
    }
    catch(const out_of_range& ex)
    {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "SubObject not found. SubObject size " << data.size() << " with index " << index;
        throw new std::out_of_range(string(ss.str()));
    }
}

EntityType Object::EntityTypeAtSubObject(int index) const
{
    try
    {
        return data.at(index).first;
    }
    catch(const out_of_range& ex)
    {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "SubObject not found. SubObject size " << data.size() << " with index " << index;
        throw new std::out_of_range(string(ss.str()));
    }
    
}

const vector<EntityData>& Object::GetEntitiesAtSubObjectNoSID(int time) const
{
    try
    {
        return data.at(time).second.at(0);
    }
    catch(const out_of_range& ex)
    {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "SubObject not found. SubObject size " << data.size() << " with index " << time;
        throw new std::out_of_range(string(ss.str()));
    }
}

const vector<EntityData>& Object::GetEntitiesAtSubObjectBySID(int time, int sID) const
{
    try
    {
        return data.at(time).second.at(sID);
    }
    catch(const out_of_range& ex)
    {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "SubObject not found. SubObject size " << data.size() << " with index " << time;
        throw new std::out_of_range(string(ss.str()));
    }    
}
