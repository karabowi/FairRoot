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
// -----                      PndMCTrack source file                   -----
// -----                  Created 03/08/04  by V. Friese (CbmMCTrack)  -----
// -----                  Created 11/02/09  by  M. Al-Turany           -----
// -------------------------------------------------------------------------

#include <iostream>
#include "TMCProcess.h" //for TMCProcessName
#include "FairLogger.h"

#include "PndMCTrack.h"
#include <limits>
using namespace std;
// -----   Default constructor   -------------------------------------------
PndMCTrack::PndMCTrack()
  : TObject(), fPdgCode(0), fPx(0), fPy(0), fPz(0), fE(0), fMotherID(-1), fSecondMotherID(-1), fGeneratorFlags(0), fStartX(0), fStartY(0), fStartZ(0), fStartT(0), fPoints(0)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
/*// Not used at all?
PndMCTrack::PndMCTrack(Int_t pdgCode, Int_t motherID, TVector3 startVertex,
           Double_t startTime, TLorentzVector momentum, Int_t  nPoint){
  fPdgCode  = pdgCode;
  fMotherID = motherID;
  fSecondMotherID = -1;
  fStartX   = startVertex.X();
  fStartY   = startVertex.Y();
  fStartZ   = startVertex.Z();
  fStartT   = startTime;
  fPx       = momentum.Px();
  fPy       = momentum.Py();
  fPz       = momentum.Pz();
  fE        = momentum.E();
  if (nPoint >= 0) fPoints = nPoint;
  else             fPoints = 0;
  fGeneratorFlags=0;
}
*/
// -------------------------------------------------------------------------

// -----   Copy constructor   ----------------------------------------------
PndMCTrack::PndMCTrack(const PndMCTrack &track)
  : TObject(track), fPdgCode(track.fPdgCode), fPx(track.fPx), fPy(track.fPy), fPz(track.fPz), fE(track.fE), fMotherID(track.fMotherID), fSecondMotherID(track.fSecondMotherID),
    fGeneratorFlags(track.fGeneratorFlags), fStartX(track.fStartX), fStartY(track.fStartY), fStartZ(track.fStartZ), fStartT(track.fStartT), fPoints(track.fPoints),
    fProcess(track.fProcess)
{
}
// -------------------------------------------------------------------------

// -----   Constructor from TParticle   ------------------------------------
PndMCTrack::PndMCTrack(TParticle *part)
  : TObject(), fPdgCode(part->GetPdgCode()), fPx(part->Px()), fPy(part->Py()), fPz(part->Pz()), fE(part->Energy()), fMotherID(part->GetMother(0)),
    fSecondMotherID(part->GetMother(1)), fGeneratorFlags(0), fStartX(part->Vx()), fStartY(part->Vy()), fStartZ(part->Vz()), fStartT(part->T() * 1e09), fPoints(0),
    fProcess(part->GetUniqueID())
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMCTrack::~PndMCTrack() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndMCTrack::Print(Int_t trackID) const
{
  cout << "Track " << trackID << ", mother : " << fMotherID << ", secondmother : " << fSecondMotherID << ", Type " << fPdgCode << ", momentum (" << fPx << "/" << fPy << "/" << fPz
       << "/" << fE << ") GeV"
       << ", position (" << fStartX << "/" << fStartY << "/" << fStartZ << "/" << fStartT << ") cm,s, Generatorflags: " << fGeneratorFlags << " caused by "
       << TMCProcessName[fProcess] << endl;
}
// -------------------------------------------------------------------------

TLorentzVector PndMCTrack::Get4Momentum() const
{
  /*
      Double_t mass=0.0;
      Double_t ene=0.0;
      TParticlePDG*
      fParticlePDG = TDatabasePDG::Instance()->GetParticle(fPdgCode);

      if (fParticlePDG)
       mass   = fParticlePDG->Mass();

      if ( mass >= 0 ) {
    ene  = TMath::Sqrt(mass*mass + fPx*fPx +fPy*fPy +fPz*fPz);
      }
  */
  return TLorentzVector(fPx, fPy, fPz, fE);
}

// -----   Public method GetNPoints   --------------------------------------
Int_t PndMCTrack::GetNPoints(PndDetectorId detId) const
{
  if (detId == PndDetectorId::kDRC)
    return ((fPoints & (3 << 0)) >> 0);
  else if (detId == PndDetectorId::kMDT)
    return ((fPoints & (3 << 2)) >> 2);
  else if (detId == PndDetectorId::kMVD)
    return ((fPoints & (3 << 4)) >> 4);
  else if (detId == PndDetectorId::kRICH)
    return ((fPoints & (3 << 6)) >> 6);
  else if (detId == PndDetectorId::kEMC)
    return ((fPoints & (3 << 8)) >> 8);
  else if (detId == PndDetectorId::kSTT || detId == PndDetectorId::kSTT2)
    return ((fPoints & (3 << 10)) >> 10);
  else if (detId == PndDetectorId::kFTOF)
    return ((fPoints & (3 << 12)) >> 12);
  else if (detId == PndDetectorId::kTOF)
    return ((fPoints & (3 << 14)) >> 14);
  else if (detId == PndDetectorId::kGEM)
    return ((fPoints & (3 << 16)) >> 16);
  else if (detId == PndDetectorId::kDSK)
    return ((fPoints & (3 << 18)) >> 18);
  else if (detId == PndDetectorId::kHYP)
    return ((fPoints & (3 << 20)) >> 20);
  else if (detId == PndDetectorId::kRPC)
    return ((fPoints & (3 << 22)) >> 22);
  else if (detId == PndDetectorId::kLUMI)
    return ((fPoints & (3 << 24)) >> 24);
  else if (detId == PndDetectorId::kHYPG)
    return ((fPoints & (3 << 26)) >> 26);
  else if (detId == PndDetectorId::kFTS)
    return ((fPoints & (3 << 28)) >> 28);
  else if (detId == PndDetectorId::kOT)
    return ((fPoints & (3 << 30)) >> 30);

  else {
    LOG(error) << " PndMCTrack::GetNPoints: Unknown detector ID " << static_cast<int>(detId);
    return 0;
  }
}
void PndMCTrack::SetNPoints(PndDetectorId iDet, Int_t nPoints)
{

  if (iDet == PndDetectorId::kDRC) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 0))) | (nPoints << 0);
  }

  else if (iDet == PndDetectorId::kMDT) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 2))) | (nPoints << 2);
  }

  else if (iDet == PndDetectorId::kMVD) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 4))) | (nPoints << 4);
  }

  else if (iDet == PndDetectorId::kRICH) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 6))) | (nPoints << 6);
  }

  else if (iDet == PndDetectorId::kEMC) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 8))) | (nPoints << 8);
  }

  else if (iDet == PndDetectorId::kSTT || iDet == PndDetectorId::kSTT2) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 10))) | (nPoints << 10);
  }

  else if (iDet == PndDetectorId::kFTOF) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 12))) | (nPoints << 12);
  }

  else if (iDet == PndDetectorId::kTOF) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 14))) | (nPoints << 14);
  }

  else if (iDet == PndDetectorId::kGEM) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 16))) | (nPoints << 16);
  } else if (iDet == PndDetectorId::kDSK) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 18))) | (nPoints << 18);
  }

  else if (iDet == PndDetectorId::kHYP) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 20))) | (nPoints << 20);
  } else if (iDet == PndDetectorId::kRPC) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 22))) | (nPoints << 22);
  }

  else if (iDet == PndDetectorId::kLUMI) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 24))) | (nPoints << 24);
  }

  else if (iDet == PndDetectorId::kHYPG) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 26))) | (nPoints << 26);
  }

  else if (iDet == PndDetectorId::kFTS) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 28))) | (nPoints << 28);
  }

  else if (iDet == PndDetectorId::kOT) {
    if (nPoints < 0)
      nPoints = 0;
    else if (nPoints > 3)
      nPoints = 3;
    fPoints = (fPoints & (~(3 << 30))) | (nPoints << 30);
  }

  else
    LOG(error) << " PndMCTrack::SetNPoints: Unknown detector ID " << static_cast<int>(iDet);
}
//

ClassImp(PndMCTrack)
