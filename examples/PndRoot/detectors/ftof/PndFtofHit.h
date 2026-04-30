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
// -----                    Forward tof header file                -----
// -----                  Created 01.04.09 by A. Sanchez             -----
// -------------------------------------------------------------------------

/** PndFtofHit.h
 *@author A. Sanchez <a.sanchez@gsi.de>
 **
 ** A hit in a tof wall station of Ftof. In addition to the base class
 ** FairHit, it holds the number of the reconstructed hits (column and row).
 **/

#ifndef PNDFTOFHIT_H
#define PNDFTOFHIT_H

#include "TVector3.h"
#include "TString.h"
#include "FairHit.h"

class PndFtofHit : public FairHit {

 public:
  /** Default constructor (not for use) **/
  PndFtofHit();

  /** Standard constructor with all paramneters
   *@param trackID Track ID
   *@param detID   Detector ID
   *@param detName Detector Name
   *@param pos     Position vector [cm]
   *@param dpos    Position error vector [cm]
   *@param charge  Charge measured in hit
   *@param NPixelHits Number of pixels fired for this event
   **/
  PndFtofHit(Int_t trackID, Int_t detID, TString detName, Double_t time, Double_t dt, TVector3 &pos, TVector3 &dpos, Int_t index, Double_t charge);

  // PndFtofHit(PndFtofHit& c);
  /** Destructor **/
  virtual ~PndFtofHit();

  /** Accessors **/

  void SetTrackID(Int_t id) { fTrackID = id; };
  void SetDetName(TString name) { fDetName = name; };
  void SetCharge(Double_t charge) { fCharge = charge; };
  void SetTime(Double_t time) { ftime = time; };
  // void SetNPixelHits(Int_t pixel){fNPixelHits = pixel;};

  TString GetDetName() const { return fDetName; }
  Int_t GetTrackID() { return fTrackID; };
  Double_t GetCharge() { return fCharge; };
  Double_t GetTime() { return ftime; };
  Double_t GetDt() { return fdt; };
  // Int_t 	GetNPixelHits() const { return fNPixelHits; }
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ); }

  /** Screen output **/
  virtual void Print(const Option_t *opt = nullptr) const;

 public:
  TString fDetName; // Detector name
  Int_t fTrackID;
  Double_t fCharge, ftime, fdt;
  // Int_t fNPixelHits;
  ClassDef(PndFtofHit, 6);
};

#endif
