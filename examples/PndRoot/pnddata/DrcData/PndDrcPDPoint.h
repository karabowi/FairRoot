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
// -----                      PndDrcPDPoint header file                   -----
// -----                  Created 09/04/08  by E. Fioravanti and A. Cecchi               -----
// -------------------------------------------------------------------------

/**  PndDrcPDPoint.h
 **/

#ifndef PNDDRCPDPOINT_H
#define PNDDRCPDPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"

class PndDrcPDPoint : public FairMCPoint {

 public:
  /** Default constructor **/
  PndDrcPDPoint();

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
  PndDrcPDPoint(Int_t trackID, Int_t detID, Int_t mcpId, Int_t barPintId, TVector3 pos, TVector3 mom, TVector3 momAtEV, Double_t timeAtEV, Double_t tof, Double_t length,
                Int_t pdgCode, Int_t eventID);

  /** Copy constructor **/
  // PndDrcPDPoint(const PndDrcPDPoint& point) { *this = point; }; // not needed

  /** Destructor **/
  virtual ~PndDrcPDPoint();

  /** Accessors **/
  Int_t GetPdgCode() const { return fPdgCode; };
    TVector3 GetMomAtEV() const { return TVector3(fmomAtEVX,fmomAtEVY,fmomAtEVZ); };
  Double_t GetTimeAtEV() const { return ftimeAtEV; };
  Int_t GetDetectorID() const { return fDetectorID; };
  Int_t GetBarPointID() const { return fBarPointId; };
  Int_t GetMcpId() const { return fMcpId; };

  /** Modifiers **/
  void SetPdgCode(Int_t id) { fPdgCode = id; };

  /** Output to screen **/
  //  virtual void Print(const Option_t* opt = 0) const = 0;
  virtual void Print(const Option_t *opt) const;

 protected:
  Int_t fPdgCode;
  Int_t fMcpId;
  Int_t fBarPointId;
    //  TVector3 fmomAtEV;
  Double_t fmomAtEVX{0.};
  Double_t fmomAtEVY{0.};
    Double_t fmomAtEVZ{0.};
  Double_t ftimeAtEV;

  ClassDef(PndDrcPDPoint, 3)
};

#endif
