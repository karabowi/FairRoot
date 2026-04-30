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
// -------------------------------------------------------------------------

#ifndef PNDMVDPIXELCLUSTERTASK_H
#define PNDMVDPIXELCLUSTERTASK_H

#include "PndSdsPixelClusterTask.h"
#include "PndMvdSimplePixelClusterFinder.h"
#include "PndMvdChargeWeightedPixelMapping.h"
#include "TString.h"
#include "FairRootManager.h"

class PndMvdPixelClusterTask : public PndSdsPixelClusterTask {
 public:
  /** Default constructor **/
  PndMvdPixelClusterTask(TString parName = "MVDPixelDigiPar", TString totParName = "MVDPixelTotDigiPar");

  /** Destructor **/
  virtual ~PndMvdPixelClusterTask();
  PndMvdPixelClusterTask(const PndMvdPixelClusterTask &) = delete;
  PndMvdPixelClusterTask &operator=(const PndMvdPixelClusterTask &) = delete;

  virtual void GetParList(TList *tempList);
  virtual void SetClusterFinderMQ(TList *tempList);
  virtual void SetBackMappingMQ(TList *tempList);

  virtual void SetParContainers();
  virtual void SetBranchNames();
  virtual void SetClusterFinder();
  virtual void SetBackMapping();
  virtual void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);

 private:
  TString fParName;
  TString fTotParName;
  PndSdsTotDigiPar *fTotDigiPar;
  PndSensorNamePar *fSensorNamePar;
  ClassDef(PndMvdPixelClusterTask, 3);
};

#endif /* MVDCLUSTERTASK_H */
