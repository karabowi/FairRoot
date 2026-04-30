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
// -----                       PndDrcPDHit source file                   -----
// -----               Created 12/11/07  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndDrcPDHit.h"

// -----   Default constructor   -------------------------------------------
PndDrcPDHit::PndDrcPDHit()
  : FairHit(), fSensorId(-999), fTime(-999.), fTimeThreshold(-999.), fBarID(-999), fBoxID(-999), fTrackID(-999), fTrackIniVertex(TVector3()), fMrID(-999), fMrIDPho(-999),
    fPdg(-999), fTrackMom(TVector3()), fTimeAtBar(-999), fEvtTim(-999), fEventTim(-999), fPileUp(-999), fEventNo(-999)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------

PndDrcPDHit::PndDrcPDHit(Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Double_t time, Double_t timeThreshold, Int_t index)
  : FairHit(detID, pos, dpos, index), fSensorId(sensorId), fTime(time), fTimeThreshold(timeThreshold), fBarID(), fBoxID(), fTrackID(), fTrackIniVertex(TVector3()), fMrID(),
    fMrIDPho(), fPdg(), fTrackMom(TVector3()), fTimeAtBar(), fEvtTim(), fEventTim(), fPileUp(), fEventNo()
{
}

// -----   Destructor   ----------------------------------------------------
PndDrcPDHit::~PndDrcPDHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndDrcPDHit::Print(const Option_t *opt) const
{
  (void)opt; // to remove "unused" warnings
  cout << "DIRC Photon hit: Time = " << fTime << endl;
}
// -------------------------------------------------------------------------
void PndDrcPDHit::SetBarID(Int_t BarID)
{
  fBarID = BarID;
}
void PndDrcPDHit::SetBoxID(Int_t BoxID)
{
  fBoxID = BoxID;
}
void PndDrcPDHit::SetTrackID(Int_t TrackID)
{
  fTrackID = TrackID;
}
void PndDrcPDHit::SetTrackIniVertex(TVector3 TrackIniVertex)
{
  fTrackIniVertex = TrackIniVertex;
}
void PndDrcPDHit::SetMotherID(Int_t MrID)
{
  fMrID = MrID;
}
void PndDrcPDHit::SetPdgCode(Int_t Pdg)
{
  fPdg = Pdg;
}
void PndDrcPDHit::SetTrackMom(TVector3 TrackMom)
{
  fTrackMom = TrackMom;
}
void PndDrcPDHit::SetMotherIDPho(Int_t MrIDPho)
{
  fMrIDPho = MrIDPho;
}
void PndDrcPDHit::SetTimeAtBar(Double_t TimeAtBar)
{
  fTimeAtBar = TimeAtBar;
}
void PndDrcPDHit::SetEvtTim(Double_t EvtTim)
{
  fEvtTim = EvtTim;
}
void PndDrcPDHit::SetEventTim(Double_t EventTim)
{
  fEventTim = EventTim;
}
void PndDrcPDHit::SetPileUp(Double_t pileup)
{
  fPileUp = pileup;
}
void PndDrcPDHit::SetEventNo(Int_t EventNo)
{
  fEventNo = EventNo;
}

ClassImp(PndDrcPDHit)
