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
// -----                     PndGeoTofPar  header file                  -----
// -----               Created 30.03.09 by A. Sanchez            -----
// -------------------------------------------------------------------------

#ifndef PNDGEOFTOFPAR_H
#define PNDGEOFTOFPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoFtofPar : public FairParGenericSet {
 public:
  TObjArray *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

  PndGeoFtofPar(const char *name = "PndGeoFtofPar", const char *title = "Ftof Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndGeoFtofPar(void);
  void clear(void);
  void putParams(FairParamList *);
  Bool_t getParams(FairParamList *);
  TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
  TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

  ClassDef(PndGeoFtofPar, 1)
};

#endif /* !PNDGEOFTOFPAR_H */
