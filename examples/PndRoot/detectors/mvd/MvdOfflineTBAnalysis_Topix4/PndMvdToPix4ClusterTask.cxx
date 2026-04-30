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
// -----                PndMvdPixelClusterTasksource file             -----
// -------------------------------------------------------------------------

#include "PndMvdToPix4ClusterTask.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsDigiTopix4.h"
// -----   Default constructor   -------------------------------------------
PndMvdToPix4ClusterTask::PndMvdToPix4ClusterTask(TString parName, TString totParName) : PndMvdPixelClusterTask(parName, totParName)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdToPix4ClusterTask::~PndMvdToPix4ClusterTask() {}
// -------------------------------------------------------------------------

std::vector<PndSdsDigiPixel> PndMvdToPix4ClusterTask::ConvertAndFilter(TClonesArray *digidata)
{
  std::vector<PndSdsDigiPixel> result;
  for (Int_t iPoint = 0; iPoint < digidata->GetEntriesFast(); iPoint++) { // Just conversion
    PndSdsDigiTopix4 *myDigi = (PndSdsDigiTopix4 *)(digidata->At(iPoint));
    if (myDigi->GetLeadingEdge() == 2729 || myDigi->GetLeadingEdge() == 2730 || myDigi->GetTrailingEdge() == 2730 || myDigi->GetTrailingEdge() == 2731) {
      std::cout << "Digi filtered: " << *myDigi << std::endl;
    } else {
      result.push_back(*myDigi);
    }
  }
  return result;
}

ClassImp(PndMvdToPix4ClusterTask);
