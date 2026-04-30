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
// -----                   PndGemFindTracks source file                -----
// -----                  Created 19.03.2009  by R. Karabowicz         -----
// -----                 according to the PndDchFindTracks             -----
// -------------------------------------------------------------------------

// Pnd includes
#include "PndGemFindTracks.h"
#include "PndGemTrackFinder.h"
#include "PndGemDigiPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairLogger.h"

// ROOT includes
#include "TClonesArray.h"

#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"

#include <iomanip>

// c++ includes
using std::cout;
using std::endl;
using std::setw;

// c++ declaration
class iostream;

// Pnd declaration
class FairBaseParSet;
class PndGemHit;
class PndGemDigi;
class PndTrack;
class PndTrackCand;
// class PndTrackCandHit;

// -----   Default constructor   -------------------------------------------
PndGemFindTracks::PndGemFindTracks()
{
  fDigiPar = nullptr;
  fFinder = nullptr;
  fGemHitOrDigiArray = nullptr;
  fTrackArray = nullptr;
  fTrackCandArray = nullptr;
  fNofTracks = 0;
  fUseHitOrDigi = "hit";

  fTTime = 0.;
  fTNofTracks = 0;
  fTNofEvents = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemFindTracks::PndGemFindTracks(const char *name,
                                   const char *title, //  //[R.K.03/2017] unused variable(s)
                                   PndGemTrackFinder *finder)
  : FairTask(name)
{
  fDigiPar = nullptr;
  fFinder = finder;
  fGemHitOrDigiArray = nullptr;
  fTrackArray = nullptr;
  fTrackCandArray = nullptr;
  fNofTracks = 0;
  fUseHitOrDigi = "hit";

  fTTime = 0.;
  fTNofTracks = 0;
  fTNofEvents = 0;
  SetTitle(title);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemFindTracks::~PndGemFindTracks()
{
  fTrackArray->Delete();
  fTrackCandArray->Delete();
}
// -------------------------------------------------------------------------

// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndGemFindTracks::Init()
{

  // Check for Track finder
  if (!fFinder) {
    LOG(warn) << " PndGemFindTracks::Init: No track finder selected!";
    return kERROR;
  }

  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndGemFindTracks::Init: "
         << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  if ("hit" == fUseHitOrDigi) {
    LOG(info) << " PndGemFindTracks::Init: Works on hits!";

    // Get Gem hit Array
    fGemHitOrDigiArray = (TClonesArray *)ioman->GetObject("GEMHit");
    if (!fGemHitOrDigiArray) {
      LOG(warn) << " PndGemFindTracks::Init: No PndGemHit array!";
      return kERROR;
    }
  }

  if ("digi" == fUseHitOrDigi) {
    LOG(info) << " PndGemFindTracks::Init: Works on digis!";

    // Get Gem digi Array
    fGemHitOrDigiArray = (TClonesArray *)ioman->GetObject("GEMDigi");
    if (!fGemHitOrDigiArray) {
      LOG(warn) << " PndGemFindTracks::Init: No PndGemDigi array!";
      return kERROR;
    }
  }

  // Create and register PndTrack array
  fTrackArray = new TClonesArray("PndTrack", 100);
  ioman->Register("GEMTrack", "Gem Tracks", fTrackArray, kTRUE);

  // Create and register PndTrackCand array
  fTrackCandArray = new TClonesArray("PndTrackCand", 100);
  ioman->Register("GEMTrackCand", "Gem Track Cands", fTrackCandArray, kTRUE);

  // Call the Init method of the track finder
  fFinder->Init();

  LOG(info) << " " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations.";
  LOG(info) << " " << fName.Data() << "::Init(). Initialization succesfull.";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void PndGemFindTracks::SetParContainers()
{
  FairRunAna *ana = FairRunAna::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("PndGeoPassivePar");

  fDigiPar = (PndGemDigiPar *)rtdb->getContainer("PndGemDetectors");
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndGemFindTracks::Exec(Option_t *)
{

  fTimer.Start();

  fTrackArray->Delete();
  fTrackCandArray->Delete();

  fNofTracks = fFinder->DoFind(fGemHitOrDigiArray, fTrackArray, fTrackCandArray);

  fTimer.Stop();
  fTTime += fTimer.RealTime();
  fTNofTracks += fNofTracks;
  fTNofEvents += 1;
}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndGemFindTracks::Finish()
{
  fTrackArray->Delete();
  fTrackCandArray->Delete();

  cout << "-------------------- " << fName.Data() << " : Summary ---------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Tracks:     " << setw(10) << fTNofTracks << "    ( " << (Double_t)fTNofTracks / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Time:       " << setw(10) << fTTime << "    ( " << fTTime / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "                           ( " << fTTime / ((Double_t)fTNofTracks) << " per track )" << endl;
  cout << "---------------------------------------------------------------------" << endl;

  fFinder->Finish();
}
// -------------------------------------------------------------------------

ClassImp(PndGemFindTracks)
