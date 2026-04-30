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
// -----                         PndGemDetector header file                    -----
// -----                  Created 27/10/08  by R.Kliemt                        -----
// -------------------------------------------------------------------------

/**  PndGemDetector.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Defines the active detector PndGemDetector. Constructs the geometry and
 ** registers MCPoints.
 **/

#ifndef PNDGEMDETECTOR_H
#define PNDGEMDETECTOR_H

#include "PndGemMCPoint.h"

#include "FairDetector.h"

#include "TString.h"
#include "TLorentzVector.h"

#include <vector>

class TClonesArray;
class TGeoNode;
class PndGemMCPoint;
class FairVolume;

class PndGemDetector : public FairDetector {

 public:
  /** Default constructor **/
  PndGemDetector();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndGemDetector(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndGemDetector();

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
   ** If verbosity level is set, Print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();

  virtual void FinishRun();

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
   ** Constructs the PndGemDetector geometry
   **/
  virtual void ConstructGeometry();
  // void ConstructRootGeometry();
  //   void ConstructASCIIGeometry();
  // void ExpandNode(TGeoNode *fN);

  void MisalignDetector();

  void SetExclusiveSensorType(const TString sens);
  void SetRadDamOption(bool val) { fUseRadDamOption = val; };
  bool GetRadDamOption() { return fUseRadDamOption; }

    FairModule* CloneModule() const override;
    Bool_t IsSensitive(const std::string& name) override;

    std::vector<FairMCPoint*> GetPointVectorForIndex(Int_t iColl) const override {
        if (iColl == 0) {
            // Build a vector of base pointers from the concrete vector — no copy of data
            std::vector<FairMCPoint*> ptrs;
            ptrs.reserve(fPointVector->size());
            for (auto& p : *fPointVector)
                ptrs.push_back(&p);   // FairMCPoint* points into the existing vector elements
            return ptrs;
        }
        return {};
    }

    
 private:
  Int_t GetSensorId(TString detName);

  /** Track information to be stored until the track leaves the
      active volume. **/
  Int_t fTrackID;                  //!  track index
  Int_t fVolumeID;                 //!  Det id
  TLorentzVector fPosIn;           //!  entry position in global frame
  TLorentzVector fPosOut;          //!  exit position in global frame
  TLorentzVector fMomIn;           //!  momentum
  TLorentzVector fMomOut;          //!  momentum
  Double32_t fTime;                //!  time
  Double32_t fLength;              //!  length
  Double32_t fELoss;               //!  energy loss
                                   //  std::map<TString, Int_t> fVolumeIDMap;	 //!  map to create a unique Detector ID
                                   //  PndGemGeoHandling* fGeoH;	     //! Gives Access to the Path info of a hit
  Int_t fPosIndex;                 //!

    std::vector<PndGemMCPoint>* fPointVector{new std::vector<PndGemMCPoint>};
    
  bool fUseRadDamOption;           //! enables the detection of neutral particles

  std::vector<std::string> fListOfSensitives;

  bool CheckIfSensitive(std::string name);

  /** Private method AddHit
   **
   ** Adds a PndGemMCPoint to the HitCollection
   **/
  void AddHit(Int_t trackID, Int_t detID, Int_t sensID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss);

  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();

    PndGemDetector(const PndGemDetector&);
    PndGemDetector& operator=(const PndGemDetector&);


  ClassDef(PndGemDetector, 5);
};

inline void PndGemDetector::ResetParameters()
{
  fTrackID = 0;
  fVolumeID = 1;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
}

#endif
