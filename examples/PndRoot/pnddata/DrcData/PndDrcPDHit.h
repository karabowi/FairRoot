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
// -----                     PndDrcPDHit header file                     -----
// -----               Created 11/10/06 by Annalisa Cecchi             -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDDRCPDHIT_H
#define PNDDRCPDHIT_H

#include "FairHit.h"

class PndDrcPDHit : public FairHit {

 public:
  /** Default constructor **/
  PndDrcPDHit();

  PndDrcPDHit(Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Double_t time, Double_t timeThreshold, Int_t index);

  /** Copy constructor **/
  // PndDrcPDHit(const PndDrcPDHit& hit) { *this = hit; }; // not needed

  /** Destructor **/
  virtual ~PndDrcPDHit();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = "") const;

  /** Modifiers **/

  void SetBarID(Int_t BarID);
  void SetBoxID(Int_t BoxID);
  void SetTrackID(Int_t TrackID);
  void SetTrackIniVertex(TVector3 TrackIniVertex);
  void SetMotherID(Int_t MrID);
  void SetPdgCode(Int_t Pdg);
  void SetTrackMom(TVector3 TrackMom);
  void SetMotherIDPho(Int_t MrIDPho);
  void SetTimeAtBar(Double_t TimeAtBar);
  void SetEvtTim(Double_t EvtTim);
  void SetEventTim(Double_t EventTim);
  void SetPileUp(Double_t pileup);
  void SetEventNo(Int_t EventNo);

  /** Accessors **/
  virtual Double_t GetTime() { return fTime; }
  virtual Double_t GetTimeThreshold() { return fTimeThreshold; }
  virtual Int_t GetRefIndex() { return fRefIndex; }
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ); }
  Int_t GetSensorId() { return fSensorId; }
  Int_t GetBarID() const { return fBarID; }
  Int_t GetBoxID() const { return fBoxID; }
  Int_t GetTrackID() const { return fTrackID; }
  TVector3 GetTrackIniVertex() { return fTrackIniVertex; }
  Int_t GetMotherID() const { return fMrID; }
  Int_t GetPdgCode() const { return fPdg; }
  TVector3 GetTrackMom() { return fTrackMom; }
  Int_t GetMotherIDPho() const { return fMrIDPho; }
  Double_t GetTimeAtBar() const { return fTimeAtBar; }
  Double_t GetEvtTim() const { return fEvtTim; }
  Double_t GetEventTim() const { return fEventTim; }
  Double_t GetPileUp() const { return fPileUp; }
  Int_t GetEventNo() const { return fEventNo; }

 protected:
  Int_t fSensorId;
  Double_t fTime, fTimeThreshold;
  Int_t fBarID;
  Int_t fBoxID;
  Int_t fTrackID;
  TVector3 fTrackIniVertex;
  Int_t fMrID;
  Int_t fMrIDPho;
  Int_t fPdg;
  TVector3 fTrackMom;
  Double_t fTimeAtBar;
  Double_t fEvtTim;
  Double_t fEventTim;
  Double_t fPileUp;
  Int_t fEventNo;

  ClassDef(PndDrcPDHit, 1)
};

#endif // PNDDRCPDHIT_H
