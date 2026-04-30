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
//  PndFtof
//
//
//  created by A. Sanchez
//  modifyed by Yu.Naryshkin
///////////////////////////////////////////////////////////////

#include "PndFtof.h"

#include "PndFtofPoint.h"

#include "FairGeoTransform.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoMedium.h"
#include "FairGeoInterface.h"
#include "FairGeoMedia.h"
#include "FairLogger.h"

#include "PndGeoFtof.h"
#include "FairGeoRootBuilder.h"
#include "PndStack.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "PndDetectorList.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TString.h"
#include "TList.h"
#include "TGeoBBox.h"
#include "TGeoMCGeometry.h"
#include "TObjArray.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"
//#include "FairGeoG3Builder.h"

#include <string>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;
using std::ostringstream;

// -----   Default constructor   -------------------------------------------
PndFtof::PndFtof()
  : FairDetector(), fListOfSensitives(), par(), fTrackID(-1), fVolumeID(-1), fEventID(-1), fPosIn(), fMomIn(), fPosOut(), fMomOut(), fPLout(0), fPLin(0), fTime(0), fLength(0),
    fELoss(0), fPosIndex(-1), fpdgCode(-1), SiId(0), CId(0), alId(0), beId(0), CpipeId(0), fcharge(0), fmass(0), fdist(0)
{
  fVerboseLevel = 0;

  fListOfSensitives.push_back("Ftof_Central_Strip");  // Root_Test.root
  fListOfSensitives.push_back("Ftof_Beam_Strip");     // Root_Test.root
  fListOfSensitives.push_back("Ftof_Vertical_Strip"); // Root_Test.root
  // adding side tof plates
  fListOfSensitives.push_back("Stof_plate");
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndFtof::PndFtof(const char *name, Bool_t active)
  : FairDetector(name, active), fListOfSensitives(), par(), fTrackID(-1), fVolumeID(-1), fEventID(-1), fPosIn(), fMomIn(), fPosOut(), fMomOut(), fPLout(0), fPLin(0), fTime(0),
    fLength(0), fELoss(0), fPosIndex(-1), fpdgCode(-1), SiId(0), CId(0), alId(0), beId(0), CpipeId(0), fcharge(0), fmass(0), fdist(0)
{
  fVerboseLevel = 0;

  fListOfSensitives.push_back("Ftof_Central_Strip");  // Root_Test.root
  fListOfSensitives.push_back("Ftof_Beam_Strip");     // Root_Test.root
  fListOfSensitives.push_back("Ftof_Vertical_Strip"); // Root_Test.root
  // adding side tof plates
  fListOfSensitives.push_back("Stof_plate");
}
// -------------------------------------------------------------------------

PndFtof::PndFtof(const PndFtof& rhs)
    : FairDetector(rhs), fListOfSensitives(), par(), fTrackID(-1), fVolumeID(-1), fEventID(-1), fPosIn(), fMomIn(), fPosOut(), fMomOut(), fPLout(0), fPLin(0), fTime(0),
    fLength(0), fELoss(0), fPosIndex(-1), fpdgCode(-1), SiId(0), CId(0), alId(0), beId(0), CpipeId(0), fcharge(0), fmass(0), fdist(0)
{
  fListOfSensitives.push_back("Ftof_Central_Strip");  // Root_Test.root
  fListOfSensitives.push_back("Ftof_Beam_Strip");     // Root_Test.root
  fListOfSensitives.push_back("Ftof_Vertical_Strip"); // Root_Test.root
  // adding side tof plates
  fListOfSensitives.push_back("Stof_plate");
}

// -----   Destructor   ----------------------------------------------------
PndFtof::~PndFtof()
{
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void PndFtof::Initialize()
{
  // Init function

  FairDetector::Initialize();
  FairRun *sim = FairRun::Instance();
  FairRuntimeDb *rtdb = sim->GetRuntimeDb();
  par = (PndGeoFtofPar *)(rtdb->getContainer("PndGeoFtofPar"));
  par->setChanged();
  par->setInputVersion(sim->GetRunId(), 1);

  // TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  std::cout << " -I- Initializing PndFtof()" << std::endl;

  if (0 == gGeoManager) {
    std::cout << " -E- No gGeoManager in PndFtof::Initialize()!" << std::endl;
    abort();
  }
}
// -------------------------------------------------------------------------
void PndFtof::BeginEvent()
{
  // Begin of the event
}

// -----   Public method ProcessHits  --------------------------------------

Bool_t PndFtof::ProcessHits(FairVolume *vol)
{
  if (gMC->TrackCharge() == 0)
    return kTRUE; // skip neutrals

  TVector3 radt;

  if (gMC->IsTrackEntering()) {
    fELoss = 0.;
    fEventID = gMC->CurrentEvent();
    fTime = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    fmass = gMC->TrackMass();     // mass (GeV)
    fcharge = gMC->TrackCharge(); // charge?
    fpdgCode = gMC->TrackPid();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
  }

  // Sum energy loss for all steps in the active volume

  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create CbmStsPoint.

  TLorentzVector PL;
  gMC->TrackMomentum(PL);

  if ((gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())) {
    Int_t TrNo = gMC->GetStack()->GetCurrentTrackNumber();
    if ((TrNo == fTrackID) && (fELoss > 0.)) {
      Int_t cp = -1;
      gMC->CurrentVolOffID(1, cp);
      fVolumeID = vol->getMCid();
      // Int_t fVolid = gMC->CurrentVolID(cp); //[R.K. 01/2017] unused variable?
      TString FullName = gMC->CurrentVolPath();

      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

      radt = fPosOut.Vect();
      fdist = radt.Perp();
      fPLin = fMomIn.P();
      fPLout = fMomOut.P();

      AddHit(fTrackID, fEventID, fVolumeID, FullName, TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()), TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
             TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()), TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()), fTime, fLength, fELoss, fcharge, fmass, fpdgCode, fdist, fPLin,
             fPLout);

      PndStack *stack = (PndStack *)gMC->GetStack();
      stack->AddPoint(PndDetectorId::kFTOF);
    }

    ResetParameters();
  }

  return kTRUE;

} // ProcessHits

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndFtof::EndOfEvent()
{
  if (fVerboseLevel)
    Print();

  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndFtof::Register()
{
  FairRootManager::Instance()->RegisterAny("FtofPoint", fPointVector, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray *PndFtof::GetCollection(Int_t iColl) const
{
  return nullptr;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndFtof::Print() const
{
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void PndFtof::Reset()
{
    fPointVector->clear();
  fPosIndex = 0;
}
// ----------------------------------------------------------------------------

// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void PndFtof::CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  // cout << "-I- PndFtof: " << nEntries << " entries to add." << endl;
  TClonesArray &clref = *cl2;
  PndFtofPoint *oldpoint = nullptr;
  for (Int_t i = 0; i < nEntries; i++) {
    oldpoint = (PndFtofPoint *)cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndFtofPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- PndFtof: " << cl2->GetEntriesFast() << " merged entries." << endl;
}
// ----------------------------------------------------------------------------
// -----   Public method ConstructGeometry   ----------------------------------
void PndFtof::ConstructGeometry()
{

  TString fileName = GetGeometryFileName();

  if (fileName.EndsWith(".root")) {
    ConstructRootGeometry();
  } else {

    ConstructASCIIGeometry();
  }
}
void PndFtof::ConstructASCIIGeometry()
{

  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();

  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumSci = Media->getMedium("polyvinyltoluene");

  // Int_t nmed=
  geobuild->createMedium(CbmMediumSci);
  TString vname = "cave";
  TGeoVolume *vcave = gGeoManager->FindVolumeFast(vname);

  const Double_t kBCentX = 2.5;  // half length(cm) //from EMC TDR
  const Double_t kBCentY = 70.0; // half length (cm) //?
  const Double_t kBCentZ = 0.75; // half length (cm) //?

  const Double_t kBBeamX = 2.5;  // half length(cm) //from EMC TDR
  const Double_t kBBeamY = 30.0; // alf length (cm) //?
  const Double_t kBBeamZ = 0.75; // half length (cm) //?

  const Double_t kBVertX = 5.0;  // half length (cm) //from EMC TDR
  const Double_t kBVertY = 70.0; // half length (cm) //?
  const Double_t kBVertZ = 0.75; // half length (cm) //from EMC TDR

  TGeoBBox *CentShape;
  TGeoBBox *BeamShape;
  TGeoBBox *VertShape;

  TGeoVolume *CentVol;
  TGeoVolume *BeamVol;
  TGeoVolume *VertVol;

  Double_t bx[26];  //,by[26],bz[26]; //[R.K. 01/2017] unused variable?
  Double_t bvx[50]; //,bvy[50],bvz[50]; //[R.K. 01/2017] unused variable?
  TGeoVolumeAssembly *SubunitVol = new TGeoVolumeAssembly("Ftof_strips");

  std::stringstream name;
  std::stringstream namB;
  std::stringstream namV;
  //  std::stringstream name1;	//[T.S. 02/2021] unused variable
  //  std::stringstream namB1;	//[T.S. 02/2021] unused variable
  //  std::stringstream namV1;  //[T.S. 02/2021] unused variable
  // char name2[17]; //[R.K. 01/2017] unused variable?
  // char namB2[17]; //[R.K. 01/2017] unused variable?
  // char namV2[17]; //[R.K. 01/2017] unused variable?

  for (int i = 0; i < 26; i++) {
    bx[i] = -40 + kBCentX * ((2 * i) + 1);
  }

  for (int i = 0; i < 26; i++) {
    if (i < 8 || i > 17) {
      TGeoCombiTrans *trc1 = NULL;

      // Double_t offset; //[R.K. 01/2017] unused variable?
      name << "Centshape" << i;
      //      name1 << "Ftof_Central_Strip" << i;

      CentShape = new TGeoBBox("Centshape", kBCentX, kBCentY, kBCentZ);
      CentVol = new TGeoVolume("Ftof_Central_Strip", CentShape, gGeoManager->GetMedium("polyvinyltoluene"));
      if (i < 8)
        trc1 = new TGeoCombiTrans(bx[i], 0, 750, new TGeoRotation());
      else if (i > 17)
        trc1 = new TGeoCombiTrans(bx[i - 5], 0, 750, new TGeoRotation());

      trc1->SetName(name.str().c_str());
      trc1->RegisterYourself();
      SubunitVol->AddNode(CentVol, i, trc1);
      AddSensitiveVolume(CentVol);
    } else {
      Double_t offset;
      namB << "Beamshape" << i;
      //      namB1 << "Ftof_Beam_Strip" << std::setfill('0') << std::setw(2) << i;
      BeamShape = new TGeoBBox("Beamshape", kBBeamX, kBBeamY, kBBeamZ);
      BeamVol = new TGeoVolume("Ftof_Beam_Strip", BeamShape, gGeoManager->GetMedium("polyvinyltoluene"));

      if (i == 8 || i == 9) {

        (i == 8 ? (offset = -70 + kBBeamY) : (offset = 70 - kBBeamY));
        TGeoCombiTrans *trc1 = new TGeoCombiTrans(bx[8], offset, 750, new TGeoRotation());

        trc1->SetName(namB.str().c_str());
        trc1->RegisterYourself();
        SubunitVol->AddNode(BeamVol, i, trc1);
        AddSensitiveVolume(BeamVol);
      }
      if (i == 10 || i == 11) {

        (i == 10 ? (offset = -70 + kBBeamY) : (offset = 70 - kBBeamY));
        TGeoCombiTrans *trc1 = new TGeoCombiTrans(bx[9], offset, 750, new TGeoRotation());

        trc1->SetName(namB.str().c_str());
        trc1->RegisterYourself();
        SubunitVol->AddNode(BeamVol, i, trc1);
        AddSensitiveVolume(BeamVol);
      }
      if (i == 12 || i == 13) {

        (i == 12 ? (offset = -70 + kBBeamY) : (offset = 70 - kBBeamY));
        TGeoCombiTrans *trc1 = new TGeoCombiTrans(bx[10], offset, 750, new TGeoRotation());

        trc1->SetName(namB.str().c_str());
        trc1->RegisterYourself();
        SubunitVol->AddNode(BeamVol, i, trc1);
        AddSensitiveVolume(BeamVol);
      }
      if (i == 14 || i == 15) {

        (i == 14 ? (offset = -70 + kBBeamY) : (offset = 70 - kBBeamY));
        TGeoCombiTrans *trc1 = new TGeoCombiTrans(bx[11], offset, 750, new TGeoRotation());

        trc1->SetName(namB.str().c_str());
        trc1->RegisterYourself();
        SubunitVol->AddNode(BeamVol, i, trc1);
        AddSensitiveVolume(BeamVol);
      }
      if (i == 16 || i == 17) {

        (i == 16 ? (offset = -70 + kBBeamY) : (offset = 70 - kBBeamY));
        TGeoCombiTrans *trc1 = new TGeoCombiTrans(bx[12], offset, 750, new TGeoRotation());

        trc1->SetName(namB.str().c_str());
        trc1->RegisterYourself();
        SubunitVol->AddNode(BeamVol, i, trc1);
        AddSensitiveVolume(BeamVol);
      }
    }
  }

  for (int i = 0; i < 24; i++) {
    bvx[i] = -280 + kBVertX * ((2 * i) + 1);
  }

  for (int i = 0; i < 26; i++) {

    bvx[i + 24] = 60 + kBVertX * ((2 * i) + 1);
  }

  for (int i = 0; i < 50; i++) {

    namV << "VertShape" << i;
    //    sprintf(namV1, "Ftof_Vertical_Strip%d", i);

    VertShape = new TGeoBBox("VertShape", kBVertX, kBVertY, kBVertZ);
    VertVol = new TGeoVolume("Ftof_Vertical_Strip", VertShape, gGeoManager->GetMedium("polyvinyltoluene"));
    TGeoCombiTrans *trc1 = new TGeoCombiTrans(bvx[i], 0, 750, new TGeoRotation());

    trc1->SetName(namV.str().c_str());
    trc1->RegisterYourself();
    SubunitVol->AddNode(VertVol, i, trc1);
    AddSensitiveVolume(VertVol);
  }

  vcave->AddNode(SubunitVol, 0, new TGeoCombiTrans());
}

// -------------------------------------------------------------------------
bool PndFtof::CheckIfSensitive(std::string name)
{
  for (size_t i = 0; i < fListOfSensitives.size(); i++) {
    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}

// -----   Private method AddHit   --------------------------------------------

void PndFtof::AddHit(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 pos, TVector3 mom, TVector3 posout, TVector3 momout,
                              Double_t, // time //[R.K.03/2017] unused variable(s)
                              Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t dist, Double_t PLin, Double_t PLout)
{
  fPointVector->push_back(PndFtofPoint(trackID, evtID, detID, detName, pos, mom, posout, momout, fTime, length, eLoss, charge, mass, pdgCode, dist, PLin, PLout));
      
}

Bool_t PndFtof::IsSensitive(const std::string& name)
{
  for (size_t i = 0; i < fListOfSensitives.size(); i++) {
    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}


FairModule* PndFtof::CloneModule() const
{
    return new PndFtof(*this);
}

ClassImp(PndFtof)
