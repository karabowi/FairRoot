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
// -----                    PndElectronCorrectionTest source file           -----
// -------------------------------------------------------------------------

#include "PndElectronCorrectionTest.h"

#include "FairMCPoint.h"
#include "FairTrackParH.h"
#include "FairRootManager.h"

#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndEmcBump.h"
#include "PndGenfitField2.h"
#include "MaterialEffects.h"
#include "TGeoMaterialInterface.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "RKTrackRep.h"
#include "StateOnPlane.h"
#include "FieldManager.h"
#include "Exception.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include <iostream>

// -----   Default constructor   ------------------------------------------
PndElectronCorrectionTest::PndElectronCorrectionTest() : PndPersistencyTask("ElectronCorrectionTest", 1) {}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndElectronCorrectionTest::PndElectronCorrectionTest(const char *name, Int_t iVerbose) : PndPersistencyTask(name, iVerbose) {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndElectronCorrectionTest::~PndElectronCorrectionTest() {}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndElectronCorrectionTest::SetParContainers() {}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus PndElectronCorrectionTest::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();
  if (fTrackArrays.size() == 0) {
    fTrackArrays["BarrelTrack"] = nullptr;
  }

  for (auto &array : fTrackArrays) {
    array.second = static_cast<TClonesArray *>(ioman->GetObject(array.first));
    if (array.second == nullptr) {
    }
  }

  fEmcDetector = static_cast<TClonesArray *>(ioman->GetObject(fEmcDetectorName));

  fMcTracks = static_cast<TClonesArray *>(ioman->GetObject("MCTrack"));
  fPhotonStart = FairRootManager::Instance()->Register("PhotonStart", "PndMCTrack", "prop", kTRUE);

  // fGeanePropagator = new FairGeanePro();

  fHistos["hPhotonMomentum"] = new TH1D("hPhotonMomentum", "hPhotonMomentum", 4000, 0, 40);
  fHistos["hPhotonsPerE"] = new TH1D("hPhotonsPerE", "hPhotonsPerE", 20, 0, 20);

  f2DHistos["hRelativePhotonPosition"] = new TH2D("hRelativePhotonPosition", "hRelativePhotonPosition", 720, -9, 9, 1440, -18, 18);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndElectronCorrectionTest::Exec(Option_t *)
{

  std::cout << "*** EventNr: " << FairRootManager::Instance()->GetEntryNr() << " ***" << std::endl;
  fPhotonStart->Delete();
  for (auto array : fTrackArrays) {
    std::cout << "Checking tracks: " << array.first << " " << array.second->GetEntries() << std::endl;
    TClonesArray *trackArray = array.second;
    bool onePass = true; // just check for the first electron. Otherwise curling electrons may cause problems.
    for (int i = 0; i < trackArray->GetEntries() && onePass == true; i++) {
      PndTrack *track = static_cast<PndTrack *>(trackArray->At(i));
      std::vector<FairLink> mcLinks = track->GetSortedMCTracks();
      if (mcLinks.size() > 0) {
        PndMCTrack *mcTrack = static_cast<PndMCTrack *>(fMcTracks->At(mcLinks[0].GetIndex()));

        if (mcTrack->GetMotherID() == -1 && mcTrack->GetPdgCode() == 11) { // use only primary electron
          onePass = false;
          std::cout << "Track: " << *track << std::endl;

          std::cout << "MCTrack: ";
          mcTrack->Print();
          std::cout << std::endl;

          for (int emcBumpIndex = 0; emcBumpIndex < fEmcDetector->GetEntries(); emcBumpIndex++) {
            PndEmcBump *emcBump = static_cast<PndEmcBump *>(fEmcDetector->At(emcBumpIndex));
            std::cout << emcBumpIndex << " " << emcBump << std::endl;
            std::vector<FairLink> mcLinksBump = emcBump->GetSortedMCTracks();
            int photonsPerE = 0;
            if (mcLinksBump.size() > 0) {
              std::cout << "MCLinkBump " << mcLinksBump[0] << std::endl;
              std::cout << "MCLinkBump " << *emcBump << std::endl;

              PndMCTrack *bumpTrack = static_cast<PndMCTrack *>(fMcTracks->At(mcLinksBump[0].GetIndex()));

              std::cout << emcBumpIndex << " BumpMCTrack: ";
              bumpTrack->Print();
              std::cout << std::endl;
              if (bumpTrack->GetPdgCode() == 22 && bumpTrack->GetMotherID() == 0) {
                photonsPerE++;
                TVector3 photonMom = bumpTrack->GetMomentum();
                TVector3 eMCMom = mcTrack->GetMomentum();
                TVector3 diff = photonMom - eMCMom;
                fHistos["hPhotonMomentum"]->Fill(photonMom.Mag());
                f2DHistos["hRelativePhotonPosition"]->Fill(diff.Theta(), diff.Phi());
              }
            }
            fHistos["hPhotonsPerE"]->Fill(photonsPerE);
          }

          //          TVector3 rkPos;
          //          TVector3 rkMom;
          //
          //          genfit::FieldManager::getInstance()->init(new PndGenfitField2());
          //          genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());
          //          genfit::MaterialEffects::getInstance()->setEnergyLossBrems(false);
          //
          //          genfit::RKTrackRep trackrep(fPropPdg);
          //          genfit::StateOnPlane state(&trackrep);
          //          state.setPosMom(inPar.GetPosition(), inPar.GetMomentum());
          //          state.setChargeSign(ppdg->Charge());
          //          try {
          //            std::cout << state.extrapolateToCylinder(54.0) << std::endl;
          //          } catch (const genfit::Exception &e) {
          //            std::cout << "Exception caught: " << e.what() << std::endl;
          //            fGenfitNotFitted++;
          //          }
          //
          //          trackrep.getPosMom(state, rkPos, rkMom);
          //          //          std::cout << "RKPos: " << rkPos.X() << "/" << rkPos.Y() << "/" << rkPos.Z() << std::endl;
          //          //          std::cout << "RadLength: " << trackrep->getRadiationLenght() << std::endl;
          //          //          std::cout << "SciTilPointMom " << pointMom.X() << "/" << pointMom.Y() << "/" << pointMom.Z() << std::endl;
          //          //          std::cout << "RKMom: " << rkMom.X() << "/" << rkMom.Y() << "/" << rkMom.Z() << std::endl;
          //
          //          fHistos["hRadLength"]->Fill(trackrep.getRadiationLenght());
          //
          //          fHistos["hGFDiffXYZ"]->Fill((pointPos - rkPos).Mag());
          //          fHistos["hGFDiffPt"]->Fill(pointMom.Pt() - rkMom.Pt());
          //          fHistos["hGFDiffPz"]->Fill(pointMom.Z() - rkMom.Z());
          //          fHistos["hGFPropPt"]->Fill(rkMom.Pt());
          //          fHistos["hGFPropPz"]->Fill(rkMom.Z());
          //
          //          TH1 *cumulativeXYZ = fHistos["hGFDiffXYZ"]->GetCumulative();
          //          fHistos[cumulativeXYZ->GetName()] = cumulativeXYZ;
          //
          //          state.setPosMom(inPar.GetPosition(), inPar.GetMomentum());
          //          state.setChargeSign(ppdg->Charge());
          //          try {
          //            std::cout << state.extrapolateToCylinder(fInnerExtrapoRadius) << std::endl; // middle of silicon layer
          //          } catch (const genfit::Exception &e) {
          //            std::cout << "Exception caught: " << e.what() << std::endl;
          //            fGenfitNotFitted++;
          //          }
          //
          //          TVector3 rkInnerPos;
          //          TVector3 rkInnerMom;
          //
          //          trackrep.getPosMom(state, rkInnerPos, rkInnerMom);
          //          //          std::cout << "RKPos SiliconLayer: " << rkPos.X() << "/" << rkPos.Y() << "/" << rkPos.Z() << std::endl;
          //
          //          TVector3 photonMomentum = GetMCMomentumCorrection();
          //          //          std::cout << "MC PhotonMom: " << photonMomentum.X() << "/" << photonMomentum.Y() << "/" << photonMomentum.Z() << std::endl;
          //          TVector3 rkCorrMom = rkInnerMom - photonMomentum;
          //          state.setPosMom(rkInnerPos, rkCorrMom);
          //          state.setChargeSign(ppdg->Charge());
          //
          //          try {
          //            std::cout << state.extrapolateToCylinder(54.0) << std::endl; // outer layer
          //          } catch (const genfit::Exception &e) {
          //            std::cout << "Exception caught: " << e.what() << std::endl;
          //            fGenfitNotFitted++;
          //          }
          //
          //          trackrep.getPosMom(state, rkPos, rkMom);
          //          //          std::cout << "SciTilPoint: " << pointPos.X() << "/" << pointPos.Y() << "/" << pointPos.Z() << std::endl;
          //          //          std::cout << "RKPosA corrected: " << rkPos.X() << "/" << rkPos.Y() << "/" << rkPos.Z() << std::endl;
          //          //          std::cout << "SciTilPointMom " << pointMom.X() << "/" << pointMom.Y() << "/" << pointMom.Z() << std::endl;
          //          //          std::cout << "RKMomA corrected: " << rkMom.X() << "/" << rkMom.Y() << "/" << rkMom.Z() << std::endl;
          //
          //          fHistos["hGFCorrADiffXYZ"]->Fill((pointPos - rkPos).Mag());
          //          fHistos["hGFCorrADiffPt"]->Fill(pointMom.Pt() - rkMom.Pt());
          //          fHistos["hGFCorrADiffPz"]->Fill(pointMom.Z() - rkMom.Z());
          //          fHistos["hGFCorrAPropPt"]->Fill(rkMom.Pt());
          //          fHistos["hGFCorrAPropPz"]->Fill(rkMom.Z());
          //
          //          f2DHistos["hPropProjection"]->Fill(rkPos.Z(), rkPos.Phi() * rkPos.Perp());
          //
          //          TH1 *cumulativeCorrAXYZ = fHistos["hGFCorrADiffXYZ"]->GetCumulative();
          //          fHistos[cumulativeCorrAXYZ->GetName()] = cumulativeCorrAXYZ;
          //
          //          photonMomentum = GetEnergyHitCorrection(rkInnerPos);
          //
          //          //          std::cout << "Energie PhotonMom: " << photonMomentum.X() << "/" << photonMomentum.Y() << "/" << photonMomentum.Z() << std::endl;
          //          rkCorrMom = rkInnerMom - photonMomentum;
          //          state.setPosMom(rkInnerPos, rkCorrMom);
          //          state.setChargeSign(ppdg->Charge());
          //
          //          try {
          //            std::cout << state.extrapolateToCylinder(54.0) << std::endl; // outer layer
          //          } catch (const genfit::Exception &e) {
          //            std::cout << "Exception caught: " << e.what() << std::endl;
          //            fGenfitNotFitted++;
          //          }
          //
          //          trackrep.getPosMom(state, rkPos, rkMom);
          //          //          std::cout << "SciTilPoint: " << pointPos.X() << "/" << pointPos.Y() << "/" << pointPos.Z() << std::endl;
          //          //          std::cout << "RKPosB corrected: " << rkPos.X() << "/" << rkPos.Y() << "/" << rkPos.Z() << std::endl;
          //          //          std::cout << "SciTilPointMom " << pointMom.X() << "/" << pointMom.Y() << "/" << pointMom.Z() << std::endl;
          //          //          std::cout << "RKMomB corrected: " << rkMom.X() << "/" << rkMom.Y() << "/" << rkMom.Z() << std::endl;
          //
          //          fHistos["hGFCorrBDiffXYZ"]->Fill((pointPos - rkPos).Mag());
          //          fHistos["hGFCorrBDiffPt"]->Fill(pointMom.Pt() - rkMom.Pt());
          //          fHistos["hGFCorrBDiffPz"]->Fill(pointMom.Z() - rkMom.Z());
          //          fHistos["hGFCorrBPropPt"]->Fill(rkMom.Pt());
          //          fHistos["hGFCorrBPropPz"]->Fill(rkMom.Z());
          //
          //          TH1 *cumulativeCorrBXYZ = fHistos["hGFCorrBDiffXYZ"]->GetCumulative();
          //          fHistos[cumulativeCorrBXYZ->GetName()] = cumulativeCorrBXYZ;
          //        }
          //      }
          //    }
          //    for (int i = 0; i < det->GetEntries(); i++) {
          //      FairMCPoint *point = static_cast<FairMCPoint *>(det->At(i));
          //      if (IsPhotonFromMother(point, 11)) {
          //        std::vector<FairLink> mcLinks = point->GetSortedMCTracks();
          //        if (mcLinks.size() > 0) {
          //          PndMCTrack *mcTrack = static_cast<PndMCTrack *>(fMcTracks->At(mcLinks[0].GetIndex()));
          //          new ((*fPhotonStart)[fPhotonStart->GetEntries()]) PndMCTrack(*mcTrack);
        }
      }
    }
  }
  return;
}

// -----   Private method ReInit   -----------------------------------------
InitStatus PndElectronCorrectionTest::ReInit()
{
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void PndElectronCorrectionTest::Reset() {}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndElectronCorrectionTest::Finish()
{

  TFile *f = new TFile("PropagationHistos.root", "RECREATE");
  f->cd();
  for (auto histo : fHistos) {
    histo.second->Write();
  }

  for (auto histo2D : f2DHistos) {
    histo2D.second->Write();
  }
  //  std::cout << "Geane not fitted: " << fGeaneNotFitted << std::endl;
  //  std::cout << "Genfit not fitted: " << fGenfitNotFitted << std::endl;
}
// -------------------------------------------------------------------------

// TVector3 PndElectronCorrectionTest::GetPhotonCorrection(int option)
//{
//  if (option == 1) {
//    return GetMCMomentumCorrection();
//  } else if (option == 2) {
//    return GetEnergyHitCorrection();
//  }
//  return TVector3();
//}
// -------------------------------------------------------------------------

Bool_t PndElectronCorrectionTest::IsPhotonFromMother(FairMCPoint *point, Int_t motherPdgCode)
{
  std::vector<FairLink> mcLinks = point->GetSortedMCTracks();
  if (mcLinks.size() > 0) { // check if their mother is the initial electron (should be photons)
    PndMCTrack *mcTrack = static_cast<PndMCTrack *>(fMcTracks->At(mcLinks[0].GetIndex()));
    PndMCTrack *mother;
    if (mcTrack->GetMotherID() > -1) {
      mother = static_cast<PndMCTrack *>(fMcTracks->At(mcTrack->GetMotherID()));
      if (mcTrack->GetPdgCode() == 22) {
        if (mother->GetPdgCode() == motherPdgCode) {
          return kTRUE;
        }
      }
    }
  }
  return kFALSE;
}

ClassImp(PndElectronCorrectionTest);
