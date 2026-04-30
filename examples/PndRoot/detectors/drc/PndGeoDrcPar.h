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
// -----                   PndGeoDrcPar header file                    -----
// -----               Created 11/10/06  by A. Cecchi                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEODRCPAR_H
#define PNDGEODRCPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class PndGeoDrcPar : public FairParGenericSet {
 public:
  TObjArray *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

  PndGeoDrcPar(const char *name = "PndGeoDrcPar", const char *title = "Dirc Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndGeoDrcPar(void);
  void clear(void);
  void putParams(FairParamList *);
  Bool_t getParams(FairParamList *);
  TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
  TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

  ClassDef(PndGeoDrcPar, 1)
};

#endif /* !PndGeoDrcPar_H */
