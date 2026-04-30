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
// -----                  PndLmdHitMergeTask source file               -----
// -------------------------------------------------------------------------

#include <cmath>
#include <map>
#include <vector>

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "PndLmdHitMergeTask.h"
#include "PndSdsHit.h"
#include "PndSdsMergedHit.h"
#include "PndLmdGeometryHelper.h"

#include "TClonesArray.h"
#include "TH2D.h"

// -----   Default constructor   -------------------------------------------
PndLmdHitMergeTask::PndLmdHitMergeTask() : FairTask("LMD Hit Merging Task"), fHitArray(nullptr), fMergedHitArray(nullptr)
{
  fHitBranchName = "LMDHitsPixel";
  hdxdy = new TH2D("hdxdy", "; #deltax, #mum; #deltay, #mum", 1000, -100, 100, 1000, -100, 100);
}

// -----   Named constructor   -------------------------------------------
PndLmdHitMergeTask::PndLmdHitMergeTask(const char *name) : FairTask(name), fHitArray(nullptr), fMergedHitArray(nullptr)
{
  fHitBranchName = "LMDHitsPixel";
  hdxdy = new TH2D("hdxdy", "; #deltax, #mum; #deltay, #mum", 1000, -100, 100, 1000, -100, 100);
}

// -----   Destructor   ----------------------------------------------------
PndLmdHitMergeTask::~PndLmdHitMergeTask() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndLmdHitMergeTask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndLmdHitMergeTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray *)ioman->GetObject(fHitBranchName);
  //
  if (!fHitArray) {
    std::cout << "-W- PndLmdHitMergeTask::Init: "
              << "No LmdHitsPixel array!" << std::endl;
    return kERROR;
  }

  // set output arrays
  // fMergedHitArray = new TClonesArray("PndSdsHit");
  fMergedHitArray = new TClonesArray("PndSdsMergedHit");
  ioman->Register("LMDHitsMerged", "PndLmd", fMergedHitArray, true);
  Info("Init", "Initialisation successfull");
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndLmdHitMergeTask::Exec(Option_t *)
{
  if (fVerbose > 2)
    std::cout << " **Starting PndLmdHitMergeTask::Exec()**" << std::endl;

  // Reset output array
  // fMergedHitArray = FairRootManager::Instance()->GetTClonesArray("LmdHits");
  // if ( ! fMergedHitArray ) Fatal("Exec", "No OutputArray");
  fMergedHitArray->Delete();

  // Get input array
  fHitArray = (TClonesArray *)FairRootManager::Instance()->GetObject(fHitBranchName);
  if (!fHitArray) {
    std::cout << "-W- PndLmdHitMergeTask::Init: "
              << "No Hit array!" << std::endl;
    return;
  }

  // when we have no hits, we can end the event here.
  if (fHitArray->GetEntriesFast() == 0)
    return;

  PndLmdGeometryHelper &LmdGeoHelper = PndLmdGeometryHelper::getInstance();
  double HitDistanceThreshold = std::pow(75.0, 2); // 3sigma=75mu squared (hit resolution)

  // -------   SEARCH Close Hits  ------
  std::vector<unsigned int> mergewithIDs, mergedHits;
  // std::map<unsigned int, std::vector<unsigned int>> backmap;
  //  PndSdsHit* tmphit;
  PndSdsMergedHit *tmphit;
  unsigned int newHits = 0;
  for (Int_t iHit = 0; iHit < fHitArray->GetEntriesFast(); ++iHit) {
    if (fVerbose > 2) {
      if (iHit == 0)
        std::cout << "#### NEW event ####" << std::endl;
    }

    auto result = std::find(mergedHits.begin(), mergedHits.end(), iHit);
    // if already merged skip this hit
    if (result != mergedHits.end())
      continue;

    mergewithIDs.clear();
    PndSdsHit *myHit1 = (PndSdsHit *)(fHitArray->At(iHit));

    for (Int_t jHit = iHit + 1; jHit < fHitArray->GetEntriesFast(); ++jHit) { // check other hits
      PndSdsHit *myHit2 = (PndSdsHit *)(fHitArray->At(jHit));

      // first check if they are allowed to overlap by geometry definition
      auto const &HitInfo1 = LmdGeoHelper.getHitLocationInfo(myHit1->GetSensorID());
      auto const &HitInfo2 = LmdGeoHelper.getHitLocationInfo(myHit2->GetSensorID());

      // the direct check from the lmd geo helper does not check all overlapping areas
      // only the larger ones
      if (HitInfo1.detector_half == HitInfo2.detector_half && HitInfo1.plane == HitInfo2.plane && HitInfo1.module == HitInfo2.module &&
          HitInfo1.module_side != HitInfo2.module_side) {
        // double dz = (myHit1->GetZ()) - (myHit2->GetZ());
        TVector3 hit1 = LmdGeoHelper.transformPndGlobalToLmdLocal(myHit1->GetPosition());
        TVector3 hit2 = LmdGeoHelper.transformPndGlobalToLmdLocal(myHit2->GetPosition());
        double dx = 1e4 * (hit1.X() - hit2.X()); // in um
        double dy = 1e4 * (hit1.Y() - hit2.Y()); // in um

        // now check the distance in the xy plane
        if (std::pow(dx, 2) + std::pow(dy, 2) < HitDistanceThreshold) { // 3*sigma_dx, sigma_dx=25mkm
          hdxdy->Fill(dx, dy);

          mergewithIDs.push_back(jHit);
          mergedHits.push_back(jHit);
        }
      }
    }

    if (mergewithIDs.size() > 0) { // merge hits
      if (fVerbose > 4)
        std::cout << "hit merged with: " << mergewithIDs.size() << " hits" << std::endl;
      tmphit = new ((*fMergedHitArray)[newHits]) PndSdsMergedHit(*myHit1, 0);

      double x = tmphit->GetX(), y = tmphit->GetY(), z = tmphit->GetZ();
      for (unsigned int iMerge = 0; iMerge < mergewithIDs.size(); iMerge++) { // loop over hits to merge in
        PndSdsHit *myHit2 = (PndSdsHit *)(fHitArray->At(mergewithIDs.at(iMerge)));
        x += myHit2->GetX();
        y += myHit2->GetY();
        z += myHit2->GetZ();
        tmphit->SetSecondMCHit(myHit2->GetRefIndex()); //!!! works only in case merging 2 hits
        tmphit->SetIsMerged(true);
      }
      x /= (mergewithIDs.size() + 1);
      y /= (mergewithIDs.size() + 1);
      z /= (mergewithIDs.size() + 1);
      tmphit->SetX(x);
      tmphit->SetY(y);
      tmphit->SetZ(z);
      if (fVerbose > 4) {
        std::cout << "!!! Merged hit !!!" << std::endl;
        tmphit->Print();
      }
    } else { // dont merge, just use original hit
      tmphit = new ((*fMergedHitArray)[newHits]) PndSdsMergedHit(*myHit1, -1);
      tmphit->SetIsMerged(false);
    }

    newHits++;
  }

  if (fVerbose > 1)
    LOG(info) << " PndLmdHitMergeTask: out of " << fHitArray->GetEntriesFast() << " Hits " << fMergedHitArray->GetEntriesFast() << " Hits merged.";
  return;
}

void PndLmdHitMergeTask::Finish()
{
  LOG(info) << "saving histograms to file...";
  hdxdy->Write();
}

ClassImp(PndLmdHitMergeTask);
