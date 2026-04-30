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
//  PndEmcApd
//
//  Filler of PndEmcApdPoint
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmcApd.h"
#include "PndEmcApdPoint.h"
#include "PndEmcReader.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairGeoMedia.h"
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "TObjArray.h"
#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoArb8.h"
#include "TGeoMatrix.h"

#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "PndStack.h"
#include "TString.h"
#include "TObject.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndEmcApd::PndEmcApd()
  : FairDetector(), fTrackID(0), fVolumeID(0), fEventID(-1), fPos(0, 0, 0, 0), fMom(0, 0, 0, 0), fTime(0), fLength(0), fELoss(0), fPosIndex(0),
    fApdCollection(new TClonesArray("PndEmcApdPoint"))
{
  // fApdCollection        = new TClonesArray("PndEmcApdPoint");
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndEmcApd::PndEmcApd(const char *name, Bool_t active)
  : FairDetector(name, active), fTrackID(0), fVolumeID(0), fEventID(-1), fPos(0, 0, 0, 0), fMom(0, 0, 0, 0), fTime(0), fLength(0), fELoss(0), fPosIndex(0),
    fApdCollection(new TClonesArray("PndEmcApdPoint"))
{
  // fApdCollection        = new TClonesArray("PndEmcApdPoint");
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndEmcApd::~PndEmcApd()
{
  if (fApdCollection) {
    fApdCollection->Delete();
    delete fApdCollection;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void PndEmcApd::Initialize()
{
  // Init function

  FairDetector::Initialize();
  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?
}
// -------------------------------------------------------------------------
void PndEmcApd::BeginEvent()
{
  // Begin of the event
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t PndEmcApd::ProcessHits(FairVolume *)
{ // vol //[R.K.03/2017] unused variable(s)

  TString nam = gMC->CurrentVolName();

  // ---------------------------------------------------------------------------------
  // Getting parameters for the ROOT file with geometry for Forward Enc-Cap.
  // Each of the subvolume name for FwEndCap geometry in the ROOT file contains "Vol".
  // Int_t copyNoCrys=-1,copyNoBox=-1,copyNoSub=-1,copyNoQuar=-1; //[R.K. 01/2017] unused variable?
  // Int_t idCrys=-1,idBox=-1,idSub=-1,idQuar=-1; //[R.K. 01/2017] unused variable?
  Int_t copyNo = -1; //, id = -1; //[R.K.03/2017] unused variable
  Int_t nMod = -1, nRow = -1, nCrys = -1;
  Short_t nFlag = 0;
  if (gMC->IsTrackEntering()) {
    nFlag = -1;
  }
  if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {
    nFlag = 1;
  }

  fTrackID = gMC->GetStack()->GetCurrentTrackNumber(); // trk ID
  fEventID = gMC->CurrentEvent();
  fELoss = gMC->Edep();
  fLength = gMC->TrackLength();
  fTime = gMC->TrackTime();
  gMC->TrackPosition(fPos); // cm
  gMC->TrackMomentum(fMom); // GeV

  sscanf(nam, "apd%dr%dc%d", &nMod, &nRow, &nCrys);
  gMC->CurrentVolOffID(2, copyNo); // id =  //[R.K.03/2017] unused variable

  fVolumeID = nMod * 100000000 + nRow * 1000000 + copyNo * 10000 + nCrys;

  TVector3 pos(fPos.X(), fPos.Y(), fPos.Z());

  AddHit(fTrackID, fVolumeID, fEventID, TVector3(fPos.X(), fPos.Y(), fPos.Z()), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, fELoss, nMod, nRow, nCrys, copyNo,
         nFlag);

  ResetParameters();

  return kTRUE;
}
// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndEmcApd::EndOfEvent()
{
  if (fVerboseLevel)
    Print();
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndEmcApd::Register()
{
  FairRootManager::Instance()->Register("EmcApdPoint", "Emc", fApdCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray *PndEmcApd::GetCollection(Int_t iColl) const
{
  if (iColl == 0)
    return fApdCollection;

  return nullptr;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndEmcApd::Print() const
{
  Int_t nHits = fApdCollection->GetEntriesFast();
  // cout << "-I- PndEmcApd: " << nHits << " points registered in this event."
  //<< endl;

  if (fVerboseLevel > 1)
    for (Int_t i = 0; i < nHits; i++)
      (*fApdCollection)[i]->Print();
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void PndEmcApd::Reset()
{
  fApdCollection->Clear();

  fPosIndex = 0;
}
// ----------------------------------------------------------------------------

// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void PndEmcApd::CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  // cout << "-I- PndEmcApd: " << nEntries << " entries to add." << endl;
  TClonesArray &clref = *cl2;
  PndEmcApdPoint *oldpoint = nullptr;
  for (Int_t i = 0; i < nEntries; i++) {
    oldpoint = (PndEmcApdPoint *)cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndEmcApdPoint(*oldpoint);
    fPosIndex++;
  }
  //  cout << " -I- PndEmcApd: " << cl2->GetEntriesFast() << " merged entries."
  //   << endl;
}

// ----------------------------------------------------------------------------
// -----   Public method ConstructGeometry   ----------------------------------
void PndEmcApd::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();

  if (fileName.EndsWith(".dat")) {
    std::cout << "                                               " << std::endl;
    std::cout << " ====== EMCAPD::ConstructASCIIGeometry()====== " << std::endl;
    std::cout << " ============================================= " << std::endl;
    ConstructASCIIGeometry();
  } else {
    std::cout << "Geometry format not supported " << std::endl;
  }
}

void PndEmcApd::ConstructASCIIGeometry()
{
  // Definition of materials

  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumSi = Media->getMedium("silicon");
  geobuild->createMedium(CbmMediumSi); // Int_t nmedSi = //[R.K.03/2017] unused variable

  TGeoVolume *flayer1 = new TGeoVolumeAssembly("ApdLayer1");
  TGeoVolume *flayer2 = new TGeoVolumeAssembly("ApdLayer2");
  TGeoVolume *flayer6 = new TGeoVolumeAssembly("ApdLayer6");

  Bool_t bIsModuleOn[6] = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE};
  // Bool_t isFirst = kTRUE; //[R.K. 01/2017] unused variable?

  PndEmcReader read(GetGeometryFileName());
  for (Int_t module = read.GetMinModules(); module <= read.GetMaxModules(); module++) {
    cout << "Emc APD module = " << module;
    cout << endl << "******** " << endl;

    for (Int_t row = read.GetMinRows(module); row <= read.GetMaxRows(module); row++) {
      for (Int_t crystal = read.GetMinCrystals(module, row); crystal <= read.GetMaxCrystals(module, row); crystal++) {

        Text_t buffer[30];
        sprintf(buffer, "apd0%dr%dc%d", module, row, crystal);
        DataG4 data = read.GetData(module, row, crystal);

        if (data.module == -1)
          continue; // if the pad is not present, do not create geometry

        // Construction of target spectrometer geometry
        TGeoTrap *trap = new TGeoTrap(data.pDz / 10., data.pTheta, data.pPhi, data.pDy1 / 10., data.pDx1 / 10., data.pDx2 / 10., data.pAlp1, data.pDy2 / 10., data.pDx3 / 10.,
                                      data.pDx4 / 10., data.pAlp2);
        TGeoVolume *volume;
        volume = new TGeoVolume(buffer, trap, gGeoManager->GetMedium("silicon"));

        volume->SetLineColor(5);
        TGeoRotation rot;
        rot.RotateZ(data.tau);
        rot.RotateY(data.theta);
        rot.RotateZ(data.phi);

        if (module == 1)
          flayer1->AddNode(volume, 0,
                           new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10. + 3.7, new TGeoRotation(rot))); // shift of 37 mm with respect to interaction point
        if (module == 2)
          flayer2->AddNode(volume, 0,
                           new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10. + 3.7, new TGeoRotation(rot))); // shift of 37 mm with respect to interaction point
        if (module == 6)
          flayer6->AddNode(volume, 0, new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10., new TGeoRotation(rot)));
        bIsModuleOn[module - 1] = kTRUE;
        AddSensitiveVolume(volume);
      }
    }
  }

  TGeoVolume *flayer12 = new TGeoVolumeAssembly("EmcApd12");
  if (bIsModuleOn[0])
    flayer12->AddNode(flayer1, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));
  if (bIsModuleOn[1])
    flayer12->AddNode(flayer2, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));
  TString vname = "cave";
  vname = vname.Strip();
  TGeoVolume *vcave = gGeoManager->FindVolumeFast(vname.Data());
  if (bIsModuleOn[0] || bIsModuleOn[1])
    vcave->AddNode(flayer12, 1);
  if (bIsModuleOn[5])
    vcave->AddNode(flayer6, 1);

  // 15 copies for barrel part of EMC
  if (bIsModuleOn[0] || bIsModuleOn[1])
    for (Int_t n = 1; n <= 15; n++) {
      TGeoRotation rot1;
      rot1.RotateZ(22.5 * n);
      vcave->AddNode(flayer12, n + 1, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(rot1)));
    }
}

// -----   Private method AddHit   --------------------------------------------
PndEmcApdPoint *PndEmcApd::AddHit(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss, Short_t mod, Short_t row,
                                  Short_t crys, Short_t copy, Short_t flag)
{
  TClonesArray &clref = *fApdCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel > 1)
    cout << "-I- PndEmcApd: Adding Point at IN (" << pos.X() << ", " << pos.Y() << ", " << pos.Z() << ") cm, detector " << detID << ", evt " << evtID << ", track " << trackID
         << ", energy loss " << eLoss * 1e06 << " keV, module " << mod << " row " << row << " crystal " << crys << " copy " << copy << endl;

  return new (clref[size]) PndEmcApdPoint(trackID, detID, evtID, pos, mom, time, length, eLoss, mod, row, crys, copy, flag);
}

inline void PndEmcApd::ResetParameters()
{
  fTrackID = -999;
  fVolumeID = -999;
  fEventID = -999;
  fPos.SetXYZT(0., 0., 0., 0.);
  fMom.SetXYZT(0., 0., 0., 0.);
  fTime = -999;
  fLength = -999;
  fELoss = -999;
}

// ----

ClassImp(PndEmcApd)
