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

#ifndef PNDEMCGEOPAR_H
#define PNDEMCGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndEmcGeoPar : public FairParGenericSet {
 public:
  PndEmcGeoPar(const char *name = "PndEmcGeoPar", const char *title = "Emc Geometry Version Flag", const char *context = "TestDefaultContext");
  ~PndEmcGeoPar(void);
  void clear(void);

  void putParams(FairParamList *);
  Bool_t getParams(FairParamList *);

  Int_t GetMapperVersion() { return fMapperVersion; }
  void SetMapperVersion(Int_t mapperVersion)
  { // AB
    fMapperVersion = mapperVersion;
  }

  Int_t GetGeometryVersion() { return fGeometryVersion; }
  void SetGeometryVersion(Int_t geometryVersion) { fGeometryVersion = geometryVersion; }

  void InitEmcMapper();

 private:
  Int_t fMapperVersion;
  Int_t fGeometryVersion;

  ClassDef(PndEmcGeoPar, 2)
};

#endif /* !PNDEMCGEOPAR_H */
