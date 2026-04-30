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
// -----                     CbmHypGe header file                        -----

// -------------------------------------------------------------------------

#ifndef PNDHYPGE_H
#define PNDHYPGE_H

#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairDetector.h"
//#include "PndGeoHypGePar.h"
//#include "GeCluster.h"

using namespace std;

class TClonesArray;
class PndHypGePoint;
// class PndHypGesciPoint;
class FairVolume;

class PndHypGe : public FairDetector {

 public:
  /** Default constructor **/
  PndHypGe();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndHypGe(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndHypGe();

  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for MUO detector and mirror.
   **/
  virtual void Initialize();

  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndHypGePoints and PndHypGeMirrorPoints and adds
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

  void ConstructHPGeGeometry();

  PndHypGePoint *AddHit(Int_t trackID, Int_t evtID, Int_t pdgCode, Int_t charge, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, Short_t copy);

  PndHypGePoint *AddGeAlHit(Int_t trackID, Int_t evtID, Int_t pdgCode, Int_t charge, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, Short_t copy);

  /** Setting distance to interaction point **/

  void SetDisIP(Double_t dist)
  {
    fdist = dist; // cm
  }

  void SetPathGeo(TString pgeo) { fPathGeo = pgeo.Data(); }

 private:
  std::vector<std::string> fListOfSensitives;
  bool CheckIfSensitive(std::string name);

  // PndGeoHypGePar *par;
  Int_t fpdgCode;
  Int_t fcharge;

  Int_t fTrackID;      //  track index
  Int_t fVolumeID;     //  volume id
  Int_t fEventID;      //  event id
  TLorentzVector fPos; //  position
  TLorentzVector fMom; //  momentum
  Double32_t fTime;    //  time
  Double32_t fLength;  //  length
  Double32_t fELoss;   //  energy loss
  Int_t fPosIndex;     //
  Short_t fnCopy;
  Double_t fdist;
  TString fPathGeo;

  // Int_t volDetector;     //  MC volume ID of MUO

  TClonesArray *fHypGeCollection;   // Hit collection
  TClonesArray *fHypGeAlCollection; // Hit collection
  // TClonesArray* fHypGecapCollection;        // Hit collection
  // reset all parameters
  void ResetParameters();

  ClassDef(PndHypGe, 3)
};

#endif
