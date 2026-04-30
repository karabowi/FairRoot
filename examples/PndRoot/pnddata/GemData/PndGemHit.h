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
// -----                    PndGemHit header file                      -----
// -----                  Created by R.Kliemt          -----
// -------------------------------------------------------------------------

/** PndGemHit.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** A hit in a silicon sensor. In addition to the base class
 ** FairHit, it holds the number of digis & charge entry.
 ** There is also a bottom index for double sided strip sensors.
 ** All coordinates are in the LAB frame.
 **/

#ifndef PNDGEMHIT_H
#define PNDGEMHIT_H

#include "TVector3.h"
#include "TString.h"
#include "FairHit.h"

#include <stdio.h>
#include <iostream>

class PndGemHit : public FairHit {
  friend std::ostream &operator<<(std::ostream &out, const PndGemHit &hit)
  {
    out << "Gem hit" /*in detector " << hit.GetDetName()*/ << " at (" << hit.GetX() << ", " << hit.GetY() << ", " << hit.GetZ() << ") cm "
        << " with " << hit.GetCharge() << " e"
        << ", Cluster No. " << hit.GetRefIndex() << std::endl;

    return out;
  }

 public:
  /** Default constructor (not for use) **/
  PndGemHit();

  /** Standard constructor with all paramneters
   *@param detID   Detector ID
   *@param detName Detector Name
   *@param pos     Position vector [cm]
   *@param dpos    Position error vector [cm]
   *@param index   Array index of ClusterCand
   *@param charge  Charge measured in hit
   *@param NDigiHits Number of pixels fired for this event
   **/
  PndGemHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Int_t index, Double_t charge, Int_t NDigiHits, TString fromStr = "GEMPoint");
  PndGemHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Int_t digi1, Int_t digi2, Double_t dr, Double_t dp, Int_t index, TString fromStr = "GEMCluster");
  PndGemHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Double_t charge, Double_t time, Int_t digi1, Int_t digi2, Double_t dr, Double_t dp, Int_t index,
            TString fromStr = "GEMCluster");

  // PndGemHit(PndGemHit& c);
  /** Destructor **/
  virtual ~PndGemHit();

  /** Accessors **/

  void SetCharge(Double_t charge) { fCharge = charge; }
  void SetNDigiHits(Int_t pixel) { fNDigiHits = pixel; }
  void SetBotIndex(Int_t id) { fBotIndex = id; }
  void SetErrors(Double_t dr, Double_t dp)
  {
    fDr = dr;
    fDp = dp;
  };
  void SetDigiNr(Int_t digi1, Int_t digi2)
  {
    fDigiNr[0] = digi1;
    fDigiNr[1] = digi2;
  };

  Double_t GetCharge() const { return fCharge; }
  Int_t GetNDigiHits() const { return fNDigiHits; }
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ); }
  Int_t GetBotIndex() const { return fBotIndex; }
  Double_t GetEloss() const { return (fCharge * 3.61e-9); } // 3.6 eV/Electron in Silicon

  Double_t GetDr() const { return fDr; };
  Double_t GetDp() const { return fDp; };
  Int_t GetDigiNr(Int_t iside) const
  {
    if (iside * (iside - 1) == 0)
      return fDigiNr[iside];
    return -1;
  };

  Int_t GetSystemId() const { return ((fDetectorID & (31 << 27)) >> 27); }
  Int_t GetStationNr() const { return ((fDetectorID & (8191 << 8)) >> 8); }
  Int_t GetSensorNr() const
  { // sensor number within station
    return ((fDetectorID & (3 << 6)) >> 6);
  }

  //   // CAUTION The errors in the GemHit are LOCAL, but the coordinates are in the LAB
  //
  //   Double_t GetDxLocal() const { return fDx;};
  //   Double_t GetDyLocal() const { return fDy;};
  //   Double_t GetDzLocal() const { return fDz;};
  //   void PositionErrorLocal(TVector3& dpos) const;

  //   /** overloaded accessors **/
  //   Double_t GetDx() {return GetD(0);};
  //   Double_t GetDy() {return GetD(1);};
  //   Double_t GetDz() {return GetD(2);};
  //   void PositionError(TVector3& dpos);

  /** Screen output **/
  virtual void Print(const Option_t *opt = nullptr) const;

 private:
  Int_t fDigiNr[2];
  Double_t fDr; // error in radius calculation
  Double_t fDp; // error in angle calculation

  Double_t fCharge; /// deposited Charge
  Int_t fNDigiHits; /// number of fired Digis for this hit,
                    // if more then in the cluster cand, look for the bottom cluster.
  Int_t fBotIndex;  /// bottom side of strip clusters
  ClassDef(PndGemHit, 1);
};

// inline void PndGemHit::PositionErrorLocal(TVector3& dpos) const {
//   dpos.SetXYZ(fDx, fDy, fDz);
// }
//
// inline void PndGemHit::PositionError(TVector3& dpos) {
//   dpos.SetXYZ(GetDx(), GetDy(), GetDz());
// }

#endif
