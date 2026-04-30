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

#include "PndFtsSignalOverlap.h"

#include "PndFtsHit.h"
#include "PndFtsHitInfo.h"
#include "PndFtsPoint.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TVector3.h"
#include "TMath.h"
#include "TParticlePDG.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndFtsSignalOverlap::PndFtsSignalOverlap()
{
  fVerbose = 0;
  fOriginalHitArray = nullptr;
}

PndFtsSignalOverlap::PndFtsSignalOverlap(TClonesArray *OriginalHitArray)
{
  fVerbose = 0;
  fOriginalHitArray = OriginalHitArray;
}
// -------------------------------------------------------------------------

PndFtsSignalOverlap::PndFtsSignalOverlap(Int_t, TClonesArray *OriginalHitArray)
{ // verbose //[R.K.03/2017] unused variable(s)
  fVerbose = 0;
  fOriginalHitArray = OriginalHitArray;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFtsSignalOverlap::~PndFtsSignalOverlap()
{
  delete fOriginalHitArray;
}

// -------------------------------------------------------------------------
Bool_t PndFtsSignalOverlap::OverlapSimultaneousSignals(TClonesArray *OverlapHitArray)
{

  if (fOriginalHitArray == nullptr)
    return kFALSE;
  if (OverlapHitArray == nullptr)
    return kFALSE;
  fOverlapHitArray = OverlapHitArray;
  fOverlapHitArray->Clear();

  // vector of the firing tubes
  std::vector<int> firingtubes;
  std::vector<int>::iterator it;

  // maps tubeID to hitID (the hitID of the chosen signal)
  std::map<int, int> maptubetohit;

  Int_t nhits = fOriginalHitArray->GetEntriesFast();

  for (int ihit = 0; ihit < nhits; ihit++) {
    PndFtsHit *hit = (PndFtsHit *)fOriginalHitArray->At(ihit);
    if (!hit)
      continue;

    // get which tube it is
    Int_t tubeid = hit->GetTubeID();

    // has the tube already fired?
    it = find(firingtubes.begin(), firingtubes.end(), tubeid);

    // if not, add it to the firing tubes and map
    if (it == firingtubes.end()) {
      firingtubes.push_back(tubeid);
      maptubetohit[tubeid] = ihit; // CHECK
    } else {
      // geet the old hit
      int oldhitid = maptubetohit[tubeid]; // CHECK
      PndFtsHit *oldhit = (PndFtsHit *)fOriginalHitArray->At(oldhitid);
      if (!oldhit)
        continue;
      double thisiso = hit->GetIsochrone();
      double oldiso = oldhit->GetIsochrone();

      // this this hit comes first, replace it in map
      if (thisiso < oldiso) {
        maptubetohit[tubeid] = ihit; // CHECK
      }
      // else do nothing
    }
  }

  // copy to output only the hits in the map
  for (size_t itube = 0; itube < firingtubes.size(); itube++) {
    int tubeid = firingtubes[itube];
    int hitid = maptubetohit[tubeid];
    Bool_t wrote = WriteToOutputHit(hitid);
    if (wrote == kFALSE)
      cout << "error in writing hit " << hitid << endl;
  }

  return kTRUE;
}

Bool_t PndFtsSignalOverlap::WriteToOutputHit(Int_t hitid)
{
  PndFtsHit *hit = (PndFtsHit *)fOriginalHitArray->At(hitid);
  if (!hit)
    return kFALSE;
  TClonesArray &clref = *fOverlapHitArray;
  Int_t size = clref.GetEntriesFast();
  PndFtsHit *hitnew = new (clref[size]) PndFtsHit();
  *hitnew = *hit;
  hitnew->SetDetectorID(hit->GetDetectorID());

  return kTRUE;
}

ClassImp(PndFtsSignalOverlap);
