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
// PndGeoOtPar header file
//
// Class for geometry parameters of OT
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndGeoFtsPar by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

#ifndef PNDGEOOTPAR_H
#define PNDGEOOTPAR_H
// from FairRoot
#include <FairParGenericSet.h>
// from ROOT
#include <TH1F.h>
#include <TObjArray.h>

class PndGeoOtPar : public FairParGenericSet {
 private:
  TObjArray *fGeoSensNodes;         /** List of FairGeoNodes for sensitive volumes*/
  TObjArray *fGeoPassNodes;         /** List of FairGeoNodes for sensitive volumes*/
  Int_t fGeoType;                   // modif
  Double_t fTubeInRad, fTubeOutRad; // modif

  PndGeoOtPar(const PndGeoOtPar &L);
  PndGeoOtPar &operator=(const PndGeoOtPar &) { return *this; }

 public:
  PndGeoOtPar(const char *name = "PndGeoOtPar", const char *title = "Ot Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndGeoOtPar(void);
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

  ClassDef(PndGeoOtPar, 1)
};

#endif /* !PNDGEOOTPAR_H */
