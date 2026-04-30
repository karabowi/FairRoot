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

// @author S. Pflueger
// @author R. Klasen, roklasen@uni-mainz.de or r.klasen@gsi.de or r.klasen@ep1.rub.de

#ifndef LMD_LMDMC_PNDLMDGEOMETRYHELPER_H_
#define LMD_LMDMC_PNDLMDGEOMETRYHELPER_H_

#include <exception>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <TGeoManager.h>
#include <TVector3.h>

struct PndLmdHitLocationInfo {
  // we use small data types here as they can be packed into 8 bytes
  unsigned char detector_half;
  unsigned char plane;
  unsigned char module;
  unsigned char module_side;
  unsigned char module_sensor_id;

  friend std::ostream &operator<<(std::ostream &stream, const PndLmdHitLocationInfo &hit_info)
  {
    stream << "detector half: " << (unsigned int)hit_info.detector_half << "\n";
    stream << "detector plane: " << (unsigned int)hit_info.plane << "\n";
    stream << "module on half plane: " << (unsigned int)hit_info.module << "\n";
    stream << "side of module: " << (unsigned int)hit_info.module_side << "\n";
    stream << "sensor id: " << (unsigned int)hit_info.module_sensor_id << "\n";

    return stream;
  }

  bool operator<(const PndLmdHitLocationInfo &o)
  {
    return std::tie(detector_half, plane, module, module_side, module_sensor_id) < std::tie(o.detector_half, o.plane, o.module, o.module_side, o.module_sensor_id);
  }
};

struct PndLmdOverlapInfo {
  std::string path1;
  std::string path2;
  std::string pathModule;
  int moduleID;
  int overlapID;
  int sectorID;
  int id1;
  int id2;

  TGeoHMatrix mat1;
  TGeoHMatrix mat2;

  // two dummy HitLocationInfo objects for the sensors
  PndLmdHitLocationInfo hit1;
  PndLmdHitLocationInfo hit2;

  friend std::ostream &operator<<(std::ostream &stream, const PndLmdOverlapInfo &overlap_info)
  {
    stream << "path 1: " << overlap_info.path1 << "\n";
    stream << "path 2: " << overlap_info.path2 << "\n";
    stream << "module path: " << overlap_info.pathModule << "\n";
    stream << "id 1: " << overlap_info.id1 << "\n";
    stream << "id 2: " << overlap_info.id2 << "\n";
    stream << "overlapID: " << overlap_info.overlapID << "\n";
    return stream;
  }
};

class PndLmdGeometryHelper {
  boost::property_tree::ptree geometry_properties;
  std::vector<std::pair<std::string, bool>> navigation_paths;

  std::map<std::string, PndLmdHitLocationInfo> volume_path_to_hit_info_mapping;
  std::map<int, PndLmdHitLocationInfo> sensor_id_to_hit_info_mapping;

  std::string lmd_root_path;

  TGeoManager *fGeoManager;

  std::mutex accessMutex;
  std::vector<std::string> fAlignableVolumes;

  PndLmdGeometryHelper(const std::string &geo_params_config_file_url = "") : fGeoManager(gGeoManager)
  {
    std::string file_url(geo_params_config_file_url);
    if (geo_params_config_file_url == "") {
      file_url = std::getenv("VMCWORKDIR");
      file_url += "/macro/detectors/lmd/lmd-geo-params.json"; // FIXME: Put to input/ or params/
    }
    // load parameters
    try {
      read_json(file_url, geometry_properties);
    } catch (std::exception &e) {
      std::cerr << "PndLmdGeometryHelper::PndLmdGeometryHelper(): ERROR! Parameter file not present!\n";
      std::cerr << "Was looking for file: " << file_url << "\n";
      exit(1);
    }

    if (!fGeoManager) {
      std::cerr << "ERROR! gGeoManager is unitialized! Please populate the gGeoManager or initialize a run!\n";
      exit(2);
    }

    auto pt_general = geometry_properties.get_child("general");
    for (boost::property_tree::ptree::value_type &nav_path : pt_general.get_child("navigation_paths")) {
      navigation_paths.push_back(std::make_pair(nav_path.second.get<std::string>("name"), nav_path.second.get<bool>("is_alignable")));
    }

    TString actPath = fGeoManager->GetPath();
    std::stringstream lmd_path;
    fGeoManager->CdTop();
    lmd_path << fGeoManager->GetPath() << "/" << navigation_paths[0].first << "_0";
    lmd_root_path = lmd_path.str();
    if (actPath != "" && actPath != " ")
      fGeoManager->cd(actPath);
  }

  const PndLmdHitLocationInfo &createMappingEntry(int sensor_id);
  const PndLmdHitLocationInfo &createMappingEntry(const std::string &volume_path);

  PndLmdHitLocationInfo translateVolumePathToHitLocationInfo(const std::string &volume_path) const;

 public:
  static PndLmdGeometryHelper &getInstance()
  {
    static PndLmdGeometryHelper instance;
    return instance;
  }

  virtual ~PndLmdGeometryHelper();

  PndLmdGeometryHelper(const PndLmdGeometryHelper &) = delete;
  void operator=(const PndLmdGeometryHelper &) = delete;

  const PndLmdHitLocationInfo &getHitLocationInfo(const std::string &volume_path);
  // this function with the global sensor id is much faster for lookups
  // so use that if you need speed!

  const PndLmdHitLocationInfo &getHitLocationInfo(int sensor_id);

  std::vector<int> getAvailableOverlapIDs();
  int getOverlapIdFromSensorIDs(int id1, int id2);
  int getModuleIDFromSensorID(int sensorID);
  int getSectorIDfromSensorID(int sensorID);
  std::string getModulePathFromModuleID(int moduleID);

  TVector3 transformPndGlobalToLmdLocal(const TVector3 &vec);
  TVector3 transformPndGlobalToSensor(const TVector3 &vec, int sensorId);

  bool isOverlappingArea(const int id1, const int id2);

  const std::string getPath(unsigned char...);

  const TGeoHMatrix getMatrixPndGlobalToSensor(const int sensorId);
  const TGeoHMatrix getMatrixSensorToPndGlobal(const int sensorId);

  const TGeoHMatrix getMatrixPndGlobalToLmdLocal();
  const TGeoHMatrix getMatrixLmdLocalToPndGlobal();

  std::vector<std::string> getAllAlignPaths(bool sensors = true, bool modules = false, bool planes = false, bool halfs = false, bool detector = false);

  // can be restriced by half, plane and module
  std::vector<PndLmdOverlapInfo> getOverlapInfos(int iHalf = -1, int iPlane = -1, int iModule = -1);

  std::vector<std::string> getAllAlignableVolumePaths();
};

#endif /* LMD_LMDMC_PNDLMDGEOMETRYHELPER_H_ */
