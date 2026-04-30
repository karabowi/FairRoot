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

/*
 * PndBranchBurstBuilder_timeCut_STB.cxx
 *
 *  Created on: July 13, 2017
 *      Author: Steinschaden
 *
 */

#include "TClonesArray.h"
//#include "TROOT.h"
//#include "TString.h"
//#include "TVector3.h"
//#include "TMath.h"

// framework includes
#include "FairRootManager.h"
#include "FairTimeStamp.h"

#include "PndBranchBurstBuilder_timeCut_STB.h"

PndBranchBurstBuilder_timeCut_STB::PndBranchBurstBuilder_timeCut_STB() : PndBranchBurstBuilder()
{
  fOutputPrefix = "Burst_STB";
  fTimePeriod = 2000.0; // in nano seconds
  fBurstNum = 0;
  fWriteOut = false;
}

PndBranchBurstBuilder_timeCut_STB::~PndBranchBurstBuilder_timeCut_STB() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndBranchBurstBuilder_timeCut_STB::Init()
{

  InitStatus status = PndBranchBurstBuilder::Init();

  for (size_t i = 0; i < fInBranchNames.size(); i++) {
    fTempArrays.push_back(new TClonesArray(fInArrays[i]->GetClass()->GetName()));
  }

  return status;
}

TClonesArray *PndBranchBurstBuilder_timeCut_STB::GetBurstData(size_t branchNum)
{

  TClonesArray *emptyArray = new TClonesArray(fInArrays[branchNum]->GetClass()->GetName());
  double timeStamp;

  if (branchNum == 0) { // only the "master branch" is controling the writeout to ensure synchron writeouts
    fWriteOut = false;
    if (fInArrays[branchNum]->GetEntriesFast() > 0) {
      fWriteOut = true;                                                             // set potential writeout
      for (int i = 0; i < fInArrays[branchNum]->GetEntriesFast(); i++) {            // loop over all timestamps of the event
        timeStamp = ((FairTimeStamp *)fInArrays[branchNum]->At(i))->GetTimeStamp(); // access the timestamps
        if (timeStamp < fBurstNum * fTimePeriod + fTimePeriod) {                    // if at least one timestamp is within the Range
          fWriteOut = false;                                                        // deactivate the WriteOut
          break;                                                                    // save computing time and break the loop
        }
      }
    }
    if (fWriteOut == true)
      fBurstNum++; // increase the Burst number for the next Burst

  } // masterBranch end

  if (fWriteOut == true) {
    emptyArray->AbsorbObjects(fTempArrays[branchNum]); // prepare writeOut and "clear" the temporary buffer arrays
  }
  fTempArrays[branchNum]->AbsorbObjects(fInArrays[branchNum]); // absorb  data into  buffer

  return emptyArray; // writeout empty or Data array.
}

void PndBranchBurstBuilder_timeCut_STB::FinishTask()
{
  // write out the data collected for a maybe last and uncomplete Burst

  for (size_t i = 0; i < fInBranchNames.size(); i++) {
    fOutArrays[i]->Delete(); // make sure data written out from old events are deleted
    fOutArrays[i]->AbsorbObjects(fTempArrays[i]);
  }
  FairRootManager::Instance()->SetLastFill();
}

ClassImp(PndBranchBurstBuilder_timeCut_STB);
