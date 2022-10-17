#include "Group.h"

using namespace std;
using namespace BD5;

Group::Group(const std::string& in_Name, const std::vector<std::string>& in_Groups, const std::vector<std::string>& in_Datasets) :
    fullName(in_Name), groups(in_Groups), datasets(in_Datasets)
{

}

std::string Group::FullName() const
{
    return fullName;
}

vector<string> Group::Groups()
{
    return groups;
}

vector<string> Group::Datasets()
{
    return datasets;
}