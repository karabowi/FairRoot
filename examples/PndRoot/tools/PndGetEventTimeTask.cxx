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

#include "PndGetEventTimeTask.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TTree.h"
#include "TBranch.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairTimeStamp.h"
#include "FairFileSource.h"
#include "FairLogger.h"

PndGetEventTimeTask::PndGetEventTimeTask() : FairTask("Pnd Get Event Time"), fEventTime(new std::vector<double>) {}

PndGetEventTimeTask::~PndGetEventTimeTask() {}

void PndGetEventTimeTask::SetParContainers()
{
  // FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb(); //[R.K. 01/2017] unused variable?
  //  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

InitStatus PndGetEventTimeTask::ReInit()
{
  InitStatus stat = kSUCCESS;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndGetEventTimeTask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();
  FairSource* source = ioman->GetSource();
  if (source->GetSourceType() == Source_Type::kFILE){
	  for (int i = 0; i < source->CheckMaxEventNo(); i++){
		  source->ReadEvent(i);
		  fEventTimeCopy.push_back(((FairFileSource*)source)->GetEventTime());
		  std::cout << fEventTimeCopy.back() << std::endl;
	  }
  }

  ioman->RegisterAny("EventTimes", fEventTime, kFALSE);

  LOG(info) << " PndGetEventTimeTask: Initialisation successfull #Times: " << fEventTimeCopy.size();
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndGetEventTimeTask::Exec(Option_t *)
{
	*fEventTime = fEventTimeCopy;
}

void PndGetEventTimeTask::FinishEvent()
{
  delete (fEventTime);
  fEventTime = nullptr;
}

ClassImp(PndGetEventTimeTask);
