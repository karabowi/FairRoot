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
//
//  CbmHypGe
//
//  Filler of CbmHypGePoint
//
//  By A.Sanchez
//
///////////////////////////////////////////////////////////////

#include <iostream>

#include "PndHypGe.h"
#include "PndHypGePoint.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoMedia.h"
#include "FairGeoNode.h"
#include "PndGeoHypGe.h"
#include "FairGeoRootBuilder.h"
#include "PndStack.h"
#include "FairGeoMedium.h"
#include "FairRootManager.h"
#include "FairModule.h"
#include "FairVolume.h"
#include "GeCluster.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoSphere.h"
#include "TGeoTube.h"
#include "TGeoPgon.h"
#include "TGeoCompositeShape.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"

#include "TString.h"

// -----   Default constructor   -------------------------------------------
PndHypGe::PndHypGe()
{
  fHypGeCollection = new TClonesArray("PndHypGePoint");
  fHypGeAlCollection = new TClonesArray("PndHypGePoint");

  fPosIndex = 0;
  fEventID = -1;
  fdist = -78.;

  fListOfSensitives.push_back("Crystal"); // Root_Test.root
  fListOfSensitives.push_back("Capsule"); // Root_Test.root
  // fListOfSensitives.push_back("DeckelVol");//Root_Test.root
  // fListOfSensitives.push_back("Kappe");//Root_Test.root
  fListOfSensitives.push_back("Cryostat");
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndHypGe::PndHypGe(const char *name, Bool_t active) : FairDetector(name, active)
{
  fHypGeCollection = new TClonesArray("PndHypGePoint");
  fHypGeAlCollection = new TClonesArray("PndHypGePoint");

  fPosIndex = 0;
  fEventID = -1;
  fdist = -78.;

  fListOfSensitives.push_back("Crystal"); // Root_Test.root
  fListOfSensitives.push_back("Capsule"); // Root_Test.root
  // fListOfSensitives.push_back("DeckelVol");//Root_Test.root
  // fListOfSensitives.push_back("Kappe");//Root_Test.root
  fListOfSensitives.push_back("Cryostat");
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypGe::~PndHypGe()
{
  if (fHypGeCollection) {
    fHypGeCollection->Delete();
    delete fHypGeCollection;
  }
  if (fHypGeAlCollection) {
    fHypGeAlCollection->Delete();
    delete fHypGeAlCollection;
  }

  /*if (fHypGecapCollection) {
     fHypGecapCollection->Delete();
     delete fHypGecapCollection;
     }*/
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void PndHypGe::Initialize()
{
  // Init function

  FairDetector::Initialize();
  FairRun *sim = FairRun::Instance();
  FairRuntimeDb *rtdb = sim->GetRuntimeDb();
  // par=(PndGeoHypGePar*)(rtdb->getContainer("PndGeoHypGePar"));

  // TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
}

// -------------------------------------------------------------------------
void PndHypGe::BeginEvent()
{
  // Begin of the event
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t PndHypGe::ProcessHits(FairVolume *vol)
{
  Int_t pdgCode;
  Int_t copyNo = -1, id = -1;
  // TLorentzVector P;

  fcharge = gMC->TrackCharge();
  // TString nam = gMC->CurrentVolName();

  TString nam = gMC->CurrentVolName();
  if ((nam.BeginsWith("Crystal")))
  // cout << "Error <CbmEmc::ProcessHits> : " << nam << " not EMC volume" << endl;
  // else
  {
    if (gMC->IsTrackEntering()) {
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber(); // trk ID
      fEventID = gMC->CurrentEvent();

      fpdgCode = gMC->TrackPid();

      fELoss = 0.; // GeV
      fLength = gMC->TrackLength();
      fTime = gMC->TrackTime() * 1.0e09;
      gMC->TrackPosition(fPos); // cm
      gMC->TrackMomentum(fMom); // GeV
                                // if (fpdgCode==2112)cout<<" proton "<<fMom.E()<<endl;
    }
    // P=gMC->TrackMomentum(fMom);
    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // if(gMC->Edep()>0.5)cout<<"energy loss"<<fELoss<<" TID= "<<fTrackID
    //<<" pdg "<<fpdgCode<<" "<<fMom.E()<<endl;
    // if (fpdgCode==2112)cout<<" proton step "<<gMC->Edep()<<endl;

    // Set additional parameters at exit of active volume. Create CbmDrcPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {

      // if (fpdgCode==2112)cout<<" neutron out "<<fELoss<<endl;
      if ((fELoss == 0.) && (fpdgCode != 2112)) {
        // ResetParameters();
        return kFALSE;
      }

      // id = gMC->CurrentVolOffID(1,copyNo);
      // gMC->TrackPosition(fPos); // cm
      // gMC->TrackMomentum(fMom); // GeV

      // cout <<"energy loss"<<fMom.Px()<<endl;
      // sscanf(nam,"GeCrystal%dc", &copyNo);
      // fnCopy = copyNo;
      fnCopy = vol->getCopyNo();
      cout << " Vol Name: " << gMC->CurrentVolPath() << " vol id " << vol->getMCid() << " copyNo " << vol->getCopyNo() << endl;
      // if ( pdgCode )
      //{
      AddHit(fTrackID, fEventID, fpdgCode, fcharge, TVector3(fPos.X(), fPos.Y(), fPos.Z()), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, fELoss, fnCopy);
      //}

      ResetParameters();
    }
  }

  // if (nam.BeginsWith("Kappe")|| nam.BeginsWith("Capsule") || nam.BeginsWith("Deckel"))
  if (nam.BeginsWith("Cryostat") || nam.BeginsWith("Capsule"))
  // cout << "Error <CbmEmc::ProcessHits> : " << nam << " not EMC volume" << endl;
  // else
  { //  if ( gMC->IsTrackEntering() )
    if (gMC->IsTrackEntering()) {
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber(); // trk ID
      fEventID = gMC->CurrentEvent();

      fpdgCode = gMC->TrackPid();

      fELoss = gMC->Edep();
      fLength = gMC->TrackLength();
      fTime = gMC->TrackTime() * 1.0e09;
      gMC->TrackPosition(fPos); // cm
      gMC->TrackMomentum(fMom); // GeV
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();
    // cout <<"enrgy loss"<<fELoss<<endl;

    // Set additional parameters at exit of active volume. Create CbmDrcPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {
      if ((fELoss == 0.) && (fpdgCode != 2112)) {
        // ResetParameters();
        return kFALSE;
      }

      // id = gMC->CurrentVolOffID(1,copyNo);
      // gMC->TrackPosition(fPos); // cm
      // gMC->TrackMomentum(fMom); // GeV

      // cout <<"energy loss"<<fMom.Px()<<endl;
      // sscanf(nam,"laySci%04d", &copyNo);
      // fnCopy = copyNo;
      fnCopy = vol->getCopyNo();
      cout << " Vol Name: " << gMC->CurrentVolPath() << " vol id " << vol->getMCid() << " copyNo " << vol->getCopyNo() << endl;

      TVector3 pos(fPos.X(), fPos.Y(), fPos.Z());
      // if ( pdgCode )  {

      AddGeAlHit(fTrackID, fEventID, fpdgCode, fcharge, TVector3(fPos.X(), fPos.Y(), fPos.Z()), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, fELoss, fnCopy);
      //}

      ResetParameters();
    }
  }
  return kTRUE;
}
// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndHypGe::EndOfEvent()
{
  if (fVerboseLevel)
    Print();
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndHypGe::Register()
{
  FairRootManager::Instance()->Register("HypGePoint", "HypGe", fHypGeCollection, kTRUE);
  FairRootManager::Instance()->Register("HypGeAlPoint", "HypGeAl", fHypGeAlCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray *PndHypGe::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fHypGeCollection;
  if (iColl == 1)
    return fHypGeAlCollection;

  return nullptr;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndHypGe::Print() const
{
  Int_t nHits = fHypGeCollection->GetEntriesFast();
  LOG(info) << " PndHypGe: " << nHits << " points registered in this event.";

  if (fVerboseLevel > 1)
    for (Int_t i = 0; i < nHits; i++)
      (*fHypGeCollection)[i]->Print();
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void PndHypGe::Reset()
{
  fHypGeCollection->Delete();
  fHypGeAlCollection->Delete();
  // fHypGecapCollection->Delete();
  fPosIndex = 0;
}
// ----------------------------------------------------------------------------

// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void PndHypGe::CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  // cout << "-I- PndHypGe: " << nEntries << " entries to add." << endl;
  TClonesArray &clref = *cl2;
  PndHypGePoint *oldpoint = nullptr;
  for (Int_t i = 0; i < nEntries; i++) {
    oldpoint = (PndHypGePoint *)cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndHypGePoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- PndHypGe: " << cl2->GetEntriesFast() << " merged entries." << endl;
}
// ----------------------------------------------------------------------------
// -----   Public method ConstructGeometry   ----------------------------------
void PndHypGe::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();

  if (fileName.EndsWith(".root")) {
    ConstructRootGeometry();
  } else {
    ConstructHPGeGeometry();
  }
}

void PndHypGe::ConstructHPGeGeometry()
{
  cout << "----- constructing HPGe Geometry default -----" << endl;
  Double_t deg = TMath::Pi() / 180.;

  // vacuum = gGeoManager->Medium("vacuum");
  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

  FairGeoMedium *medGe = Media->getMedium("germanium");
  Int_t nmedGe = geobuild->createMedium(medGe);

  // Scintillator plate
  FairGeoMedium *medsci = Media->getMedium("polypropylene");
  Int_t nmedsci = geobuild->createMedium(medsci);

  // aluminum capsule
  FairGeoMedium *medcap = Media->getMedium("HYPaluminium");
  Int_t nmedcap = geobuild->createMedium(medcap);

  // TGeoSphere* logicWorld= new TGeoSphere(0.,50.,90.,180.,0.,360.);

  // TGeoVolume* sphere = new TGeoVolume("sphere",logicWorld,  medPWO);

  TGeoVolume *sphere = new TGeoVolumeAssembly("sphere");

  TGeoTube *Crystal_Tube = new TGeoTube("tube", 0., 7.0 / 2., 7.8 / 2.);

  cout << "tube fZ[%i]=%5.2f  fRmin[%i]=%5.2f  fRmax[%i]=%5.2f"
       << " "
       << " " << Crystal_Tube->GetDZ() << " " << Crystal_Tube->GetRmin() << " " << Crystal_Tube->GetRmax() << endl;

  TGeoPgon *Crystal_Polyhedra = new TGeoPgon("Pgon", 0., 360., 6, 2);

  Crystal_Polyhedra->DefineSection(0, -3.9, 0., 3.50 * cos(30. * deg));
  Crystal_Polyhedra->DefineSection(1, 3.9, 0., (3.50));

  // scintillator plate
  TGeoPgon *lay_sci = new TGeoPgon("PgLay", 0., 360., 6, 2);
  // first thickness 0.5 cm,
  // second 1.cm
  lay_sci->DefineSection(0, -0.25, 0., 3.50 * cos(30. * deg));
  lay_sci->DefineSection(1, 0.25, 0., (3.50) * cos(30. * deg));
  // TGeoPgon* lay_sci ;

  // aluminum capsule
  TGeoPgon *lay_capl = new TGeoPgon("PgcapL", 0., 360., 6, 2);
  // first thickness 0.5 cm,
  // second 1.cm
  lay_capl->DefineSection(0, -4.2, 0., 3.144);
  lay_capl->DefineSection(1, 4.2, 0., (3.627));

  TGeoPgon *lay_caps = new TGeoPgon("PgcapS", 0., 360., 6, 2);
  // first thickness 0.5 cm,
  // second 1.cm
  lay_caps->DefineSection(0, -3.95, 0., 3.078);
  lay_caps->DefineSection(1, 3.95, 0., (3.553));

  for (Int_t j = 0; j < Crystal_Polyhedra->GetNz(); j++) {

    cout << "fZ[%i]=%5.2f  fRmin[%i]=%5.2f  fRmax[%i]=%5.2f"
         << " " << j << " " << Crystal_Polyhedra->GetZ()[j] << " " << Crystal_Polyhedra->GetRmin()[j] << " " << Crystal_Polyhedra->GetRmax()[j] << endl;
  }

  //###real geometry
  //###TGeoCompositeShape* logicCrystal_test = new TGeoCompositeShape("test","Pgon*tube");

  TGeoPgon *logicCrystal_test;
  logicCrystal_test = Crystal_Polyhedra;
  TGeoCompositeShape *lay_cap = new TGeoCompositeShape("test", "(PgcapL)-(PgcapS)");

  TList *ClusterList = 0;
  TList *ClusterList1 = 0;
  TList *ClusterList2 = 0;
  TList *ClusterList3 = 0;
  TList *ClusterList4 = 0;
  TList *ClusterList5 = 0;
  TList *ClusterList6 = 0;
  TList *ClusterList7 = 0;
  TList *ClusterList8 = 0;
  TList *ClusterList9 = 0;
  TList *ClusterList10 = 0;
  TList *ClusterList11 = 0;
  TList *ClusterList12 = 0;
  TList *ClusterList13 = 0;
  TList *ClusterList14 = 0;
  // TList *ClusterList15=0;

  GeCluster *HypGe = new GeCluster();

  HypGe->SetPathGeo(fPathGeo.Data());

  ClusterList = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30.0, 168.0, 90., 2.0, 1000); // L
  // ClusterList->Dump();
  for (Int_t i = 0; i < ClusterList->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList->At(i));
  }

  ClusterList1 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30.0, 168.0, 18, 2.0, 1200); // N

  for (Int_t i = 0; i < ClusterList1->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList1->At(i));
  }

  ClusterList2 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30.1, 168.0, 162, 2.0, 400); // D

  for (Int_t i = 0; i < ClusterList2->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList2->At(i));
  }

  ClusterList3 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30.3, 168.0, 234, 2.0, 600); // F

  for (Int_t i = 0; i < ClusterList3->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList3->At(i));
  }

  ClusterList9 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30., 168.0, 306., 2.0, 1400); // Q

  for (Int_t i = 0; i < ClusterList9->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList9->At(i));
  }
  ClusterList4 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 32.2, 148.0, 54.0, 2.0, 1100); // M
  for (Int_t i = 0; i < ClusterList4->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList4->At(i));
  }

  ClusterList5 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 32.9, 148.0, 126.0, 2.0, 300); // C
  for (Int_t i = 0; i < ClusterList5->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList5->At(i));
  }

  ClusterList6 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 31.4, 148.0, 198.0, 2.0, 500); // E
  for (Int_t i = 0; i < ClusterList6->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList6->At(i));
  }

  ClusterList7 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 32.6, 148.0, 270.0, 2.0, 1500); // R
  for (Int_t i = 0; i < ClusterList7->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList7->At(i));
  }
  ClusterList8 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30.3, 148.0, 342.0, 2.0, 1300); // P
  for (Int_t i = 0; i < ClusterList8->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList8->At(i));
  }

  ClusterList10 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30.0, 140.0, 90., 2.0, 700); // G
  // ClusterList->Dump();
  for (Int_t i = 0; i < ClusterList10->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList10->At(i));
  }

  ClusterList11 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30.2, 140.0, 18., 2.0, 800); // J

  for (Int_t i = 0; i < ClusterList11->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList11->At(i));
  }

  ClusterList12 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30.15, 140.0, 162., 2.0, 100); // A

  for (Int_t i = 0; i < ClusterList12->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList12->At(i));
  }

  ClusterList13 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30.15, 140.0, 234., 2.0, 200); // B

  for (Int_t i = 0; i < ClusterList13->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList13->At(i));
  }

  ClusterList14 = HypGe->CreateCluster(logicCrystal_test, lay_sci, lay_cap, *sphere, 30.2, 140.0, 306., 2.0, 900); // K

  for (Int_t i = 0; i < ClusterList14->GetSize(); i++) {
    AddSensitiveVolume((TGeoVolume *)ClusterList14->At(i));
  }

  TString vname = "cave";

  TGeoVolume *vcave = gGeoManager->FindVolumeFast(vname);

  vcave->AddNode(sphere, 1, new TGeoCombiTrans(0., 0., fdist, new TGeoRotation(0)));
}

// -------------------------------------------------------------------------
bool PndHypGe::CheckIfSensitive(std::string name)
{
  for (Int_t i = 0; i < fListOfSensitives.size(); i++) {
    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}

// -----   Private method AddHit   --------------------------------------------
PndHypGePoint *PndHypGe::AddHit(Int_t trackID, Int_t evtID, Int_t pdgCode, Int_t charge, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss, Short_t copy)
{
  TClonesArray &clref = *fHypGeCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel > 1)
    cout << "-I- PndHypGe: Adding Point at IN (" << pos.X() << ", " << pos.Y() << ", " << pos.Z() << ") cm,  evt " << evtID << ", track " << trackID << ", energy loss "
         << eLoss * 1e06 << " keV "
         << " copy " << copy << endl;

  return new (clref[size]) PndHypGePoint(trackID, evtID, pdgCode, charge, pos, mom, time, length, eLoss, copy);
}
PndHypGePoint *
PndHypGe::AddGeAlHit(Int_t trackID, Int_t evtID, Int_t pdgCode, Int_t charge, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss, Short_t copy)
{
  TClonesArray &clref = *fHypGeAlCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel > 1)
    cout << "-I- PndHypGe: Adding Point at IN (" << pos.X() << ", " << pos.Y() << ", " << pos.Z() << ") cm,  evt " << evtID << ", track " << trackID << ", energy loss "
         << eLoss * 1e06 << " keV "
         << " copy " << copy << endl;

  return new (clref[size]) PndHypGePoint(trackID, evtID, pdgCode, charge, pos, mom, time, length, eLoss, copy);
}

inline void PndHypGe::ResetParameters()
{
  fTrackID = 0;
  fVolumeID = 0;
  fEventID = 0;
  fnCopy = 0;

  fPos.SetXYZT(0., 0., 0., 0.);
  fMom.SetXYZT(0., 0., 0., 0.);
  fTime = 0;
  fLength = 0;
  fELoss = 0;
}

// ----

///-------------------------------

ClassImp(PndHypGe)
