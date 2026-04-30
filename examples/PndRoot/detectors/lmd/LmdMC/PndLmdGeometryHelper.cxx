//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

#include "PndLmdGeometryHelper.h"
#include "PndGeoHandling.h"

#include <algorithm>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "TGeoPhysicalNode.h"

using std::cout;
using std::string;

PndLmdGeometryHelper::~PndLmdGeometryHelper() {}

/*
 * This function does all the important work of translating a lmd volume path to
 * the PndLmdHitLocationInfo. It uses the navigation paths as given in the
 * lmd geometry parameter config file.
 * The structure is as following
 * -> half: up: 0; down: 1
 * -> plane: 0 to n_p (n_p: number of detector planes -1); counting in beam
 * direction
 * -> module: 0 to n_m (n_m: number of modules per half plane -1);
 *    for top half: counting from left to right (clockwise) in beam direction
 *    for top half: counting from right to left (clockwise) in beam direction
 * -> sensor: sensor counter for a module. Total number of sensors per module is
 *    2x (modules per side). Counting starts on front (w.r.t. beam direction)
 *
 */
PndLmdHitLocationInfo PndLmdGeometryHelper::translateVolumePathToHitLocationInfo(const std::string &volume_path) const
{
  PndLmdHitLocationInfo hit_info;
  std::stringstream reg_exp;
  for (auto const &nav_path : navigation_paths) {
    reg_exp << "/" << nav_path.first << "_(\\d+)";
  }

  std::smatch match;

  if (std::regex_search(volume_path, match, std::regex(reg_exp.str()))) {
    hit_info.detector_half = (unsigned char)std::stoul(match[2]);
    hit_info.plane = (unsigned char)std::stoul(match[3]);
    hit_info.module = (unsigned char)std::stoul(match[4]);
    unsigned char sensor_id((unsigned char)std::stoul(match[5]));
    hit_info.module_side = 0;
    hit_info.module_sensor_id = sensor_id;

    unsigned int sensors_per_module_side = geometry_properties.get<unsigned int>("general.sensors_per_module_side");

    if (sensor_id > sensors_per_module_side - 1) {
      hit_info.module_side = 1;
      sensor_id = sensor_id % (sensors_per_module_side - 1);
    }

    // now we have to make sure the TGeoManager builds the matrix cache
    // correctly the only way we found (analogous to PndGeoHandling) is to
    // dive down to this node from the top, then the matrix multiplications
    // are done correctly
    TString actPath = fGeoManager->GetPath();
    fGeoManager->cd(volume_path.c_str());
    PndGeoHandling::Instance()->cd(fGeoManager->GetCurrentNode());
    if (actPath != "" && actPath != " ")
      fGeoManager->cd(actPath);

  } else {
    throw std::runtime_error("PndLmdGeometryHelper::translateVolumePathToHitLocationInfo: geometry "
                             "navigation paths mismatch!"
                             " Seems like you used a different lmd geo config file to create a lmd "
                             "root geometry which was use in your simulations...");
  }

  return hit_info;
}

std::vector<PndLmdOverlapInfo> PndLmdGeometryHelper::getOverlapInfos(int iHalf, int iPlane, int iModule)
{
  std::vector<PndLmdOverlapInfo> result;

  bool all = false;

  if (iHalf < 0 || iPlane < 0 || iModule < 0) {
    all = true;
  }
  // FIXME: this should be read from config file
  if (iHalf > 2 || iPlane > 4 || iModule > 5) {
    std::cerr << "ERROR. Invalid module specified.\n";
    return result;
  }

  PndGeoHandling *geo_handling = PndGeoHandling::Instance();
  if (!geo_handling) {
    std::cerr << "WARNING! No geoHandling present!\n";
    exit(1);
  }

  PndLmdHitLocationInfo hitLoc;

  // FIXME: also, this might break if our sensor ids are not 0-319. this needs handling!
  for (int iSensor = 0; iSensor < 320; iSensor++) {
    for (int jSensor = iSensor; jSensor < 320; jSensor++) {
      if (isOverlappingArea(iSensor, jSensor)) {

        if (!all) {
          hitLoc = getHitLocationInfo(iSensor);
          if (iHalf != hitLoc.detector_half)
            continue;
          if (iPlane != hitLoc.plane)
            continue;
          if (iModule != hitLoc.module)
            continue;
        }

        PndLmdOverlapInfo temp;

        std::string path1(geo_handling->GetPath(int(iSensor)).Data());
        gGeoManager->cd(path1.c_str());
        temp.mat1 = gGeoManager->GetCurrentMatrix();
        gGeoManager->CdUp(); // exclude toActiveRect from path
        path1 = (gGeoManager->GetPath());
        std::string path2(geo_handling->GetPath(int(jSensor)).Data());
        gGeoManager->cd(path2.c_str());
        temp.mat2 = gGeoManager->GetCurrentMatrix();
        gGeoManager->CdUp(); // exclude toActiveRect from path
        path2 = (gGeoManager->GetPath());
        gGeoManager->CdUp();
        std::string pathModule(gGeoManager->GetPath());

        auto overlapID = getOverlapIdFromSensorIDs(iSensor, jSensor);
        temp.path1 = path1;
        temp.path2 = path2;
        temp.pathModule = pathModule;
        temp.overlapID = overlapID;
        temp.moduleID = getModuleIDFromSensorID(iSensor);
        temp.sectorID = getSectorIDfromSensorID(iSensor);
        temp.id1 = iSensor;
        temp.id2 = jSensor;

        temp.hit1 = getHitLocationInfo(iSensor);
        temp.hit2 = getHitLocationInfo(jSensor);

        result.push_back(temp);
      }
    }
  }

  return result;
}

const PndLmdHitLocationInfo &PndLmdGeometryHelper::createMappingEntry(int sensor_id)
{
  PndGeoHandling *geo_handling = PndGeoHandling::Instance();

  std::string vol_path(geo_handling->GetPath(sensor_id).Data());

  PndLmdHitLocationInfo hit_loc_info = translateVolumePathToHitLocationInfo(vol_path);
  volume_path_to_hit_info_mapping[vol_path] = hit_loc_info;
  sensor_id_to_hit_info_mapping[sensor_id] = hit_loc_info;

  return sensor_id_to_hit_info_mapping[sensor_id];
}

const PndLmdHitLocationInfo &PndLmdGeometryHelper::createMappingEntry(const std::string &volume_path)
{
  PndGeoHandling *geo_handling = PndGeoHandling::Instance();

  int sensor_id(geo_handling->GetShortID(volume_path.c_str()));

  PndLmdHitLocationInfo hit_loc_info = translateVolumePathToHitLocationInfo(volume_path);
  volume_path_to_hit_info_mapping[volume_path] = hit_loc_info;
  sensor_id_to_hit_info_mapping[sensor_id] = hit_loc_info;

  return volume_path_to_hit_info_mapping[volume_path];
}

const PndLmdHitLocationInfo &PndLmdGeometryHelper::getHitLocationInfo(const std::string &volume_path)
{
  auto const &result = volume_path_to_hit_info_mapping.find(volume_path);
  if (result != volume_path_to_hit_info_mapping.end()) {
    return result->second;
  } else {
    return createMappingEntry(volume_path);
  }
}

const PndLmdHitLocationInfo &PndLmdGeometryHelper::getHitLocationInfo(int sensor_id)
{
  auto const &result = sensor_id_to_hit_info_mapping.find(sensor_id);
  if (result != sensor_id_to_hit_info_mapping.end()) {
    return result->second;
  } else {
    return createMappingEntry(sensor_id);
  }
}

std::vector<int> PndLmdGeometryHelper::getAvailableOverlapIDs()
{
  std::vector<int> result;
  auto infos = getOverlapInfos();
  for (auto &info : infos) {
    result.push_back(info.overlapID);
  }
  return result;
}

TVector3 PndLmdGeometryHelper::transformPndGlobalToLmdLocal(const TVector3 &global)
{
  Double_t result[3];
  Double_t temp[3];

  global.GetXYZ(temp);

  auto matrix = getMatrixPndGlobalToLmdLocal();
  matrix.MasterToLocal(temp, result);

  return TVector3(result);
}

TVector3 PndLmdGeometryHelper::transformPndGlobalToSensor(const TVector3 &global, int sensor_id)
{
  Double_t result[3];
  Double_t temp[3];

  global.GetXYZ(temp);

  auto matrix = getMatrixPndGlobalToSensor(sensor_id);
  matrix.MasterToLocal(temp, result);

  return TVector3(result);
}

// that's because the geoManager only produces the matrix to the next super volume
const TGeoHMatrix PndLmdGeometryHelper::getMatrixPndGlobalToSensor(const int sensorId)
{

  // to ensure only one thred at a time can do this
  std::lock_guard<std::mutex> lock(accessMutex);

  PndGeoHandling *geo_handling = PndGeoHandling::Instance();
  std::string vol_path(geo_handling->GetPath(int(sensorId)).Data());

  TString actPath = fGeoManager->GetPath();
  // go to active part of sensor
  fGeoManager->cd(vol_path.c_str());
  // we want the matrix of the whole sensor. so just cd up once
  fGeoManager->CdUp();

  TGeoHMatrix matrix = *fGeoManager->GetCurrentMatrix();
  if (actPath != "" && actPath != " ")
    fGeoManager->cd(actPath);

  return matrix;
}

const TGeoHMatrix PndLmdGeometryHelper::getMatrixSensorToPndGlobal(const int sensorId)
{
  return getMatrixPndGlobalToSensor(sensorId).Inverse();
}

std::string PndLmdGeometryHelper::getModulePathFromModuleID(int moduleID)
{
  auto allOverlaps = getOverlapInfos();
  for (auto overlapInfo : allOverlaps) {
    if (overlapInfo.moduleID == moduleID) {
      return overlapInfo.pathModule;
    }
  }
  return "ERROR! Module ID not in list of available overlaps!";
}

int PndLmdGeometryHelper::getSectorIDfromSensorID(int sensorID)
{
  int fhalf, fmodule;

  auto const &infoOne = getHitLocationInfo(sensorID);

  // 0 or 1
  fhalf = infoOne.detector_half;

  // 0,1,2,3,4
  fmodule = infoOne.module;

  if (fhalf == 0) {
    return fmodule; // should go from 0-4
  }

  else if (fhalf == 1) {
    return fmodule + 5; // should go from 5-9
  }
  return -1;
}

int PndLmdGeometryHelper::getModuleIDFromSensorID(int sensorID)
{

  int fhalf, fplane, fmodule;

  auto const &infoOne = getHitLocationInfo(sensorID);

  // 0 or 1
  fhalf = infoOne.detector_half;

  // 0,1,2,3
  fplane = infoOne.plane;

  // 0,1,2,3,4
  fmodule = infoOne.module;

  //! this only works if there are 10 module on a plane and two half detectors
  //! if the detector undergoes a redesign, this needs to be changed too!
  if (fhalf == 0) {
    return fplane * 10 + fmodule; // should go from 0-19
  }

  else if (fhalf == 1) {
    return fplane * 10 + fmodule + 5; // should go from 20-39
  }
  return -1;
}

int PndLmdGeometryHelper::getOverlapIdFromSensorIDs(int id1, int id2)
{

  int fhalf, fplane, fmodule, fside, fsensor;
  int bhalf, bplane, bmodule, bside, bsensor;

  auto const &infoOne = getHitLocationInfo(id1);
  auto const &infoTwo = getHitLocationInfo(id2);

  int smalloverlap = -1;

  fhalf = infoOne.detector_half;
  bhalf = infoTwo.detector_half;

  fside = infoOne.module_side;
  bside = infoTwo.module_side;

  fplane = infoOne.plane;
  bplane = infoTwo.plane;

  fmodule = infoOne.module;
  bmodule = infoTwo.module;

  fsensor = infoOne.module_sensor_id;
  bsensor = infoTwo.module_sensor_id;

  if (bhalf != fhalf) {
    return -1;
  }

  if (fside == bside) {
    return -1;
  }

  if (bplane != fplane) {
    return -1;
  }

  if (bmodule != fmodule) {
    return -1;
  }

  // check if the ids are sorted
  if (fside > bside) {
    cout << "PndLmdGeometryHelper::WARNING: pair with ids (" << id1 << ", " << id2 << ") is not sorted.\n";
    cout << "this shouldn't happen here anymore!\n";

    // the last checks only involve fsensor and bsensor
    std::swap(fsensor, bsensor);
  }

  //* this is for the new 8 sensor per module version
  if (fsensor == 0 && bsensor == 4) {
    smalloverlap = 0;
  }

  else if (fsensor == 1 && bsensor == 7) {
    smalloverlap = 1;
  }

  else if (fsensor == 2 && bsensor == 7) {
    smalloverlap = 2;
  }

  else if (fsensor == 3 && bsensor == 7) {
    smalloverlap = 3;
  }

  else if (fsensor == 3 && bsensor == 5) {
    smalloverlap = 4;
  }

  else if (fsensor == 3 && bsensor == 6) {
    smalloverlap = 5;
  }

  else if (fsensor == 1 && bsensor == 5) {
    smalloverlap = 6;
  }

  /* this is for the old 10 sensors per module version
  // 0to5
  if (fsensor == 0 && bsensor == 5) {
    smalloverlap = 0;
  }
  // 3to8
  else if (fsensor == 3 && bsensor == 8) {
    smalloverlap = 1;
  }
  // 4to9
  else if (fsensor == 4 && bsensor == 9) {
    smalloverlap = 2;
  }
  // 3to6
  else if (fsensor == 3 && bsensor == 6) {
    smalloverlap = 3;
  }
  // 1to8
  else if (fsensor == 1 && bsensor == 8) {
    smalloverlap = 4;
  }
  // 2to8
  else if (fsensor == 2 && bsensor == 8) {
    smalloverlap = 5;
  }
  // 2to9
  else if (fsensor == 2 && bsensor == 9) {
    smalloverlap = 6;
  }
  // 3to7
  else if (fsensor == 3 && bsensor == 7) {
    smalloverlap = 7;
  }
  // 4to7
  else if (fsensor == 4 && bsensor == 7) {
    smalloverlap = 8;
  }
  */
  // don't overlap, return -1
  else {
    return -1;
  }
  return smalloverlap;
  // return 1000 * fhalf + 100 * fplane + 10 * fmodule + smalloverlap;
}

const TGeoHMatrix PndLmdGeometryHelper::getMatrixPndGlobalToLmdLocal()
{

  // to ensure only one thred at a time can do this
  std::lock_guard<std::mutex> lock(accessMutex);

  TString actPath = fGeoManager->GetPath();

  fGeoManager->cd(lmd_root_path.c_str());
  TGeoHMatrix *matrix = (TGeoHMatrix *)(fGeoManager->GetCurrentMatrix());

  if (actPath != "" && actPath != " ")
    fGeoManager->cd(actPath);

  return *matrix;
}

const TGeoHMatrix PndLmdGeometryHelper::getMatrixLmdLocalToPndGlobal()
{
  return getMatrixPndGlobalToLmdLocal().Inverse();
}

bool PndLmdGeometryHelper::isOverlappingArea(const int id1, const int id2)
{
  return (getOverlapIdFromSensorIDs(id1, id2) > -1);
}

std::vector<std::string> PndLmdGeometryHelper::getAllAlignPaths(bool sensors, bool modules, bool planes, bool halfs, bool detector)
{

  std::vector<std::string> result;
  auto all_volume_paths = getAllAlignableVolumePaths();

  std::vector<std::string> filter_strings;

  if (detector)
    filter_strings.push_back(navigation_paths[0].first);
  if (halfs)
    filter_strings.push_back(navigation_paths[1].first);
  if (planes)
    filter_strings.push_back(navigation_paths[2].first);
  if (modules)
    filter_strings.push_back(navigation_paths[3].first);
  if (sensors)
    filter_strings.push_back(navigation_paths[4].first);

  std::cout << "total number of alignable volumes: " << all_volume_paths.size() << std::endl;

  for (auto filter_string : filter_strings) {
    auto found(all_volume_paths.begin());
    while (found != all_volume_paths.end()) {
      found = std::find_if(found, all_volume_paths.end(), [&](const std::string &s) {
        std::stringstream reg_exp;
        reg_exp << "^.*/" << filter_string << "_(\\d+)/*$";
        std::smatch match;
        return std::regex_search(s, match, std::regex(reg_exp.str()));
      });

      if (found != all_volume_paths.end()) {
        result.push_back(*found);
        ++found;
      }
    }
  }

  // just for fun and so it's easier to examine it
  std::sort(result.begin(), result.end());

  return result;
}

std::vector<std::string> PndLmdGeometryHelper::getAllAlignableVolumePaths()
{
  TGeoNode *node = fGeoManager->GetTopNode();

  // there is a nasty bug here somewhere when the following block is called more than once.
  // then, the last entry of the first time is appended to every entry, but somewhere
  // in the middle. I don't have time to properly fix it, and these regexes are hard to parse.
  // so were just making sure this block is only called once and the vector only populated once.
  // every subsequent time, just return the result of the first run.
  // as a bonus, it's much faster this way

  if (fAlignableVolumes.size() < 1) {
    if (fGeoManager->GetNAlignable() > 0) {
      for (int i = 0; i < fGeoManager->GetNAlignable(); ++i) {
        TGeoPNEntry *entry = fGeoManager->GetAlignableEntry(i);
        if (entry) {
          fAlignableVolumes.push_back(entry->GetPath());
        }
      }
    } else {
      std::vector<std::pair<TGeoNode *, std::string>> current_paths;
      current_paths.push_back(std::make_pair(node, fGeoManager->GetPath()));

      while (current_paths.size() > 0) {
        auto temp_current_paths = current_paths;
        current_paths.clear();
        for (auto const curpath : temp_current_paths) {
          std::stringstream reg_exp;
          for (auto nav_path : navigation_paths) {
            reg_exp << "/" + nav_path.first << "_(\\d+)";
            std::smatch match;
            if (std::regex_search(curpath.second, match, std::regex(reg_exp.str() + "/*$"))) {
              if (nav_path.second)
                fAlignableVolumes.push_back(curpath.second);
              break;
            }
          }
          for (int i = 0; i < curpath.first->GetNdaughters(); ++i) {
            node = curpath.first->GetDaughter(i);
            std::stringstream full_path;
            full_path << curpath.second << "/" << node->GetName();
            current_paths.push_back(std::make_pair(node, full_path.str()));
          }
        }
      }
    }
  }
  return fAlignableVolumes;
}
