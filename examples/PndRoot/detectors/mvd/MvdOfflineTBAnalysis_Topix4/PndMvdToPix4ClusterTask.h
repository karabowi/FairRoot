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

#ifndef PNDMVDTOPIX4CLUSTERTASK_H
#define PNDMVDTOPIX4CLUSTERTASK_H

#include "PndMvdPixelClusterTask.h"
#include "PndMvdSimplePixelClusterFinder.h"
#include "PndMvdChargeWeightedPixelMapping.h"
#include "TString.h"
#include "FairRootManager.h"

class PndMvdToPix4ClusterTask : public PndMvdPixelClusterTask {
 public:
  /** Default constructor **/
  PndMvdToPix4ClusterTask(TString parName = "MVDPixelDigiPar", TString totParName = "MVDPixelTotDigiPar");

  /** Destructor **/
  virtual ~PndMvdToPix4ClusterTask();

 protected:
  virtual std::vector<PndSdsDigiPixel> ConvertAndFilter(TClonesArray *digidata);

 private:
  ClassDef(PndMvdToPix4ClusterTask, 1);
};

#endif /* MVDTOPIX4CLUSTERTASK_H */
