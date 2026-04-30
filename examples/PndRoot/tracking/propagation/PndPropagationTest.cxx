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
// -----                    PndPropagationTest source file           -----
// -------------------------------------------------------------------------

#include "PndPropagationTest.h"

#include "FairMCPoint.h"
#include "FairTrackParH.h"
#include "FairRootManager.h"

#include "PndMCTrack.h"
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
PndPropagationTest::PndPropagationTest() : PndPersistencyTask("PropagationTest", 1) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndPropagationTest::PndPropagationTest(Int_t propParticle, Int_t iVerbose) : PndPersistencyTask("PropagationTest", iVerbose), fPropPdg(propParticle) {}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndPropagationTest::PndPropagationTest(const char *name, Int_t iVerbose) : PndPersistencyTask(name, iVerbose) {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndPropagationTest::~PndPropagationTest() {}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndPropagationTest::SetParContainers() {}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus PndPropagationTest::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();
  if (fDetArrays.size() == 0) {
    fDetArrays["IdealTubePoint"] = nullptr;
  }

  for (auto &array : fDetArrays) {
    array.second = static_cast<TClonesArray *>(ioman->GetObject(array.first));
    if (array.second == nullptr) {
    }
  }

  fPhotonDetector = static_cast<TClonesArray *>(ioman->GetObject(fPhotonDetectorName));

  fMcTracks = static_cast<TClonesArray *>(ioman->GetObject("MCTrack"));
  fPhotonStart = FairRootManager::Instance()->Register("PhotonStart", "PndMCTrack", "prop", kTRUE);

  fGeanePropagator = new FairGeanePro();

  fHistos["hDiffXYZ"] = new TH1D("hDiffXYZ", "hDiffXYZ", 400, 0, 40);
  fHistos["hDiffPt"] = new TH1D("hDiffPt", "hDiffPt", 400, -1, 1);
  fHistos["hDiffPz"] = new TH1D("hDiffPz", "hDiffPz", 400, -1, 1);
  fHistos["hSimPt"] = new TH1D("hSimPt", "hSimPt", 400, 0, 2);
  fHistos["hPropPt"] = new TH1D("hPropPt", "hPropPt", 400, 0, 2);
  fHistos["hSimPz"] = new TH1D("hSimPz", "hSimPz", 400, 0, 2);
  fHistos["hPropPz"] = new TH1D("hPropPz", "hPropPz", 400, 0, 2);

  fHistos["hPullX"] = new TH1D("hPullX", "hPullX", 100, -4, 4);
  fHistos["hPullY"] = new TH1D("hPullY", "hPullY", 100, -4, 4);
  fHistos["hPullZ"] = new TH1D("hPullZ", "hPullZ", 100, -4, 4);
  fHistos["hPullPx"] = new TH1D("hPullPx", "hPullPx", 100, -4, 4);
  fHistos["hPullPy"] = new TH1D("hPullPy", "hPullPy", 100, -4, 4);
  fHistos["hPullPz"] = new TH1D("hPullPz", "hPullPz", 100, -4, 4);

  fHistos["hGFDiffXYZ"] = new TH1D("hGFDiffXYZ", "hGFDiffXYZ", 400, 0, 40);
  fHistos["hGFDiffPt"] = new TH1D("hGFDiffPt", "hGFDiffPt", 400, -1, 1);
  fHistos["hGFDiffPz"] = new TH1D("hGFDiffPz", "hGFDiffPz", 400, -1, 1);
  fHistos["hGFPropPt"] = new TH1D("hGFPropPt", "hGFPropPt", 400, 0, 2);
  fHistos["hGFPropPz"] = new TH1D("hGFPropPz", "hGFPropPz", 400, 0, 2);

  fHistos["hGFCorrADiffXYZ"] = new TH1D("hGFCorrADiffXYZ", "hGFCorrADiffXYZ", 400, 0, 40);
  fHistos["hGFCorrADiffPt"] = new TH1D("hGFCorrADiffPt", "hGFCorrADiffPt", 400, -0.04, 0.04);
  fHistos["hGFCorrADiffPz"] = new TH1D("hGFCorrADiffPz", "hGFCorrADiffPz", 400, -1, 1);
  fHistos["hGFCorrAPropPt"] = new TH1D("hGFCorrAPropPt", "hGFCorrAPropPt", 400, 0, 2);
  fHistos["hGFCorrAPropPz"] = new TH1D("hGFCorrAPropPz", "hGFCorrAPropPz", 400, 0, 2);

  fHistos["hGFCorrBDiffXYZ"] = new TH1D("hGFCorrBDiffXYZ", "hGFCorrBDiffXYZ", 400, 0, 40);
  fHistos["hGFCorrBDiffPt"] = new TH1D("hGFCorrBDiffPt", "hGFCorrBDiffPt", 400, -0.04, 0.04);
  fHistos["hGFCorrBDiffPz"] = new TH1D("hGFCorrBDiffPz", "hGFCorrBDiffPz", 400, -1, 1);
  fHistos["hGFCorrBPropPt"] = new TH1D("hGFCorrBPropPt", "hGFCorrBPropPt", 400, 0, 2);
  fHistos["hGFCorrBPropPz"] = new TH1D("hGFCorrBPropPz", "hGFCorrBPropPz", 400, 0, 2);

  fHistos["hRadLength"] = new TH1D("hRadLength", "hRadLength", 1000, 0, 1);

  f2DHistos["hPointProjection"] = new TH2D("hPointProjection", "hPointProjection", 400, -10, 10, 720, 54 * 0, 54 * TMath::Pi());
  f2DHistos["hPropProjection"] = new TH2D("hPropProjection", "hPropProjection", 400, -10, 10, 720, 54 * 0, 54 * TMath::Pi());
  f2DHistos["hPhotonProjection"] = new TH2D("hPhotonProjection", "hPhotonProjection", 400, -10, 10, 720, 54 * 0, 54 * TMath::Pi());

  f2DHistos["photonMomDiffMCXY"] = new TH2D("photonMomDiffMCXY", "photonMomDiffMCXY", 100, -0.04, 0.04, 100, -0.04, 0.04);
  f2DHistos["photonMomDiffHitXY"] = new TH2D("photonMomDiffHitXY", "photonMomDiffHitXY", 100, -0.04, 0.04, 100, -0.04, 0.04);
  f2DHistos["photonMomDiffMCTZ"] = new TH2D("photonMomDiffMCTZ", "photonMomDiffMCTZ", 100, -0.04, 0.04, 100, -0.04, 0.04);
  f2DHistos["photonMomDiffHitTZ"] = new TH2D("photonMomDiffHitTZ", "photonMomDiffHitTZ", 100, -0.04, 0.04, 100, -0.04, 0.04);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndPropagationTest::Exec(Option_t *)
{

  std::cout << "*** EventNr: " << FairRootManager::Instance()->GetEntryNr() << " ***" << std::endl;
  fPhotonStart->Delete();
  for (auto array : fDetArrays) {
    std::cout << "Checking detector: " << array.first << std::endl;
    TClonesArray *det = array.second;
    bool onePass = true; // just check for the first electron. Otherwise curling electrons may cause problems.
    for (int i = 0; i < det->GetEntries() && onePass == true; i++) {
      FairMCPoint *point = static_cast<FairMCPoint *>(det->At(i));
      std::vector<FairLink> mcLinks = point->GetSortedMCTracks();
      if (mcLinks.size() > 0) {
        PndMCTrack *mcTrack = static_cast<PndMCTrack *>(fMcTracks->At(mcLinks[0].GetIndex()));

        if (mcTrack->GetMotherID() == -1 && mcTrack->GetPdgCode() == 11) { // use only primary electron
          onePass = false;
          PndMCTrack *mcTrack = static_cast<PndMCTrack *>(fMcTracks->At(mcLinks[0].GetIndex()));
          //          std::cout << "MCTrack: ";
          //          mcTrack->Print();
          //          std::cout << std::endl;
          TVector3 mom = mcTrack->GetMomentum();
          TVector3 momError(mom.Pt() * 0.01, mom.Pt() * 0.01, mom.Pz() * 0.01);
          TVector3 posError(0.001, 0.001, 0.001);
          TParticlePDG *ppdg = TDatabasePDG::Instance()->GetParticle(mcTrack->GetPdgCode());
          FairTrackParH inPar(mcTrack->GetStartVertex(), mom, posError, momError, ppdg->Charge());
          FairTrackParH outPar;

          TVector3 pointPos;     // position of MC point
          TVector3 pointMom;     // momentum at MC point
          TVector3 propPos;      // position of propagated point
          TVector3 propPosError; // error on propagated point
          TVector3 propMom;      // momentum at propagated point
          TVector3 propMomError; // error on propagated momentum

          point->Position(pointPos);
          point->Momentum(pointMom);

          //          fGeanePropagator->SetPCAPropagation(1, 1, inPar);
          //          PCAOutputStruct result = fGeanePropagator->FindPCA(1, fPropPdg, pointPos);

          fGeanePropagator->SetPoint(pointPos);
          fGeanePropagator->PropagateToPCA(1, 1);

          Bool_t rc = fGeanePropagator->Propagate(&inPar, &outPar, fPropPdg); // First propagation at module

          if (rc == 0) {
            fGeaneNotFitted++;
          }

          propPos = outPar.GetPosition();
          propPosError.SetXYZ(outPar.GetDX(), outPar.GetDY(), outPar.GetDZ());
          propMom = outPar.GetMomentum();
          propMomError.SetXYZ(outPar.GetDPx(), outPar.GetDPy(), outPar.GetDPz());

          //          std::cout << "SciTilPoint: " << pointPos.X() << "/" << pointPos.Y() << "/" << pointPos.Z() << std::endl;
          //          std::cout << rc << " " << propPos.X() << "/" << propPos.Y() << "/" << propPos.Z() << std::endl;
          //          std::cout << outPar.GetDX() << "/" << outPar.GetDY() << "/" << outPar.GetDZ() << std::endl;
          //          std::cout << outPar.GetDPx() << "/" << outPar.GetDPy() << "/" << outPar.GetDPz() << std::endl;

          TVector3 diffPos = pointPos - propPos;
          TVector3 diffMom = pointMom - propMom;
          double diffPt = pointMom.Pt() - propMom.Pt();

          fHistos["hDiffXYZ"]->Fill(diffPos.Mag());
          fHistos["hPullX"]->Fill(diffPos.X() / propPosError.X());
          fHistos["hPullY"]->Fill(diffPos.Y() / propPosError.Y());
          fHistos["hPullZ"]->Fill(diffPos.Z() / propPosError.Z());
          fHistos["hDiffPt"]->Fill(diffPt);
          fHistos["hDiffPz"]->Fill(diffMom.Z());

          fHistos["hPullPx"]->Fill(diffMom.X() / propMomError.X());
          fHistos["hPullPy"]->Fill(diffMom.Y() / propMomError.Y());
          fHistos["hPullPz"]->Fill(diffMom.Z() / propMomError.Z());

          fHistos["hSimPt"]->Fill(pointMom.Pt());
          fHistos["hPropPt"]->Fill(propMom.Pt());
          fHistos["hSimPz"]->Fill(pointMom.Z());
          fHistos["hPropPz"]->Fill(propMom.Z());

          f2DHistos["hPointProjection"]->Fill(pointPos.Z(), pointPos.Phi() * pointPos.Perp());

          TVector3 rkPos;
          TVector3 rkMom;

          genfit::FieldManager::getInstance()->init(new PndGenfitField2());
          genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());
          genfit::MaterialEffects::getInstance()->setEnergyLossBrems(false);

          genfit::RKTrackRep trackrep(fPropPdg);
          genfit::StateOnPlane state(&trackrep);
          state.setPosMom(inPar.GetPosition(), inPar.GetMomentum());
          state.setChargeSign(ppdg->Charge());
          try {
            std::cout << state.extrapolateToCylinder(54.0) << std::endl;
          } catch (const genfit::Exception &e) {
            std::cout << "Exception caught: " << e.what() << std::endl;
            fGenfitNotFitted++;
          }

          trackrep.getPosMom(state, rkPos, rkMom);
          //          std::cout << "RKPos: " << rkPos.X() << "/" << rkPos.Y() << "/" << rkPos.Z() << std::endl;
          //          std::cout << "RadLength: " << trackrep->getRadiationLenght() << std::endl;
          //          std::cout << "SciTilPointMom " << pointMom.X() << "/" << pointMom.Y() << "/" << pointMom.Z() << std::endl;
          //          std::cout << "RKMom: " << rkMom.X() << "/" << rkMom.Y() << "/" << rkMom.Z() << std::endl;

          fHistos["hRadLength"]->Fill(trackrep.getRadiationLenght());

          fHistos["hGFDiffXYZ"]->Fill((pointPos - rkPos).Mag());
          fHistos["hGFDiffPt"]->Fill(pointMom.Pt() - rkMom.Pt());
          fHistos["hGFDiffPz"]->Fill(pointMom.Z() - rkMom.Z());
          fHistos["hGFPropPt"]->Fill(rkMom.Pt());
          fHistos["hGFPropPz"]->Fill(rkMom.Z());

          TH1 *cumulativeXYZ = fHistos["hGFDiffXYZ"]->GetCumulative();
          fHistos[cumulativeXYZ->GetName()] = cumulativeXYZ;

          state.setPosMom(inPar.GetPosition(), inPar.GetMomentum());
          state.setChargeSign(ppdg->Charge());
          try {
            std::cout << state.extrapolateToCylinder(fInnerExtrapoRadius) << std::endl; // middle of silicon layer
          } catch (const genfit::Exception &e) {
            std::cout << "Exception caught: " << e.what() << std::endl;
            fGenfitNotFitted++;
          }

          TVector3 rkInnerPos;
          TVector3 rkInnerMom;

          trackrep.getPosMom(state, rkInnerPos, rkInnerMom);
          //          std::cout << "RKPos SiliconLayer: " << rkPos.X() << "/" << rkPos.Y() << "/" << rkPos.Z() << std::endl;

          TVector3 photonMomentum = GetMCMomentumCorrection();
          //          std::cout << "MC PhotonMom: " << photonMomentum.X() << "/" << photonMomentum.Y() << "/" << photonMomentum.Z() << std::endl;
          TVector3 rkCorrMom = rkInnerMom - photonMomentum;
          state.setPosMom(rkInnerPos, rkCorrMom);
          state.setChargeSign(ppdg->Charge());

          try {
            std::cout << state.extrapolateToCylinder(54.0) << std::endl; // outer layer
          } catch (const genfit::Exception &e) {
            std::cout << "Exception caught: " << e.what() << std::endl;
            fGenfitNotFitted++;
          }

          trackrep.getPosMom(state, rkPos, rkMom);
          //          std::cout << "SciTilPoint: " << pointPos.X() << "/" << pointPos.Y() << "/" << pointPos.Z() << std::endl;
          //          std::cout << "RKPosA corrected: " << rkPos.X() << "/" << rkPos.Y() << "/" << rkPos.Z() << std::endl;
          //          std::cout << "SciTilPointMom " << pointMom.X() << "/" << pointMom.Y() << "/" << pointMom.Z() << std::endl;
          //          std::cout << "RKMomA corrected: " << rkMom.X() << "/" << rkMom.Y() << "/" << rkMom.Z() << std::endl;

          fHistos["hGFCorrADiffXYZ"]->Fill((pointPos - rkPos).Mag());
          fHistos["hGFCorrADiffPt"]->Fill(pointMom.Pt() - rkMom.Pt());
          fHistos["hGFCorrADiffPz"]->Fill(pointMom.Z() - rkMom.Z());
          fHistos["hGFCorrAPropPt"]->Fill(rkMom.Pt());
          fHistos["hGFCorrAPropPz"]->Fill(rkMom.Z());

          f2DHistos["hPropProjection"]->Fill(rkPos.Z(), rkPos.Phi() * rkPos.Perp());

          TH1 *cumulativeCorrAXYZ = fHistos["hGFCorrADiffXYZ"]->GetCumulative();
          fHistos[cumulativeCorrAXYZ->GetName()] = cumulativeCorrAXYZ;

          photonMomentum = GetEnergyHitCorrection(rkInnerPos);

          //          std::cout << "Energie PhotonMom: " << photonMomentum.X() << "/" << photonMomentum.Y() << "/" << photonMomentum.Z() << std::endl;
          rkCorrMom = rkInnerMom - photonMomentum;
          state.setPosMom(rkInnerPos, rkCorrMom);
          state.setChargeSign(ppdg->Charge());

          try {
            std::cout << state.extrapolateToCylinder(54.0) << std::endl; // outer layer
          } catch (const genfit::Exception &e) {
            std::cout << "Exception caught: " << e.what() << std::endl;
            fGenfitNotFitted++;
          }

          trackrep.getPosMom(state, rkPos, rkMom);
          //          std::cout << "SciTilPoint: " << pointPos.X() << "/" << pointPos.Y() << "/" << pointPos.Z() << std::endl;
          //          std::cout << "RKPosB corrected: " << rkPos.X() << "/" << rkPos.Y() << "/" << rkPos.Z() << std::endl;
          //          std::cout << "SciTilPointMom " << pointMom.X() << "/" << pointMom.Y() << "/" << pointMom.Z() << std::endl;
          //          std::cout << "RKMomB corrected: " << rkMom.X() << "/" << rkMom.Y() << "/" << rkMom.Z() << std::endl;

          fHistos["hGFCorrBDiffXYZ"]->Fill((pointPos - rkPos).Mag());
          fHistos["hGFCorrBDiffPt"]->Fill(pointMom.Pt() - rkMom.Pt());
          fHistos["hGFCorrBDiffPz"]->Fill(pointMom.Z() - rkMom.Z());
          fHistos["hGFCorrBPropPt"]->Fill(rkMom.Pt());
          fHistos["hGFCorrBPropPz"]->Fill(rkMom.Z());

          TH1 *cumulativeCorrBXYZ = fHistos["hGFCorrBDiffXYZ"]->GetCumulative();
          fHistos[cumulativeCorrBXYZ->GetName()] = cumulativeCorrBXYZ;
        }
      }
    }
    for (int i = 0; i < det->GetEntries(); i++) {
      FairMCPoint *point = static_cast<FairMCPoint *>(det->At(i));
      if (IsPhotonFromMother(point, 11)) {
        std::vector<FairLink> mcLinks = point->GetSortedMCTracks();
        if (mcLinks.size() > 0) {
          PndMCTrack *mcTrack = static_cast<PndMCTrack *>(fMcTracks->At(mcLinks[0].GetIndex()));
          new ((*fPhotonStart)[fPhotonStart->GetEntries()]) PndMCTrack(*mcTrack);
        }
      }
    }
  }
  return;
}

// -----   Private method ReInit   -----------------------------------------
InitStatus PndPropagationTest::ReInit()
{
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void PndPropagationTest::Reset() {}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndPropagationTest::Finish()
{

  TFile *f = new TFile("PropagationHistos.root", "RECREATE");
  f->cd();
  for (auto histo : fHistos) {
    histo.second->Write();
  }

  for (auto histo2D : f2DHistos) {
    histo2D.second->Write();
  }
  std::cout << "Geane not fitted: " << fGeaneNotFitted << std::endl;
  std::cout << "Genfit not fitted: " << fGenfitNotFitted << std::endl;
}
// -------------------------------------------------------------------------

// TVector3 PndPropagationTest::GetPhotonCorrection(int option)
//{
//  if (option == 1) {
//    return GetMCMomentumCorrection();
//  } else if (option == 2) {
//    return GetEnergyHitCorrection();
//  }
//  return TVector3();
//}
// -------------------------------------------------------------------------

TVector3 PndPropagationTest::GetMCMomentumCorrection()
{
  TVector3 photonMomentum;
  //  std::cout << "GetMCMomentumCorrection" << std::endl;
  for (int j = 0; j < fPhotonDetector->GetEntries(); j++) { // go through all points on detector
    FairMCPoint *photonPoint = static_cast<FairMCPoint *>(fPhotonDetector->At(j));
    if (IsPhotonFromMother(photonPoint, 11)) {
      std::vector<FairLink> mcLinks = photonPoint->GetSortedMCTracks();
      if (mcLinks.size() > 0) { // check if their mother is the initial electron (should be photons)
        PndMCTrack *mcTrack = static_cast<PndMCTrack *>(fMcTracks->At(mcLinks[0].GetIndex()));
        if (mcTrack->GetStartVertex().Pt() > 4)
          continue;

        TVector3 currentMom;
        TVector3 currentPos;
        photonPoint->Momentum(currentMom);
        photonPoint->Position(currentPos);
        photonMomentum += currentMom;

        TVector3 momDiff = mcTrack->GetMomentum() - currentMom;
        f2DHistos["photonMomDiffMCXY"]->Fill(momDiff.X(), momDiff.Y());
        f2DHistos["photonMomDiffMCTZ"]->Fill(momDiff.Pt(), momDiff.Z());
        f2DHistos["hPhotonProjection"]->Fill(currentPos.Z(), currentPos.Perp() * currentPos.Phi());
      }
    }
  }
  return photonMomentum;
}
// -------------------------------------------------------------------------

TVector3 PndPropagationTest::GetEnergyHitCorrection(TVector3 &innerPos)
{
  TVector3 photonMomentum;
  //  std::cout << "GetEnergyHitCorrection" << std::endl;

  for (int j = 0; j < fPhotonDetector->GetEntries(); j++) { // go through all points on detector
    FairMCPoint *photonPoint = static_cast<FairMCPoint *>(fPhotonDetector->At(j));
    if (IsPhotonFromMother(photonPoint, 11)) {
      std::vector<FairLink> mcLinks = photonPoint->GetSortedMCTracks();
      if (mcLinks.size() > 0) { // check if their mother is the initial electron (should be photons)
        PndMCTrack *mcTrack = static_cast<PndMCTrack *>(fMcTracks->At(mcLinks[0].GetIndex()));
        if (mcTrack->GetStartVertex().Pt() > 4)
          continue;
      }
      TVector3 photonMom;
      TVector3 photonPos;
      TVector3 currentPos = innerPos;
      photonPoint->Momentum(photonMom);
      photonPoint->Position(photonPos);
      TVector3 direction = photonPos - currentPos;

      //      std::cout << j << " : photPos " << photonPos.X() << "/" << photonPos.Y() << "/" << photonPos.Z() << std::endl;
      //      std::cout << j << " : hitPos " << currentPos.X() << "/" << currentPos.Y() << "/" << currentPos.Z() << std::endl;
      //      std::cout << j << " : direction " << direction.X() << "/" << direction.Y() << "/" << direction.Z() << std::endl;

      direction = direction.Unit();
      //      std::cout << j << " : unitVector " << direction.X() << "/" << direction.Y() << "/" << direction.Z() << std::endl;

      direction *= photonMom.Mag();

      //      std::cout << j << " : mag " << photonMom.Mag() << " * " << direction.X() << "/" << direction.Y() << "/" << direction.Z() << std::endl;

      photonMomentum += direction;
      //      std::vector<FairLink> mcLinks = photonPoint->GetSortedMCTracks();
      if (mcLinks.size() > 0) { // check if their mother is the initial electron (should be photons)
        PndMCTrack *mcTrack = static_cast<PndMCTrack *>(fMcTracks->At(mcLinks[0].GetIndex()));
        TVector3 momDiff = mcTrack->GetMomentum() - direction;
        f2DHistos["photonMomDiffHitXY"]->Fill(momDiff.X(), momDiff.Y());
        f2DHistos["photonMomDiffHitTZ"]->Fill(momDiff.Pt(), momDiff.Z());
      }
    }
  }
  //  std::cout << " photonMom " << photonMomentum.X() << "/" << photonMomentum.Y() << "/" << photonMomentum.Z() << std::endl;

  return photonMomentum;
}

Bool_t PndPropagationTest::IsPhotonFromMother(FairMCPoint *point, Int_t motherPdgCode)
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

ClassImp(PndPropagationTest);
