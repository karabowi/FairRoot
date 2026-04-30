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

// -------------------------------------------------------------------------
// -----                     PndGeoSciTPar  header file                  -----
// -----               Created 30.03.09 by A. Sanchez            -----
// -------------------------------------------------------------------------

#ifndef PNDGEOSCITPAR_H
#define PNDGEOSCITPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoSciTPar : public FairParGenericSet {
 public:
  TObjArray *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

  PndGeoSciTPar(const char *name = "PndGeoSciTPar", const char *title = "SciT Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndGeoSciTPar(void);
  void clear(void);
  void putParams(FairParamList *);
  Bool_t getParams(FairParamList *);
  TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
  TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

  ClassDef(PndGeoSciTPar, 1)
};

#endif /* !PNDGEOSCITPAR_H */
