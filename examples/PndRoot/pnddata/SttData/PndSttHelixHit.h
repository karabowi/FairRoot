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

// ======================================
//
// hits recostructed via HELIX FIT in STT
//
// ======================================
#ifndef PNDSTTHELIXHIT_H
#define PNDSTTHELIXHIT_H 1

#include "TVector3.h"
#include "FairHit.h"

class PndSttHit;

class PndSttHelixHit : public FairHit {

 public:
  /** Default constructor **/
  PndSttHelixHit();

  /** Standard constructor
   *@param detID          Detector unique volume ID
   *@param tubeID         Unique tube ID
   *@param hitIndex       Index of corresponding STTHit
   *@param mcindex        Index of corresponding MCPoint
   *@param pos            Reco position coordinates X Y Z [cm]
   *@param dpos           Errors on reco position coordinates [cm]
   *@param isochrone      Drift radius [cm]
   *@param isochroneError Error on drift radius [cm]
   *@param dedx           De/Dx
   **/

  PndSttHelixHit(Int_t detID, Int_t tubeID, Int_t hitindex, Int_t mcindex, TVector3 &pos, TVector3 &dpos, Double_t isochrone, Double_t isochroneError, Double_t dedx);

  // void CopyHitToHelixHit(PndSttHit *aHit, Int_t hitindex);

  /** Destructor **/
  virtual ~PndSttHelixHit();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = nullptr) const
  {
    std::cout << " opt = " << opt << std::endl;
    return;
  }

  /** Public method Clear  **/
  void Clear();

  /** Accessors **/
  Int_t GetHitIndex() { return fHitIndex; }

  /** Accessors **/
  Double_t GetIsochrone() const { return fIsochrone; };
  Double_t GetIsochroneError() const { return fIsochroneError; };
  Double_t GetdEdx() const { return fdEdx; };

  /** Modifiers **/
  void SetHitIndex(Int_t hitindex) { fHitIndex = hitindex; }
  void SetIsochrone(Double_t isochrone) { fIsochrone = isochrone; };
  void SetIsochroneError(Double_t isochroneError) { fIsochroneError = isochroneError; };
  void SetdEdx(Double_t dedx) { fdEdx = dedx; }
  void Print();

  // tube ID // CHECK added
  void SetTubeID(Int_t tubeid) { fTubeID = tubeid; }
  Int_t GetTubeID() { return fTubeID; }

 protected:
  /** This variable contains the hit index **/
  Int_t fHitIndex;

  /** de/dx **/
  Double_t fdEdx;

  /** isochrone **/
  Double_t fIsochrone, fIsochroneError;

  /** tubeID **/
  Int_t fTubeID; // CHECK added

  ClassDef(PndSttHelixHit, 2);
};

#endif
