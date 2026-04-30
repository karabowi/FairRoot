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

#include "PndFtsHoughTrackerTaskQA.h"
#include "PndFtsHoughTrackerTask.h"

#include <iostream>
#include <math.h>

// FTS
#include "PndGeoFtsPar.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"
#include "PndFtsHit.h"
#include "FairHit.h"

// magnetic field
#include "FairField.h"

// (Hough) tracking
#include "PndFtsHoughTrackFinder.h"
#include "PndFtsHoughTrackFinderQA.h"
#include "PndFtsHoughSpace.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "PndFtsHoughTrackCand.h"

// histogramming / plotting
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"

// peak finder
#include "TSpectrum2.h"

// root IO

#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"

#include "TString.h"

// TODO this list can probably be shorter
//#include "PndDetectorList.h"

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"

#include "TObjectTable.h"

#include "PndFtsMapCreator.h"

using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
PndFtsHoughTrackerTaskQA::PndFtsHoughTrackerTaskQA(Int_t verbose) : PndFtsHoughTrackerTask(verbose, kTRUE), fTrackFinderParams(), fNParabolasToFind(1), fNEvtsWithEnoughParabolas(0)
{
  if (3 < fVerbose)
    std::cout << "PndFtsHoughTrackerTaskQA is the tracker ptr " << this << '\n';
}

// ---- Destructor ----------------------------------------------------
PndFtsHoughTrackerTaskQA::~PndFtsHoughTrackerTaskQA()
{
  if (fVerbose > 3)
    fLogger->Info(MESSAGE_ORIGIN, "Destructor of PndFtsHoughTrackerTaskQA");
}

// ---- Init ----------------------------------------------------------
InitStatus PndFtsHoughTrackerTaskQA::Init()
{
  if (fVerbose > 3)
    Info("Init", "Initilization of PndFtsHoughTrackerTaskQA");

  return PndFtsHoughTrackerTask::Init();
}

// ---- ReInit  -------------------------------------------------------
InitStatus PndFtsHoughTrackerTaskQA::ReInit()
{
  InitStatus stat = kSUCCESS;
  if (3 < fVerbose)
    fLogger->Info(MESSAGE_ORIGIN, "Re- Initilization of PndFtsHoughTrackerTaskQA");
  PndFtsHoughTrackerTask::ReInit();
  return stat;
}

// ---- Exec ----------------------------------------------------------
void PndFtsHoughTrackerTaskQA::Exec(Option_t *)
{
  if (1 < fVerbose)
    Info("Exec", "Exec of PndFtsHoughTrackerTaskQA on event %i", fEventNr);

  if (3 < fVerbose)
    std::cout << "PndFtsHoughTrackFinder::Exec tracker ptr " << this << '\n';

  // determine how many parabolas should be found (by performing ideal track finding)

  // run track finder
  PndFtsHoughTrackFinder trackFinder(this);
  trackFinder.OverwriteTrackFinderParams(fTrackFinderParams);
  trackFinder.FindTracks();

  // check if we found correct number of parabolas
  if (fNParabolasToFind <= trackFinder.getNParabolasFound())
    ++fNEvtsWithEnoughParabolas;

  if (3 < fVerbose)
    Info("Exec", "End eventloop.");
  ++fEventNr;
}

void PndFtsHoughTrackerTaskQA::FinishEvent() {}

// ---- Finish --------------------------------------------------------
void PndFtsHoughTrackerTaskQA::Finish()
{
  if (3 < fVerbose)
    Info("Finish", "Found %i tracks.", fTrackCands->GetEntriesFast());

  // print parameters to screen if we have found enough parabolas
  std::cout << fNEvtsWithEnoughParabolas << " events have >= " << fNParabolasToFind << " parabola.\n";
  std::cout << " \n";
}

ClassImp(PndFtsHoughTrackerTaskQA)
