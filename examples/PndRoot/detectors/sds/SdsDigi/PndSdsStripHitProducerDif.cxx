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
// -----                PndSdsStripHitProducer source file             -----
// -----                     WITH CHARGE DIFFUSION                     -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TVector2.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "PndSdsStripHitProducerDif.h"
#include "PndSdsMCPoint.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairLogger.h"
#include "PndStringSeparator.h"
#include "PndSdsCalcStripDif.h"
#include "PndSdsDigiStrip.h"
#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndSdsStripHitProducerDif::PndSdsStripHitProducerDif() : PndSdsTask("SSD Strip Digi Producer(PndSdsStripHitProducerDif)")
{
  fBranchName = "SSDPoint";
  //  stripHits = 0;

  /*  fTopPitch = 0.;
    fBotPitch = 0.;
    fOrient = 0.;
    fSkew = 0.;
    fTopAnchor = TVector2(0,0);
    fBotAnchor = TVector2(0,0);
    fNrTopFE = 0;
    fNrBotFE = 0;
    fNrFECh = 0;
    fThreshold = 0.;
    fNoise = 0.;*/
  fOverrideParams = false;
  //  fHitArray  = new TClonesArray("PndSdsHit");
  //	fStripArray	= new TClonesArray("PndSdsStripHit");
  fPersistance = kTRUE;
  fGeoH = PndGeoHandling::Instance();
}
// -------------------------------------------------------------------------

PndSdsStripHitProducerDif::PndSdsStripHitProducerDif(Double_t topPitch, Double_t botPitch, Double_t ori, Double_t skew, TVector2 topAnchor, TVector2 botAnchor, Int_t nrTopFE,
                                                     Int_t nrBotFE, Int_t nrFECh, Double_t threshold, Double_t noise, TString sensorType, TString feType)
  : PndSdsTask("SSD Strip Digi Producer")
{
  // This constructor is probably not needed anymore, since the parameters are
  // read in via an ascii file.
  std::cout << " -W- Obsolete constructor for PndSdsStripHitProducer called."
            << "Ssd strip sensors in barrel and disk are set to the SAME." << std::endl;

  fBranchName = "SSDPoint";
  fOverrideParams = true;
  SetParamSet(topPitch, botPitch, ori, skew, topAnchor, botAnchor, nrTopFE, nrBotFE, nrFECh, threshold, noise, "Rect", feType);
  SetParamSet(topPitch, botPitch, ori, skew, topAnchor, botAnchor, nrTopFE, nrBotFE, nrFECh, threshold, noise, "Trap", feType);
  std::cout << "SSD Strip Digi Producer initiated" << std::endl;
  fGeoH = PndGeoHandling::Instance();
}

void PndSdsStripHitProducerDif::SetParamSet(Double_t topPitch, Double_t botPitch, Double_t ori, Double_t skew, TVector2 topAnchor, TVector2 botAnchor, Int_t nrTopFE, Int_t nrBotFE,
                                            Int_t nrFECh, Double_t threshold, Double_t noise, TString sensorType, TString feType)
{
  FairRun *ana = FairRun::Instance();
  // FairRootManager* ioman = FairRootManager::Instance();
  if (0 == fDigiParRect || 0 == fDigiParTrap)
    SetParContainers();
  if (fOverrideParams) {
    if (sensorType.Contains("Rect"))
      fCurrentDigiPar = fDigiParRect;
    else if (sensorType.Contains("Trap"))
      fCurrentDigiPar = fDigiParTrap;
    fCurrentDigiPar->SetTopPitch(topPitch);
    fCurrentDigiPar->SetBotPitch(botPitch);
    fCurrentDigiPar->SetSkew(skew);
    fCurrentDigiPar->SetOrient(ori);
    fCurrentDigiPar->SetTopAnchor(topAnchor);
    fCurrentDigiPar->SetBotAnchor(botAnchor);
    fCurrentDigiPar->SetNrFECh(nrFECh);
    fCurrentDigiPar->SetNrTopFE(nrTopFE);
    fCurrentDigiPar->SetNrBotFE(nrBotFE);
    fCurrentDigiPar->SetThreshold(threshold);
    fCurrentDigiPar->SetNoise(noise);
    fCurrentDigiPar->SetSensType(sensorType);
    fCurrentDigiPar->SetFeType(feType);
    fCurrentDigiPar->setChanged();
    fCurrentDigiPar->setInputVersion(ana->GetRunId(), 1);
  }
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSdsStripHitProducerDif::~PndSdsStripHitProducerDif() {}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndSdsStripHitProducerDif::SetParContainers()
{
  // called from the FairRun::Init()
  // Get Base Container

  fGeoH->SetParContainers();
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fDigiParRect = (PndSdsStripDigiPar *)(rtdb->getContainer("SSDStripDigiParRect"));
  fDigiParTrap = (PndSdsStripDigiPar *)(rtdb->getContainer("SSDStripDigiParTrap"));
}

InitStatus PndSdsStripHitProducerDif::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndSdsStripHitProducerDif::Init()
{
  // FairRun* ana = FairRun::Instance();
  FairRootManager *ioman = FairRootManager::Instance();

  SetBranchNames();
  SetMCPointType();

  // std::cout << "-I- PndSdsStripHitProucer::Init() " << fGeoH->GetPath("1_1/212_0/") << std::endl;

  if (!ioman) {
    std::cout << "-E- PndSdsStripHitProducer::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);
  if (!fPointArray) {
    std::cout << "-W- PndSdsStripHitProducer::Init: "
              << "No SSDPoint array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  //  fHitArray = new TClonesArray("PndSdsHit");
  //  ioman->Register("SSDHit", "SSD", fHitArray, kTRUE);

  // Create and register output array
  fStripArray = new TClonesArray("PndSdsDigiStrip");
  ioman->Register("SSDStripDigis", "SSD", fStripArray, GetPersistency());

  // Create and register parameter array
  //  fStripArray = new TClonesArray("PndSdsDigiPar");
  //  ioman->Register("SSDDigiParam", "SSD", fDigiParRect, fPersistance);

  LOG(info) << " PndSdsStripHitProducer: Initialisation successfull";

  if (!fDigiParRect) {
    LOG(error) << " PndSdsStripHitProducer: DigiPar Rect Container does not exist!";
    return kERROR;
  }
  if (!fDigiParTrap) {
    LOG(error) << " PndSdsStripHitProducer: DigiPar Trap Container does not exist!";
    return kERROR;
  }

  if (fVerbose > 2)
    fDigiParRect->Print();
  if (fVerbose > 2)
    fDigiParTrap->Print();

  fStripCalcTopRect = new PndSdsCalcStripDif(fDigiParRect, SensorSide::kTOP);
  fStripCalcBotRect = new PndSdsCalcStripDif(fDigiParRect, SensorSide::kBOTTOM);
  fStripCalcTopTrap = new PndSdsCalcStripDif(fDigiParTrap, SensorSide::kTOP);
  fStripCalcBotTrap = new PndSdsCalcStripDif(fDigiParTrap, SensorSide::kBOTTOM);
  fStripCalcTopRect->SetVerboseLevel(fVerbose);
  fStripCalcBotRect->SetVerboseLevel(fVerbose);
  fStripCalcTopTrap->SetVerboseLevel(fVerbose);
  fStripCalcBotTrap->SetVerboseLevel(fVerbose);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndSdsStripHitProducerDif::Exec(Option_t *)
{
  // Reset output array
  fStripArray->Clear();

  // Declare some variables
  PndSdsMCPoint *point = nullptr;

  //  Int_t detID = 0;       // Detector ID
  //     Int_t trackID = 0;     // Track index

  // Loop over PndSdsMCPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  if (fVerbose > 0) {
    std::cout << " Nr of Points: " << nPoints << std::endl;
  }

  Int_t iStrip = 0;

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndSdsMCPoint *)fPointArray->At(iPoint);
    if (kFALSE == SelectSensorParams(point->GetSensorID()))
      continue;

    if (fVerbose > 2) {
      std::cout << "***** Strip Digi for " << fCurrentDigiPar->GetSensType() << " ******" << std::endl;
      std::cout << " DetName : " << fGeoH->GetPath(point->GetSensorID()) << std::endl;
    }
    if (!point) {
      std::cout << "No Point!" << std::endl;
      continue;
    }
    if (fVerbose > 2) {
      std::cout << "****Global Point: " << std::endl;
      point->Print("");
    }

    // transform to local sensor system... (mc point has the ID not the path to the volume)
    TVector3 posInL = fGeoH->MasterToLocalShortId(point->GetPosition(), point->GetSensorID());
    TVector3 posOutL = fGeoH->MasterToLocalShortId(point->GetPositionOut(), point->GetSensorID());

    if (fVerbose > 2) {
      posInL.Print();
      posOutL.Print();
      std::cout << "Energy: " << point->GetEnergyLoss() << std::endl;
    }
    //      detID   = point->GetDetectorID();

    // Top Side
    if (fVerbose > 2)
      std::cout << "Top Side: " << std::endl;
    // Calculate a cluster of Strips fired
    std::vector<PndSdsStrip> topStrips = fCurrentStripCalcTop->GetStrips(posInL.X(), posInL.Y(), posInL.Z(), posOutL.X(), posOutL.Y(), posOutL.Z(), point->GetEnergyLoss());

    if (topStrips.size() != 0) {
      if (fVerbose > 1)
        std::cout << "SensorStrips: " << std::endl;
      for (std::vector<PndSdsStrip>::const_iterator kit = topStrips.begin(); kit != topStrips.end(); ++kit) {
        AddDigi(iStrip, iPoint, DetectorType::kMVDHitsStrip, point->GetSensorID(), fCurrentStripCalcTop->CalcFEfromStrip(kit->GetIndex()),
                fCurrentStripCalcTop->CalcChannelfromStrip(kit->GetIndex()), kit->GetCharge());
        if (fVerbose > 1)
          std::cout << *kit << std::endl;
      }
    } else if (fVerbose > 2)
      std::cout << "Top side empty" << std::endl;

    // Bottom Side
    if (fVerbose > 2)
      std::cout << "Bottom Side: " << std::endl;
    std::vector<PndSdsStrip> botStrips = fCurrentStripCalcBot->GetStrips(posInL.X(), posInL.Y(), posInL.Z(), posOutL.X(), posOutL.Y(), posOutL.Z(), point->GetEnergyLoss());
    if (botStrips.size() != 0) {
      if (fVerbose > 2)
        std::cout << " SensorStrips: " << std::endl;
      for (std::vector<PndSdsStrip>::const_iterator kit = botStrips.begin(); kit != botStrips.end(); ++kit) {
        AddDigi(iStrip, iPoint, DetectorType::kMVDHitsStrip, point->GetSensorID(), fCurrentStripCalcBot->CalcFEfromStrip(kit->GetIndex()) + fCurrentDigiPar->GetNrTopFE(),
                fCurrentStripCalcBot->CalcChannelfromStrip(kit->GetIndex()), kit->GetCharge());
        if (fVerbose > 2)
          std::cout << *kit << std::endl;
      }
    } else if (fVerbose > 2)
      std::cout << "Bottom side empty" << std::endl;

  } // Loop over MCPoints

  // Event summary
  if (fVerbose > 1)
    LOG(info) << " PndSdsStripHitProducer: " << nPoints << " PndSdsMCPoints, " << iStrip << " Digis created.";
}
// -------------------------------------------------------------------------

void PndSdsStripHitProducerDif::AddDigi(Int_t &iStrip, Int_t iPoint, Int_t detID, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge)
{
  Bool_t found = kFALSE;
  PndSdsDigiStrip *aDigi = 0;
  for (Int_t kstr = 0; kstr < iStrip && found == kFALSE; kstr++) {
    aDigi = (PndSdsDigiStrip *)fStripArray->At(kstr);
    if (aDigi->GetDetID() == detID && aDigi->GetSensorID() == sensorID && aDigi->GetFE() == fe && aDigi->GetChannel() == chan) {
      aDigi->AddCharge(charge);
      aDigi->AddIndex(iPoint);
      found = kTRUE;
      //		((PndSdsDigiStrip*)(*fStripArray)[kstr])->AddCarge(charge);
      //		((PndSdsDigiStrip*)(*fStripArray)[kstr])->AddIndex(iPoint);
      //		return;
    }
  }
  if (found == kFALSE) { // TODO: Simulate a timestamp
    new ((*fStripArray)[iStrip]) PndSdsDigiStrip(iPoint, detID, sensorID, fe, chan, 0, fMCPointType, charge);
    iStrip++;
  }
}

// -------------------------------------------------------------------------

// void PndSdsStripHitProducerDif::GetLocalHitPoints(PndSdsMCPoint* myPoint, TVector3& myHitIn, TVector3& myHitOut)
// {
//
//   if (fVerbose > 1)
//     std::cout << "GetLocalHitPoints" << std::endl;
//   TGeoHMatrix trans = GetTransformation(fGeoH->GetPath(myPoint->GetDetName()).Data());
//
//   Double_t posIn[3];
//   Double_t posOut[3];
//   Double_t posInLocal[3];
//   Double_t posOutLocal[3];
//
//   posIn[0] = myPoint->GetX();
//   posIn[1] = myPoint->GetY();
//   posIn[2] = myPoint->GetZ();
//
//   posOut[0] = myPoint->GetXOut();
//   posOut[1] = myPoint->GetYOut();
//   posOut[2] = myPoint->GetZOut();
//
//   if (fVerbose > 1){
//     for (Int_t i = 0; i < 3; i++)
//       std::cout << "posIn "<< i << ": " << posIn[i] << std::endl;
//
//     trans.Print("");
//   }
//
//   trans.MasterToLocal(posIn, posInLocal);
//   trans.MasterToLocal(posOut, posOutLocal);
//
//   if (fVerbose > 1) {
//     for (Int_t i = 0; i < 3; i++){
//       std::cout << "posInLocal "<< i << ": " << posInLocal[i] << std::endl;
//       std::cout << "posOutLocal "<< i << ": " << posOutLocal[i] << std::endl;
//     }
//   }
//
//   //posIn/OutLocal have the center of the coordinate system in the center of the shape
//   //typically sensors have their coordinate system centered at the lower left corner
//
// //   TVector3 offset = GetSensorDimensions(fGeoH->GetPath(myPoint->GetDetName()).Data());
//
// //   posInLocal[0] += 0.5*offset.x();
// //   posInLocal[1] += 0.5*offset.y();
// //   //posInLocal[2] += offset.z();
// //
// //   posOutLocal[0] += 0.5*offset.x();
// //   posOutLocal[1] += 0.5*offset.y();
// //   //posOutLocal[2] += offset.z();
//
//   myHitIn.setVector(posInLocal);
//   myHitOut.setVector(posOutLocal);
//
// }
// -------------------------------------------------------------------------

// TGeoHMatrix PndSdsStripHitProducerDif::GetTransformation(std::string detName) const
// {
//   gGeoManager->cd(detName.c_str());
//   TGeoHMatrix* transMat = gGeoManager->GetCurrentMatrix();
//   if (fVerbose > 1)
//   transMat->Print("");
//   return *transMat;
// }
// -------------------------------------------------------------------------
//
// TVector3 PndSdsStripHitProducerDif::GetSensorDimensions(std::string detName) const
// {
//   gGeoManager->cd(detName.c_str());
//   TGeoVolume* actVolume = gGeoManager->GetCurrentVolume();
//   TGeoBBox* actBox = (TGeoBBox*)(actVolume->GetShape());
//   TVector3 result;
//   result.SetX(actBox->GetDX());
//   result.SetY(actBox->GetDY());
//   result.SetZ(actBox->GetDZ());
//
//   //result.Dump();
//
//   return result;
// }
// -------------------------------------------------------------------------

Bool_t PndSdsStripHitProducerDif::SelectSensorParams(Int_t sensorID)
{
  /// TODO change this to a switch on DetID==2 ?
  TString detpath = fGeoH->GetPath(sensorID);
  if (!(detpath.Contains("Strip")))
    return kFALSE;

  if (detpath.Contains(fDigiParRect->GetSensType())) {
    fCurrentStripCalcTop = fStripCalcTopRect;
    fCurrentStripCalcBot = fStripCalcBotRect;
    fCurrentDigiPar = fDigiParRect;
  } else if (detpath.Contains(fDigiParTrap->GetSensType())) {
    fCurrentStripCalcTop = fStripCalcTopTrap;
    fCurrentStripCalcBot = fStripCalcBotTrap;
    fCurrentDigiPar = fDigiParTrap;
  } else {
    if (fVerbose > 1)
      std::cout << "detector name does not contain 'Rect' or 'Trap'" << std::endl;
    if (fVerbose > 2)
      std::cout << " DetName : " << detpath << std::endl;
    return kFALSE;
  }
  return kTRUE;
}

// -------------------------------------------------------------------------

ClassImp(PndSdsStripHitProducerDif);
