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
// -----                      PndDrcEVPoint header file                -----
// -----                  Created 16/06/13  by H. Kumawat              -----
// -------------------------------------------------------------------------

#ifndef PNDDRCEVPOINT_H
#define PNDDRCEVPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"

class PndDrcEVPoint : public FairMCPoint {

 public:
  /** Default constructor **/
  PndDrcEVPoint();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Point coordinates [cm]
   *@param mom      Momentum of track at MCPoint [GeV]
   *@param momAtEV  Momentum of track at entering the EV [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  PndDrcEVPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Int_t pdgCode, Int_t eventID, Double_t timestart, Double_t timestartEV,
                Double_t VeloPhoton, TVector3 normal = TVector3(0, 0, 0));

  /** Copy constructor **/
  // PndDrcEVPoint(const PndDrcEVPoint& point) { *this = point; }; // not needed

  /** Destructor **/
  virtual ~PndDrcEVPoint();

  /** Accessors **/
  Int_t GetPdgCode() const { return fPdgCode; };
  //  TVector3 GetMomAtEV()	   const{ return fmomAtEV;    };
    TVector3 GetNormal() { return TVector3(fNormalX, fNormalY, fNormalZ); };
  Int_t GetDetectorID() const { return fDetectorID; };
  virtual Double_t GetTimeStart() { return fTimeStart; }
  virtual Double_t GetTimeAtEVEntrance() { return fTimeAtEVEntrance; }
  virtual Double_t GetVeloPhoton() { return fVeloPhoton; }

  /** Modifiers **/
  void SetPdgCode(Int_t id) { fPdgCode = id; };

  /** Output to screen **/
  //  virtual void Print(const Option_t* opt = 0) const = 0;
  virtual void Print(const Option_t *opt) const;

 protected:
  Int_t fPdgCode;
  Double_t fTimeStart;
  Double_t fTimeAtEVEntrance;
  Double_t fVeloPhoton;
    //  TVector3 fNormal;
    Double_t fNormalX{0.};
    Double_t fNormalY{0.};
    Double_t fNormalZ{0.};
  //  TVector3 fmomAtEV;

  ClassDef(PndDrcEVPoint, 1)
};

#endif
