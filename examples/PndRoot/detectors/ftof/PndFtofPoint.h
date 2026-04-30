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

// ------------------------------------------------------------------------
// -----                     PndFtofpoint header file                  -----
// -----               Created by A.Sanchez                 -----
// ------------------------------------------------------------------------

#ifndef PNDFTOFPOINT_H
#define PNDFTOFPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "PndMCPoint.h"
#include "TString.h"

class PndFtofPoint : public PndMCPoint {

 public:
  /** Default constructor **/
  PndFtofPoint();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param posIn    Ccoordinates at entrance to active volume [cm]
   *@param posOut   Coordinates at exit of active volume [cm]
   *@param momIn    Momentum of track at entrance [GeV]
   *@param momOut   Momentum of track at exit [GeV]
   *@param posInLocal    Local coordinates at entrance to active volume [cm]
   *@param posOutLocal   Local coordinates at exit of active volume [cm]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/

  PndFtofPoint(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 posin, TVector3 momin, TVector3 posout, TVector3 momout, Double_t tof, Double_t length,
               Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t distance, Double_t PLin, Double_t PLout);

  /** Copy constructor **/
  PndFtofPoint(const PndFtofPoint &point) : PndMCPoint(point) { *this = point; };

  /** Destructor **/
  virtual ~PndFtofPoint();

  /** Accessors **/
  Int_t GetVolumeID() const { return fVolumeID; };

  Double_t GetPLin() const { return fPLin; };
  Double_t GetPLout() const { return fPLout; };

  Double_t Getdist() const { return fdist; };
  // Double_t Gettheta()         const { return ftheta; };

  TString GetDetName() const { return fDetName; };
  Double_t GetMass() const { return fmass; };

  /** Output to screen **/
  virtual void Print(const Option_t *opt) const;

 protected:
  Double32_t fPLin = 0.;
  Double32_t fPLout = 0.;
  Double32_t fmass = 0.;
  Double32_t fcharge = 0.;
  Double32_t fdist = 0.;

    TString fDetName; //!

  Int_t fVolumeID = -999;
  Int_t fpdgCode = 0;

  ClassDef(PndFtofPoint, 1)
};

#endif
