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
// -----                       PndDrc header file                      -----
// -----               Created 11/10/06 by Annalisa Cecchi             -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDDRC_H
#define PNDDRC_H

#include <array>

#include "TLorentzVector.h"
#include "FairDetector.h"
#include "TGraph.h"
#include "TRandom.h"
#include "TArrayI.h"
#include "PndGeoHandling.h"

#include "PndDrcPDPoint.h"
#include "PndDrcEVPoint.h"
#include "PndDrcBarPoint.h"

class TClonesArray;
class FairVolume;
class PndGeoDrc;
class TClonesArray;

class PndDrc : public FairDetector {

 public:
  /** Default constructor **/
  PndDrc();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndDrc(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndDrc();

  /*! \brief  Kill secondaries at its production point.
    \param ss
  */
  void StopSecondaries(Bool_t ss = kFALSE) { fStopSecondaries = ss; }

  /*! \brief  Kill charged track by exiting the DIRC to avoid it hitting the large EV.
    \param sctad
  */
  void StopChargedTrackAfterDIRC(Bool_t sctad = kTRUE) { fStopChargedTrackAfterDIRC = sctad; }

  /*! \brief Set MCP-Pmt's quantum effeciency.
    \param 0 - Std QE
    \param 1 - Hiq 400
  */
  void SetDetEffId(Int_t val = 2) { fDetEffId = val; }

  /*! \brief  Kill photons at production point according to the detector efficiency distribution.
    \param dep

    NOTE! Transport efficiency can not be used together with Detector efficiency:
  it can not be used like:
  SetDetEffAtProduction(kTRUE)
  SetTransportEffAtProduction(kTRUE)

  at least one of them should be kFALSE
  */
  void SetDetEffAtProduction(Bool_t dep = kFALSE) { fDetEffAtProduction = dep; }

  /*! \brief  Kill photons at production point according to the transport efficiency distribution.
  \param tra

  NOTE! Transport efficiency can not be used together with Detector efficiency:
  it can not be used like:
  SetDetEffAtProduction(kTRUE)
  SetTransportEffAtProduction(kTRUE)

  at least one of them should be kFALSE

  */
  void SetTransportEffAtProduction(Bool_t tra = kFALSE) { fTransportEffAtProduction = tra; }

  /*! \brief  Choose between ideal and real mirror:
    \param mir
    kFALSE - take ideal mirror, reflectivity = 1 for photons within energy range 1..10 eV
    kTRUE  - take real  mirror, reflectivity has realistic values for photons within energy range 1.907..6.199 eV
  */
  void SetMirrorReal(Bool_t mir = kTRUE) { fTakeRealReflectivity = mir; }

  void SetBlackLensSides(Bool_t lesi = kFALSE) { fSetBlackLens = lesi; }

  /*! \brief  Set time after which photons are killed.
    \param ti The time (ns)
  */
  void SetStopTime(Double_t ti = -1.)
  {
    fPhoMaxTime = ti;
    if (fPhoMaxTime > 0.) {
      fStopTime = kTRUE;
    }
    if (fPhoMaxTime <= 0.) {
      fStopTime = kFALSE;
    }
  }

  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for DIRC detector and mirror.
   **/

  void SetOptionForLUT(Bool_t oplu = kFALSE) { fOptionForLUT = oplu; }

  virtual void Initialize();
  void SetPersistency(Bool_t v = kTRUE) { fPersistency = v; }

  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndDrcPoints and PndDrcMirrorPoints and adds
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume *vol = 0);

  virtual void FinishPrimary();

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
  virtual void CopyClones(TClonesArray *clPD1, TClonesArray *clPD2, TClonesArray *clBar1, TClonesArray *clBar2,
                          /*TClonesArray* clSdsPD1, TClonesArray* clSdsPD2, TClonesArray* clSdsBar1, TClonesArray* clSdsBar2, */ Int_t offset);

  // Int_t GetSensorId(TString);

  void FinishRun();

  /** Virtual method Construct geometry
   **
   **/
  virtual void ConstructGeometry();
  virtual void ConstructOpGeometry();
  //  void ConstructASCIIGeometry();
  //  virtual void ConstructRootGeometry();
  std::vector<std::string> fListOfSensitives;
  bool CheckIfSensitive(std::string name);

  void NumberOfBounces(TVector3, TVector3, Int_t, Int_t *, Int_t *, Double_t *, Double_t *);
  Double_t FindOutPoint(Double_t, Double_t, Double_t, Double_t *, Bool_t);

  void AddHit(Int_t trackID, Int_t copyNo, Int_t mcpId, TVector3 pos, TVector3 mom, TVector3 momAtEV, Double_t timeAtEV, Double_t time, Double_t length, Int_t pdgCode, Int_t eventID);

  void AddEVHit(Int_t trackID, Int_t copyNo, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Int_t eventID, Double_t timestart,
                          Double_t timestartEV, Double_t VeloPhoton, TVector3 normal);

  void AddBarHit(Int_t trackID, Int_t copyNo, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t nBar, Int_t eventID, Double_t mass);

  void SetRunCherenkov(Bool_t ch) { fRunCherenkov = ch; };

    FairModule* CloneModule() const override;
    Bool_t IsSensitive(const std::string& name) override;

 private:
  Bool_t fPersistency;

  // basic parameters of DIRC
  Double_t fpi;
  Double_t fzup;
  Double_t fzdown;
  Double_t fradius;
  Double_t fhthick;
  Double_t fpipehAngle;
  Double_t fbbGap;

  Double_t fbbnum;
  Double_t fbarnum;
  Double_t fphi0;
  Double_t fdphi;
  Double_t fbarwidth;

  PndGeoHandling *fGeoH;

  Bool_t fRunCherenkov; //  Switch ON/OFF Cherenkov propagation
  Int_t fTrackID;       //  track index
  Int_t fBarId;
  TLorentzVector fPos; //  position
  TLorentzVector fMom; //  momentum
  Double32_t fTime;    //  time
  Double32_t fLength;  //  length
  Double32_t fLengthEV;
  Double_t fAngIn;
  Int_t fNBar;
  Int_t fPosIndex;
  Int_t volDetector; // MC volume ID of drc
  Double_t fMass;
  TLorentzVector fMomAtEV;
  TLorentzVector fAna_point;
  Double_t fTimeAtEV;
  Double_t fTimeAtEVEntrance;
  Double_t fTimeStart;
  Double_t fVeloPhoton;
  Int_t fEVreflections;

  std::array<TGraph, 2> fDetEff; // Detector Efficiency as a function of photon wavelength
  Int_t fDetEffId;
  Bool_t fDetEffAtProduction;
  Bool_t fTransportEffAtProduction;

  Bool_t fStopTime;
  Double_t fPhoMaxTime;
  Bool_t fTakeRealReflectivity;
  Bool_t fStopSecondaries;
  Bool_t fStopChargedTrackAfterDIRC;
  Bool_t fSetBlackLens;
  Bool_t fOptionForLUT;

  PndGeoDrc *fGeo; // Pointer to basic DRC geometry data

  Int_t fPdgCode;
  Double_t fThetaC;


    std::vector<PndDrcPDPoint>* fPDPointVector{new std::vector<PndDrcPDPoint>};
    std::vector<PndDrcEVPoint>* fEVPointVector{new std::vector<PndDrcEVPoint>};
    std::vector<PndDrcBarPoint>* fBarPointVector{new std::vector<PndDrcBarPoint>};

    
  Int_t fEventID;

    PndDrc(const PndDrc&);
    PndDrc& operator=(const PndDrc&);


  ClassDef(PndDrc, 0)
};

#endif
