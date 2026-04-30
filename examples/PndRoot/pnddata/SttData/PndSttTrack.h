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
// -----                      PndSttTrack header file                  -----
// -----                  Created 28/03/06  by R. Castelijns           -----
// -------------------------------------------------------------------------

/**  PndSttTrack.h
 *@author R.Castelijns <r.castelijns@fz-juelich.de>
 **
 ** STT local track. Holds lists of PndSttHits and the fitted
 ** track parameters. The fit parameters are of type FairTrackParam
 ** and can only be accesssed and modified via this class.
 **/

#ifndef PNDSTTTRACK_H
#define PNDSTTTRACK_H

#include <map>
#include "TArrayI.h"
#include "TObject.h"
#include "FairTrackParam.h"
#include "FairMultiLinkedData_Interface.h"
#include "TClonesArray.h"

class PndSttHit;

class PndSttTrack : public FairMultiLinkedData_Interface {
 public:
  /** Default constructor **/
  PndSttTrack();

  /** Destructor **/
  virtual ~PndSttTrack();

  /** Public method Print
   ** Output to screen
   **/
  void Print();

  /** Native Hits attached via PndTrackCand**/
  Int_t GetTrackCandIndex() { return fTrackCandIndex; }

  /** Reconstructed "Helix" Hits **/
  Int_t GetNofHelixHits() const { return fHelixHits.GetSize(); };
  Int_t GetHelixHitIndex(Int_t iHit) const { return fHelixHits.At(iHit); };
  Int_t GetPidHypo() const { return fPidHypo; };
  Int_t GetFlag() const { return fFlag; };
  Double_t GetChi2Long() const { return fChi2Long; };
  Double_t GetChi2Rad() const { return fChi2Rad; };
  Int_t GetNDF() const { return fNDF; };

  /** parameters of the helix: d0, phi0, Rad, tanlambda, z0 **/
  Double_t GetDist() { return fDist; };
  Double_t GetPhi() { return fPhi; };
  Double_t GetRad() { return fRad; };
  Double_t GetTanL() { return fTanL; };
  Double_t GetZ() { return fZ; };

  Int_t GetCharge() { return fH; };

  // Momentum, but where?? // CHECK
  /*   Double_t GetMomentum(); */
  /*   Double_t GetPlong(); */
  /*   Double_t GetPtot(); */

  // track length calculation
  Double_t CalculateScosl(Double_t x, Double_t y);
  // find the tri-momentum in the PCA to a point
  TVector3 *MomentumAtPoint(TVector3 *point);
  // find the PCA to a point
  TVector3 *PCAToPoint(TVector3 *point);

  /** Modifiers  **/
  void SetTrackCandIndex(Int_t trackCandID) { fTrackCandIndex = trackCandID; };
  void SetPidHypo(Int_t pid) { fPidHypo = pid; };
  void SetParameters(Double_t d, Double_t phi, Double_t r, Double_t tanl, Double_t z)
  {
    fDist = d;
    fPhi = phi;
    fRad = r;
    fTanL = tanl;
    fZ = z;
  };

  void SetDist(Double_t dist) { fDist = dist; }
  void SetPhi(Double_t phi) { fPhi = phi; }
  void SetRad(Double_t r) { fRad = r; }
  void SetTanL(Double_t tanl) { fTanL = tanl; }
  void SetZ(Double_t z) { fZ = z; }
  void SetCharge(Int_t charge) { fH = charge; }

  void SetFlag(Int_t flag) { fFlag = flag; };
  void SetChi2Long(Double_t chi2) { fChi2Long = chi2; };
  void SetChi2Rad(Double_t chi2) { fChi2Rad = chi2; };
  void SetNDF(Int_t ndf) { fNDF = ndf; };

  void AddHelixHit(Int_t size, Int_t index, Int_t helixhitindex);

 private:
  /** Arrays containg the indices of the helixhits attached to the track **/
  TArrayI fHelixHits;

  /** PID hypothesis used by the track fitter **/
  Int_t fPidHypo; //! // CHECK not for now, maybe in future if needed

  /** Track parameters of the helix **/
  Double_t fDist, fPhi, fRad, fTanL, fZ;
  Int_t fH;

  /** Quality flag **/
  Int_t fFlag;

  /** RMS deviation of hit coordinates to track **/
  Double_t fChi2Long; //! // CHECK not for now, maybe in future if needed
  Int_t fNDF;         //! // CHECK not for now, maybe in future if needed
  Double_t fChi2Rad;  //! // CHECK not for now, maybe in future if needed

  /** track cand **/
  Int_t fTrackCandIndex;

  ClassDef(PndSttTrack, 1);
};

#endif
