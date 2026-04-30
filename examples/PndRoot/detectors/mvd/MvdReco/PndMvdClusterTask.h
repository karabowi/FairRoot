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


#ifndef PNDMVDCLUSTERTASK_H
#define PNDMVDCLUSTERTASK_H

#include "PndPersistencyTask.h"

class TClonesArray;

class PndMvdClusterTask : public PndPersistencyTask {
 public:
  /** Default constructor **/
  //     PndMvdClusterTask();

  PndMvdClusterTask();

  /** Destructor **/
  virtual ~PndMvdClusterTask();

  void SetPersistance(Bool_t pers);

 private:
  ClassDef(PndMvdClusterTask, 2);
};

#endif /* MVDCLUSTERTASK_H */
