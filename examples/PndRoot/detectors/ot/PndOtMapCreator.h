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

////////////////////////////////////////////////////////////////////////////
// PndOtMapCreator source file
//
// Mapper for OT
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndGeoFts by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

#ifndef PNDOTMAPCREATOR_H
#define PNDOTMAPCREATOR_H

// from ROOT
#include <TObject.h>
// standard
#include <iostream>
#include <map>

class PndOtTube;
class PndGeoOtPar;
class PndGeoOtPar;

class TClonesArray;

class PndOtMapCreator : public TObject {
 public:
  PndOtMapCreator();
  PndOtMapCreator(Int_t geoType);
  PndOtMapCreator(PndGeoOtPar *par);
  ~PndOtMapCreator();

  // general functions
  void SetGeneralParameters();

  // general functions, to be specified depending on geo type
  Int_t GetTubeIDFromPath(TString path);
  PndOtTube *GetTubeFromTubeID(Int_t tubeid);
  TClonesArray *FillTubeArray();
  Int_t GetChamberIDFromPath(TString path);
  Int_t GetChamberIDFromName(TString name);
  Int_t GetLayerID(Int_t chamberid, TString path);
  Int_t IsSkew(Int_t layerid);
  Int_t GetTubeIDTot(Int_t chamberid, Int_t layerid, TString path);
  Int_t GetOTTubeIDTot(Int_t chamberid, Int_t layerid, Int_t module_type, Int_t module_number, Int_t tube_in_module, Int_t strawCh12);

 private:
  Int_t GetTubeIDFromName(TString name);
  TString GetNameFromPath(TString path);
  TString GetNameFromTubeID(Int_t tubeid, Bool_t isCopy);
  PndOtTube *GetTubeFromNameToFill(TString tubename, Int_t tubeid, Int_t layerid);
  int GetTypeNumTube(TString path, int &module_type, int &module_number, int &tube_in_module);

  Int_t fGeoType{0};
  Double_t fTubeInRad{0.}, fTubeOutRad{0.};
  PndGeoOtPar *fOtParameters{nullptr};
  int fModShiftOT1[3][20] = {{192, 256, 320, 384, 448, 1536, 1600, 1664, 1728, 1792, 2880, 2944, 3008, 3072, 3136, 4224, 4288, 4352, 4416, 4480},
                             {64, 128, 512, 576, 1408, 1472, 1856, 1920, 2752, 2816, 3200, 3264, 4096, 4160, 4544, 4608, 0, 0, 0, 0},
                             {0, 32, 640, 1344, 1376, 1984, 2688, 2720, 3328, 4032, 4064, 4672, 0, 0, 0, 0, 0, 0, 0, 0}};
  int fModShiftOT2[3][28] = {
    {128, 192, 256, 320, 384, 448, 512, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 2816, 2880, 2944, 3008, 3072, 3136, 3200, 4160, 4224, 4288, 4352, 4416, 4480, 4544},
    {64, 576, 1408, 1920, 2752, 3264, 4096, 4608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 32, 640, 1344, 1376, 1984, 2688, 2720, 3328, 4032, 4064, 4672, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

  // fGeoType1
  // std::map<int, int> fCopy_Map;

  PndOtMapCreator(const PndOtMapCreator &L);
  PndOtMapCreator &operator=(const PndOtMapCreator &) { return *this; }

 protected:
  ClassDef(PndOtMapCreator, 1)
};

#endif
