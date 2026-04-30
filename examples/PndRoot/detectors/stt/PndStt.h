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
// -----                         PndStt header file                    -----
// -----                  Created 25/03/06  by R. Castelijns           -----
// -----                          Based on CbmTof                      -----
// -------------------------------------------------------------------------

/**  PndStt.h
 *@author R.Castelijns <r.castelijns@fz-juelich.de>
 **
 ** Defines the active detector STT. Constructs the geometry and
 ** registers MCPoints.
 **/

#ifndef PNDSTT_H
#define PNDSTT_H

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "FairDetector.h"
#include "PndGeoSttPar.h"
#include "FairRun.h"

#include <string>
#include <sstream>

class TClonesArray;
class PndSttPoint;
class FairVolume;

class PndStt : public FairDetector {

 public:
  /** Default constructor **/
  PndStt();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndStt(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndStt();

  virtual void Initialize();

  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmTrdPoints and adds them to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume *vol = 0);

  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();

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
   ** Constructs the STT geometry
   **/
  virtual void ConstructGeometry();
  void SetDefaultSensorNames();

 protected:
  std::vector<std::string> fListOfSensitives;
  bool CheckIfSensitive(std::string name);

 private:
  /** Track information to be stored until the track leaves the
      active volume. **/
  Int_t fTrackID;        //!  track index
  Int_t fVolumeID;       //!  volume id
  TVector3 fPosInLocal;  //!  entry position in straw frame
  TVector3 fPosOutLocal; //!  exit position in straw frame
  TVector3 fMomIn;       //!  momentum
  TVector3 fMomOut;      //!  momentum
  Double_t fTime;        //!  time
  Double_t fLength;      //!  length
  Double_t fELoss;       //!  energy loss
  Double_t fMass;        //!  particle mass
  Bool_t fIsInitialized;

  Int_t fPosIndex;              //!
  TClonesArray *fSttCollection; //! Hit collection
  TVector3 fpostot;             // global frame hit position (in)// da cancellare
  TVector3 fpostotin;           // global frame hit position (in)// da cancellare
  TVector3 fpostotout;          // global frame hit position (in)// da cancellare

  TObjArray *fPassNodes; //!

  // geometry type
  Int_t fGeoType;

  // flag to tell if a track entered the tube
  Bool_t fInFlag; //!

  /** Private method AddHit
   **
   ** Adds a CbmTrdPoint to the HitCollection
   **/
  PndSttPoint *AddHit(Int_t trackID, Int_t detID, Int_t tubeID, TVector3 pos, TVector3 posInLocal, TVector3 posOutLocal, TVector3 momIn, TVector3 momOut, Double_t time,
                      Double_t length, Double_t eLoss, Double_t mass);

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

  PndStt(const PndStt &L);
  PndStt &operator=(const PndStt &) { return *this; };

  ClassDef(PndStt, 3)
};

inline void PndStt::ResetParameters()
{
  fTrackID = fVolumeID = 0;
  fPosInLocal.SetXYZ(0.0, 0.0, 0.0);
  fPosOutLocal.SetXYZ(0.0, 0.0, 0.0);
  fMomIn.SetXYZ(0.0, 0.0, 0.0);
  fMomOut.SetXYZ(0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
  fpostot.SetXYZ(0.0, 0.0, 0.0);    // da cancellare
  fpostotin.SetXYZ(0.0, 0.0, 0.0);  // da cancellare
  fpostotout.SetXYZ(0.0, 0.0, 0.0); // da cancellare
  fMass = 0;
}

#endif
