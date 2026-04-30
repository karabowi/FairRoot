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

//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemMatchHits source file                 -----
// -----                  Created 12/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

// Includes from GEM
#include "PndGemMatchHits.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNode.h"

#include "PndGemHit.h"
#include "PndGemMCPoint.h"
#include "PndGemDigiPar.h"
#include "PndGemSensor.h"

#include <iostream>
#include <iomanip>
#include <map>

using std::cerr;
using std::cout;
using std::endl;
using std::fixed;
using std::left;
using std::map;
using std::pair;
using std::right;
using std::setprecision;
using std::setw;

// -----   Default constructor   ------------------------------------------
PndGemMatchHits::PndGemMatchHits() : FairTask("GEM MatchHits", 1)
{
  fDigiPar = nullptr;
  fPoints = nullptr;
  fHits = nullptr;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemMatchHits::PndGemMatchHits(Int_t iVerbose) : FairTask("GEM MatchHitsr", iVerbose)
{
  fDigiPar = nullptr;
  fPoints = nullptr;
  fHits = nullptr;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndGemMatchHits::PndGemMatchHits(const char *name, Int_t iVerbose) : FairTask(name, iVerbose)
{
  fDigiPar = nullptr;
  fPoints = nullptr;
  fHits = nullptr;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemMatchHits::~PndGemMatchHits()
{
  Reset();
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndGemMatchHits::Exec(Option_t *)
{
  Int_t nofPoints = fPoints->GetEntriesFast();
  Int_t nofHits = fHits->GetEntriesFast();

  if (fVerbose)
    cout << "PndGemMatchHits::Exec() with " << nofPoints << " points and " << nofHits << " hits." << endl;

  Int_t nHits = 0;
  Int_t nMatchedHits = 0;
  Int_t nFakeHits = 0;
  Int_t nMultiHits = 0;

  vector<Double_t> pointZ;
  vector<Double_t> pointR;
  vector<Double_t> pointP;
  for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++) {
    PndGemMCPoint *currentPndGemMCPoint = (PndGemMCPoint *)fPoints->At(iPoint);

    Double_t pointX = currentPndGemMCPoint->GetX();
    Double_t pointY = currentPndGemMCPoint->GetY();

    if (fVerbose > 1)
      cout << " .... " << pointX << "  " << pointY << " " << currentPndGemMCPoint->GetZ() << endl;

    Double_t phiAValue = TMath::ATan(pointX / pointY);
    if (pointY < 0)
      phiAValue += TMath::Pi();
    else if (pointX < 0)
      phiAValue += 2. * TMath::Pi();

    pointZ.push_back(currentPndGemMCPoint->GetZ());
    pointR.push_back(TMath::Sqrt(pointX * pointX + pointY * pointY));
    pointP.push_back(phiAValue);

    if (fVerbose > 1)
      cout << "point " << iPoint << ", sensor Id = " << currentPndGemMCPoint->GetSensorId() << " (" << pointZ[pointZ.size() - 1] << "," << pointR[pointR.size() - 1] << ","
           << pointP[pointP.size() - 1] << ")" << endl;
  }

  for (Int_t iHit = 0; iHit < nofHits; iHit++) {
    PndGemHit *currentPndGemHit = (PndGemHit *)fHits->At(iHit);

    Double_t hitX = currentPndGemHit->GetX();
    Double_t hitY = currentPndGemHit->GetY();
    Double_t hitZ = currentPndGemHit->GetZ();

    if (fVerbose > 1)
      cout << " .... " << hitX << "  " << hitY << " " << currentPndGemHit->GetZ() << endl;

    Double_t hitP = TMath::ATan(hitX / hitY);
    if (hitY < 0)
      hitP += TMath::Pi();
    else if (hitX < 0)
      hitP += 2. * TMath::Pi();
    Double_t hitR = TMath::Sqrt(hitX * hitX + hitY * hitY);

    if (fVerbose > 1) {
      cout << "-------------------------------------------" << endl;
      cout << "hit " << iHit << " (" << hitZ << "," << hitR << "," << hitP << ")" << endl;
    }

    Int_t matchPoint = -1;
    Double_t closestDistance = 1000.;
    Bool_t multiHit = kFALSE;
    for (size_t iPoint = 0; iPoint < pointZ.size(); iPoint++) {
      //     if ( fVerbose > 1 )
      if (TMath::Abs(pointZ[iPoint] - hitZ) > currentPndGemHit->GetDz()) { /*cout << "FAILED Z" << endl;*/
        continue;
      }
      if (TMath::Abs(pointR[iPoint] - hitR) > currentPndGemHit->GetDr() * TMath::Sqrt(3.)) { /*cout << "FAILED R" << endl;*/
        continue;
      }

      if (hitP < 1.0 && pointP[iPoint] > 5.5)
        hitP += 2. * TMath::Pi();
      if (hitP > 5.5 && pointP[iPoint] < 1.0)
        hitP -= 2. * TMath::Pi();

      if (TMath::Abs(TMath::Tan(pointP[iPoint] - hitP)) > currentPndGemHit->GetDp() * TMath::Sqrt(3.) / hitR) { /*cout << "FAILED PHI" << endl;*/
        continue;
      }
      if (fVerbose > 1) {
        cout << "matched with "
             << " (" << pointZ[iPoint] << "," << pointR[iPoint] << "," << pointP[iPoint] << ") " << endl;
        cout << "DP = " << currentPndGemHit->GetDp() << " after transf. = " << currentPndGemHit->GetDp() * TMath::Sqrt(3.) / hitR << " while p_p = " << pointP[iPoint] << " "
             << " h_p = " << hitP << endl;
        cout << "PASSED WITH POINT " << iPoint << endl;
      }
      Double_t distance = TMath::Sqrt((pointR[iPoint] - hitR) * (pointR[iPoint] - hitR) / currentPndGemHit->GetDr() / currentPndGemHit->GetDr() +
                                      (pointP[iPoint] - hitP) * (pointP[iPoint] - hitP) / currentPndGemHit->GetDp() / currentPndGemHit->GetDp());
      if (matchPoint != -1)
        multiHit = kTRUE;
      if (distance > closestDistance)
        continue;
      closestDistance = distance;
      matchPoint = iPoint;
    }
    currentPndGemHit->SetRefIndex(matchPoint);
    if (matchPoint != -1) {
      PndGemMCPoint *matchp = (PndGemMCPoint *)fPoints->At(matchPoint);
      currentPndGemHit->SetBotIndex(matchp->GetTrackID());
    } else {
      currentPndGemHit->SetBotIndex(-1);
    }

    nHits++;
    if (matchPoint != -1)
      nMatchedHits++;
    else
      nFakeHits++;
    if (multiHit)
      nMultiHits++;
  }

  fNHits += nHits;
  fNMatchedHits += nMatchedHits;
  fNFakeHits += nFakeHits;
  fNMultiHits += nMultiHits;

  if (fVerbose) {
    cout << "************PndGemMatchHits**************" << endl;
    cout << " Number of all hits " << nHits << endl;
    cout << " Number of matched hits " << nMatchedHits << " -> " << 100. * (Double_t)nMatchedHits / (Double_t)nHits << endl;
    cout << " Number of fake hits " << nFakeHits << " -> " << 100. * (Double_t)nFakeHits / (Double_t)nHits << endl;
    cout << " Number of multi hits " << nMultiHits << " -> " << 100. * (Double_t)nMultiHits / (Double_t)nHits << endl;
    cout << "*****************************************" << endl;
  }
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemMatchHits::SetParContainers()
{

  // Get run and runtime database
  FairRunAna *run = FairRunAna::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get GEM digitisation parameter container. FairRuntimeDb is responsible for deleting "PndGemDetectors" container.
  fDigiPar = (PndGemDigiPar *)(db->getContainer("PndGemDetectors"));
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus PndGemMatchHits::Init()
{

  // Get input array
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");
  fPoints = (TClonesArray *)ioman->GetObject("GEMPoint");

  fHits = (TClonesArray *)ioman->GetObject("GEMHit");

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemMatchHits::ReInit()
{

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void PndGemMatchHits::Reset()
{
  fNHits = fNMatchedHits = fNFakeHits = fNMultiHits = 0;
}
// -------------------------------------------------------------------------

// -----   Private method Finish   -----------------------------------------
void PndGemMatchHits::Finish()
{
  cout << "************PndGemMatchHits summary**************" << endl;
  cout << " Number of all hits " << fNHits << endl;
  cout << " Number of matched hits " << fNMatchedHits << " -> " << 100. * (Double_t)fNMatchedHits / (Double_t)fNHits << endl;
  cout << " Number of fake hits " << fNFakeHits << " -> " << 100. * (Double_t)fNFakeHits / (Double_t)fNHits << endl;
  cout << " Number of multi hits " << fNMultiHits << " -> " << 100. * (Double_t)fNMultiHits / (Double_t)fNHits << endl;
  cout << "*************************************************" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndGemMatchHits)
