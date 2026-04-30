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

////////////////////////////////////////////////////////////////////////////
// PndOt header file
//
// Class for simulation of OT
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndOt by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

#ifndef PNDOT_H
#define PNDOT_H

// from PandaRoot, this library
#include "PndGeoOtPar.h"
#include "PndOtMapCreator.h"
// from PandaRoot, pnddata
#include <PndOtPoint.h>
// from FairRoot
#include <FairDetector.h>
#include <FairRun.h>
// from ROOT
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TVector3.h>
// standard
#include <string>
#include <sstream>

class TClonesArray;
class FairVolume;

class PndOt : public FairDetector {

 public:
  /** Default constructor **/
  PndOt();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndOt(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndOt();

  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndOtPoints and adds them to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume *vol = 0) override;

  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent() override;

  /** Virtual method Register
   **
   ** Registers the hit collection in the ROOT manager.
   **/
  virtual void Register() override;

  /** Accessor to the hit collection **/
  virtual TClonesArray *GetCollection(Int_t iColl) const override;

  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print(Option_t * /*option*/ = "") const override;

  /** Virtual method Reset
   **
   ** Clears the hit collection
   **/
  virtual void Reset() override;

  virtual void Initialize() override;

  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
  virtual void CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset) override;

  /** Virtual method Construct geometry
   **
   ** Constructs the STT geometry
   **/
  virtual void ConstructGeometry() override;

 private:
  /** GFTrack information to be stored until the track leaves the
      active volume. **/
  Int_t fTrackID;              //!  track index
  Int_t fVolumeID;             //!  volume id
  TLorentzVector fPos;         //!  wire position in global frame
  TLorentzVector fPosIn;       //!  entry position in global frame
  TLorentzVector fPosOut;      //!  exit position in global frame
  TLorentzVector fPosInLocal;  //!  entry position in straw frame
  TLorentzVector fPosOutLocal; //!  exit position in straw frame
  TLorentzVector fMomIn;       //!  momentum
  TLorentzVector fMomOut;      //!  momentum
  Double_t fTime;              //!  time
  Double_t fLength;            //!  length
  Double_t fELoss;             //!  energy loss
  Double_t fMass;              //!  particle mass
  Bool_t fIsInitialized;

  Int_t fPosIndex;             //!
  TClonesArray *fOtCollection; //! Hit collection
  TLorentzVector fpostot;      // global frame hit position (in)// da cancellare
  TLorentzVector fpostotin;    // global frame hit position (in)// da cancellare
  TLorentzVector fpostotout;   // global frame hit position (in)// da cancellare

  TObjArray *fPassNodes; //!
  // geometry type
  Bool_t valid;
  Int_t fGeoType;

  PndOtMapCreator *fMapper; //!

  /** Private method AddHit
   **
   ** Adds a CbmTrdPoint to the HitCollection
   **/
  PndOtPoint *AddHit(Int_t trackID, Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, TVector3 pos, TVector3 posInLocal, TVector3 posOutLocal, TVector3 momIn,
                     TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Double_t mass);

  std::string GetStringPart(std::string &aSrc, Int_t part, char aDelim);
  bool Split(std::string &aDest, std::string &aSrc, char aDelim);

  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();

  /** Private method GetSquaredDistanceFromWire
   **
   ** Returns the square of the distance of the current trackpoint to the wire
   *@return     distance
   **/
  float GetSquaredDistanceFromWire();

  PndOt(const PndOt &L);
  PndOt &operator=(const PndOt &) { return *this; }

  ClassDef(PndOt, 1)
};

inline void PndOt::ResetParameters()
{
  fTrackID = fVolumeID = 0;
  fPos.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosInLocal.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOutLocal.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
  fpostot.SetXYZM(0.0, 0.0, 0.0, 0.0);    // da cancellare
  fpostotin.SetXYZM(0.0, 0.0, 0.0, 0.0);  // da cancellare
  fpostotout.SetXYZM(0.0, 0.0, 0.0, 0.0); // da cancellare
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMass = 0;
  fIsInitialized = kFALSE;
  valid = kFALSE;
}

#endif // PNDOT_H
