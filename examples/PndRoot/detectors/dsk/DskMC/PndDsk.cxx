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

// ----------------------------------------------------------------------------
// -----                        PndDsk source file                        -----
// -----                   Created 23/10/07 by P. Koch                    -----
// ----------------------------------------------------------------------------

#include <iostream>
#include <math.h>
using std::cout;
using std::endl;

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"
#include "TFile.h"

#include "TGeoCone.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"
#include "TObject.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"
#include "FairLogger.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "FairGeoRootBuilder.h"

#include "PndDetectorList.h"
#include "PndStack.h"

#include "PndDsk.h"
#include "PndDskCerenkov.h"
#include "PndDskParticle.h"
#include "PndDskTrackPoint.h"

// -----   Default constructor   ----------------------------------------------
PndDsk::PndDsk()
  : fDebugLevel(0), fStoreCerenkovs(kTRUE), fStoreParticles(kTRUE), fStoreTrackPoints(kFALSE), fStoreFLGHits(kFALSE), fCalcPWay(kFALSE), fMeasureTotalRefAngle(kFALSE), fPDE(1.)
{
  fDskCerenkovCollection = new TClonesArray("PndDskCerenkov");
  fDskParticleCollection = new TClonesArray("PndDskParticle");
  fDskTrackPointCollection = new TClonesArray("PndDskTrackPoint");
  fDskFLGHitArray = new TClonesArray("PndDskFLGHit");
  fGeo = new PndGeoDskFLG();
}
// ----------------------------------------------------------------------------

// -----   Standard constructor   ---------------------------------------------
PndDsk::PndDsk(const char *name, Bool_t active)
  : FairDetector(name, active), fDebugLevel(0), fStoreCerenkovs(kTRUE), fStoreParticles(kTRUE), fStoreTrackPoints(kFALSE), fStoreFLGHits(kFALSE), fCalcPWay(kFALSE),
    fMeasureTotalRefAngle(kFALSE), fPDE(1.)
{
  fDskCerenkovCollection = new TClonesArray("PndDskCerenkov");
  fDskParticleCollection = new TClonesArray("PndDskParticle");
  fDskTrackPointCollection = new TClonesArray("PndDskTrackPoint");
  fDskFLGHitArray = new TClonesArray("PndDskFLGHit");
  fGeo = new PndGeoDskFLG();
}
// ----------------------------------------------------------------------------

PndDsk::PndDsk(const PndDsk& rhs)
    : FairDetector(rhs), fDebugLevel(0), fStoreCerenkovs(kTRUE), fStoreParticles(kTRUE), fStoreTrackPoints(kFALSE), fStoreFLGHits(kFALSE), fCalcPWay(kFALSE),
    fMeasureTotalRefAngle(kFALSE), fPDE(1.)
{
  fDskCerenkovCollection = new TClonesArray("PndDskCerenkov");
  fDskParticleCollection = new TClonesArray("PndDskParticle");
  fDskTrackPointCollection = new TClonesArray("PndDskTrackPoint");
  fDskFLGHitArray = new TClonesArray("PndDskFLGHit");
  fGeo = new PndGeoDskFLG();
}

// -----   Destructor   -------------------------------------------------------
PndDsk::~PndDsk()
{
  if (nullptr != fDskCerenkovCollection) {
    fDskCerenkovCollection->Delete();
    delete fDskCerenkovCollection;
  }
  if (nullptr != fDskParticleCollection) {
    fDskParticleCollection->Delete();
    delete fDskParticleCollection;
  }
  if (nullptr != fDskTrackPointCollection) {
    fDskTrackPointCollection->Delete();
    delete fDskTrackPointCollection;
  }
  if (nullptr != fDskFLGHitArray) {
    fDskFLGHitArray->Delete();
    delete fDskFLGHitArray;
  }
  if (fGeo)
    delete fGeo;
}
// ----------------------------------------------------------------------------

// -----   Public method Intialize   ------------------------------------------
void PndDsk::Initialize()
{
  FairDetector::Initialize();
  // FairRun       *sim  = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb *rtdb = sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?
  SetTrapFraction("$VMCWORKDIR/input/trapfrac_disc.root");
}

void PndDsk::SetTrapFraction(std::string name)
{
  TFile *f = new TFile(name.c_str());

  for (int i = 0; i < 5; i++) {
    trapfrac[i] = 0;
  }

  if (f->IsZombie()) {
    cout << " -W-  (PndDsk::Initialize) - trapfrac_disc file "
         << " doesn't exist. Using constant trapping fraction _trap=" << 0.7 << endl;
  } else {
    trapfrac[0] = (TH2F *)f->Get("hacc0");
    trapfrac[1] = (TH2F *)f->Get("hacc1");
    trapfrac[2] = (TH2F *)f->Get("hacc2");
    trapfrac[3] = (TH2F *)f->Get("hacc3");
    trapfrac[4] = (TH2F *)f->Get("hacc4");

    for (int i = 0; i < 5; i++)
      trapfrac[i]->SetDirectory(0);

    f->Close();
  }
  delete f;
}
// ----------------------------------------------------------------------------

// -----   Public method ProcessHits  -----------------------------------------
Bool_t PndDsk::ProcessHits(FairVolume *vol)
{
  fPdgCode = gMC->TrackPid();
  if (fPdgCode == 50000050) {
    return ProcessHitsCerenkov_FLG(vol);
  } else {

    return ProcessHitsParticle(vol);
  }
}
// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndDsk::EndOfEvent()
{
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndDsk::Register()
{
    fDskCerenkovCollection->Clear();
    fDskParticleCollection->Clear();
    fDskTrackPointCollection->Clear();
    fDskFLGHitArray->Clear();

    FairRootManager::Instance()->RegisterAny("DskCerenkov", fCerVector, kTRUE);
    FairRootManager::Instance()->RegisterAny("DskParticle", fPartVector, kTRUE);
    FairRootManager::Instance()->RegisterAny("DskTrackPoints", fPointVector, kTRUE);
    FairRootManager::Instance()->RegisterAny("DskFLGHit", fHitVector, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray *PndDsk::GetCollection(Int_t iColl) const
{
  return nullptr;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndDsk::Print() const
{
  LOG(info) << " PndDsk::Print() was called, but is not yet implemented.";
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void PndDsk::Reset()
{
    fCerVector->clear();
    fPartVector->clear();
    fPointVector->clear();
    fHitVector->clear();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void PndDsk::CopyClones(TClonesArray *, TClonesArray *, Int_t) // cl1 cl2 offset //[R.K.03/2017] unused variable(s)
{
  LOG(info) << " PndDsk::CopyClones() was called, but is not yet implemented.";
}
// ----------------------------------------------------------------------------

// -----   Public method ConstructGeometry   ----------------------------------
void PndDsk::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if (fileName.EndsWith("_top.root")) {
    ConstructRootGeometry();
    //   } else if (fileName.EndsWith(".geo")) {
    //     ConstructASCIIGeometry();
  } else { // FLG Version

    FairGeoLoader *drcgeoLoad = FairGeoLoader::Instance();
    FairGeoInterface *drcgeoFace = drcgeoLoad->getGeoInterface();

    FairGeoMedia *Media = drcgeoFace->getMedia();
    FairGeoBuilder *geobuild = drcgeoLoad->getGeoBuilder();

    // Call materials
    FairGeoMedium *fusedSil = Media->getMedium("FusedSil");
    geobuild->createMedium(fusedSil);
    FairGeoMedium *nlak33a = Media->getMedium("NLAK33A");
    geobuild->createMedium(nlak33a);
    FairGeoMedium *air = Media->getMedium("DIRCair");
    geobuild->createMedium(air);
    FairGeoMedium *airNoSens = Media->getMedium("DIRCairNoSens");
    geobuild->createMedium(airNoSens);
    FairGeoMedium *mirror = Media->getMedium("Mirror");
    geobuild->createMedium(mirror);
    FairGeoMedium *marcol82 = Media->getMedium("Marcol82");
    geobuild->createMedium(marcol82);

    TGeoVolume *cave = gGeoManager->GetTopVolume();

    double rmax = fGeo->radius() / 10.;              // maximum radius  cm
    double rmin = 0;                                 // minimum radius  cm
    double thickness = fGeo->thickness() / 10.;      // thickness of plate  cm
    double z_position = fGeo->postion_plate() / 10.; // position of plate  cm

    // window
    Double_t const fWindowHeightHalf = z_position * TMath::Tan(5. * TMath::DegToRad()); // ~17.15 cm
    Double_t const fWindowWidthHalf = z_position * TMath::Tan(10. * TMath::DegToRad()); // ~34.56 cm

    new TGeoBBox("DW", fWindowWidthHalf, fWindowHeightHalf, thickness);

    TGeoCone *baseVol = new TGeoCone("baseVol", thickness + 0.1, rmin, rmax + 0.1, rmin, rmax + 0.1);
    new TGeoCone("logicPlate", thickness, rmin, rmax, rmin, rmax);
    TGeoCompositeShape *logicPlate_DW = new TGeoCompositeShape("logicPlate - DW");

    TGeoVolume *dskVol = new TGeoVolume("DskBase", baseVol, gGeoManager->GetMedium("DIRCairNoSens"));
    TGeoVolume *plateVol = new TGeoVolume("DskPlate", logicPlate_DW, gGeoManager->GetMedium("FusedSil"));

    cave->AddNode(dskVol, 1, new TGeoCombiTrans(0, 0, z_position, new TGeoRotation(0)));
    dskVol->AddNode(plateVol, 1, new TGeoCombiTrans(0, 0, 0, new TGeoRotation(0)));
    LOG(info) << "PndDsk::AddSensitive " << plateVol->GetName();
    AddSensitiveVolume(plateVol);
  }
}
// ----------------------------------------------------------------------------

// -----   Public method CheckIfSensitive   -----------------------------------
Bool_t PndDsk::CheckIfSensitive(std::string name) // name //[R.K.03/2017] unused variable(s)
{
    LOG(info) << "PndDsk::CheckIfSensitive(" << name << ")";
  return kTRUE;
  //   if (name.compare("radiator") == 0) { // "radiator"
  //     return kTRUE;
  //   }
  //   if (name.find("mcp") == 0) { // "mcp*"
  //     return kTRUE;
  //   }
  //   if (name.find("mirror") == 0) { // "mirror*"
  //     return kTRUE;
  //   }
}
// ----------------------------------------------------------------------------

// -----   Add Hit to HitCollection   --------------------------------------
void PndDsk::AddHit(Int_t trackID, Int_t detectorID, TVector3 position_store, TVector3 momentum_store, Double_t time, Double_t angIn, Double_t thetaC_store,
                             TVector3 Cherenkov_photon, Int_t light_guide, Int_t pixel)
{
    fHitVector->push_back(PndDskFLGHit(trackID, detectorID, position_store, momentum_store, time, angIn, thetaC_store, Cherenkov_photon, light_guide, pixel));
}

// -----   Public method AddCerenkov   ----------------------------------------
void PndDsk::AddCerenkov(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Double_t energy, Double_t wavelength, Int_t motherTrackID,
                                    Int_t motherPdgCode, TString motherPdgName)
{
  TClonesArray &clRef = *fDskCerenkovCollection;
  Int_t size = clRef.GetEntriesFast();

  auto temp = new (clRef[size]) PndDskCerenkov(trackID, detectorID, position, momentum, time, energy, wavelength, motherTrackID, motherPdgCode, motherPdgName);
  
    fCerVector->push_back(PndDskCerenkov(trackID, detectorID, position, momentum, time, energy, wavelength, motherTrackID, motherPdgCode, motherPdgName));
}
// ----------------------------------------------------------------------------

// -----   Public method AddParticle   ----------------------------------------
void PndDsk::AddParticle(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Int_t pdgCode, TString pdgName, Double_t energy,
                                    Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName, Double_t mass, Double_t angIn, Double_t thetaC, Int_t nPhot)
{
      TClonesArray &clRef = *fDskParticleCollection;
  Int_t size = clRef.GetEntriesFast();

  auto temp = new (clRef[size])
    PndDskParticle(trackID, detectorID, position, momentum, time, pdgCode, pdgName, energy, motherTrackID, motherPdgCode, motherPdgName, mass, angIn, thetaC, nPhot);

    fPartVector->push_back(PndDskParticle(trackID, detectorID, position, momentum, time, pdgCode, pdgName, energy, motherTrackID, motherPdgCode, motherPdgName, mass, angIn, thetaC, nPhot));
}
// ----------------------------------------------------------------------------

// -----   Public method AddTrackPoint   ----------------------------------------
void PndDsk::AddTrackPoint(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Double_t length, Double_t eLoss)
{
    fPointVector->push_back(PndDskTrackPoint(trackID, detectorID, position, momentum, time, length, eLoss));
}
// ----------------------------------------------------------------------------

// -----   Private method ProcessHitsCerenkov, focusing light guide version   -------------------------------
Bool_t PndDsk::ProcessHitsCerenkov_FLG(FairVolume *vol)
{
  // gather information
  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

  // search the photon in our collection
  Bool_t searching = kTRUE;
  PndDskCerenkov *pCerenkov = nullptr;
  TIter iter(fDskCerenkovCollection);
  while (searching && (pCerenkov = (PndDskCerenkov *)iter.Next())) {
    if (pCerenkov->GetTrackID() == fTrackID)
      searching = kFALSE;
  }

  gMC->TrackPosition(tmpLVec);
  fPosition = tmpLVec.Vect();
  gMC->TrackMomentum(tmpLVec);
  fMomentum = tmpLVec.Vect() * 1.e9;

  // if fStoreTrackPoints is set gather all necessary data now
  if (fStoreTrackPoints) {
    fDetectorID = vol->getMCid();
    gMC->TrackPosition(tmpLVec);
    fPosition = tmpLVec.Vect();      // in [cm]
    fTime = gMC->TrackTime() * 1.e9; // in [ns], global time
    gMC->TrackMomentum(tmpLVec);
    fMomentum = tmpLVec.Vect() * 1.e9; // in [eV]
    fLength = gMC->TrackLength();      // in [cm]
    fELoss = gMC->Edep();              // in [GeV]
    AddTrackPoint(fTrackID, fDetectorID, fPosition, fMomentum, fTime, fLength, fELoss);
  }

  // if it is a new photon
  if (searching) {
    // gather informations, if not already done
    if (!fStoreTrackPoints) {
      fDetectorID = vol->getMCid();
      gMC->TrackPosition(tmpLVec);
      fPosition = tmpLVec.Vect();      // in [cm]
      fTime = gMC->TrackTime() * 1.e9; // in [ns], global time
      gMC->TrackMomentum(tmpLVec);
      fMomentum = tmpLVec.Vect() * 1.e9; // in [eV]
    }
    fEnergy = tmpLVec.E() * 1.e9;          // in [eV], photon energy
    fWavelength = 1.239841874e3 / fEnergy; // hc = 4.13566733e-15[eVs] * 299792458e+09[nm/s]

    fMotherTrackID = gMC->GetStack()->GetCurrentTrack()->GetFirstMother();
    if (fMotherTrackID > -1) {
      TParticle *mother = ((PndStack *)(gMC->GetStack()))->GetParticle(fMotherTrackID);
      fMotherPdgCode = mother->GetPdgCode();
      fMotherPdgName = mother->GetName();
    } else {
      fMotherPdgCode = 0;
      fMotherPdgName = "unknown";
    }

    PndStack *stack = (PndStack *)gMC->GetStack();
    stack->AddPoint(PndDetectorId::kDSK);

    // after we collected all information that is available on its creation
    // we will add it to the collection
    AddCerenkov(
      // data of first appearance (FairMCPoint)
      fTrackID, fDetectorID, fPosition, fMomentum, fTime,
      // (PndDskCerenkov)
      fEnergy, fWavelength,
      // data of mother
      fMotherTrackID, fMotherPdgCode, fMotherPdgName);

    // if the cerenkov doesnt meet our criteria, add it to collection (already done)
    // but stopp the track imediately. no need to track it further.

    if (DoNotTrackCerenkov()) {
      gMC->StopTrack();
      return kTRUE;
    }

    // is Cerenkov is in our collection
  } else {
    gMC->TrackPosition(tmpLVec);
    TVector3 pos = tmpLVec.Vect();
    gMC->TrackMomentum(tmpLVec);
    TVector3 dir = tmpLVec.Vect();

    // if(pos.Perp()>0.9*fGeo->radius()&&dir.Z()>0) {
    if (dir.Z() > 0) { // deal with the cherenkov photon myself
      Int_t i_FLG = -99, i_Pixel = -99;
      // cout<<"pos: "<<pos.X()<<" "<<pos.Y()<<" "<<pos.Z()<<"  dir: "<<dir.X()<<" "<<dir.Y()<<" "<<dir.Z()<<"  phi: "<<dir.Phi()<<endl;

      if (dir.Theta() > fGeo->reflect_threshold())
        fGeo->Propagate(pos, dir, i_FLG, i_Pixel);

      double effi = 0.1;

      if (i_FLG != -99 && gRandom->Rndm() < effi) {
        TVector3 Cherenkov_photon(0, 0, 1);
        double angIn = 0, thetaC_store = 0;
        AddHit(fTrackID, fDetectorID, fPosition, fMomentum, fTime, angIn, thetaC_store, Cherenkov_photon, i_FLG, i_Pixel);
      }
      gMC->StopTrack();
    }
  }
  return kTRUE;
}

// -----   Private method ProcessHitsCerenkov   -------------------------------
Bool_t PndDsk::ProcessHitsCerenkov(FairVolume *vol)
{

  /// Debug Tracks stepps
  // fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
  // cout << "New Step @ " << gMC->TrackTime() * 1.e9 << endl;
  // if (gMC->IsNewTrack()) {
  //   cout << "Track " << fTrackID << " is new,      called from volume: " << vol->GetName() << endl;
  // }
  // if (gMC->IsTrackOut()) {
  //   cout << "Track " << fTrackID << " is out,      called from volume: " << vol->GetName() << endl;
  // }
  // if (gMC->IsTrackDisappeared()) {
  //   cout << "Track " << fTrackID << " disappeared, called from volume: " << vol->GetName() << endl;
  // }
  // if (gMC->IsTrackStop()) {
  //   cout << "Track " << fTrackID << " stopped,     called from volume: " << vol->GetName() << endl;
  // }
  // if (! gMC->IsTrackInside()) {
  //   cout << "Track " << fTrackID << " on boundary, called from volume: " << vol->GetName() << endl;
  // }
  // if (gMC->IsTrackInside()) {
  //   cout << "Track " << fTrackID << " is inside,   called from volume: " << vol->GetName() << endl;
  // }
  // if (gMC->IsTrackExiting()) {
  //   cout << "Track " << fTrackID << " is exiting,  called from volume: " << vol->GetName() << endl;
  // }
  // if (gMC->IsTrackEntering()) {
  //   cout << "Track " << fTrackID << " is entering, called from volume: " << vol->GetName() << endl;
  // }

  // gather information
  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

  // search the photon in our collection
  Bool_t searching = kTRUE;
  PndDskCerenkov *pCerenkov = nullptr;
  TIter iter(fDskCerenkovCollection);
  while (searching && (pCerenkov = (PndDskCerenkov *)iter.Next())) {
    if (pCerenkov->GetTrackID() == fTrackID)
      searching = kFALSE;
  }

  // cout<<"trackID: "<<fTrackID<<endl;
  gMC->TrackPosition(tmpLVec);
  fPosition = tmpLVec.Vect();
  // cout<<"position: "<<fPosition.X()<<" "<<fPosition.Y()<<" "<<fPosition.Z()<<endl;
  gMC->TrackMomentum(tmpLVec);
  fMomentum = tmpLVec.Vect() * 1.e9;
  // cout<<"momentum: "<<fMomentum.X()<<" "<<fMomentum.Y()<<" "<<fMomentum.Z()<<endl;

  // if fStoreTrackPoints is set gather all necessary data now
  if (fStoreTrackPoints) {
    fDetectorID = vol->getMCid();
    gMC->TrackPosition(tmpLVec);
    fPosition = tmpLVec.Vect();      // in [cm]
    fTime = gMC->TrackTime() * 1.e9; // in [ns], global time
    gMC->TrackMomentum(tmpLVec);
    fMomentum = tmpLVec.Vect() * 1.e9; // in [eV]
    fLength = gMC->TrackLength();      // in [cm]
    fELoss = gMC->Edep();              // in [GeV]
    AddTrackPoint(fTrackID, fDetectorID, fPosition, fMomentum, fTime, fLength, fELoss);
  }

  // if it is a new photon
  if (searching) {
    // gather informations, if not already done
    if (!fStoreTrackPoints) {
      fDetectorID = vol->getMCid();
      gMC->TrackPosition(tmpLVec);
      fPosition = tmpLVec.Vect();      // in [cm]
      fTime = gMC->TrackTime() * 1.e9; // in [ns], global time
      gMC->TrackMomentum(tmpLVec);
      fMomentum = tmpLVec.Vect() * 1.e9; // in [eV]
    }
    fEnergy = tmpLVec.E() * 1.e9;          // in [eV], photon energy
    fWavelength = 1.239841874e3 / fEnergy; // hc = 4.13566733e-15[eVs] * 299792458e+09[nm/s]

    fMotherTrackID = gMC->GetStack()->GetCurrentTrack()->GetFirstMother();
    if (fMotherTrackID > -1) {
      TParticle *mother = ((PndStack *)(gMC->GetStack()))->GetParticle(fMotherTrackID);
      fMotherPdgCode = mother->GetPdgCode();
      fMotherPdgName = mother->GetName();
    } else {
      fMotherPdgCode = 0;
      fMotherPdgName = "unknown";
    }

    PndStack *stack = (PndStack *)gMC->GetStack();
    stack->AddPoint(PndDetectorId::kDSK);

    // after we collected all information that is available on its creation
    // we will add it to the collection
    AddCerenkov(
      // data of first appearance (FairMCPoint)
      fTrackID, fDetectorID, fPosition, fMomentum, fTime,
      // (PndDskCerenkov)
      fEnergy, fWavelength,
      // data of mother
      fMotherTrackID, fMotherPdgCode, fMotherPdgName);

    // if the cerenkov doesnt meet our criteria, add it to collection (already done)
    // but stopp the track imediately. no need to track it further.

    if (DoNotTrackCerenkov()) {
      gMC->StopTrack();
      return kTRUE;
    }

    // is Cerenkov is in our collection
  } else {
    // gather informations, if not already done
    if (!fStoreTrackPoints) {
      gMC->TrackPosition(tmpLVec);
      fPosition = tmpLVec.Vect(); // in [cm]
    }

    // add projected way
    if (fCalcPWay) {
      pCerenkov->AddPWay(fPosition);
    }
    // and numer of reflections if IsTrackEntering() in the radiator
    /// @todo needs to take glue, mirrors and diffrent media into account!
    TString volName = vol->GetName();
    if ((gMC->IsTrackEntering()) && (volName.BeginsWith("radiator"))) {
      pCerenkov->AddReflection();

      // get simulated total reflection angle
      if (fMeasureTotalRefAngle) {
        if (pCerenkov->GetNofReflections() == 5) {
          pCerenkov->Set5RefPosition(fPosition);
          /// calculate total reflection angle
          gMC->TrackMomentum(tmpLVec);
          fMomentum = tmpLVec.Vect();
          pCerenkov->SetTotalRefAngle(TMath::PiOver2() - TVector3(0., 0., -1.).Angle(fMomentum));
        }
      }
    }

    // the dichroic mirrors need to be simulated inhere as it cant be done
    // with material definitions.
    // unfortunately there is no way to tell a Cerenkov to be reflected or not.
    // At the moment, ever Cerenkov is reflected (ideal mirror) if its not stopped
    // in here (already in the mirror, not the mcp ...).
    // So the following code will give a simple MC, but wont work for a more
    // more complicated setup behind the mirrors!

    // this is what it should be:
    //     // if cerenkov reaches mcp: detect it
    //     if ((vol->getName()).BeginsWith("mcp")) {
    // and this is what is is at the moment:
    if (volName.BeginsWith("mirror")) {

      //       sscanf((vol->getName()).Data(),"mcp%hu",&fDetType);
      sscanf(volName.Data(), "mirror%hu", &fDetType);

      // decide if photon is detected
      if (DichroicMirrorTransmitted(pCerenkov->GetWavelength(), fDetType)) {

        // detector number. They are counted as the createRootGeoFile script created them.
        // we need the copy number of the mother of the mcp, thats the copy number of the detector.
        gMC->CurrentVolOffID(1, fDetNumber); // detector

        // other data
        /// @todo data probably already collected! needs improvement
        fDetTime = gMC->TrackTime() * 1.0e09; // in [ns]
        gMC->TrackMomentum(tmpLVec);
        fDetMomentum = tmpLVec.Vect() * 1.e9; // in [eV]
        // calculations for resolution study
        // this angle is between 0deg and 180deg!
        TVector3 tmpMom;
        pCerenkov->Momentum(tmpMom);
        fPrimaryAngleToCerenkov = TVector3(fPrimaryHitMomentum.Px(), fPrimaryHitMomentum.Py(), 0.).Angle(TVector3(tmpMom.Px(), tmpMom.Py(), 0.));
        // and keep the track length
        fLength = gMC->TrackLength(); // [cm]
        // now store our values
        pCerenkov->SetFinalValues(fDetNumber, fDetType, fDetTime, fDetMomentum, fLength, fPrimaryHitAngle, fPrimaryAngleToCerenkov);
        // and finally: stopp the track as it got absorbed in mcp
        gMC->StopTrack();
      }
    }
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------

// -----   Private method DichroicMirrorTransmitted   -------------------------
Bool_t PndDsk::DichroicMirrorTransmitted(Double_t wl, Int_t det)
{
  // this is a VERY simple implementation, but better a lot better than
  // using media definitions
  if (((det == 0) && ((wl >= 400.) && (wl <= 500.))) || ((det == 1) && ((wl > 500.) && (wl <= 700.)))) {
    return kTRUE;
  } else {
    return kFALSE;
  }
}
// ----------------------------------------------------------------------------

// -----   Private method ProcessHitsParticle   -------------------------------
Bool_t PndDsk::ProcessHitsParticle(FairVolume *vol)
{

  // it is entering any sensitive volume
  if (gMC->TrackCharge() != 0. && gMC->IsTrackEntering()) {

    // we need its TrackID
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

    // find it in our collection
    Bool_t searching = true;
    TIter iter(fDskParticleCollection);
    PndDskParticle *pParticle;
    while (searching && (pParticle = (PndDskParticle *)iter.Next())) {
      if (pParticle->GetTrackID() == fTrackID)
        searching = false;
    }

    // this track is not yet in out collection
    if (searching) {

      PndStack *stack = (PndStack *)gMC->GetStack();
      stack->AddPoint(PndDetectorId::kDSK);

      // this particle is a new one, so store its values, as we want to know which particle
      // created the Cerenkovs.
      // other data of CmbMCPoint. Position and Time is the point of first appearance
      fDetectorID = vol->getMCid();
      gMC->TrackPosition(tmpLVec);
      fPosition = tmpLVec.Vect();      // in [cm]
      fTime = gMC->TrackTime() * 1.e9; // in [ns], global time
      gMC->TrackMomentum(tmpLVec);
      fMomentum = tmpLVec.Vect(); // in [GeV]
      fEnergy = tmpLVec.E();      // in [GeV], particle energy
      // PDG code was already stored, but we want the name too
      fPdgName = ((PndStack *)(gMC->GetStack()))->GetCurrentTrack()->GetName();
      // gather needed information about mother
      fMotherTrackID = gMC->GetStack()->GetCurrentTrack()->GetFirstMother();
      if (fMotherTrackID > -1) {
        TParticle *mother = ((PndStack *)(gMC->GetStack()))->GetParticle(fMotherTrackID);
        fMotherPdgCode = mother->GetPdgCode();
        fMotherPdgName = mother->GetName();
      } else {
        fMotherPdgCode = 0;
        fMotherPdgName = "unknown";
      }

      ///----------------------------------------calc thetaC
      Double_t Px = fMomentum.Px();
      Double_t Py = fMomentum.Py();
      Double_t Pz = fMomentum.Pz();
      Double_t fP = sqrt(Px * Px + Py * Py + Pz * Pz);
      Double_t fMass = gMC->TrackMass();

      // Double_t fAngIn;
      if (fabs(Pz / fP) > 1. || fP == 0.) {
        fAngIn = -1.;
      } else {
        fAngIn = acos(Pz / fP);
      }
      // Double_t fThetaC;
      if (fabs(1. / (1.47 * (fP / fEnergy))) > 1. || fP == 0. || fEnergy == 0.) {
        fThetaC = -1.;
      } else {
        fThetaC = acos(1 / (1.47 * (fP / fEnergy)));
      }

      // calc number of produced photons
      double lambda1 = 280e-9; // range of wavelength, which is seen by the PMT/PD: copied from fsim
      double lambda2 = 330e-9;
      double alpha = 7.2974e-3; // finestructure constant
      double thickness = 20e-3; // disc thickness, 20mm
      double l = fabs(thickness / cos(fMomentum.Theta()));
      double effNphotons = 0.2;
      double nPhotMin = 5;
      // deteremine trapping fraction
      double trapped = 0.7; // use constant now...
      // estimate the number of initially produced cherenkov photons
      double nPhot = 0, res = 0;
      double thtdeg = fMomentum.Theta() * 180 / TMath::Pi();
      int npid = -1;
      if (fPdgCode == 11)
        npid = 0;
      else if (fPdgCode == 13)
        npid = 1;
      else if (fPdgCode == 211)
        npid = 2;
      else if (fPdgCode == 321)
        npid = 3;
      else if (fPdgCode == 2212)
        npid = 4;

      if (npid >= 0 && trapfrac[npid])
        trapped = npid < 0 ? 0.0 : trapfrac[npid]->GetBinContent(trapfrac[npid]->FindBin(fP < 6.0 ? fP : 6.0, thtdeg));

      if (fP != 0) {
        nPhot = 2 * TMath::Pi() * alpha * l * (1. / lambda1 - 1. / lambda2) * (1 - (fEnergy * fEnergy) / (fP * fP * 1.47 * 1.47));
        nPhot = gRandom->Poisson(nPhot);
        nPhot *= trapped * effNphotons;
        if (nPhot <= nPhotMin) {
          // cout<<"too few photons detected..."<<endl;
        }
        if (nPhot > 100)
          nPhot = 100;

        if (nPhot > 0)
          res = 0.01 / sqrt(nPhot);
      }

      // cout<<"dsk dirc.... fMass: "<<fMass<<" fP: "<<fP<<"  fEnergy: "<<fEnergy<<"  fThetaC: "<<fThetaC<<"  fPdgCode: "<<fPdgCode<<endl;
      // cout<<"position: "<<fPosition.Px()<<" "<<fPosition.Py()<<" "<<fPosition.Pz()<<endl;
      // cout<<"momentum: "<<Px<<" "<<Py<<" "<<Pz<<endl;
      fThetaC = gRandom->Gaus(fThetaC, res);
      //    fErrThetaC = 0.; //rad

      // if this is the primary particle (TrackID == 0), save its momentum for further use
      // these values need to be initialised in constructor, cause we need to know if the primary
      // ever reaches the disk:
      // if fPrimaryHitAngle is 0, the primary never reached the disk, as a value of 0 is impossible
      // for the disk (would be along the beampipe)
      // As the Cerenkovs will use the fPrimaryHitMomentum the primary needs to call ProcessHits
      // before the first Cerenkov does, but i assume thats this will always be the case.
      if (fTrackID == 0) {
        fPrimaryHitMomentum = fMomentum;
        // angle between momentum and z-axis
        fPrimaryHitAngle = fMomentum.Angle(TVector3(0., 0., 1.));
      }

      // now add particle
      AddParticle(
        // data of first appearance (FairMCPoint)
        fTrackID, fDetectorID, fPosition, fMomentum, fTime,
        // (PndDskCerenkov)
        fPdgCode, fPdgName, fEnergy,
        // data of mother
        fMotherTrackID, fMotherPdgCode, fMotherPdgName, fMass, fAngIn, fThetaC, int(nPhot));
    }
  }

  // The other case we are interested in is when the particles leaves the disk or is stopped in some way
  if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {

    // we need its TrackID
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

    // find it in our collection
    Bool_t searching = true;
    TIter iter(fDskParticleCollection);
    PndDskParticle *pParticle;
    while (searching && (pParticle = (PndDskParticle *)iter.Next())) {
      if (pParticle->GetTrackID() == fTrackID)
        searching = false;
    }

    if (searching) {
      // cout << "-W-  PndDskDetector::ProcessHitsParticle: Particle with trackID = " << fTrackID
      //      << "disappeared without being in the collection! Something is badly wrong here." << endl;
      // we found it
    } else {
      gMC->TrackPosition(tmpLVec);
      fEndPosition = tmpLVec.Vect(); // in [cm]
      fEndTime = tmpLVec.T() * 1.e9; // in [ns]
      gMC->TrackMomentum(tmpLVec);
      fEndMomentum = tmpLVec.Vect(); // in [GeV]
      fEndEnergy = tmpLVec.E();      // in [GeV]
      pParticle->SetFinalValues(fEndPosition, fEndMomentum, fEndTime, fEndEnergy);
    }
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------

// -----   Private method TrackCerenkov   -----------------------------------
Bool_t PndDsk::DoNotTrackCerenkov()
{
  // available data:
  // fDetectorID
  // fPosition         in [cm]
  // fTime             in [ns], global time
  // fMomentum         in [eV]
  // fEnergy           in [eV]
  // fWavelength       in [nm]

  // in our interval?
  if ((fWavelength < 400.) || (fWavelength > 700.)) {
    return kTRUE;
  }

  // detect every Cerenkov
  if (1. == fPDE) {
    return kFALSE;
  }

  Double_t pdeValue = 0.; // FIXME is 0 a sensible initialisation value?

  // if equal to 2, we will use formula: fit for BINP_MCP, done by Avetik
  if (2. == fPDE) {
    pdeValue = (fWavelength *
                  (fWavelength * (fWavelength * (fWavelength * (fWavelength * (fWavelength * (0.24980E-13) - 0.10697E-09) + 0.18451E-06) - 0.16338E-03) + 0.77842E-01) - 18.806) +
                1806.4) *
               0.004;
    // 0.004:  pdeValue /= 100, cause it returns percent;
    //                  *= 0.4, cause faktor of 40% given by MCP
  }

  // apply quantum efficiency
  if (gRandom->Uniform() > pdeValue) {
    // the Cerenkov wont be detected. So no need to track it.
    return kTRUE;
  }

  // in general we detect
  return kFALSE;
}
// ----------------------------------------------------------------------------


Bool_t PndDsk::IsSensitive(const std::string& name)
{
    if (name.find("DskPlate") != std::string::npos)
        return true;
    return false;
}


FairModule* PndDsk::CloneModule() const
{
    return new PndDsk(*this);
}

ClassImp(PndDsk)
