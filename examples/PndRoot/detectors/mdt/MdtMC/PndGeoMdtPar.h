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

#ifndef PNDGEOMDTPAR_H
#define PNDGEOMDTPAR_H 1

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoMdtPar : public FairParGenericSet {
 public:
  TObjArray *fSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray *fPassNodes; /** List of FairGeoNodes for passive  volumes */

  PndGeoMdtPar(const char *name = "PndGeoMdtPar", const char *title = "MDT Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndGeoMdtPar(void);
  void clear(void);
  void putParams(FairParamList *);
  Bool_t getParams(FairParamList *);
  TObjArray *GetSensitiveNodes() { return fSensNodes; }
  TObjArray *GetPassiveNodes() { return fPassNodes; }

  ClassDef(PndGeoMdtPar, 1)
};

#endif /* !PNDGEOMDTPAR_H */
