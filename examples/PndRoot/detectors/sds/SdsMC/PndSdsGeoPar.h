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

#ifndef PNDSDSGEOPAR_H
#define PNDSDSGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"

class PndSdsGeoPar : public FairParGenericSet {
 public:
  PndSdsGeoPar(const char *name = "PndSdsGeoPar", const char *title = "PndSds Geometry Parameters", const char *context = "TestDefaultContext");
  PndSdsGeoPar(PndSdsGeoPar &other) : FairParGenericSet(other), fGeoSensNodes(other.fGeoSensNodes), fGeoPassNodes(other.fGeoPassNodes){};
  ~PndSdsGeoPar(void);
  PndSdsGeoPar &operator=(PndSdsGeoPar &other)
  {
    fGeoSensNodes = other.fGeoSensNodes;
    fGeoPassNodes = other.fGeoPassNodes;
    return *this;
  };
  void clear(void);
  void putParams(FairParamList *l);
  Bool_t getParams(FairParamList *l);
  TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
  TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

 private:
  TObjArray *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  ClassDef(PndSdsGeoPar, 2);
};

#endif /* !PNDSDSGEOPAR_H */
