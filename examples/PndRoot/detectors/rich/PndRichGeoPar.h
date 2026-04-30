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

#ifndef PNDRICHGEOPAR_H
#define PNDRICHGEOPAR_H

#include "FairParGenericSet.h"

class TObjArray;
class FairParamList;

class PndRichGeoPar : public FairParGenericSet {
 public:
  /** List of FairGeoNodes for sensitive  volumes */
  TObjArray *fGeoSensNodes;

  /** List of FairGeoNodes for sensitive  volumes */
  TObjArray *fGeoPassNodes;

  PndRichGeoPar(const char *name = "PndRichGeoPar", const char *title = "PndRich Geometry Parameters", const char *context = "TestDefaultContext");
  ~PndRichGeoPar(void);
  void clear(void);
  void putParams(FairParamList *);
  Bool_t getParams(FairParamList *);
  TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
  TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

 private:
  PndRichGeoPar(const PndRichGeoPar &);
  PndRichGeoPar &operator=(const PndRichGeoPar &);

  ClassDef(PndRichGeoPar, 1)
};

#endif
