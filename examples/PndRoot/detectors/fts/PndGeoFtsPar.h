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

////////////////////
///////////////////

#ifndef PNDGEOFTSPAR_H
#define PNDGEOFTSPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndGeoFtsPar : public FairParGenericSet {
 private:
  TObjArray *fGeoSensNodes;         /** List of FairGeoNodes for sensitive volumes*/
  TObjArray *fGeoPassNodes;         /** List of FairGeoNodes for sensitive volumes*/
  Int_t fGeoType;                   // modif
  Double_t fTubeInRad, fTubeOutRad; // modif

  PndGeoFtsPar(const PndGeoFtsPar &L);
  PndGeoFtsPar &operator=(const PndGeoFtsPar &) { return *this; }

 public:
  PndGeoFtsPar(const char *name = "PndGeoFtsPar", const char *title = "Fts Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndGeoFtsPar(void);
  void clear(void);
  void putParams(FairParamList *);
  Bool_t getParams(FairParamList *);
  TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
  TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

  // Additional function
  void SetGeometryType(Int_t geoType) { fGeoType = geoType; }
  void SetTubeInRad(Double_t inrad) { fTubeInRad = inrad; }
  void SetTubeOutRad(Double_t outrad) { fTubeOutRad = outrad; }
  Int_t GetGeometryType() { return (Int_t)fGeoType; };
  Double_t GetTubeInRad() { return (Double_t)fTubeInRad; };
  Double_t GetTubeOutRad() { return (Double_t)fTubeOutRad; };

  ClassDef(PndGeoFtsPar, 1)
};

#endif /* !PNDGEOFTSPAR_H */
