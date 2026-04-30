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

/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMvdMQFileSampler.cpp
 *
 * @since 2016-03-08
 * @author R. Karabowicz
 */

#include <PndMvdMQFileSampler.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "FairMQLogger.h"

#include "FairMQMessage.h"
#include "TMessage.h"

using namespace std;

PndMvdMQFileSampler::PndMvdMQFileSampler() : FairMQDevice(), fRunAna(nullptr), fSource(nullptr), fInputObjects(), fNObjects(0), fMaxIndex(-1), fBranchNames(), fFileNames() {}

void PndMvdMQFileSampler::InitTask()
{
  fRunAna = new FairRunAna();
  if (fFileNames.size() > 0) {
    fSource = new FairFileSource(fFileNames.at(0).c_str());
    for (unsigned int ifile = 1; ifile < fFileNames.size(); ifile++)
      fSource->AddFile(fFileNames.at(ifile));
  }
  fSource->Init();
  LOG(info) << "Going to request " << fBranchNames.size() << "  branches:";
  for (unsigned int ibrn = 0; ibrn < fBranchNames.size(); ibrn++) {
    LOG(info) << " requesting branch \"" << fBranchNames[ibrn] << "\"";
    int branchStat = fSource->ActivateObject((TObject **)&fInputObjects[fNObjects], fBranchNames[ibrn].c_str()); // should check the status...
    if (fInputObjects[fNObjects]) {
      LOG(info) << "Activated object \"" << fInputObjects[fNObjects] << "\" with name \"" << fBranchNames[ibrn] << "\" (" << branchStat << ")";
      fNObjects++;
    }
  }
  if (fMaxIndex < 0)
    fMaxIndex = fSource->CheckMaxEventNo();
  LOG(info) << "Input source has " << fMaxIndex << " events.";
}

// helper function to clean up the object holding the data after it is transported.
void free_tmessage2(void * /*data*/, void *hint)
{
  delete (TMessage *)hint;
}

void PndMvdMQFileSampler::Run()
{
  int eventCounter = 0;

  // Check if we are still in the RUNNING state.
  while (CheckCurrentState(RUNNING)) {
    if (eventCounter == fMaxIndex)
      break;

    Int_t readEventReturn = fSource->ReadEvent(eventCounter);

    if (readEventReturn != 0)
      break;

    TMessage *message[1000];
    FairMQParts parts;

    for (int iobj = 0; iobj < fNObjects; iobj++) {
      message[iobj] = new TMessage(kMESS_OBJECT);
      message[iobj]->WriteObject(fInputObjects[iobj]);
      parts.AddPart(NewMessage(message[iobj]->Buffer(), message[iobj]->BufferSize(), free_tmessage2, message[iobj]));
    }

    Send(parts, "data-out");

    eventCounter++;
  }

  LOG(info) << "Going out of RUNNING state.";
}

PndMvdMQFileSampler::~PndMvdMQFileSampler() {}
