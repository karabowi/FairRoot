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

#include <iostream>

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"
#include "TParticle.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoArb8.h"
#include "TGeoTrd2.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TVirtualMC.h"

#include "FairVolume.h"
// add on for debug
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairModule.h"
#include "PndDetectorList.h"
#include "PndStack.h"

#include "PndMdt.h"
#include "PndMdtID.h"
#include "PndMdtPoint.h"

using namespace std;

// -----   Default constructor   -------------------------------------------
PndMdt::PndMdt()
  : FairDetector(), fTrkIn(-1), fELoss(0), fTime(0), fPos_In(), fMom_In(), fVerboseLevel(0), mdtMagnet(0), mdtCoil(0), mdtMFI(0), fBarrel(""), fEndcap(""),
    fMuonFilter(""), fForward("")
{
}
// -------------------------------------------------------------------------

// -----   Inherited constructor   -----------------------------------------
PndMdt::PndMdt(const char *name, Bool_t active)
  : FairDetector(name, active), fTrkIn(-1), fELoss(0), fTime(0), fPos_In(), fMom_In(), fVerboseLevel(0), mdtMagnet(0), mdtCoil(0), mdtMFI(0), fBarrel(""),
    fEndcap(""), fMuonFilter(""), fForward("")
{
}
// -------------------------------------------------------------------------

PndMdt::PndMdt(const PndMdt& rhs)
    : FairDetector(rhs), fTrkIn(-1), fELoss(0), fTime(0), fPos_In(), fMom_In(), fVerboseLevel(0), mdtMagnet(0), mdtCoil(0), mdtMFI(0), fBarrel(""),
    fEndcap(""), fMuonFilter(""), fForward("")
{
}

// -----   Destructor   ----------------------------------------------------
PndMdt::~PndMdt()
{
}
// -------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndMdt::Print() const
{
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void PndMdt::Reset()
{
  fPointVector->clear();
}
// ----------------------------------------------------------------------------

// -----   Public method ResetParameters   ------------------------------------
void PndMdt::ResetParameters()
{
  fELoss = 0.;
  fTime = 0.;
}
// ----------------------------------------------------------------------------

// -----   Public method ConstructGeometry   ----------------------------------
void PndMdt::ConstructGeometry()
{
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  if (fBarrel != "") {
    if (fBarrel == "fast" || fBarrel == "Fast") {
      ConstructGeometryFast();
    } else if (fBarrel.EndsWith(".root")) {
      SetGeometryFileName(fBarrel);
      std::cout << "=======  MDT::  ConstructGeometry()  ========" << endl;
      std::cout << "File name, " << fBarrel << endl;
      std::cout << "=============================================" << endl;
      ConstructRootGeometry();
    } else {
      std::cout << "PndMdt::ConstructGeometry : No good MDT Barrel definition " << std::endl;
      exit(0);
    }
  }

  if (fEndcap != "") {
    if (fEndcap.EndsWith(".root")) {
      SetGeometryFileName(fEndcap);
      ConstructRootGeometry();
    } else if (fBarrel != "fast" && fBarrel != "Fast") {
      std::cout << "PndMdt::ConstructGeometry : No good MDT Endcap definition " << std::endl;
      exit(0);
    }
  }

  if (fMuonFilter != "") {
    if (fMuonFilter == "fast" || fMuonFilter == "Fast") {
      PndMdtMuonFilter();
    } else if (fMuonFilter.EndsWith(".root")) {
      SetGeometryFileName(fMuonFilter);
      ConstructRootGeometry();
    } else {
      std::cout << "PndMdt::ConstructGeometry : No good MDT Muon Filter definition " << std::endl;
      exit(0);
    }
  }

  if (fForward != "") {
    if (fForward == "fast" || fForward == "Fast") {
      PndMdtForward();
    } else if (fForward.EndsWith(".root")) {
      SetGeometryFileName(fForward);
      ConstructRootGeometry();
    } else {
      std::cout << "PndMdt::ConstructGeometry : No good MDT Forward definition " << std::endl;
      exit(0);
    }
  }

  if (mdtMagnet)
    PndMdtMagnet();
  if (mdtMFI)
    PndMdtMFIron();
  if (mdtCoil)
    PndMdtCoil();

  return;
}
// ----------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void PndMdt::Initialize()
{
  FairDetector::Initialize();
  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?
  // PndGeoMdtPar* par=(PndGeoMdtPar*)(rtdb->getContainer("PndGeoMdtPar"));

  // TObjArray *fSensNodes = par->GetSensitiveNodes();
}
// -------------------------------------------------------------------------

// -----   Public method BeginEvent   --------------------------------------
void PndMdt::BeginEvent() {}
// -------------------------------------------------------------------------

// -----   Public method ProcessHits  --------------------------------------
Bool_t PndMdt::ProcessHits(FairVolume *vol)
{
  TString name = gMC->CurrentVolOffName(1);
  if (name.Contains("BA") || name.Contains("Cell"))
    ProcessHitsRoot(vol);
  else
    ProcessHitsFast(vol);

  // if (gMC->IsTrackEntering() || gMC->IsNewTrack() )
  return kTRUE;
}

// -----   Public method ProcessHitsFast  --------------------------------------
Bool_t PndMdt::ProcessHitsFast(FairVolume *vol)
{
  if (gMC->TrackCharge() == 0)
    return kTRUE; // skip neutrals

  TString name = vol->GetName();

  if (gMC->IsTrackEntering() || gMC->IsNewTrack()) {
    fPos_In.SetXYZM(0., 0., 0., 0.);
    fMom_In.SetXYZM(0., 0., 0., 0.);
    gMC->TrackPosition(fPos_In);
    gMC->TrackMomentum(fMom_In);
    fTrkIn = gMC->GetStack()->GetCurrentTrackNumber();
    fTime = gMC->TrackTime() * 1e9;
  }; // end entering

  fELoss += gMC->Edep();

  if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {
    Int_t TrNo = gMC->GetStack()->GetCurrentTrackNumber();
    // Int_t pdg= gMC->TrackPid(); //[R.K. 01/2017] unused variable?
    if ((TrNo == fTrkIn) && (fELoss > 0.)) {
      TLorentzVector lPos, lMom;
      Int_t iMod;
      Int_t iOct;
      Int_t iLayer;
      Int_t iBox;
      Int_t iWire;
      sscanf(name, "MDT%is%il%ib%iw%i", &iMod, &iOct, &iLayer, &iBox, &iWire);

      Int_t detectorId = PndMdtID::Identifier(iMod, iOct, iLayer, iBox, iWire);
      gMC->TrackPosition(lPos); // cm
      gMC->TrackMomentum(lMom); // GeV
      fPointVector->push_back(PndMdtPoint(TrNo, detectorId, lPos.Vect(), lMom.Vect(), fTime, gMC->TrackLength(), fELoss, fPos_In.Vect(), fMom_In.Vect()));
      
      /**if you add a point then tell the stack! here*/
      PndStack *stack = (PndStack *)gMC->GetStack();
      stack->AddPoint(PndDetectorId::kMDT);
    };

    ResetParameters();
  };

  return kTRUE;
}

// -----   Public method ProcessHitsRoot  --------------------------------------
Bool_t PndMdt::ProcessHitsRoot(FairVolume *) // vol //[R.K.03/2017] unused variable(s)
{
  if (gMC->TrackCharge() == 0)
    return kTRUE; // skip neutrals

  TString name = gMC->CurrentVolName();
  TString path = gMC->CurrentVolPath();
  if (fVerboseLevel)
    std::cout << "Path: " << path << std::endl;

  if (gMC->IsTrackEntering() || gMC->IsNewTrack()) {
    fELoss = 0.;
    gMC->TrackPosition(fPos_In);
    gMC->TrackMomentum(fMom_In);
    fTrkIn = gMC->GetStack()->GetCurrentTrackNumber();
    fTime = gMC->TrackTime() * 1e9;
  }; // end entering

  fELoss += gMC->Edep();

  if (fVerboseLevel)
    cout << "pdg: " << gMC->TrackPid() << "\teloss: " << fELoss << ", name" << gMC->ParticleName(gMC->TrackPid()) << endl;

  if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {
    Int_t TrNo = gMC->GetStack()->GetCurrentTrackNumber();
    // Int_t pdg= gMC->TrackPid(); //[R.K. 01/2017] unused variable?
    if ((TrNo == fTrkIn) && (fELoss > 0.)) {
      TLorentzVector lPos, lMom;
      Int_t iMod = -1;
      Int_t iOct;
      Int_t iLayer;
      Int_t iBox;
      Int_t iWire;
      gMC->CurrentVolID(iWire);
      gMC->CurrentVolOffID(2, iBox);
      gMC->CurrentVolOffID(3, iLayer);
      gMC->CurrentVolOffID(4, iOct);
      if (path.Contains("Barrel"))
        iMod = 1;
      if (path.Contains("Endcap"))
        iMod = 2;
      if (path.Contains("MF"))
        iMod = 3;
      if (path.Contains("Forward"))
        iMod = 4;

      if (fVerboseLevel)
        cout << iMod << "\t" << iOct << "\t" << iLayer << "\t" << iBox << "\t" << iWire;
      Int_t detectorId = PndMdtID::Identifier(iMod, iOct, iLayer, iBox, iWire);

      if (fVerboseLevel)
        cout << "\t" << detectorId << endl;
      gMC->TrackPosition(lPos); // cm
      gMC->TrackMomentum(lMom); // GeV
      fPointVector->push_back(PndMdtPoint(TrNo, detectorId, lPos.Vect(), lMom.Vect(), fTime, gMC->TrackLength(), fELoss, fPos_In.Vect(), fMom_In.Vect()));
      
      /**if you add a point then tell the stack! here*/
      PndStack *stack = (PndStack *)gMC->GetStack();
      stack->AddPoint(PndDetectorId::kMDT);
    };

    ResetParameters();
  };

  return kTRUE;
}

// -------------------------------------------------------------------------
Bool_t PndMdt::CheckIfSensitive(std::string name)
{
  if ((name.find("MDT") != std::string::npos) || name.find("GasCell") != std::string::npos) {
    return kTRUE;
  }

  return kFALSE;
}

// ----------------------------------------------------------------------------

TClonesArray *PndMdt::GetCollection(Int_t iColl) const
{

    return nullptr;
}
Int_t PndMdt::PdgToIndex(Int_t pdg)
{
  Int_t abspdg = abs(pdg);
  if (abspdg == 11)
    return 0;
  else if (abspdg == 211)
    return 2;
  else if (abspdg == 13)
    return 1;
  else if (abspdg == 2212)
    return 4;
  else if (abspdg == 321)
    return 3;
  else
    return 1;
}

// -----   Public method EndOfEvent   -----------------------------------------
void PndMdt::EndOfEvent()
{
  if (fVerboseLevel)
    Print();
  Reset();
}
// ----------------------------------------------------------------------------

Bool_t PndMdt::IsSensitive(const std::string& name)
{
  if ((name.find("MDT") != std::string::npos) || name.find("GasCell") != std::string::npos) {
    return true;
  }
  return false;
}


FairModule* PndMdt::CloneModule() const
{
    return new PndMdt(*this);
}

void PndMdt::Register()
 {   FairRootManager::Instance()->RegisterAny("MdtPoint", fPointVector, kTRUE); }

ClassImp(PndMdt)
