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
// -----                     PndEmcApd header file                        -----
// -----               Created 07/04/08  by S.Spataro                  -----
// -------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCAPD_H
#define PNDEMCAPD_H

#include "TClonesArray.h"
#include "TVector3.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "FairDetector.h"

class TClonesArray;
class PndEmcApdPoint;
class FairVolume;

class PndEmcApd : public FairDetector {

 public:
  /** Default constructor **/
  PndEmcApd();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndEmcApd(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndEmcApd();

  /** Virtual method Initialize
   ** Initialises detector.
   **/
  virtual void Initialize();

  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndEmcApdPoints and adds
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume *vol = 0);

  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();

  virtual void BeginEvent();
  /** Virtual method Register
   **
   ** Registers the hit collection in the ROOT manager.
   **/
  virtual void Register();

  /** Accessor to the hit collection **/
  virtual TClonesArray *GetCollection(Int_t iColl) const;

  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print() const;

  /** Virtual method Reset
   **
   ** Clears the hit collection
   **/
  virtual void Reset();

  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
  virtual void CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset);

  /** Virtual method Construct geometry
   **
   **/
  virtual void ConstructGeometry();

  // construct detector from .dat file
  void ConstructASCIIGeometry();

  PndEmcApdPoint *AddHit(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, Short_t mod, Short_t row, Short_t crys,
                         Short_t copy, Short_t flag);

 private:
  Int_t fTrackID;      //!  track index
  Int_t fVolumeID;     //!  volume id
  Int_t fEventID;      //!  event id
  TLorentzVector fPos; //!  position
  TLorentzVector fMom; //!  momentum
  Double32_t fTime;    //!  time
  Double32_t fLength;  //!  length
  Double32_t fELoss;   //!  energy loss
  Int_t fPosIndex;     //!

  TClonesArray *fApdCollection; //! Hit collection

  // reset all parameters
  void ResetParameters();

  PndEmcApd(const PndEmcApd &L);
  PndEmcApd &operator=(const PndEmcApd &) { return *this; }

  ClassDef(PndEmcApd, 1)
};

#endif
