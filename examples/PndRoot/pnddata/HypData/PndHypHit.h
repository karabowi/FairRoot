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
// -----                    MvdHit header file                -----
// -----                  Created by T.Stockmanns             -----
// -----            modified for hyp purpose by A.Sanchez
// -------------------------------------------------------------------------

/** PndSdsHit.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>

 **
 ** A hit in a silicon sensor. In addition to the base class
 ** FairHit, it holds the number of digis & charge entry.
 ** There is also a bottom index for double sided strip sensors.
 ** All coordinates are in the LAB frame.

 ** modified to PndHypHit by A. Sanchez <a.sanchez@gsi.de>
 **/

#ifndef PNDHYPHIT_H
#define PNDHYPHIT_H

#include "TVector3.h"
#include "TString.h"
#include "FairHit.h"

#include <stdio.h>
#include <iostream>
#include "PndDetectorList.h"

class PndHypHit : public FairHit {
  friend std::ostream &operator<<(std::ostream &out, const PndHypHit &hit)
  {

    out << "Hyp hit in " << hit.GetDetName() << " at" << std::endl;
    out << "(" << hit.GetX() << ", " << hit.GetY() << ", " << hit.GetZ() << ") cm "
        << " with " << hit.GetCharge() << " e"
        << ", Cluster No. " << hit.GetRefIndex();
    if (hit.GetBotIndex() > -1)
      out << " " << hit.GetBotIndex();
    out << std::endl;

    return out;
  }

 public:
  /** Default constructor (not for use) **/
  PndHypHit();

  /** Standard constructor with all paramneters
   *@param trackID Track ID
   *@param detID   Detector ID
   *@param detName Detector Name
   *@param pos     Position vector [cm]
   *@param dpos    Position error vector [cm]
   *@param charge  Charge measured in hit
   *@param index   Array index of ClusterCand
   *@param NDigiHits Number of digis fired for this event
   **/

  // PndHypHit(Int_t trackID, Int_t detID, TString detName,
  //    TVector3& pos, TVector3& dpos, Int_t index, Double_t charge, Int_t NPixelHits);

  PndHypHit(Int_t detID, TString detName, TVector3 &pos, TVector3 &dpos, Int_t index, Double_t charge, Int_t NDigiHits);

  // PndHypHit(PndHypHit& c);
  /** Destructor **/
  virtual ~PndHypHit();

  /** Accessors **/

  // void SetTrackID(Int_t id){fTrackID = id;};
  // new version const added
  void SetDetName(TString name) { fDetName = name; };
  void SetCharge(Double_t charge) { fCharge = charge; };
  void SetNDigiHits(Int_t pixel) { fNDigiHits = pixel; };
  void SetBotIndex(Int_t id) { fBotIndex = id; }

  TString GetDetName() const { return fDetName; }
  // Int_t 	GetTrackID(){return fTrackID;};
  Double_t GetCharge() const { return fCharge; };
  Int_t GetNDigiHits() const { return fNDigiHits; }
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ); }
  Int_t GetBotIndex() const { return fBotIndex; }
  Double_t GetEloss() const { return (fCharge * 3.61e-9); } // 3.6 eV/Electron in Silicon

  /** Screen output **/
  virtual void Print(const Option_t *opt = nullptr) const;
  // new version

 private:
  // Double_t GetD(Int_t i);

  TString fDetName; // Detector name
  // Int_t fTrackID;
  Double_t fCharge;
  Int_t fNDigiHits;
  Int_t fBotIndex; /// bottom side of strip clusters

  ClassDef(PndHypHit, 7);
};

#endif
