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

/////////////////////////////////////////////////////////////
//  PndGemIdealHitProducer
//  Filler of PndGemHit
/////////////////////////////////////////////////////////////////

/** PndGemFindHits
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15.02.2009
 *@version 1.0
 **
 ** PANDA task class for producing ideal PndGemHits
 ** Task level RECO
 **/

// Includes from GEM
#include "PndGemDigitize.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TRandom.h"

#include "PndDetectorList.h"
#include "PndGemIdealHitProducer.h"

#include "PndGemMCPoint.h"
#include "PndGemHit.h"
#include "PndGemMCPoint.h"
#include "PndGemDigiPar.h"
#include "PndGemSensor.h"
#include "PndGemDigi.h"

#include <iomanip>

using std::cout;
using std::endl;
using std::flush;
using std::setprecision;
using std::setw;

// -----   Default constructor   -------------------------------------------
PndGemIdealHitProducer::PndGemIdealHitProducer() : FairTask("Ideal GEM hit Producer")
{
  fDigiPar = nullptr;

  fTNofEvents = 0;
  fTNofPoints = 0;
  fTNofHits = 0;
}

// -----   Constructor   ---------------------------------------------------
PndGemIdealHitProducer::PndGemIdealHitProducer(const char *name, Int_t iVerbose) : FairTask(name, iVerbose)
{
  fDigiPar = nullptr;

  fTNofEvents = 0;
  fTNofPoints = 0;
  fTNofHits = 0;
}

// -----   Destructor   ----------------------------------------------------
PndGemIdealHitProducer::~PndGemIdealHitProducer()
{
  if (fHitArray) {
    fHitArray->Delete();
    delete fHitArray;
  }
}

// -----   Private method SetParContainers   -------------------------------
void PndGemIdealHitProducer::SetParContainers()
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

// -----   Public method Init   --------------------------------------------
InitStatus PndGemIdealHitProducer::Init()
{

  std::cout << " INITIALIZATION OF Ideal Gem Hit Producer***************" << std::endl;

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndGemIdealHitProducer::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("GEMPoint");
  if (!fPointArray) {
    std::cout << "-W- PndGemIdealHitProducer::Init: "
              << "Array of GEMPoints not found!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndGemHit");

  ioman->Register("GEMHit", "GEM ideal hit", fHitArray, kTRUE);

  LOG(info) << " PndGemIdealHitProducer: Intialization successfull";
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndGemIdealHitProducer::Exec(Option_t *)
{

  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No hit array");

  fTNofEvents++;

  PndGemSensor *sensor;

  Int_t nofHits = 0;
  fHitArray->Delete();
  Double_t dr, dp;
  Double_t radius, innerR;

  for (Int_t iPoint = 0; iPoint < fPointArray->GetEntriesFast(); iPoint++) {
    PndGemMCPoint *currentPndGemMCPoint = (PndGemMCPoint *)fPointArray->At(iPoint);

    Double_t posIn[3] = {currentPndGemMCPoint->GetX(), currentPndGemMCPoint->GetY(), currentPndGemMCPoint->GetZ()};

    Int_t sensorId = currentPndGemMCPoint->GetSensorId();

    TString nodeName = fDigiPar->GetNodeName(sensorId);

    gGeoManager->cd(nodeName.Data());
    TGeoNode *curNode = gGeoManager->GetCurrentNode();

    sensor = (PndGemSensor *)fDigiPar->GetSensor(sensorId);

    fTNofPoints++;

    Double_t locPosIn[4];

    curNode->MasterToLocal(posIn, locPosIn);

    radius = TMath::Sqrt(locPosIn[0] * locPosIn[0] + locPosIn[1] * locPosIn[1]);
    innerR = sensor->GetInnerRadius();
    if (sensor->GetStripAngle(0) == 0. && sensor->GetStripAngle(1) == 90.) {
      dp = sensor->GetPitch(0) * radius / innerR / TMath::Sqrt(12.);
      dr = sensor->GetPitch(1) / TMath::Sqrt(12.);
    }

    if (sensor->GetStripAngle(0) == -sensor->GetStripAngle(1)) {
      Double_t pitch = (sensor->GetPitch(0) + sensor->GetPitch(1)) / 2.;
      dp = pitch * radius / innerR / TMath::Sqrt(12.);
      Double_t cosAng = TMath::Cos(TMath::DegToRad() * sensor->GetStripAngle(0));
      Double_t xh = -innerR * cosAng + TMath::Sqrt(innerR * innerR * cosAng * cosAng - innerR * innerR + radius * radius);
      Double_t backAng = TMath::ACos((radius * radius + xh * xh - innerR * innerR) / (2. * radius * xh));
      dr = dp / TMath::Tan(backAng);
    }

    Double_t phiAValue = TMath::ATan(locPosIn[0] / locPosIn[1]);
    if (locPosIn[1] < 0)
      phiAValue += TMath::Pi();
    else if (locPosIn[0] < 0)
      phiAValue += 2. * TMath::Pi();

    Double_t rSmear, pSmear;

    gRandom->Rannor(rSmear, pSmear);
    rSmear = radius + rSmear * dr;
    pSmear = 0. + pSmear * dp;

    if (rSmear < innerR || rSmear > sensor->GetOuterRadius())
      rSmear = radius;

    locPosIn[0] = rSmear * TMath::Sin(phiAValue) + pSmear * TMath::Cos(phiAValue);
    locPosIn[1] = rSmear * TMath::Cos(phiAValue) - pSmear * TMath::Sin(phiAValue);

    TVector3 pos(locPosIn[0], locPosIn[1], sensor->GetZ0());

    Double_t sigma = dp;
    if (dr > sigma)
      sigma = dr;
    TVector3 dpos(sigma, sigma, sensor->GetD());

    new ((*fHitArray)[nofHits++]) PndGemHit(static_cast<int>(DetectorType::kGemHit), pos, dpos, -1, -1, dr, dp, iPoint);
    fTNofHits++;
  } // end of loop over Points
}

// -----   Private method Finish   -----------------------------------------
void PndGemIdealHitProducer::Finish()
{
  if (fHitArray)
    fHitArray->Delete();

  cout << "-------------------- " << fName.Data() << " : Summary ---------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Points:        " << setw(10) << fTNofPoints << "    ( " << (Double_t)fTNofPoints / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Hits:          " << setw(10) << fTNofHits << "    ( " << (Double_t)fTNofHits / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofHits / ((Double_t)fTNofEvents) / ((Double_t)fDigiPar->GetNSensors()) << " per sensor )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofHits / ((Double_t)fTNofPoints) << " per point )" << endl;
  cout << "---------------------------------------------------------------------" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndGemIdealHitProducer)
