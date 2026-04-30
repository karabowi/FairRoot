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

#include "PndFtsHoughTrackFinderQA.h"

ClassImp(PndFtsHoughTrackFinderQA);

PndFtsHoughTrackFinderQA::PndFtsHoughTrackFinderQA(PndFtsHoughTrackerTask *trackerTask) : PndFtsHoughTrackFinder(trackerTask)
{
  if (nullptr == fTrackerTask) {
    std::cout << "PndFtsHoughTrackFinderQA FATAL ERROR Tracker task not set.\n";
  } else {
    if (3 < fTrackerTask->GetVerbose())
      std::cout << "PndFtsHoughTrackFinderQA called with tracker ptr " << fTrackerTask << '\n';
  }
}

PndFtsHoughTrackFinderQA::~PndFtsHoughTrackFinderQA()
{
  if (3 < fTrackerTask->GetVerbose())
    fTrackerTask->fLogger->Info(MESSAGE_ORIGIN, "Destructor of PndFtsHoughTrackFinderQA");
}

void PndFtsHoughTrackFinderQA::FindTracks()
{

  PndFtsHoughTrackFinder::FindTracks();
}
