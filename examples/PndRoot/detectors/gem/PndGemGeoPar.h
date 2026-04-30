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
// -----                PndGemGeoPar header file                       -----
// -----                Created 15/02/2009  by R.Karabowicz            -----
// -------------------------------------------------------------------------

/** \class PndGemGeoPar
 *  \author R. Karabowicz
 *  \date 15/02/2009
 *  \Storing list of GEM volumes
 **/

#ifndef PNDGEMGEOPAR_H
#define PNDGEMGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"

class PndGemGeoPar : public FairParGenericSet {
 public:
  PndGemGeoPar(const char *name = "PndGemGeoPar", const char *title = "PndGem Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndGemGeoPar(void);
  void clear(void);
  void putParams(FairParamList *l);
  Bool_t getParams(FairParamList *l);
  TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
  TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

 private:
  TObjArray *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  ClassDef(PndGemGeoPar, 2);
};

#endif /* !PNDGEMGEOPAR_H */
