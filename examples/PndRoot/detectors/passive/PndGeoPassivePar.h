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

#ifndef PNDGEOPASSIVEPAR_H
#define PNDGEOPASSIVEPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"

class PndGeoPassivePar : public FairParGenericSet {
 public:
  TObjArray *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  PndGeoPassivePar(const char *name = "PndGeoPassivePar", const char *title = "Passive Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndGeoPassivePar(void);
  void clear(void);
  void putParams(FairParamList *);
  Bool_t getParams(FairParamList *);
  TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
  TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

 private:
  PndGeoPassivePar(const PndGeoPassivePar &L) : FairParGenericSet(L), fGeoSensNodes(L.fGeoSensNodes), fGeoPassNodes(L.fGeoPassNodes){};
  PndGeoPassivePar &operator=(const PndGeoPassivePar &) { return *this; }

  ClassDef(PndGeoPassivePar, 1)
};

#endif /* !PNDGEOPASSIVEPAR_H */
