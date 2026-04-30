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

// Fast Digitization

#ifndef PndSttRootGeometryInitTask_H
#define PndSttRootGeometryInitTask_H 1

#include "PndPersistencyTask.h"
#include "PndGeoSttPar.h"
#include "PndGeoHandling.h"

class PndSttRootGeometryInitTask : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndSttRootGeometryInitTask();

  /** Destructor **/
  virtual ~PndSttRootGeometryInitTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

 protected:
  void SetParContainers();

 private:
  bool fRunOnce;
  PndGeoSttPar *fSttParameters;
  PndGeoHandling *fGeoH;

  ClassDef(PndSttRootGeometryInitTask, 1);
};

#endif
