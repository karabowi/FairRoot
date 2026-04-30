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

// --------------------------------------------------------
// ----			PndDrcDigi header file	---
// ----			Created 30.5.13 by M.Patsyuk ---
// --------------------------------------------------------

/** PndDrcDigi.h
 *@author M.Patsyuk <m.patsyuk@gsi.de>
 ** \brief Data class to store the digi output of the Barrel DIRC
 **
 ** \sa PndDrcDigi.h
 **/

#ifndef PNDDRCDIGI_H
#define PNDDRCDIGI_H

#include "PndDrcDigi.h"
#include "PndDetectorList.h"
#include "FairTimeStamp.h"
#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <vector>
#include "TVector3.h"
#include <fstream>
#include <iostream>
#include "stdio.h"
#include "TMath.h"
using std::cout;
using std::endl;

class PndDrcDigi : public FairTimeStamp {
 public:
  friend std::ostream &operator<<(std::ostream &out, PndDrcDigi &digi)
  {
    out << "PndDrcDigi in: " << digi.GetDetectorId() << " charge " << digi.GetCharge() << " e"
        << " time " << digi.GetTime() << " ns"
        << " timestamp " << digi.GetTimeStamp() << " charge sharing flag " << digi.GetChargeSharingFlag() << " sensorid " << digi.GetSensorId() << ", from Point(s) ";
    std::vector<Int_t> indices = digi.GetIndices();
    for (unsigned int i = 0; i < indices.size(); i++) {
      out << indices[i] << "  ";
    }
    out << std::endl;

    return out;
  }

 public:
  PndDrcDigi();
  PndDrcDigi(std::vector<Int_t> index, Int_t detectorId, Int_t sensorId, Double_t charge, Double_t time, Int_t CSflag, Double_t timeStamp);
  PndDrcDigi(Int_t index, Int_t detectorId, Int_t sensorId, Double_t charge, Double_t time, Int_t CSflag, Double_t timeStamp);

  ~PndDrcDigi(){};

  void Print() { std::cout << *this; }

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
  void SetEventNo(Double_t EventNo);

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
  Double_t GetEventNo() const { return fEventNo; }

  Int_t GetDetectorId() const { return fDetectorId; }
  Int_t GetSensorId() const { return fSensorId; }
  Double_t GetCharge() const { return fCharge; }
  std::vector<Int_t> GetIndices() const { return fIndex; }
  Int_t GetIndex(int i = 0) const { return fIndex[i]; }
  Int_t GetNIndices() const { return fIndex.size(); }
  Double_t GetTime() const { return fTime; }
  Int_t GetChargeSharingFlag() const { return fCSflag; }

  void AddIndex(int index)
  {
    fIndex.push_back(index);
    AddLink(FairLink("DrcPDPoint", index));
  }

  void AddIndex(std::vector<Int_t> index)
  {
    fIndex = index;
    AddLinks(FairMultiLinkedData("DrcPDPoint", index));
  }

  virtual bool equal(FairTimeStamp *data)
  {
    PndDrcDigi *myDigi = dynamic_cast<PndDrcDigi *>(data);
    if (myDigi != nullptr) {
      if (fSensorId == myDigi->GetSensorId())
        return true;
    }
    return false;
  }

  virtual bool operator<(const PndDrcDigi &myDigi) const
  {
    if (fSensorId < myDigi.GetSensorId())
      return true;
    return false;
  }
  virtual bool operator>(const PndDrcDigi &myDigi) const
  {
    if (fSensorId > myDigi.GetSensorId())
      return true;
    return false;
  }
  virtual bool operator==(const PndDrcDigi &myDigi) const
  {
    if (fSensorId == myDigi.GetSensorId())
      return true;
    return false;
  }

 protected:
  std::vector<Int_t> fIndex; // indice of mc points contributing to this digi
  Int_t fDetectorId;         // uniq detector ID
  Int_t fSensorId;           // Geometry ID for sensor volume
  Double_t fCharge;          // collected charge
  Double_t fTime;            // hit time
  Int_t fCSflag;             // flag indicating is the hit was produced directly by the MC point or if it is a result of the charge sharing: 1 - charge sharing hit, 0 - initial hit
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
  Double_t fEventNo;

  ClassDef(PndDrcDigi, 2);
};

#endif
