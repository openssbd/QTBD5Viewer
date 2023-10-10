#include <iostream>
#include <sstream>
#include "BD5File.h"
#include "DataSet.h"
#include "utils.h"

using namespace std;
using namespace H5;
using namespace BD5;


void Boundaries::updateBoundaries(const Boundaries& newBoundaries)
{
    if (newBoundaries.minX < minX) {
        minX = newBoundaries.minX;
    }
    if (newBoundaries.maxX > maxX) {
        maxX = newBoundaries.maxX;
    }
    if (newBoundaries.minY < minY) {
        minY = newBoundaries.minY;
    }
    if (newBoundaries.maxY > maxY) {
        maxY = newBoundaries.maxY;
    }
    if (newBoundaries.minZ < minZ) {
        minZ = newBoundaries.minZ;
    }
    if (newBoundaries.maxZ > maxZ) {
        maxZ = newBoundaries.maxZ;
    }
}


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

std::vector<std::string> BD5File::GetObjNames(BD5::DataSet& dataset) {
    vector<string> names;
    for (int i = 0; i < dataset.NumItems(); i++) {
        names.push_back( dataset.ExtractStringAt("name", i) );
    }
    return names;
}

std::vector<std::vector<PointTrack>> BD5File::WriteTracksGeometry(const std::vector<std::vector<string>>& tracks, const std::vector<PointTrack>& pntTracks) 
{
    vector<vector<PointTrack>> vecTracks;

    for(auto& track: tracks) {
        vector<PointTrack> vecLine;
        for (auto& lineStr: track) {
            auto it = std::find_if(pntTracks.begin(), pntTracks.end(),
                [&lineStr](const PointTrack& row) {
                    return ( row.objID == lineStr);
                });
            if (it != pntTracks.end()) {
                vecLine.push_back(*it);
            }
            else {
                std::ostringstream ss;
                ss << __FILE__ << ":" << __func__ << "() " << "Not found lineStr " << lineStr;
                logger.log( string(ss.str()), LogType::ERR);
            }
        }
        vecTracks.push_back(vecLine);
    }
    return vecTracks;
}

std::vector<PointTrack> BD5File::WriteTrackInfo(int timeId, const std::vector<std::vector<EntityData>>& geometry) 
{
    vector<PointTrack> tracksGeom;

    for (auto& obj: geometry) {
        std::for_each(obj.begin(), obj.end(), 
                [&](const EntityData& element) {
            auto it = std::find_if(tracksGeom.begin(), tracksGeom.end(), 
                [&](const PointTrack& pointTrack) {
                return (pointTrack.objID == element.ID);
            });
            if (it == tracksGeom.end()) {
                // Polylines write the center of a sID polyline as a special
                // line with very special ID and Label (this is not a point in 
                // the line it is the center of the polyline)
                if ( (element.ID == "XXBoxCenterXX") && (element.Label == "XXLocalBCenterXX") ) {
                    PointTrack last = tracksGeom[tracksGeom.size()-1];
                    last.x = element.X();
                    last.y = element.Y();
                    last.z = element.Z();
                    tracksGeom.pop_back();
                    tracksGeom.push_back(last);
                } 
                else {
                    PointTrack pT = { .x = element.X(), .y = element.Y(), .z = element.Z(),
                                      .id = timeId, .objID = element.ID, .label = element.Label,
                                    };
                    tracksGeom.push_back(pT);
                }
            }
        });
    }

    return tracksGeom;
}


std::vector<std::pair<string, string>> BD5File::GetRawTrackInfo(BD5::DataSet& dataset) 
{
    std::vector<std::pair<string, string>> tracks;
    for (int i = 0; i < dataset.NumItems(); i++) {
        string from = dataset.ExtractStringAt("from", i);
        string to = dataset.ExtractStringAt("to", i);
        auto track = std::make_pair(from, to);
        tracks.push_back(track);
    }
    return tracks;
}

std::vector<std::vector<string>> BD5File::MakeTrackPaths(const std::vector<std::pair<string, string>>& tracks)
{
    vector<vector<string>> tracksGraph;
    for (size_t i = 0; i < tracks.size(); i++) {
        if (i < tracks.size() - 2) {
            auto tail = slice(tracks, i+1, tracks.size()-1);
            auto it = std::find_if(tail.begin(), tail.end(), 
                        [&tracks, &i](const pair<string, string>& tLine) {
                            return (tLine.first == tracks[i].first);
                        });
            if (it != tail.end()) {
                tracksGraph.push_back({tracks[i].first, tracks[i].second});
                tracksGraph.push_back({tracks[i].first});  // The second element is fill in after
                continue;
            }
        }
        auto lineIndex = std::find_if(tracksGraph.rbegin(), tracksGraph.rend(),
                        [&tracks, &i](const vector<string>& tLine) {
                            return (tLine[tLine.size()-1] == tracks[i].first);
                        });
        if (lineIndex != tracksGraph.rend()) {
            lineIndex->push_back(tracks[i].second);
        }
        else {
            tracksGraph.push_back({tracks[i].first, tracks[i].second});
        }
    }
    return tracksGraph;
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
    labels.clear();

    try
    {
        auto dataGroup = ReadGroup(settings.DATA);
        auto scaleDataset = ReadDataSet(settings.SCALE_UNIT);
        auto objDef = ReadDataSet(settings.OBJECT_DEF);

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

        objNames = GetObjNames(objDef);

        vector<vector<string>> trackLines;
        auto rootDatasets = dataGroup.Datasets();
        if (std::find(rootDatasets.begin(), rootDatasets.end(), "trackInfo") != rootDatasets.end()) {
            auto trackDataset = ReadDataSet(settings.TRACK_INFO);
            auto rawTrackInfo = GetRawTrackInfo(trackDataset);
            trackLines = MakeTrackPaths(rawTrackInfo);
        }

        // Snapshots capture
        for (auto& group: dataGroup.Groups())
        {
            string currentGroup = settings.DATA + "/" + group + "/object";

            auto objectGroup = ReadGroup(currentGroup);
            vector<BD5::Object> objects;
            float objectTime = 0.0;

            vector<vector<string>> currentObjLabels;

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
                int currentSID = -10000;
                std::string currentID = "Nothing";
                boundaries = Boundaries();
                Boundaries box = Boundaries();

                auto calculateCenter = [&](struct Boundaries theBox) {
                    std::map<char, float> center;
                    float x = theBox.minX + (theBox.maxX - theBox.minX) / 2;
                    float y = theBox.minY + (theBox.maxY - theBox.minY) / 2;
                    float z = theBox.minZ + (theBox.maxZ - theBox.minZ) / 2;
                    center.insert({'x', x});
                    center.insert({'y', y});
                    center.insert({'z', z});
                    return center;
                };

                // Lambda function to write the center of a polylines
                // as an additional line at the end of the entities vector (sID)
                auto registerPolylineCenter = [&](struct Boundaries theBox) {
                    std::map<char, float> center = calculateCenter(theBox);
                    EntityData polylineCenter = {"XXBoxCenterXX", 
                                                 "XXLocalBCenterXX", 
                                                 center };
                    currentEntities.push_back(polylineCenter);
                };

                vector<string> currentLabelsVector;
                string currentLabel;
                // Entities capture
                for (int i = 0; i < currentDataset.NumItems(); i++)
                {
                    string itemID = currentDataset.ExtractStringAt("ID", i);
                    string itemLabel;
                    if (currentDataset.ContainsElement("label"))
                        itemLabel = currentDataset.ExtractStringAt("label", i);
                    float cX = currentDataset.ExtractNumberAsAt<float>("x", i);
                    float cY = currentDataset.ExtractNumberAsAt<float>("y", i);
                    float cZ = 0.0f;

                    if ( !itemLabel.empty() ) {
                        if (itemLabel != currentLabel) {
                            currentLabelsVector.push_back(itemLabel);
                            currentLabel = itemLabel;
                        }
                        else {
                            auto found = find(currentLabelsVector.begin(), currentLabelsVector.end(), itemLabel);
                            if (found == currentLabelsVector.end()) {
                                currentLabelsVector.push_back(itemLabel);
                                currentLabel = itemLabel;
                            }
                        }
                    }

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

                            if (i == 0) {
                                if ( currentDataset.ContainsElement("sID") )
                                {
                                    currentSID = currentDataset.ExtractNumberAsAt<int>("sID", i);
                                    currentID = itemID;
                                }
                                else {
                                    std::ostringstream ss;
                                    ss << __FILE__ << ":" << __func__ << "() " << "line/face does not define sID member element. H5 file incomplete data";
                                    logger.log(string(ss.str()), LogType::ERR);
                                    break;
                                }
                            }
                            int sID = currentDataset.ExtractNumberAsAt<int>("sID", i);

                            if ((sID != currentSID) || (currentID != itemID) )
                            {
                                if (currentID != itemID) {
                                    if (i != 0) {
                                        registerPolylineCenter(box);
                                    }
                                    box = Boundaries();
                                }

                                groupedEntities.push_back(currentEntities);
                                currentEntities = vector<EntityData>{objData};
                                currentSID = sID;
                            }
                            else
                            {
                                currentEntities.push_back(objData);
                            }
                            currentID = itemID;

                            if (cX < box.minX) box.minX = cX;
                            if (cX > box.maxX) box.maxX = cX;
                            if (cY < box.minY) box.minY = cY;
                            if (cY > box.maxY) box.maxY = cY;
                            if (cZ < box.minZ) box.minZ = cZ;
                            if (cZ > box.maxZ) box.maxZ = cZ;
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

                    if ( (entityType == EntityType::Line) || 
                         (entityType == EntityType::Face)) {
                        boundaries.updateBoundaries(box);
                    }
                    else {
                        if (cX < boundaries.minX) boundaries.minX = cX;
                        if (cX > boundaries.maxX) boundaries.maxX = cX;
                        if (cY < boundaries.minY) boundaries.minY = cY;
                        if (cY > boundaries.maxY) boundaries.maxY = cY;
                        if (cZ < boundaries.minZ) boundaries.minZ = cZ;
                        if (cZ > boundaries.maxZ) boundaries.maxZ = cZ;
                    }
                }

                currentObjLabels.push_back(currentLabelsVector);

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
                        auto front = currentEntities.front();
                        registerPolylineCenter(box);
                        box = Boundaries();
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
            labels.push_back(currentObjLabels);           
            snapshots.push_back(BD5::Snapshot(objectTime, objects));
        }

        if (settings.BD5FILE_INFO_FLAG)
        {
            std::ostringstream ss;
            ss << __FILE__ << ":" << __func__ << "() " << "Max values scaled x= " << boundaries.maxX * scales.XScale() << " y= " << boundaries.maxY * scales.YScale() << " z= " << boundaries.maxZ * scales.ZScale();
            ss << " Min values scaled x= " << boundaries.minX * scales.XScale() << " y= " << boundaries.minY * scales.YScale() << " z= " << boundaries.minZ * scales.ZScale();
            logger.log(string(ss.str()), LogType::INFO);
        }

        if (!trackLines.empty()) {
            tracks = CreateTracks(snapshots, trackLines);
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


vector<vector<PointTrack>> BD5File::CreateTracks(const std::vector<BD5::Snapshot>& snapshots, const std::vector<std::vector<std::string>>& tLines)
{
    int t = 0;
    vector<PointTrack> tracks;
    // int total = 0;
    for (auto& snapshot: snapshots) {
        auto objects = snapshot.GetObjects();
        for (auto& obj: objects ) {
            int numSubObj = obj.GetNumSubObjects();
            for (int i = 0; i < numSubObj; i++) {
                auto subObj = obj.GetSubObject(i);
                auto currTracks = WriteTrackInfo(t, subObj);
                // total = total + currTracks.size();
                tracks.insert(tracks.end(), currTracks.begin(), currTracks.end());
            }
        }
        t++;
    }

    auto fullTracks = WriteTracksGeometry(tLines, tracks);
    return fullTracks;
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

vector<string> BD5File::ObjectsNames() const
{
    return objNames;
}

BD5::Boundaries BD5File::GetBoundaries() const
{
    return boundaries;
}

bool BD5File::HasTracks() const
{
    return !tracks.empty();
}

vector<vector<PointTrack>> BD5File::GetTracks() const
{
    return tracks;
}

vector<vector<string>> BD5File::GetLabelsAtTime(int t) {
    try {
        return labels.at(t);
    }
    catch(const exception& ex) {
        std::ostringstream ss;
        ss << __FILE__ << ":" << __func__ << "() " << ex.what();
        logger.log( string(ss.str()), LogType::ERR);
        throw;
    }
}
