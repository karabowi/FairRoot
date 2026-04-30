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

#ifndef PNDSTTMAPCREATOR_H
#define PNDSTTMAPCREATOR_H

#include <iostream>
#include <map>
#include "TObject.h"
#include "PndSttGeometryMap.h"

class PndSttTubeParameters;
class PndSttTube;
class PndGeoSttPar;
class FairGeoNode;

class PndSttMapCreator : public TObject {
 public:
  PndSttMapCreator();
  PndSttMapCreator(Int_t geoType);
  PndSttMapCreator(PndGeoSttPar *par);
  ~PndSttMapCreator();

  void SetGeneralParameters();

  // TO BE IMPLEMENTED WITH GEO TYPE n
  PndSttTubeParameters *CreateTubeParameters(FairGeoNode *pnode);
  Int_t FillSttTubeParameters(PndGeoSttPar *par, TList *volList);
  Int_t FillSttTubeParametersType2(PndGeoSttPar *par);

  TClonesArray *FillTubeArray();
  Int_t GetTubeIDFromName(TString name);
  PndSttTube *GetTubeFromParametersToFill(PndSttTubeParameters *parms);
  TString GetPathFromTubeID(Int_t tubeid, Bool_t isCopy);
  TString GetNameFromPath(TString path);
  Int_t GetTubeIDFromPath(TString path);
  PndSttTube *GetTubeFromTubeID(Int_t tubeid);

  PndSttGeometryMap *GetGeometryMap() { return fMap; }

 private:
  Int_t fGeoType;
  PndGeoSttPar *fSttParameters;
  Double_t fTubeInRad, fTubeOutRad;

  // fGeoType1
  std::map<int, int> copy_map;

  PndSttMapCreator(const PndSttMapCreator &L);
  PndSttMapCreator &operator=(const PndSttMapCreator &) { return *this; };

  // GEO TYPE 1
  PndSttTubeParameters *CreateTubeParametersGeoType1(FairGeoNode *pnode);
  Int_t FillSttTubeParametersGeoType1(PndGeoSttPar *par, TList *volList);
  TClonesArray *FillTubeArrayGeoType1();
  TString GetNameFromPathGeoType1(TString path);
  Int_t GetTubeIDFromNameGeoType1(TString name);
  PndSttTube *GetTubeFromParametersToFillGeoType1(PndSttTubeParameters *parms);
  TString GetPathFromTubeIDGeoType1(Int_t tubeid, Bool_t isCopy);
  Int_t GetTubeIDFromPathGeoType1(TString path);
  PndSttTube *GetTubeFromTubeIDGeoType1(Int_t tubeid);

  // GEO TYPE 2             This should not be done like this. There should be individual creators with common interface TS
  PndSttTubeParameters *CreateTubeParametersGeoType2(FairGeoNode *pnode);
  TClonesArray *FillTubeArrayGeoType2();
  TString GetNameFromPathGeoType2(TString path);
  Int_t GetTubeIDFromNameGeoType2(TString name);
  PndSttTube *GetTubeFromParametersToFillGeoType2(PndSttTubeParameters *parms);
  TString GetPathFromTubeIDGeoType2(Int_t tubeid, Bool_t isCopy);
  Int_t GetTubeIDFromPathGeoType2(TString path);
  PndSttTube *GetTubeFromTubeIDGeoType2(Int_t tubeid);

 protected:
  TClonesArray *fTubeArray; //!
  PndSttTube *fSttTube;     // !
  PndSttGeometryMap *fMap;  //!

  ClassDef(PndSttMapCreator, 1)
};

#endif
