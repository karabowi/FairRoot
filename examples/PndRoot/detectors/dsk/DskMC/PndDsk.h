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
// -----                     PndDsk header file                        -----
// -----                Created 23/10/07  by P. Koch                   -----
// -------------------------------------------------------------------------

#ifndef PNDDSK_H
#define PNDDSK_H

#include "TLorentzVector.h"
#include "TVector3.h"
#include "TH2F.h"
#include "PndGeoDskFLG.h"

#include "FairDetector.h"
#include "PndDskFLGHit.h"
#include "PndDskCerenkov.h"
#include "PndDskParticle.h"
#include "PndDskTrackPoint.h"

class TClonesArray;

class PndDsk : public FairDetector {

 public:
  /** Default constructor **/
  PndDsk();

  /** Standard constructor with arguments
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndDsk(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndDsk();

  /** Virtual method Initialize
   ** Initialises detector.
   **/
  virtual void Initialize();

  /** Virtual method ProcessHits
   **
   ** Decides if actions for a Cerenkov or a Particle have to be taken
   ** and calls the ProcessHitsX method accordingly.
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
   ** Registers the collections in the ROOT manager.
   **/
  virtual void Register();

  /** Virtual method GetCollection
   **
   ** Accessor to the Cerenkov and Particle collections
   **/
  virtual TClonesArray *GetCollection(Int_t iColl) const;

  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print() const;

  /** Virtual method Reset
   **
   ** Clears the collections
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

  /** Virtual method ConstructGeometry
   **
   ** Decide how to construct the geometry
   **/
  virtual void ConstructGeometry();

  /** Virtual method CheckIfSensitive
   **
   ** Decide if volume is sensitive
   *@param name  Name of the volume to check
   **/
  virtual Bool_t CheckIfSensitive(std::string name);

  /** Method DichroicMirrorTransmitted
   **
   ** simulates dichroic mirrors
   **/
  Bool_t DichroicMirrorTransmitted(Double_t wavelength, Int_t detector_type);

  /** Method AddCerenkov
   **
   ** Adds a Cerenkov to the collection
   **/
  void AddCerenkov(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Double_t energy, Double_t wavelength, Int_t motherTrackID,
                              Int_t motherPdgCode, TString motherPdgName);

  void AddHit(Int_t trackID, Int_t detectorID, TVector3 position_store, TVector3 momentum_store, Double_t time, Double_t angIn, Double_t thetaC_store,
                       TVector3 Cherenkov_photon, Int_t light_guide, Int_t pixel);

  /** Method AddParticle
   **
   ** Adds a Particle to the collection
   **/
  void AddParticle(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Int_t pdgCode, TString pdgName, Double_t energy,
                              Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName, Double_t mass, Double_t angIn, Double_t thetaC, Int_t nPhot);

  /** Method AddTrackPoint
   **
   ** Adds a TrackPoint to the collection
   **/
  void AddTrackPoint(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Double_t length, Double_t eLoss);

  /** Modifiers **/
  void SetDebugLevel(Int_t debugLevel) { fDebugLevel = debugLevel; }
  // void SetDetectors(Int_t detectorTypes, Int_t detectorsPerArray, Bool_t usingMirrors);  // not implemented
  void SetPDE(Double_t pde) { fPDE = pde; }
  void SetStoreCerenkovs(Bool_t storeCerenkovs) { fStoreCerenkovs = storeCerenkovs; }
  void SetStoreParticles(Bool_t storeParticles) { fStoreParticles = storeParticles; }
  void SetStoreTrackPoints(Bool_t storeTrackPoints) { fStoreTrackPoints = storeTrackPoints; }

  void SetTrapFraction(std::string name); // use trap fraction from fast sim, temp.

        FairModule* CloneModule() const override;
    Bool_t IsSensitive(const std::string& name) override;

 private:
  /** Private method ProcessHitsCerenkov
   **
   ** Defines the action to be taken when
   ** Cerenkov does a step inside the active volume.
   ** Creates and adds Cerenkovs to collection
   *@param vol  Pointer to the active volume
   **/
  Bool_t ProcessHitsCerenkov(FairVolume *vol = 0);
  Bool_t ProcessHitsCerenkov_FLG(FairVolume *vol = 0);
  /** Private method ProcessHitsParticle
   **
   ** Defines the action to be taken when
   ** Particle does a step inside the active volume.
   ** Creates and adds Particles to collection
   *@param vol  Pointer to the active volume
   **/
  Bool_t ProcessHitsParticle(FairVolume *vol = 0);

  /** Method TrackCerenkov
   **
   ** Decides weather to track a Cerenkov or not
   **/
  Bool_t DoNotTrackCerenkov();

  TH2F *trapfrac[5]; //!

  PndGeoDskFLG *fGeo; //!

  Int_t fDebugLevel; //! Debug level

    TClonesArray *fDskCerenkovCollection;   //! Cerenkov collection
  TClonesArray *fDskParticleCollection;   //! Particle collection
  TClonesArray *fDskTrackPointCollection; //! TrackPoint collection

  TClonesArray *fDskFLGHitArray; //! DSK hits

    std::vector<PndDskCerenkov>* fCerVector{new std::vector<PndDskCerenkov>};
    std::vector<PndDskParticle>* fPartVector{new std::vector<PndDskParticle>};
    std::vector<PndDskTrackPoint>* fPointVector{new std::vector<PndDskTrackPoint>};
    std::vector<PndDskFLGHit>* fHitVector{new std::vector<PndDskFLGHit>};

  Bool_t fStoreCerenkovs;       //! Whether to store Cerenkovs (default) or not
  Bool_t fStoreParticles;       //! Whether to store Particles (default) or not
  Bool_t fStoreTrackPoints;     //! Whether to store TrackPoints or not (default)
  Bool_t fStoreFLGHits;         //! Whether to store FLGHits (default) or not
  Bool_t fCalcPWay;             //! Whether to calc Projected Way or not (default)
  Bool_t fMeasureTotalRefAngle; //! Whether to measure total reflection angle or not (default)

  Int_t fTrackID;     //! Index of MCTrack
  Int_t fDetectorID;  //! Detector ID (volume)
  TVector3 fPosition; //! Position [cm]
  TVector3 fMomentum; //! Momentum [GeV]
  Double_t fTime;     //! Global time (since event start) [ns]
  Double_t fLength;   //! Track length since creation (without mothers) [cm]
  Double_t fELoss;    //! Energy deposit [GeV]

  Double_t fPDE;        //! Photon Detection Efficiency [0-1, 2]
  Int_t fPdgCode;       //! PDG code of current particle
  Double_t fEnergy;     //! Energy [eV / GeV]
  Double_t fWavelength; //! Vacuum wavelength hc/fEnergy [nm]

  Int_t fMotherTrackID;   //! Track ID of the particle that emitted this Cerenkov
  Int_t fMotherPdgCode;   //! PDG code of the particle that emitted this Cerenkov
  TString fMotherPdgName; //! translation of PDG code

  Short_t fDetType;      //! Detectortype that registered the Cerenkov
  Int_t fDetNumber;      //! Number of the Detector that registered the Cerenkov
  Double_t fDetTime;     //! Global time when Cerenkov was detected [ns]
  TVector3 fDetMomentum; //! Momentum when detected [eV]

  Double_t fPrimaryAngleToCerenkov; //! Angle at the moment of first appearance
  Double_t fPrimaryHitAngle;        //! Angle to the z-Axis at first appearance
  TVector3 fPrimaryHitMomentum;     //! Angle between momentum of eachs first appearance

  TString fPdgName;      //! PDG name according to fPdgCode
  TVector3 fEndPosition; //! Position when particles disappears
  Double_t fEndTime;     //! Time when particle disappears
  TVector3 fEndMomentum; //! Momentum when particle disappears
  Double_t fEndEnergy;   //! Energy when particle disappears

  Double_t fAngIn;  //!
  Double_t fThetaC; //!

  TLorentzVector tmpLVec; //! often needed, avoid allocation

            PndDsk(const PndDsk&);
    PndDsk& operator=(const PndDsk&);

  ClassDef(PndDsk, 1)
};

#endif // PNDDSK_H
