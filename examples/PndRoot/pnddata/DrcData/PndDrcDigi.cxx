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

#include "PndDrcDigi.h"

PndDrcDigi::PndDrcDigi()
  : FairTimeStamp(), fIndex(), fDetectorId(0), fSensorId(0), fCharge(0), fTime(0), fCSflag(0), fBarID(0), fBoxID(0), fTrackID(-1), fTrackIniVertex(TVector3()), fMrID(0),
    fMrIDPho(0), fPdg(0), fTrackMom(TVector3()), fTimeAtBar(0), fEvtTim(0), fEventTim(0), fPileUp(0), fEventNo(0)

{
}

PndDrcDigi::PndDrcDigi(std::vector<Int_t> index, Int_t detectorId, Int_t sensorId, Double_t charge, Double_t time, Int_t CSflag, Double_t timeStamp)
  : FairTimeStamp(timeStamp), fIndex(), fDetectorId(detectorId), fSensorId(sensorId), fCharge(charge), fTime(time), fCSflag(CSflag), fBarID(0), fBoxID(0), fTrackID(-1),
    fTrackIniVertex(TVector3()), fMrID(0), fMrIDPho(0), fPdg(0), fTrackMom(TVector3()), fTimeAtBar(0), fEvtTim(0), fEventTim(0), fPileUp(0), fEventNo(0)
{
  AddIndex(index); // FIXME: do we need the "index" parameter or do we comment this line?
}

PndDrcDigi::PndDrcDigi(Int_t index, Int_t detectorId, Int_t sensorId, Double_t charge, Double_t time, Int_t CSflag, Double_t timestamp)
  : FairTimeStamp(timestamp), fIndex(), fDetectorId(detectorId), fSensorId(sensorId), fCharge(charge), fTime(time), fCSflag(CSflag), fBarID(0), fBoxID(0), fTrackID(-1),
    fTrackIniVertex(TVector3()), fMrID(0), fMrIDPho(0), fPdg(0), fTrackMom(TVector3()), fTimeAtBar(0), fEvtTim(0), fEventTim(0), fPileUp(0), fEventNo(0)
{
  AddIndex(index); // FIXME: do we need the "index" parameter or do we comment this line?
}
void PndDrcDigi::SetBarID(Int_t BarID)
{
  fBarID = BarID;
}
void PndDrcDigi::SetBoxID(Int_t BoxID)
{
  fBoxID = BoxID;
}
void PndDrcDigi::SetTrackID(Int_t TrackID)
{
  fTrackID = TrackID;
}
void PndDrcDigi::SetTrackIniVertex(TVector3 TrackIniVertex)
{
  fTrackIniVertex = TrackIniVertex;
}
void PndDrcDigi::SetMotherID(Int_t MrID)
{
  fMrID = MrID;
}
void PndDrcDigi::SetPdgCode(Int_t Pdg)
{
  fPdg = Pdg;
}
void PndDrcDigi::SetTrackMom(TVector3 TrackMom)
{
  fTrackMom = TrackMom;
}
void PndDrcDigi::SetMotherIDPho(Int_t MrIDPho)
{
  fMrIDPho = MrIDPho;
}
void PndDrcDigi::SetTimeAtBar(Double_t TimeAtBar)
{
  fTimeAtBar = TimeAtBar;
}
void PndDrcDigi::SetEvtTim(Double_t EvtTim)
{
  fEvtTim = EvtTim;
}
void PndDrcDigi::SetEventTim(Double_t EventTim)
{
  fEventTim = EventTim;
}
void PndDrcDigi::SetPileUp(Double_t pileup)
{
  fPileUp = pileup;
}
void PndDrcDigi::SetEventNo(Double_t EventNo)
{
  fEventNo = EventNo;
}

ClassImp(PndDrcDigi);
