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
//  PndHyp
//
//  Filler of PndHypPoint
//
//  created by A. Sanchez
//
///////////////////////////////////////////////////////////////

#include <iostream>
//#include "ostringstream.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "THParticle.h"
#include "TVirtualMC.h"
#include "TString.h"
#include "FairGeoTransform.h"
#include "FairLogger.h"

#include "TGeoBBox.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "TGeoMCGeometry.h"
#include "FairGeoNode.h"
#include "FairGeoMedium.h"
#include "FairGeoMedia.h"
#include "PndGeoHyp.h"
#include "FairGeoRootBuilder.h"
#include "TGeoVoxelFinder.h"
#include "PndStack.h"
#include "PndHyp.h"
#include "PndHypPoint.h"

#include "FairRootManager.h"
#include "FairVolume.h"
// add on for debug

#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"
#include "FairRunSim.h"

#include "PndHypGeoHandling.h"

//#include "PndHypDecayer.h"
//#include "HypStatDecay.h"
#include "TArrayI.h"
#include "TMCProcess.h"

#include "TList.h"
#include "TFile.h"
#include "TGenPhaseSpace.h"

#include <string>
#include <sstream>
using std::cout;
using std::endl;
using std::ostringstream;
class FairVolume;

// -----   Default constructor   -------------------------------------------
PndHyp::PndHyp() : fcount(0), fUseFileOption(false), fUseRAZHOption(false), fUseGamOption(false), fMatBud(false)
{
  fHypCollection = new TClonesArray("PndHypPoint");
  fHypSecTarCollection = new TClonesArray("PndHypPoint");
  fHypSTMatBudCollection = new TClonesArray("PndHypPoint");

  // if(fUseRAZHOption==true && fUseFileOption==true){

  fFile = nullptr; // new TFile(fFileName,"RECREATE");//gam+nucfrag "hypBupDecay2.root"
  fEvt = nullptr;  // new TClonesArray("THParticle",50);
  ft = nullptr;    // new TTree("data","hypernuclei");

  //   activeCnt=0;
  //   weight =1.0;

  //   // define the tree branches
  //   ft->Branch("Npart",&activeCnt,"Npart/I");
  //   ft->Branch("Weigth",&weight,"Weight/D");
  //   ft->Branch("Seed",&seed,"Seed/D");
  //   ft->Branch("Particles",&fEvt,32000);
  // }

  SiId = 0;
  CId = 0;
  CpipeId = 0;
  alId = 0;
  beId = 0;
  fPosIndex = 0;

  fEventID = -1;
  fListMat = kFALSE;

  fListOfSensitives.push_back(fVolNamAb.Data()); //"stglAb");
  fListOfSensitives.push_back(fVolNamSi.Data()); //"stglSi");
  fListOfSensitives.push_back("stglpipe");
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndHyp::PndHyp(const char *name, Bool_t active) : FairDetector(name, active), fcount(0), fUseFileOption(false), fUseRAZHOption(false), fUseGamOption(false), fMatBud(false)
{
  fHypCollection = new TClonesArray("PndHypPoint");
  fHypSecTarCollection = new TClonesArray("PndHypPoint");
  fHypSTMatBudCollection = new TClonesArray("PndHypPoint");

  // if(fUseRAZHOption==true && fUseFileOption==true){

  fFile = nullptr; // new TFile(fFileName,"RECREATE");//gam+nucfrag "hypBupDecay2.root"
  fEvt = nullptr;  // new TClonesArray("THParticle",50);
  ft = nullptr;    // new TTree("data","hypernuclei");

  //   activeCnt=0;
  //   weight =1.0;

  //   // define the tree branches
  //   ft->Branch("Npart",&activeCnt,"Npart/I");
  //   ft->Branch("Weigth",&weight,"Weight/D");
  //   ft->Branch("Seed",&seed,"Seed/D");
  //   ft->Branch("Particles",&fEvt,32000);
  // }

  SiId = 0;
  CId = 0;
  alId = 0;
  beId = 0;
  fPosIndex = 0;
  fListMat = kFALSE;
  fEventID = -1;

  fListOfSensitives.push_back(fVolNamAb.Data()); //"stglAb");
  fListOfSensitives.push_back(fVolNamSi.Data()); //"stglSi");
  fListOfSensitives.push_back("stglpipe");
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHyp::~PndHyp()
{
  if (fHypCollection) {
    fHypCollection->Delete();
    delete fHypCollection;
  }
  if (fHypSecTarCollection) {
    fHypSecTarCollection->Delete();
    delete fHypSecTarCollection;
  }

  if (fHypSTMatBudCollection) {
    fHypSTMatBudCollection->Delete();
    delete fHypSTMatBudCollection;
  }

  delete fGeoH;

  if (fEvt) {
    fEvt->Delete();
    delete fEvt;
  }
  if (fFile) {

    delete fFile;
  }

  if (ft) {
    delete ft;
  }

  // delete r;
  // delete fread;
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void PndHyp::Initialize()
{
  // Init function

  FairDetector::Initialize();

  if (0 == gGeoManager) {
    std::cout << " -E- No gGeoManager in PndHyp Detector::Initialize()!" << std::endl;
    abort();
  }
  fGeoH = new PndHypGeoHandling(gGeoManager);

  // --- Opening output file for hypernuclei formation/decay

  // fread = new HypStatDecay("12C");

  if (fUseRAZHOption == true && fUseFileOption == true) {

    fFile = new TFile(fFileName, "RECREATE"); // gam+nucfrag "hypBupDecay2.root"
    fEvt = new TClonesArray("THParticle", 50);
    ft = new TTree("data", "hypernuclei");

    activeCnt = 0;
    weight = 1.0;

    // define the tree branches
    ft->Branch("Npart", &activeCnt, "Npart/I");
    ft->Branch("Weigth", &weight, "Weight/D");
    ft->Branch("Seed", &seed, "Seed/D");
    ft->Branch("Particles", &fEvt, 32000);
  }

  //-----------------------------------------------------------//

  TGeoMedium *Si = gGeoManager->GetMedium("HYPsilicon"); // fSiMat.Data());
  if (Si)
    SiId = Si->GetId();

  //----disactivated when geo file is block

  if (fVers.Contains("standard")) {

    fStandard = kTRUE;
    fCurrent = kFALSE;

    TGeoMedium *C = gGeoManager->GetMedium(fAbsMat.Data());

    if (CId)
      CId = C->GetId();

    TGeoMedium *Cpipe = gGeoManager->GetMedium(fBPipeMat.Data());
    if (CpipeId)
      CpipeId = Cpipe->GetId();

  } else if (fVers.Contains("List")) {

    fStandard = kTRUE; // sebastian fVolumeID
    fCurrent = kFALSE;

    for (size_t m = 0; m < fListOfMaterials.size(); m++) {
      gGeoManager->GetMedium(fListOfMaterials[m].Data());
    }
  }
}
// -------------------------------------------------------------------------
void PndHyp::BeginEvent()
{
  // Begin of the event
}

// -------------------------------------------------------------------------
void PndHyp::PreTrack()
{
  // Begin of the event

  fTrackStopNxtStep = kFALSE;
  // cout << " PndHyp::PreTrack() "<< endl;
}

void PndHyp::SetSpecialPhysicsCuts()
{
  // FairRun* fRun = FairRun::Instance();

  // Int_t mat = gGeoManager->GetMaterialIndex("HYPdiamond");
  // gMC->Gstpar(mat,"HADR",1.0e-9);
}

// -----   Public method ProcessHits  --------------------------------------

Bool_t PndHyp::ProcessHits(FairVolume *vol)

{

  Double_t beta;
  TString nam; // Double_t gamma; //[R.K. 01/2017] unused variable
  Int_t nSiL = -1, nAbL = -1;
  ostringstream FullName, matName;

  Int_t medId = gMC->CurrentMedium();
  TVector3 radt;
  fpdgCode = -1;
  fpdgCode = gMC->TrackPid();

  if (fTrackStopNxtStep) {
    // if( gMC->TrackPid()==3312)//&&(medId==SiId||medId==CId))
    // cout<<"particle  "<< gMC->TrackPid() << "  " <<fTrackStopNxtStep <<endl;
    gMC->StopTrack();
    return kTRUE;
  }

  TString nam2 = gMC->CurrentVolName();

  /*gMC->TrackMomentum(PiL);
    if (gMC->TrackPid()>1010000000 ||gMC->TrackPid()>1020000000 ||gMC->TrackPid()==-211) cout<<"ProcessHits :  Energy Loss  hyp "<< gMC->TrackPid() << "  "<<vol->getName() <<"
    "<<PiL.P()<<" "<<gMC->Edep()<<endl; if(PiL.P()>3.)cout<<"ProcessHits :  Energy Loss  "<< gMC->TrackPid() << "  "
    <<vol->getName() <<" "<<PiL.P()<<" "<<gMC->Edep()<<endl;
  */

  if (medId == SiId) { // hola

    if (gMC->IsTrackEntering()) {
      fELoss = 0.;
      fTime = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      fmass = gMC->TrackMass();     // mass (GeV)
      fcharge = gMC->TrackCharge(); // charge?

      if (fStartEvID > 0) {
        fEventID = gMC->CurrentEvent() + fStartEvID;
      } else
        fEventID = gMC->CurrentEvent();

      gMC->TrackPosition(fPosIn);
      gMC->TrackMomentum(fMomIn);
    }

    // Sum energy loss for all steps in the active volume

    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create CbmStsPoint.

    TLorentzVector PL;
    gMC->TrackMomentum(PL);

    if ((gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) && gMC->TrackCharge()) {
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

      //*** now the volume is through the layer number characterised.(X-Z,Z-Y)

      if (fCurrent) {
        if ((nam2.Contains("Sensor"))) {
          sscanf(nam2, "Sensor%d", &nSiL);
          fVolumeID = nSiL;
        }
      } else
        fVolumeID = vol->getCopyNo();

      //**************///

      // FullName << gGeoManager->GetPath();

      // cout << "*******  Info from gMC *************" << endl;
      // Int_t cp=-1; //[R.K.02/2017] Unused variable?
      // Int_t fVolid = gMC->CurrentVolID(cp); //[R.K.02/2017] Unused variable?

      // cout << " Vol Name: " << gMC->CurrentVolPath() <<" vol id "<<vol->getMCid()<< endl;

      /*  TString nam2 = gMC->CurrentVolName();
          if ((nam2.Contains("Si"))) {
          sscanf(nam2,"stglSi%d#01", &nSiL);
          cout << "hyp::ProcessHits> : " << nam2 <<" # "
          <<nSiL<<" "<<"Hit in "<< gGeoManager->GetPath()<<endl;
          } */

      FullName << gMC->CurrentVolPath();

      if (0 == fGeoH) {
        std::cout << " -E- No PndHypGeoHandling loaded." << std::endl;
        abort();
      }

      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

      if (fELoss == 0.)
        return kFALSE;

      radt = fPosOut.Vect();
      fdist = radt.Perp();
      beta = fMomOut.Beta();
      // gamma = fMomOut.Gamma();
      fPLin = beta;
      // fPLin = fMomIn.P();
      fPLout = fMomOut.P();

      AddHit(fTrackID, fEventID, fVolumeID, fGeoH->GetID(gMC->CurrentVolPath()), TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()), TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
             TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()), TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()), fTime, fLength, fELoss, fcharge, fmass, fpdgCode, fdist, fPLin,
             fPLout);

      // Increment number of PndMvd points for TParticle
      // PndStack* stack = (PndStack*) gMC->GetStack();
      // stack->AddPoint(DetectorId::kHYP);

      ResetParameters();
    }

    // return kTRUE;

  } // volSi

  // -----

  else if (fMatBud && (!nam2.Contains("Absorber")) && (!nam2.Contains("Sensor"))) {

    // std::cout<<nam2.Data()<<std::endl;

    if (gMC->IsTrackEntering()) {
      fELoss = 0.;
      fTime = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      fmass = gMC->TrackMass();     // mass (GeV)
      fcharge = gMC->TrackCharge(); // charge?

      if (fStartEvID > 0) {
        fEventID = gMC->CurrentEvent() + fStartEvID;
      } else
        fEventID = gMC->CurrentEvent();

      gMC->TrackPosition(fPosIn);
      gMC->TrackMomentum(fMomIn);
    }

    // Sum energy loss for all steps in the active volume

    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume.

    if ((gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) && gMC->TrackCharge()) {
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

      /// std::cout<<"  " <<std::endl;

      fVolumeID = vol->getCopyNo();

      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

      if (fELoss == 0.)
        return kFALSE;

      radt = fPosOut.Vect();
      fdist = radt.Perp();
      beta = fMomOut.Beta();

      fPLin = beta;

      fPLout = fMomOut.P();

      AddSTMatBudHit(fTrackID, fEventID, fVolumeID, nam2.Data(), TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()), TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
                     TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()), TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()), fTime, fLength, fELoss, fcharge, fmass, fpdgCode, fdist,
                     fPLin, fPLout);

      // Increment number of PndMvd points for TParticle
      // PndStack* stack = (PndStack*) gMC->GetStack();
      // stack->AddPoint(DetectorId::kHYP);

      ResetParameters();
    }

  }

  //--------
  else if ((fpdgCode == 3312) && (nam2.Contains("Ab"))) //||nam2.Contains("Si")))
  {                                                     // absorber

    if (gMC->IsTrackEntering()) {
      fELoss = 0.;
      fTime = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      fmass = gMC->TrackMass();     // mass (GeV)
      fcharge = gMC->TrackCharge(); // charge?

      if (fStartEvID > 0) {
        fEventID = gMC->CurrentEvent() + fStartEvID;
      } else
        fEventID = gMC->CurrentEvent();

      gMC->TrackPosition(fPosIn);
      gMC->TrackMomentum(fMomIn);
    }

    // Sum energy loss for all steps in the active volume

    fELoss += gMC->Edep();
    // Gamma, Beta, tau(proper time) of ximnus
    TLorentzVector PL;
    gMC->TrackMomentum(PL);
    beta = PL.Beta();

    if (beta == 0.0) {
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

      if (fCurrent) {

        if ((nam2.Contains("Absorber"))) {
          sscanf(nam2, "Absorber%d", &nAbL);
          cout << "hyp::ProcessHits> : " << nam2 << " # " << nAbL << " "
               << "Hit in " << gGeoManager->GetPath() << endl;
          fVolumeID = nAbL;
        }
      } else
        fVolumeID = vol->getMCid();

      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

      // ostringstream matName;
      TString mat[4] = {"CAbs", "Si", "Be", "Al"};
      if (medId == CId)
        matName << "CAbs";
      if (medId == SiId)
        matName << "Si";
      // cout << "Hit in fullname " << matName.str() <<endl;
      if (fELoss == 0.)
        return kFALSE;

      //---Kinetic energy:###(PL.P())^2 + Mass^2 -Mass##
      radt = fPosOut.Vect();
      fdist = radt.Perp();
      // beta = fMomOut.Beta();
      // gamma = fMomOut.Gamma();
      fPLin = beta;
      // fPLin = fMomIn.P();
      fPLout = fMomOut.P();

      AddSecTarHit(fTrackID, fEventID, fVolumeID, matName.str(), TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()), TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
                   TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()), TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()), fTime, fLength, fELoss, fcharge, fmass, fpdgCode, fdist,
                   fPLin, fPLout);

      // ***** Statistical Decay of a compound hyperfragment********
      SetHypStatDecay(fUseRAZHOption, fUseFileOption);

      fTrackStopNxtStep = kTRUE;

      // Increment number of PndMvd points for TParticle
      // PndStack* stack = (PndStack*) gMC->GetStack();
      // stack->AddPoint(DetectorId::kHYP);

      ResetParameters();

    } //

  } // no volSi

  return kTRUE;

} // ProcessHits

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndHyp::EndOfEvent()
{
  if (fVerboseLevel)
    Print();
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndHyp::Register()
{
  FairRootManager::Instance()->Register("HypPoint", "Hyp", fHypCollection, kTRUE);
  FairRootManager::Instance()->Register("HypSegTarPoint", "HypSecTarg", fHypSecTarCollection, kTRUE);
  FairRootManager::Instance()->Register("HypSTMatBudPoint", "HypMatBud", fHypSTMatBudCollection, fMatBud);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray *PndHyp::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fHypCollection;
  if (iColl == 1)
    return fHypSecTarCollection;
  if (iColl == 2)
    return fHypSTMatBudCollection;

  return nullptr;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndHyp::Print() const
{
  Int_t nHits = fHypCollection->GetEntriesFast();
  LOG(info) << " PndHyp: " << nHits << " points registered in this event.";

  if (fVerboseLevel > 1)
    for (Int_t i = 0; i < nHits; i++)
      (*fHypCollection)[i]->Print();
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void PndHyp::Reset()
{
  fHypCollection->Clear();
  fHypSecTarCollection->Clear();
  if (fHypSTMatBudCollection)
    fHypSTMatBudCollection->Clear();

  fPosIndex = 0;
}
// ----------------------------------------------------------------------------

// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void PndHyp::CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  // cout << "-I- PndHyp: " << nEntries << " entries to add." << endl;
  TClonesArray &clref = *cl2;
  PndHypPoint *oldpoint = nullptr;
  for (Int_t i = 0; i < nEntries; i++) {
    oldpoint = (PndHypPoint *)cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndHypPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- PndHyp: " << cl2->GetEntriesFast() << " merged entries." << endl;
}
// ----------------------------------------------------------------------------
// -----   Public method ConstructGeometry   ----------------------------------
void PndHyp::ConstructGeometry()
{

  TString fileName = GetGeometryFileName();

  if (fileName.EndsWith(".geo")) {
    std::cout << "Geometry format not supported " << std::endl;
    // ConstructASCIIGeometry();

  } else if (fileName.EndsWith(".root")) {
    fRootSensVol = kTRUE;

    ConstructRootGeometry();
  } else {
    std::cout << "Geometry format not supported " << std::endl;
  }
}

// -------------------------------------------------------------

// -------------------------------------------------------------------------
bool PndHyp::CheckIfSensitive(std::string name)
{
  for (size_t i = 0; i < fListOfSensitives.size(); i++) {

    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}
// -----   Public method FinishRun   -------------------------------------------
void PndHyp::FinishRun()
{
  if (fUseRAZHOption == true && fUseFileOption == true) {
    fFile->Write();
    fFile->Close();
    // delete fEvt;

    cout << " -I PndHyp::FinishRun():closing and deleting fFile fEvt " << endl;
  }
}

// ------   Private method SetHypStatDecay   -----------------------------------

void PndHyp::SetHypStatDecay(bool cal, bool active)
{
  // ***** Sequential Decay of a compound hyperfragment********

  if (cal == true) {

    if (active == true) {
      fEvt->Clear();
    }
    Int_t cnt = 0;

    cout << " increment count " << fcount << endl;

    if (fUseGamOption) {
      // default gamma emission THParticle object
      // for calibration from Xi- stopping vertex
      // momentum is given via generator

      TLorentzVector PG(0., 0., 0., 1.); //

      TLorentzVector VG;

      // std::cout<<fPosOut.X()<<" "<<fPosOut.Y()<<" "<<fPosOut.Z()<<std::endl;

      VG.SetX(fPosOut.X());
      VG.SetY(fPosOut.Y());
      VG.SetZ(fPosOut.Z());
      std::cout << VG.X() << " " << VG.Y() << " " << VG.Z() << std::endl;

      if (active == true) {
        THParticle fGamma(22, 1, 0, 0, 0, 0, 0, 0, PG, VG);

        new ((*fEvt)[cnt++]) THParticle(fGamma);
      }

    } else {
      // emission of two pions from mesonic weak decay of DHP

      TLorentzVector target4(0.0, 0.0, 0., 5.95137); // He6LL
      TLorentzVector target5(0.0, 0.0, 0., 5.7789);  // Li6L

      TLorentzVector target6(0., 0., 0., 10.60335); // Be11LL
      TLorentzVector target7(0., 0., 0., 10.41176); // B11L

      TLorentzVector W6 = target6;
      TLorentzVector W7 = target7;

      Double_t mass4[2] = {10.41176, 0.139}; // B11L
      Double_t mass5[2] = {10.25409, 0.139}; // C11

      TGenPhaseSpace ev6;
      ev6.SetDecay(W6, 2, mass4); // two-body kinematics assumption
      TGenPhaseSpace ev7;
      ev7.SetDecay(W7, 2, mass5);

      ev6.Generate(); // Double_t weight7 =  //[R.K.03/2017] unused variable
      TLorentzVector *pPi6 = ev6.GetDecay(1);
      ev7.Generate(); // Double_t weight8 =  //[R.K.03/2017] unused variable
      TLorentzVector *pPi7 = ev7.GetDecay(1);

      TLorentzVector V;
      // std::cout<<fPosOut.X()<<" "<<fPosOut.Y()<<" "<<fPosOut.Z()<<std::endl;
      V.SetX(fPosOut.X());
      V.SetY(fPosOut.Y());
      V.SetZ(fPosOut.Z());
      std::cout << V.X() << " " << V.Y() << " " << V.Z() << std::endl;
      if (active == true) {
        THParticle fpion_H(-211, 1, 0, 0, 0, 0, 0, 0, *pPi6, V);

        new ((*fEvt)[cnt++]) THParticle(fpion_H);

        THParticle fpion_L(-211, 1, 0, 0, 0, 0, 0, 0, *pPi7, V);

        new ((*fEvt)[cnt++]) THParticle(fpion_L);
        cout << cnt << endl;
      }
    }

    if (active == true) {
      activeCnt = cnt;
      ft->Fill();
    }
  }
}

// -----   Private method AddHit   --------------------------------------------

PndHypPoint *PndHyp::AddHit(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 pos, TVector3 mom, TVector3 posout, TVector3 momout, Double_t time, Double_t length,
                            Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t dist, Double_t PLin, Double_t PLout)
{
  TClonesArray &clref = *fHypCollection;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndHypPoint(trackID, evtID, detID, detName, pos, mom, posout, momout, time, length, eLoss, charge, mass, pdgCode, dist, PLin, PLout);
}

// ----

// -----   Private method AddSecTarHit   --------------------------------------------

PndHypPoint *PndHyp::AddSecTarHit(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 pos, TVector3 mom, TVector3 posout, TVector3 momout, Double_t time,
                                  Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t dist, Double_t PLin, Double_t PLout)
{
  TClonesArray &clref = *fHypSecTarCollection;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndHypPoint(trackID, evtID, detID, detName, pos, mom, posout, momout, time, length, eLoss, charge, mass, pdgCode, dist, PLin, PLout);
}

// ----

// -----   Private method AddSecTarHit   --------------------------------------------

PndHypPoint *PndHyp::AddSTMatBudHit(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 pos, TVector3 mom, TVector3 posout, TVector3 momout, Double_t time,
                                    Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t dist, Double_t PLin, Double_t PLout)
{
  TClonesArray &clref = *fHypSTMatBudCollection;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndHypPoint(trackID, evtID, detID, detName, pos, mom, posout, momout, time, length, eLoss, charge, mass, pdgCode, dist, PLin, PLout);
}

// ----

ClassImp(PndHyp)
