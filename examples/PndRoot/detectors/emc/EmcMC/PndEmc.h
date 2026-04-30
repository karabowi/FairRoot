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
// -----                     PndEmc header file                        -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMC_H
#define PNDEMC_H

#include "TClonesArray.h"
#include "TVector3.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "FairDetector.h"

class TClonesArray;
class PndEmcPoint;
class FairVolume;
class TGeoNode;

/**
 * @brief Simulation of EMC
 *
 * @ingroup PndEmc
 */
class PndEmc : public FairDetector {
 public:
  /** Default constructor **/
  PndEmc();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndEmc(const char *name, Bool_t active, Bool_t fast = kFALSE, Bool_t storepnts = kTRUE);

  /** Destructor **/
  virtual ~PndEmc();

  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for MUO detector and mirror.
   **/
  virtual void Initialize();

  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndEmcPoints and PndEmcMirrorPoints and adds
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

  // construct detector from .root file
  void ConstructRootGeometry();

  // construct detector -Barrel- from .root file
  void ConstructRootGeomMod12();

  // construct detector -BwEndCap- from .root file
  void ConstructRootGeomMod4();

  // construct detector -Fsc- from .root file
  void ConstructRootGeomMod5();

  // recursively add volumes to the geoManager
  void ExpandNode(TGeoVolume *fVol, TGeoVolume *Cave);

  //  virtual Bool_t CheckIfSensitive(std::string name);

  // general function for chosing the combination of EMC geometry
  virtual void SetGeometryVersion(const Int_t GeoNumber);
  //
  virtual void SetGeometryFileNameDouble(TString fname, TString fname2, Int_t fwbwchoice = 0, TString geoVer = "0");
  //
  virtual void SetGeometryFileNameTriple(TString fname, TString fname2, TString fname3, TString geoVer = "0");

  virtual void SetGeometryFileNameQuadruple(TString fname, TString fname2, TString fname3, TString fname4, TString geoVer = "0");

  PndEmcPoint *AddHit(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, Short_t mod, Short_t row, Short_t crys,
                      Short_t copy, Bool_t enterning, Bool_t exiting);

  void SetStorageOfData(Bool_t val); // Method to specify whether points are stored or not.

  //      replaces G3 initialisation via g3Config.C
  virtual void SetSpecialPhysicsCuts();

 private:
  Int_t fTrackID;               //!  track index
  Int_t fVolumeID;              //!  volume id
  Int_t fEventID;               //!  event id
  TLorentzVector fPos;          //!  position
  TLorentzVector fMom;          //!  momentum
  Double32_t fTime;             //!  time
  Double32_t fLength;           //!  length
  Double32_t fELoss;            //!  energy loss
  Int_t fPosIndex;              //!
  TClonesArray *fEmcCollection; //! Hit collection
  // Int_t volDetector;     //!  MC volume ID of MUO
  Bool_t bIsFastFsc; //!  Flag for fast fsc geometry
  Bool_t fStoreData; // Flag which specify whether points are stored or not
  Bool_t fwendcap;   //!  Flag for the new FwEndCap geometry
  Bool_t bwendcap;   //!  Flag for the new BwEndCap geometry

  TString fgeoName2;
  TString fgeoName3;
  TString fgeoName4;

  Int_t MapperVersion;

  // reset all parameters
  void ResetParameters();

  PndEmc(const PndEmc &L);
  PndEmc &operator=(const PndEmc &) { return *this; };

  ClassDef(PndEmc, 1)
};

#endif
