#include "ScaleUnit.h"
#include "Snapshot.h"

using namespace std;
using namespace BD5;

Snapshot::Snapshot(float in_Time, const std::vector<BD5::Object>& in_Object) :
    time(in_Time), objects(in_Object)
{

}

float Snapshot::Time() const
{
    return time;
}

const vector<Object>& Snapshot::GetObjects() const
{
    return objects;
}

const BD5::Object& Snapshot::GetObjectAt(int index) const
{
    try
    {
        return objects.at(index);
    }
    catch(const out_of_range& ex)
    {
        ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "Object not found. Objects size " << objects.size() <<  " with index " << index;
        throw new std::out_of_range(string(ss.str()));
    }   
}


