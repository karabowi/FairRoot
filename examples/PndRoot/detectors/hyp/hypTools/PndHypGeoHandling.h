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

/**
 * PndGeoHandling.h
 * @author: t.stockmanns <t.stockmans@fz-juelich.de>
 *
 * @brief Class to access the naming information of the MVD
 *
 * To save memory not anylonger the full path of a volume is stored
 * in each hit but an encrypted form of it (f.e. /1_1/34_2/101_1/).
 * The first number is the volumeID comming from the GeoManager and
 * the second number is the copy number.
 * This class helps you converting the encrypted ID into the path
 * information and vice versa.
 * It needs the informations of the GeoManager. Therefore one has to
 * ensure that either an initialized TGeoManager pointer is given in the
 * constructor with the correct goemetry or a filename with the correct geoemetry ("FAIRGeom")
 *
 */

#ifndef PNDHYPGEOHANDLING_H
#define PNDHYPGEOHANDLING_H

#include "TGeoManager.h"
#include "TString.h"
#include "TVector3.h"
#include <string>
#include <iostream>
#include <vector>
#include "FairLogger.h"

class PndHypGeoHandling {
 public:
  PndHypGeoHandling();

  PndHypGeoHandling(TString fileName);

  PndHypGeoHandling(TGeoManager *aGeoMan)
  {
    if (aGeoMan == 0)
      LOG(error) << " not a valid GeoManager";
    fGeoMan = aGeoMan;
  };

  virtual ~PndHypGeoHandling(){};
  TString GetCurrentID();            ///< returns the ID of the current node
  TString GetID(TString path);       ///< for a given TGeoManager-path the ID is returned
  TString GetPath(TString id);       ///< for a given ID the path is returned
  TString GetVolumeID(TString name); ///< returns the volume ID for a given volume name
  std::vector<TString> GetNamesLevel(Int_t level, TString startPath = "", bool fullPath = false);
  void GetOUVPath(TString path, TVector3 &o, TVector3 &u, TVector3 &v); ///< for a volume given by its path the o, u, v vectors for the plane are returned
  void GetOUVId(TString id, TVector3 &o, TVector3 &u, TVector3 &v);     ///< for a volume given by its ID the o, u, v vectors for the plane are returned

  TVector3 GetSensorDimensionsID(TString id);
  TVector3 GetSensorDimensionsPath(TString path);

  TVector3 MasterToLocalId(const TVector3 &master, const TString &id);
  TVector3 MasterToLocalPath(const TVector3 &master, const TString &id);
  TVector3 LocalToMasterId(const TVector3 &local, const TString &id);
  TVector3 LocalToMasterPath(const TVector3 &local, const TString &id);

  TVector3 MasterToLocalErrorsId(const TVector3 &master, const TString &id);
  TVector3 MasterToLocalErrorsPath(const TVector3 &master, const TString &id);
  TVector3 LocalToMasterErrorsId(const TVector3 &local, const TString &id);
  TVector3 LocalToMasterErrorsPath(const TVector3 &local, const TString &id);

  void SetVerbose(Int_t v) { fVerbose = v; }

  Bool_t cd(TString id); ///< as the cd command of TGeoManager just with the ID
  void FillLevelNames(); ///< fills vector<TString> fLevelNames with the names (or the paths) of the volumes down to the level given by fLevel

 private:
  TGeoManager *fGeoMan;
  std::vector<TString> fLevelNames;
  Int_t fVerbose;
  Int_t fLevel;
  bool fFullPath;
  ClassDef(PndHypGeoHandling, 3);
};

#endif
