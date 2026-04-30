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

////////////////////////////////////////////////////////////////////////////
// PndOtSignalOverlap source file
//
// Copied from PndFtsSingalOverlap
//
// authors: Radoslaw Karabowicz, GSI, 2024
////////////////////////////////////////////////////////////////////////////

#include "PndOtSignalOverlap.h"

// from PandaRoot, this library
#include "PndOtHitInfo.h"
#include "PndOtMapCreator.h"
// from PandaRoot, pnddata
#include <PndOtHit.h>
// from FairRoot
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>
// from ROOT
#include <TClonesArray.h>
#include <TVector3.h>
#include <TMath.h>
#include <TParticlePDG.h>
// standard
#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndOtSignalOverlap::PndOtSignalOverlap()
{
  fVerbose = 0;
  fOriginalHitArray = nullptr;
}

PndOtSignalOverlap::PndOtSignalOverlap(TClonesArray *OriginalHitArray)
{
  fVerbose = 0;
  fOriginalHitArray = OriginalHitArray;
}
// -------------------------------------------------------------------------

PndOtSignalOverlap::PndOtSignalOverlap(Int_t, TClonesArray *OriginalHitArray)
{ // verbose //[R.K.03/2017] unused variable(s)
  fVerbose = 0;
  fOriginalHitArray = OriginalHitArray;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndOtSignalOverlap::~PndOtSignalOverlap()
{
  delete fOriginalHitArray;
}

// -------------------------------------------------------------------------
Bool_t PndOtSignalOverlap::OverlapSimultaneousSignals(TClonesArray *OverlapHitArray)
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
    PndOtHit *hit = (PndOtHit *)fOriginalHitArray->At(ihit);
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
      PndOtHit *oldhit = (PndOtHit *)fOriginalHitArray->At(oldhitid);
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

Bool_t PndOtSignalOverlap::WriteToOutputHit(Int_t hitid)
{
  PndOtHit *hit = (PndOtHit *)fOriginalHitArray->At(hitid);
  if (!hit)
    return kFALSE;
  TClonesArray &clref = *fOverlapHitArray;
  Int_t size = clref.GetEntriesFast();
  PndOtHit *hitnew = new (clref[size]) PndOtHit();
  *hitnew = *hit;
  hitnew->SetDetectorID(hit->GetDetectorID());

  return kTRUE;
}

ClassImp(PndOtSignalOverlap);
