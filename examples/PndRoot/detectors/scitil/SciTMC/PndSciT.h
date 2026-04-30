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

// --------------------------------------------------------------------------
// -----                     PndSciT  header file                 	-----
// -----                    created by A. Sanchez                  	-----
// -----                   modified by D. Steinschaden                 	-----
// -----                   last update    06.2015 		       	-----
// --------------------------------------------------------------------------

#ifndef PNDSCIT_H
#define PNDSCIT_H

#include "PndGeoSciTPar.h"
#include "PndGeoHandling.h"

#include "PndSciTPoint.h"

#include "FairDetector.h"

//#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include <string>
#include <vector>

class TClonesArray;
class PndSciTPoint;
class FairVolume;

class PndSciT : public FairDetector {

 public:
  /** Default constructor **/
  PndSciT();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndSciT(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndSciT();

  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for MUO detector and mirror.
   **/
  virtual void Initialize();

  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndSciTPoints and PndSciTMirrorPoints and adds
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

  // void ConstructASCIIGeometry();

  void SetThreshold(Double_t val) { fThreshold = val; }; // default value is 0.0001 GeV

  void AddHit(Int_t eventID, Int_t trackID, Int_t sensorID, TString detName, TVector3 posin, TVector3 momin, TVector3 posout, TVector3 momout, Double_t tof,
                       Double_t length, Double_t eLoss);

    FairModule* CloneModule() const override;
    Bool_t IsSensitive(const std::string& name) override;

    
 private:
  std::vector<std::string> fListOfSensitives;
  bool CheckIfSensitive(std::string name);

  Int_t fEventID;  //  event id
  Int_t fTrackID;  //  track index
  Int_t fSensorID; // used as volume ID/ detector ID coming from PndGeoHandling
  TString fdetPath;

  TLorentzVector fPosIn;  //   position
  TLorentzVector fMomIn;  //  momentum
  TLorentzVector fPosOut; //  position
  TLorentzVector fMomOut; //  momentum

  Double32_t fTime;    //   time
  Double32_t fLength;  //   length
  Double_t fELoss;     //   energy loss
  Double_t fThreshold; // energy loss threshold

  PndGeoHandling *fGeoH;         //! Gives Access to the Path info of a hit
  PndGeoSciTPar *par;            //! for saving parameters, although not mandatory, may someone can make use out of the stored parameters
  Int_t fPosIndex;               // Used when Copying the TCloneArrays
  TClonesArray *fSciTCollection; // Hit collection
    std::vector<PndSciTPoint>* fPointVector{new std::vector<PndSciTPoint>};

  // reset all parameters
  void ResetParameters();

    PndSciTPoint(const PndSciTPoint&);
    PndSciTPoint& operator=(const PndSciTPoint&);


  ClassDef(PndSciT, 4)
};

inline void PndSciT::ResetParameters()
{
  fTrackID = fSensorID = fEventID = -1;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);

  fTime = fLength = fELoss = 0;
};

#endif
