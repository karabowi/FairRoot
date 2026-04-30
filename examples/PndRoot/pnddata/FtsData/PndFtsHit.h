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

/**
 **
 ** Meaning of RefIndex:  Index of corresponding MCPoint
 **                       -1 if fake or background hit
 **
 ** Meaning of Flag:       0 = Hit ok
 **                       -1 : Hit lost due to detection inefficiency
 **/

#ifndef PNDFTSHIT_H
#define PNDFTSHIT_H 1

#include "TVector3.h"
#include "FairHit.h"

class PndFtsHit : public FairHit {

 public:
  /** Default constructor **/
  PndFtsHit();

  /** Standard constructor
   *@param detID     Detector unique volume ID
   *@param tubeID    Unique tube ID
   *@param chamberID    Unique chamber ID
   *@param layerID    Unique layer ID
   *@param skew    flag for skewed tube
   *@param mcindex   Index of corresponding MCPoint
   *@param pos       Position coordinates of the tube [cm]
   *@param dpos      Errors in position coordinates [cm]
   *@param isochrone The radial measurement
   *@param isoerror  The erroon on the radial measurement
   *@param chDep     Deposited charge (arbitrary unit)
   **/

  // THIS ONE!
  PndFtsHit(Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, Int_t skew, Int_t mcindex, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t isochrone,
            Double_t isochroneError, Double_t chDep);

  /** Destructor **/
  virtual ~PndFtsHit();

  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t *opt = nullptr) const
  {
    std::cout << " opt = " << opt << std::endl;
    return;
  }

  /** Public method Clear
   ** Resets the isochrone and it's error to 0
   **/
  void Clear();

  /** Accessors **/
  Double_t GetIsochrone() const { return fIsochrone; };
  Double_t GetIsochroneError() const { return fIsochroneError; };
  Double_t GetPulse() const { return fPulse; };
  Double_t GetDepCharge() const { return fDepCharge; };
  Double_t GetEnergyLoss() const { return fDepCharge / 1e6; };

  /** Modifiers **/
  void SetIsochrone(Double_t isochrone) { fIsochrone = isochrone; };
  void SetIsochroneError(Double_t isochroneError) { fIsochroneError = isochroneError; };
  void SetDepCharge(Double_t depcharge) { fDepCharge = depcharge; }

  // tube ID // CHECK added
  void SetTubeID(Int_t tubeid) { fTubeID = tubeid; }
  Int_t GetTubeID() const { return fTubeID; }
  void SetChamberID(Int_t chamberid) { fChamberID = chamberid; }
  Int_t GetChamberID() const { return fChamberID; }
  void SetLayerID(Int_t layerid) { fLayerID = layerid; }
  Int_t GetLayerID() const { return fLayerID; }
  Int_t GetSkewed() const { return fSkewed; }

  virtual bool equal(FairTimeStamp *data) const
  {
    PndFtsHit *myDigi = dynamic_cast<PndFtsHit *>(data);
    if (myDigi != nullptr) {
      if (fTubeID == myDigi->GetTubeID())
        return true;
    }
    return false;
  }

  virtual bool operator<(const PndFtsHit &myDigi) const
  {
    if (fTubeID < myDigi.GetTubeID())
      return true;
    else
      return false;
  }

  friend std::ostream &operator<<(std::ostream &out, PndFtsHit &digi)
  {
    out << "PndSttHit in Tube: " << digi.GetTubeID() << " Isochrone: " << digi.GetIsochrone() << " +/- " << digi.GetIsochroneError() << " Charge: " << digi.GetDepCharge()
        << " Pulse: " << digi.GetPulse() << std::endl;
    return out;
  }

 protected:
  /** This variable contains the radial distance to the wire **/
  Double_t fIsochrone;
  /** This variable contains the error on the radial distance to the wire **/
  Double_t fIsochroneError;

  /** time pulse **/
  Double_t fPulse;

  /**  deposit charge (arbitrary units) **/
  Double_t fDepCharge;

  /** tube id **/
  Int_t fTubeID; // CHECK added
  Int_t fChamberID;
  Int_t fLayerID;
  Int_t fSkewed;

  ClassDef(PndFtsHit, 2);
};

#endif
