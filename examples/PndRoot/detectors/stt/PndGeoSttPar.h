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

#ifndef PNDGEOSTTPAR_H
#define PNDGEOSTTPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndGeoSttPar : public FairParGenericSet {
 public:
  TObjArray *fTubeParams;   // tube parameters (tubeID, half length) array
  Int_t fGeoType;           // geotype: 1 = straws_skewed_blocks_35cm_pipe.geo, 2 = <name>.root, 3 = <name>.geo
  Double_t fTubeInRad;      // tubes inner radius
  Double_t fTubeOutRad;     // tubes outer radius
  Int_t fMaxTubeId;         // maximal tube ID (for geo type 3) 
  Text_t fGeoFileName[250]; // file name of geo file (needed for geo type 3) 

  PndGeoSttPar(const char *name = "PndGeoSttPar", const char *title = "Stt Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndGeoSttPar(void);
  void clear(void);
  void putParams(FairParamList *);
  Bool_t getParams(FairParamList *);

  // additional function for geometry
  void SetGeometryType(Int_t geoType) { fGeoType = geoType; }
  void SetTubeInRad(Double_t inrad) { fTubeInRad = inrad; }
  void SetTubeOutRad(Double_t outrad) { fTubeOutRad = outrad; }
  void SetMaxTubeId(Int_t id) { fMaxTubeId = id; } 
  void SetGeoFileName(TString name)   { strcpy(fGeoFileName, name.Data()); }

  TObjArray *GetTubeParameters() { return fTubeParams; }
  Int_t GetGeometryType() { return (Int_t)fGeoType; };
  Double_t GetTubeInRad() { return (Double_t)fTubeInRad; };
  Double_t GetTubeOutRad() { return (Double_t)fTubeOutRad; };
  Int_t GetMaxTubeId() { return fMaxTubeId; } 
  TString GetGeoFileName() { return TString(fGeoFileName); }


  PndGeoSttPar(PndGeoSttPar &par);
  PndGeoSttPar &operator=(const PndGeoSttPar &) { return *this; };

  ClassDef(PndGeoSttPar, 1)
};

#endif /* !PNDGEOSTTPAR_H */
