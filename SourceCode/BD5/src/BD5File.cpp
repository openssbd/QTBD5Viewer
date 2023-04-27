#include <iostream>
#include <sstream>
#include "BD5File.h"
#include "DataSet.h"
#include "utils.h"

using namespace std;
using namespace H5;
using namespace BD5;

BD5File::BD5File() :
    logger(settings.LOG_FILE)
{   

}

BD5File::BD5File(const std::string& f) :
    logger(settings.LOG_FILE)
{   
    Open(f);
}

void BD5File::Open(const std::string& f)
{
    try
    {
        file = H5File(f, H5F_ACC_RDONLY);
        const auto dataGroup = file.openGroup(settings.DATA);
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << "The file " << f << " was opened";
        logger.log( string(ss.str()), LogType::INFO);
    } catch(const FileIException& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.getCDetailMsg();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    } catch(const GroupIException& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.getCDetailMsg();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    }  
}

BD5::ScaleUnit BD5File::GetScaleUnit(BD5::DataSet& dataset)
{
    ScaleUnit scaleUnit; 
    string scaleTypeStr = dataset.ExtractStringAt("dimension", 0);
    SpaceTime_t scaleType = ScaleUnit::GetSpaceTimeType(scaleTypeStr);
    switch (scaleType)
    {
    case SpaceTime_t::ZeroDimension:
        scaleUnit.Set0Dimension(scaleTypeStr);
        break;
    case SpaceTime_t::OneDimension:
        {
        scaleUnit.Set1Dimension(scaleTypeStr, 
                dataset.ExtractNumberAsAt<float>("xScale", 0),
                dataset.ExtractStringAt("sUnit", 0) );
        }
        break;
    case SpaceTime_t::TwoDimension:
        {
        scaleUnit.Set2Dimension(scaleTypeStr,
                dataset.ExtractNumberAsAt<float>("xScale", 0),
                dataset.ExtractNumberAsAt<float>("yScale", 0),
                dataset.ExtractStringAt("sUnit", 0) );
        }
        break;
    case SpaceTime_t::ThreeDimension:
        {        
        scaleUnit.Set3Dimension(scaleTypeStr,
                dataset.ExtractNumberAsAt<float>("xScale", 0),
                dataset.ExtractNumberAsAt<float>("yScale", 0),
                dataset.ExtractNumberAsAt<float>("zScale", 0),
                dataset.ExtractStringAt("sUnit", 0) ); 
        }
        break;
    case SpaceTime_t::ZeroDimensionAndTime:
        {
        scaleUnit.Set0DimensionAndTime(scaleTypeStr,
                dataset.ExtractNumberAsAt<float>("tScale", 0),
                dataset.ExtractStringAt("tUnit", 0) );
        }
        break;
    case SpaceTime_t::OneDimensionAndTime:
        {
        scaleUnit.Set1DimensionAndTime(scaleTypeStr,
                dataset.ExtractNumberAsAt<float>("xScale", 0),
                dataset.ExtractStringAt("sUnit", 0),
                dataset.ExtractNumberAsAt<float>("tScale", 0),
                dataset.ExtractStringAt("tUnit", 0) );            
        }
        break;
    case SpaceTime_t::TwoDimensionAndTime:
        {
        scaleUnit.Set2DimensionAndTime(scaleTypeStr,
                dataset.ExtractNumberAsAt<float>("xScale", 0),
                dataset.ExtractNumberAsAt<float>("yScale", 0),
                dataset.ExtractStringAt("sUnit", 0),
                dataset.ExtractNumberAsAt<float>("tScale", 0),
                dataset.ExtractStringAt("tUnit", 0) );  
        }

        break;
    case SpaceTime_t::ThreeDimensionAndTime:
        {
        scaleUnit.Set3DimensionAndTime(scaleTypeStr,
                dataset.ExtractNumberAsAt<float>("xScale", 0),
                dataset.ExtractNumberAsAt<float>("yScale", 0),
                dataset.ExtractNumberAsAt<float>("zScale", 0),
                dataset.ExtractStringAt("sUnit", 0),
                dataset.ExtractNumberAsAt<float>("tScale", 0),
                dataset.ExtractStringAt("tUnit", 0) );  
                                
        }
        break;
    default:
        break;
    }
    return scaleUnit;
}

TypeDescriptor BD5File::GetTypeDescriptor(const H5::CompType& compType)
{
    try
    {
        int numElements = compType.getNmembers();
        int size = compType.getSize();
        map<string, TypeElementDescriptor> elements;

        for (int i = 0; i < numElements; i++)
        {
            const auto dataType = compType.getMemberDataType(i);
            int elementSize = int(dataType.getSize());
            int offset = int(compType.getMemberOffset(i));
            TypeElementDescriptor element = TypeElementDescriptor(elementSize, offset, GetElementType(compType, i));
            elements.insert( {compType.getMemberName(i), element} );
        }
        return TypeDescriptor(elements, size);
    }
    catch(const DataTypeIException& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.getCDetailMsg();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    }
    catch(const exception& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.what();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    }
    catch(...)
    {
        throw;
    }
}

ElementType BD5File::GetElementType(const H5::CompType& compType, int index)
{
    try
    {
        ElementType elementType = ElementType::Undefined;
        switch (compType.getMemberClass(index))
        {
        case H5T_class_t::H5T_STRING:
            elementType = ElementType::String;
            break;
        case H5T_class_t::H5T_INTEGER:
        {
            auto dataType = compType.getMemberDataType(index);
            auto size = dataType.getSize();
            auto intType = compType.getMemberIntType(index);
            auto sign = intType.getSign();
            if (size == 4)
            {
                if (sign == H5T_sign_t::H5T_SGN_NONE)
                {
                    elementType = ElementType::UnsignedInteger;
                }
                else
                {
                    elementType = ElementType::Integer;
                }
            }
            if (size == 8)
            {
                if (sign == H5T_sign_t::H5T_SGN_NONE)
                {
                    elementType = ElementType::UnsignedLongLong;
                }
                else
                {
                    elementType = ElementType::LongLong;
                }
            }

            break;
        }
        case H5T_class_t::H5T_FLOAT:
        {
            auto dataType = compType.getMemberDataType(index);
            auto size = dataType.getSize();
            if (size == 4)
            {
                elementType = ElementType::Float;
            }
            if (size == 8)
            {
                elementType = ElementType::Double;
            }
            break;
        }
        default:
            elementType = ElementType::Undefined;
            break;
        }
        return elementType;
    }
    catch(...)
    {
        throw;
    }
    
}

vector<BD5::Snapshot> BD5File::Read(const std::string& f)
{
    try
    {
        Open(f);
        return Read();
    } 
    catch(...)
    {
        throw;
    }
}


vector<BD5::Snapshot> BD5File::Read()
{
    vector<BD5::Snapshot> snapshots;
    try
    {
        auto dataGroup = ReadGroup(settings.DATA);
        auto scaleDataset = ReadDataSet(settings.SCALE_UNIT);
        scales = GetScaleUnit(scaleDataset);
        if (scales.Type() == SpaceTime_t::ZeroDimension ||
            scales.Type() == SpaceTime_t::ZeroDimensionAndTime ||
            scales.Type() == SpaceTime_t::OneDimension ||
            scales.Type() == SpaceTime_t::OneDimensionAndTime )
        {
            std::ostringstream ss;
            ss << __FILE__ << ":" << __func__ << "() " << "This version only consider 2D and 3D dimensions BD5Files";
            logger.log(string(ss.str()), LogType::WARN);
            return snapshots;
        }

        // Snapshots capture
        for (auto& group: dataGroup.Groups())
        {
            string currentGroup = settings.DATA + "/" + group + "/object";

            auto objectGroup = ReadGroup(currentGroup);
            vector<BD5::Object> objects;
            float objectTime = 0.0;
            // Objects (datasets inside timeId groups) capture
            for (auto& dataset : objectGroup.Datasets())
            {
                string datasetPath = currentGroup + "/" + dataset;
                if (settings.BD5FILE_INFO_FLAG)
                {
                    std::ostringstream ss;
                    ss << __FILE__ << ":" << __func__ << "() " << "Dataset " << datasetPath;
                    logger.log(string(ss.str()), LogType::INFO);
                }                
                auto currentDataset = ReadDataSet(datasetPath);
                objectTime = currentDataset.ExtractNumberAsAt<float>("t", 0);
                const string objectEntityStr = currentDataset.ExtractStringAt("entity", 0);
                EntityType entityType = EntityData::GetEntityType(objectEntityStr);
                vector<vector<EntityData>> groupedEntities;
                vector<EntityData> vecZeroEntities; // For Point, Circle, Sphere
                vector<EntityData> currentEntities; // For Line, Face
                int currentSID = 0;
                std::string currentID = "Nothing";

                // Entities capture
                for (int i = 0; i < currentDataset.NumItems(); i++)
                {
                    string itemID = currentDataset.ExtractStringAt("ID", i);
                    string itemLabel = "";
                    if (currentDataset.ContainsElement("label"))
                        itemLabel = currentDataset.ExtractStringAt("label", i);
                    float cX = currentDataset.ExtractNumberAsAt<float>("x", i);
                    float cY = currentDataset.ExtractNumberAsAt<float>("y", i);
                    float cZ = 0.0f;

                    map<char, float> values = {{'x', cX},
                                               {'y', cY}};

                    switch (entityType)
                    {
                        case EntityType::Point:
                        {
                            if ( currentDataset.ContainsElement("z") )
                            {
                                cZ = currentDataset.ExtractNumberAsAt<float>("z", i);
                                values.insert( {'z', cZ} );
                            }                           
                            EntityData objData = { itemID, itemLabel, values };
                            vecZeroEntities.push_back(objData);
                        }
                        break;
                        case EntityType::Circle:
                        {
                            values.insert( {'r', currentDataset.ExtractNumberAsAt<float>("radius", i)} );
                            if ( currentDataset.ContainsElement("z") )
                            {
                                cZ = currentDataset.ExtractNumberAsAt<float>("z", i);
                                values.insert( {'z', cZ});
                            }

                            EntityData objData = { itemID, itemLabel, values };
                            vecZeroEntities.push_back(objData);
                        }
                        break;
                        case EntityType::Sphere:
                        {
                            cZ = currentDataset.ExtractNumberAsAt<float>("z", i);
                            values.insert( {'z', cZ} );
                            values.insert( {'r', currentDataset.ExtractNumberAsAt<float>("radius", i)} );
                                                 
                            EntityData objData = { itemID, itemLabel, values };
                            vecZeroEntities.push_back(objData);
                        }
                        break;
                        case EntityType::Line:
                        case EntityType::Face:
                        {
                            if ( currentDataset.ContainsElement("z") )
                            {
                                cZ = currentDataset.ExtractNumberAsAt<float>("z", i);
                                values.insert( {'z', cZ} );
                            }

                            EntityData objData = { itemID, itemLabel, values };

                            int sID = currentDataset.ExtractNumberAsAt<int>("sID", i);
                            if (sID != currentSID || ((i != 0) && (currentID != itemID)))
                            {
                                groupedEntities.push_back(currentEntities);
                                currentEntities = vector<EntityData>{objData};
                                currentSID = sID;
                            }
                            else
                            {
                                currentEntities.push_back(objData);
                            }
                            currentID = itemID;
                        }
                        break;
                        case EntityType::Undefined:
                            {
                            std::ostringstream ss;
                            ss << __FILE__ << ":" << __func__ << "() " << "Object type undefined";
                            logger.log(string(ss.str()), LogType::WARN);
                            break;
                            }
                    }

                    // Get max min of every coordinate
                    if (i == 0)
                    {
                        boundaries.minX = cX;
                        boundaries.maxX = cX;
                        boundaries.minY = cY;
                        boundaries.maxY = cY;
                        boundaries.minZ = cZ;
                        boundaries.maxZ = cZ;                    
                    } 
                    else 
                    {
                        if (cX < boundaries.minX) boundaries.minX = cX;
                        if (cX > boundaries.maxX) boundaries.maxX = cX;
                        if (cY < boundaries.minY) boundaries.minY = cY;
                        if (cY > boundaries.maxY) boundaries.maxY = cY;
                        if (cZ < boundaries.minZ) boundaries.minZ = cZ;
                        if (cZ > boundaries.maxZ) boundaries.maxZ = cZ;
                    }

                }

                switch (entityType)
                {
                    case EntityType::Point:
                    case EntityType::Circle:
                    case EntityType::Sphere:
                        {
                        groupedEntities = vector<vector<BD5::EntityData>>{ vecZeroEntities };

                        if (settings.BD5FILE_INFO_FLAG)
                        {
                            std::ostringstream ss;
                            ss << __FILE__ << ":" << __func__ << "() " << "Entities: " << vecZeroEntities.size();
                            logger.log(string(ss.str()), LogType::INFO);
                        }
                        }
                        break;
                    case EntityType::Line:
                    case EntityType::Face:
                        {
                        // Do not forget to include last element
                        groupedEntities.push_back(currentEntities);

                        if (settings.BD5FILE_INFO_FLAG)
                        {
                            std::ostringstream ss;
                            ss << __FILE__ << ":" << __func__ << "() " << "Entities: " << currentEntities.size();
                            logger.log(string(ss.str()), LogType::INFO);
                        }
                        break;
                        }
                    case EntityType::Undefined:
                        break;
                }
                BD5::Object currentObject = Object();
                currentObject.InsertObjectData(entityType, groupedEntities);
                objects.push_back(currentObject);
            }

            snapshots.push_back(BD5::Snapshot(objectTime, objects));
        }

        if (settings.BD5FILE_INFO_FLAG)
        {
            std::ostringstream ss;
            ss << __FILE__ << ":" << __func__ << "() " << "Max values scaled x= " << boundaries.maxX * scales.XScale() << " y= " << boundaries.maxY * scales.YScale() << " z= " << boundaries.maxZ * scales.ZScale();
            ss << " Min values scaled x= " << boundaries.minX * scales.XScale() << " y= " << boundaries.minY * scales.YScale() << " z= " << boundaries.minZ * scales.ZScale();
            logger.log(string(ss.str()), LogType::INFO);
        }

        return snapshots;
    } 
    catch(const FileIException& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.getCDetailMsg();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    } 
    catch(const GroupIException& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.getCDetailMsg();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    } 
    catch(const exception& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.what();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    }
    catch(...)
    {
        throw;
    }
}

BD5::DataSet BD5File::ReadDataSet(const string& setPath)
{
    try
    {
        const auto dataSet = file.openDataSet(setPath);
        const auto dataType = dataSet.getDataType();
        if (dataType.getClass() != H5T_COMPOUND)
        {
            throw H5::DataSetIException("DataSet does not have a Compound Type");
        }
        H5::CompType compType = H5::CompType(dataSet);
        const auto dataSpace = dataSet.getSpace();
        int rank = dataSpace.getSimpleExtentNdims();
        vector<hsize_t> ndims;
        ndims.resize(rank);
        // hsize_t ndims[rank];
        dataSpace.getSimpleExtentDims(ndims.data());
        int numRows = ndims[0];
        int size = compType.getSize() * numRows;
        std::unique_ptr<vector<char>> buffer = make_unique<vector<char>>(size);
        dataSet.read(buffer->data(), compType);
        return DataSet(setPath, rank, numRows, GetTypeDescriptor(compType), std::move(buffer));
    } 
    catch(const DataTypeIException& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.getCDetailMsg();
        logger.log( string(ss.str()), LogType::ERR);        
        throw;
    }
    catch(const DataSetIException& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.getCDetailMsg();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    } 
    catch(const DataSpaceIException& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.getCDetailMsg();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    } 
    catch(const exception& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.what();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    }
}

BD5::Group BD5File::ReadGroup(const std::string& name)
{
    try
    {
        const auto group = file.openGroup(name);
        const auto total = group.getNumObjs();

        H5std_string objectType;
        vector<string> groups;
        vector<string> datasets;
        string objName = "";

        for (hsize_t i = 0; i < total; i++) 
        {
            objName = group.getObjnameByIdx(i);
            group.getObjTypeByIdx(i, objectType);
            if (objectType == "group")
                groups.push_back(objName);
            if (objectType == "dataset")
                datasets.push_back(objName);
        }
        sort (groups.begin(), groups.end(), 
              [](string a, string b) { return stoi(a) < stoi(b); });

        if (settings.BD5FILE_INFO_FLAG)
        {
            std::ostringstream ss;
            ss << __FILE__ << ":" << __func__ << "() " << "Group " << name << " contains groups: " << groups.size() << " datasets: " << datasets.size();
            logger.log(string(ss.str()), LogType::INFO);
        }

        return Group(name, groups, datasets);
    } 
    catch(const FileIException& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.getCDetailMsg();
        logger.log( string(ss.str()), LogType::ERR);        
        throw;
    }
    catch(const GroupIException& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.getCDetailMsg();
        logger.log( string(ss.str()), LogType::ERR);        
        throw;
    }
    catch(const exception& ex)
    {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.what();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    }
}

string BD5File::Name() const
{
    return file.getFileName();
}

BD5::ScaleUnit BD5File::Scales() const
{
    return scales;
}

BD5::Boundaries BD5File::GetBoundaries() const
{
    return boundaries;
}
