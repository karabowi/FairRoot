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
// -----                     CbmGeoHypPar header file                  -----
// -----               Created by A.Sanchez                            -----
// -------------------------------------------------------------------------

#ifndef PNDGEOHYPPAR_H
#define PNDGEOHYPPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoHypPar : public FairParGenericSet {
 public:
  TObjArray *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

  PndGeoHypPar(const char *name = "PndGeoHypPar", const char *title = "Hyp Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndGeoHypPar(void);
  void clear(void);
  void putParams(FairParamList *);
  Bool_t getParams(FairParamList *);
  TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
  TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

  ClassDef(PndGeoHypPar, 1)
};

#endif /* !PNDGEOHYPPAR_H */
