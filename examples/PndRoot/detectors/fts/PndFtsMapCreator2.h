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

#ifndef PNDFTSMAPCREATOR2_H
#define PNDFTSMAPCREATOR2_H

#include <iostream>
#include <map>
#include "TObject.h"

class PndFtsTube;
class PndGeoFtsPar;

class PndFtsMapCreator2 : public TObject {
 public:
  PndFtsMapCreator2();
  PndFtsMapCreator2(Int_t geoType);
  PndFtsMapCreator2(PndGeoFtsPar *par);
  ~PndFtsMapCreator2();

  // general functions
  void SetGeneralParameters();

  // general functions, to be specified depending on geo type
  Int_t GetTubeIDFromPath(TString path);
  PndFtsTube *GetTubeFromTubeID(Int_t tubeid);
  TClonesArray *FillTubeArray();
  Int_t GetChamberIDFromPath(TString path);
  Int_t GetChamberIDFromName(TString name);
  Int_t GetLayerID(Int_t chamberid, Int_t tubeid, TString path);
  Int_t GetTubeIDTot(Int_t chamberid, Int_t layerid, Int_t tubeid, TString path);

 private:
  // geometry type 1 specific functions
  Int_t GetTubeIDFromPathGeoType1(TString path);
  Int_t GetTubeIDFromNameGeoType1(TString name);
  TString GetNameFromPathGeoType1(TString path);
  TString GetNameFromTubeIDGeoType1(Int_t tubeid, Bool_t isCopy);
  PndFtsTube *GetTubeFromTubeIDToFillGeoType1(Int_t tubeid);
  PndFtsTube *GetTubeFromNameToFillGeoType1(TString tubename);
  TClonesArray *FillTubeArrayGeoType1();

  Int_t fGeoType;
  PndGeoFtsPar *fFtsParameters;
  Double_t fTubeInRad, fTubeOutRad;

  // fGeoType1
  std::map<int, int> copy_map;

  PndFtsMapCreator2(const PndFtsMapCreator2 &L);
  PndFtsMapCreator2 &operator=(const PndFtsMapCreator2 &) { return *this; }

 protected:
  ClassDef(PndFtsMapCreator2, 1)
};

#endif
