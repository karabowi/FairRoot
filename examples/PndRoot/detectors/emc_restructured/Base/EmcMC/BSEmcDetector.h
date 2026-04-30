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
// -----					             	BSEmcDetector
// -----					Based on FairDetector and PndSdsDetector
// -----               Created 12/03/2019 by B. Salisbury
// -------------------------------------------------------------------------
#ifndef BSEMCDETECTOR_HH
#define BSEMCDETECTOR_HH

#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TLorentzVector.h"
#include "TString.h"
#include "TVector3.h"

#include "FairDetector.h"

#include "PndGeoHandling.h"

class PndSensorNameIdMap;
class TClonesArray;
class TGeoNode;
class BSEmcMCPoint;
class FairVolume;
class TGeoMatrix;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Specialization of the FairDetector for a common BSEmcDetector
 *
 * It implements common functionality between the different BSEmc detector components
 * (e.g. Forward/Backward Endcaps, Barrel) such as the ProcessHits, Point-registration, etc.
 * It is almost a one to one copy of T. Stockmanns PndSdsDetector.
 *
 * @author Tobias Stockmanns
 * @author B. Salisbury <salisbury@hiskp.uni-bonn.de>
 * @ingroup EmcBase
 */
class BSEmcDetector : public FairDetector {
 public:
  BSEmcDetector();

  BSEmcDetector(BSEmcDetector &other)
    : FairDetector(other), fListOfSensitives(other.fListOfSensitives), fGeoH(PndGeoHandling::Instance()), fIdMap(other.fIdMap), fPosIndex(other.fPosIndex),
      fOutBranchName(other.fOutBranchName), fFolderName(other.fFolderName), fPndEmcCollection(other.fPndEmcCollection), fTransformMatrix(other.fTransformMatrix)

  {
    ResetParameters();
  };

  /** Standard constructor.
   *@param name    detector name
   *@param active  sensitivity flag
   **/
  BSEmcDetector(const char *t_name, Bool_t t_active);

  /** Destructor **/
  virtual ~BSEmcDetector();

  // BSEmcDetector &operator=(BSEmcDetector &other)
  // {
  //   fListOfSensitives = other.fListOfSensitives;
  //   fPosIndex = other.fPosIndex;
  //   fOutBranchName = other.fOutBranchName;
  //   fFolderName = other.fFolderName;
  //   fPndEmcCollection = other.fPndEmcCollection;
  //   fTransformMatrix = other.fTransformMatrix;
  //   //fGeoH= PndGeoHandling::Instance();
  //   return *this;
  // };

  virtual void Initialize() /*override*/;

  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmTrdPoints and adds them to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume *t_vol = nullptr) /*override*/;

  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, Print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent() /*override*/;

  virtual void FinishRun() /*override*/;

  /** Virtual method Register
   **
   ** Registers the hit collection in the ROOT manager.
   **/
  virtual void Register() /*override*/;

  /** Accessor to the hit collection **/
  virtual TClonesArray *GetCollection(Int_t t_iColl) const /*override*/;

  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print() const;

  /** Virtual Method SetSpecialPhysicsCuts
   **
   ** replaces some Geant initialisation via g3Config.C and so on
   **/
  virtual void SetSpecialPhysicsCuts() /*override*/;

  /** Virtual method Reset
   **
   ** Clears the hit collection
   **/
  virtual void Reset() /*override*/;

  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
  virtual void CopyClones(TClonesArray *t_origin, TClonesArray *t_target, Int_t t_offset) /*override*/;

  /** Virtual method Construct geometry
   **
   ** Constructs the BSEmcDetector geometry
   **/
  virtual void ConstructGeometry() /*override*/;
  // void ConstructRootGeometry();
  virtual void ConstructASCIIGeometry() /*override*/;

  void SetFolderName(const TString &t_folder) { fFolderName = t_folder; }
  void SetBranchOutName(const TString &t_branchout) { fOutBranchName = t_branchout; }
  const TString &GetFolderName() const { return fFolderName; }
  const TString &GetBranchOutName() const { return fOutBranchName; }

  void SetSensitiveNames(const std::vector<std::string> &t_sensNames) { fListOfSensitives = t_sensNames; };
  virtual void SetDefaultSensitiveNames() { fListOfSensitives = {"Crystal"}; } /*override*/;
  const std::vector<std::string> &GetListOfSensitives() { return fListOfSensitives; }
  void SetTransformMatrix(TGeoMatrix *t_transform) { fTransformMatrix = t_transform; }
  const TGeoMatrix *GetTransformMatrix() const { return fTransformMatrix; }

  void SetIdMap(PndSensorNameIdMap *t_map) { fIdMap = t_map; }
  PndSensorNameIdMap *GetIdMap() { return fIdMap; }

  void SetPersistency(Bool_t t_storePoints) { fPersistency = t_storePoints; }

 protected:
  virtual Int_t GetDetectorId();
  Bool_t CheckIfSensitive(std::string t_name) /*override*/;

  /** Private method AddHit
   **
   ** Adds a BSEmcMCPoint to the HitCollection fPndEmcCollection
   **/
  BSEmcMCPoint *
  AddHit(Int_t t_trackID, Int_t t_detID, Int_t t_evtID, TVector3 t_pos, TVector3 t_mom, Double_t t_tof, Double_t t_length, Double_t t_eLoss, Bool_t t_enterning, Bool_t t_exiting);

  /** Private method ResetParameters
   **
   ** Resets the private members for the fPosIndex
   **/
  void ResetParameters();

 protected:
  std::vector<std::string> fListOfSensitives{}; //!
  PndGeoHandling *fGeoH{nullptr};               //!
  PndSensorNameIdMap *fIdMap{nullptr};          //!

 private:
  Int_t fPosIndex{-1};                      //!
  TString fOutBranchName{""};               //!
  TString fFolderName{""};                  //!
  TClonesArray *fPndEmcCollection{nullptr}; //! Point collection

  Int_t fTrackID{-1};                    //!  track index
  Int_t fVolumeID{-1};                   //!  Volume id
  Int_t fCopyNumber{-1};                 //!  Copy number of volume
  Int_t fEventID{-1};                    //!  event id
  TLorentzVector fPos{};                 //!  position in global frame
  TLorentzVector fMom{};                 //!  momentum
  Double32_t fTime{-1};                  //!  time
  Double32_t fLength{-1};                //!  length
  Double32_t fELoss{-1};                 //!  energy loss
  TGeoMatrix *fTransformMatrix{nullptr}; //!
  Int_t fNumberOfPoints{0};
  Bool_t fPersistency{kFALSE};
  ClassDef(BSEmcDetector, 7);
};

#endif /*BSEMCDETECTOR_HH*/
