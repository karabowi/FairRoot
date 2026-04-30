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
 
#ifndef PNDSTT2HIT_H
#define PNDSTT2HIT_H 1

#include "TVector3.h"
#include "FairHit.h"
//#include "PndTrack.h"

class PndStt2Hit : public FairHit {

 public:
  /** Default constructor **/
  PndStt2Hit();

  /** Standard constructor
   *@param mcindex    Index of corresponding MCPoint
   *@param detID      Detector unique volume ID
   *@param tubeID     Unique tube ID
   *@param pos        Position coordinates of the tube [cm]
   *@param dpos       Errors in position coordinates [cm]
   *@param thit       hit time
   *@param dist2wire  true track distance to wire
   *@param isorad  The radial measurement
   *@param isoerrorlo The lower error on on the radial measurement
   *@param isoerrorhi The higher error on on the radial measurement
   *@param chDep      Deposited charge (arbitrary unit)
   **/

  // THIS ONE!
  PndStt2Hit(Int_t mcindex, Int_t detID, Int_t tubeID, TVector3 &pos, TVector3 &dpos, Double_t thit, Double_t dist2wire, Double_t isorad, Double_t isoerrlo, Double_t isoerrhi, Double_t chDep);

  /** Destructor **/
  virtual ~PndStt2Hit();

  ///** Output to screen (not yet implemented) **/
  //virtual void Print(const Option_t *opt = nullptr) const
  //{
    //std::cout << " opt = " << opt << std::endl;
    //return;
  //}

  /** Public method Clear
   ** Resets the isochrone and it's error to 0
   **/
  void Clear();

  /** Accessors **/
  Double_t GetIsochrone() const { return fIsoRad; };
  Double_t GetIsochroneError() const { return fIsoRadError; };
  Double_t GetIsoErrLo() const { return fIsoRadErrLo; };
  Double_t GetIsoErrHi() const { return fIsoRadErrHi; };
  Double_t GetHitTime() const { return fHitTime; };
  Double_t GetDepCharge() const { return fDepCharge; };
  Double_t GetEnergyLoss() const { return fDepCharge / 1e6; };

  /** Modifiers **/
  void SetIsochrone(Double_t isochrone) { fIsoRad = isochrone; };
  void SetIsochroneError(Double_t isochroneError) { fIsoRadError = isochroneError; };
  void SetIsoErrorLoHi(Double_t isoerrlo, Double_t isoerrhi) { fIsoRadErrLo = isoerrlo; fIsoRadErrHi = isoerrhi; };
  void SetDepCharge(Double_t depcharge) { fDepCharge = depcharge; }

  // tube ID // CHECK added
  void SetTubeID(Int_t tubeid) { fTubeID = tubeid; }
  Int_t GetTubeID() const { return fTubeID; }

  // computation of dE/dx
  //Double_t ComputedEdx(PndTrack *track, Double_t tuberadius);

  virtual bool equal(FairTimeStamp *data)
  {
    PndStt2Hit *myDigi = dynamic_cast<PndStt2Hit *>(data);
    if (myDigi != nullptr) {
      if (fTubeID == myDigi->GetTubeID())
        return true;
    }
    return false;
  }

  virtual bool operator<(const PndStt2Hit &myDigi) const
  {
    if (fTubeID < myDigi.GetTubeID())
      return true;
    else
      return false;
  }

  friend std::ostream &operator<<(std::ostream &out, PndStt2Hit &digi)
  {
    out << "PndStt2Hit in Tube: " << digi.GetTubeID() << " Isochrone: " << digi.GetIsochrone() << " +/- " << digi.GetIsochroneError() << " Charge: " << digi.GetDepCharge()
        << " HitTime: " << digi.GetHitTime() << std::endl;
    return out;
  }

 protected:
  /** MC point index **/
  //Int_t fMcIndex;             
  /** tube id **/
  Int_t fTubeID; 
  /** hit time **/
  Double_t fHitTime;
  /** This variable contains the true radial distance to the wire **/
  Double_t fDist2Wire;
  /** This variable contains the radial distance to the wire **/
  Double_t fIsoRad;
  /** This variable contains the error on the radial distance to the wire **/
  Double_t fIsoRadError;
  Double_t fIsoRadErrLo;
  Double_t fIsoRadErrHi;
  /**  deposit charge (arbitrary units) **/
  Double_t fDepCharge;

  ClassDef(PndStt2Hit, 1);
};

#endif
