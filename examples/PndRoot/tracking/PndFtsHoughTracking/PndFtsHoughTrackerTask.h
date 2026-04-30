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

/**
 @class PndFtsHoughTrackerTask

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Interface between PandaRoot (data input and output) and PndFtsHoughTrackFinder (implementation of PR algorithm).

 This task provides functionality / data for the FTS PR classes by passing a pointer to itself as an argument in the object's constructor.

 This class was originally modeled after
 mvd/MvdTracking/PndMvdRiemannTrackFinderTask
 (among many others)

 TODO
 Make this task work for time-based simulation as well [have a look at PndMvdRiemannTrackFinderTask::FillHitArray()]

 Created: 18.06.2013
 */

#ifndef PndFtsHoughTrackerTask_H
#define PndFtsHoughTrackerTask_H

#include "PndPersistencyTask.h"
#include <cmath>
#include <vector>
#include "Rtypes.h" // for Double_t, Int_t, etc
#include <iostream>

#include "TVector3.h"
#include "PndFtsHit.h"
#include "TClonesArray.h"
#include "PndFtsTube.h"

class PndFtsHoughTrackCand;
class PndFtsHoughSpace;
class PndGeoFtsPar;
class FairField;

// For error reporting
#include "TString.h"
#include <stdexcept>
// for debugging
#include "FairMCPoint.h"

class PndFtsHoughTrackerTask : public PndPersistencyTask {
 public:
  enum DebugLevelMultiplicators {
    kHoughSpaces = 2,
    kAllFoundPeaksTogether = 3,
    kEachFoundPeakSeparately = 5,
    kMcTruthPeaksExclusively = 7,
    kMcTruthPeaksProjected = 11,
    kHitCurvesExclusively = 13,
    kHitCurvesProjected = 17
  };

  /** @brief Constructor with flags. Can also be used as standard constructor.
   * @param[in] verbose Verbosity level: 0 least, higher -> more output.
   * @param[in] persistence kFALSE does not write track candidates from PR to output root file.
   * @param[in] saveDebugInfo kTRUE will write internal representation of track(let) candidates to output root file.
   */
  PndFtsHoughTrackerTask(Int_t verbose = 0, Bool_t persistence = kTRUE);
  /** @brief Destructor.	 */
  ~PndFtsHoughTrackerTask();

  /** @brief Loads the parameter container from the runtime database. */
  virtual void SetParContainers();
  /** @brief Initialization of task at the beginning of a run. */
  virtual InitStatus Init();
  /** @brief ReInitiliazation of task when the runID changes. */
  virtual InitStatus ReInit();

  /** @brief Executed for each event.
   * @param opt Not used.
   */
  virtual void Exec(Option_t *opt);
  /** @brief When is this executed? After each event? */
  virtual void FinishEvent();

  /** @brief Writes output to root file, I guess. Called at the end of the run. */
  virtual void Finish();

  //----------
  // SETTERS
  //----------
  void SetVerbose(Int_t verbose) { fVerbose = verbose; };
  void SetPersistence(Bool_t val) { SetPersistency(val); };
  void SetSaveDebugInfo(Int_t saveDebugInfo) { fSaveDebugInfo = saveDebugInfo; };
  //	void SetTrackOutput(TString name = "FTSTrkHough") { fTracksArrayName = name; };

  //----------
  // GETTERS
  //----------
  Int_t GetVerbose() const { return fVerbose; };             ///< @brief Returns the verbosity level.
  Int_t GetSaveDebugInfo() const { return fSaveDebugInfo; }; ///< @brief Returns the save debug flag.
  UInt_t GetEventNr() const { return fEventNr; };            ///< @brief Returns the event number.

  //-----------
  // DATA ACCESS TO FTS
  //-----------
  /**@brief Returns the number of FTS hits.
   * @return Number of FTS hits.
   */
  Int_t GetNFtsHits() const { return fFtsHitArray->GetEntriesFast(); };
  /** @brief Returns pointer to the hit with index hitId in the FTS hit array.
   * @param[in] hitId Index (in FTS hit array) of the hit which should be returned.
   * @return Pointer to hit with index hitId in FTS hit array.
   */
  const PndFtsHit *GetFtsHit(UInt_t hitId) const
  {
    if (hitId >= (UInt_t)GetNFtsHits())
      throwError("GetFtsHit: hitId is too large.");
    //		TClonesArray *ftsHitArray= (TClonesArray *)FairRootManager::Instance()->GetObject("FTSHit");
    const PndFtsHit *myHit = (PndFtsHit *)fFtsHitArray->At(hitId);
    if (0 == myHit)
      throwError("GetFtsHit was not able to get the hit.");
    return myHit;
  };
  Int_t getMcTruthIdForHitId(UInt_t hitId) const
  {
    const PndFtsHit *const ftsHit = GetFtsHit(hitId);
    Int_t mcPointId = ftsHit->GetRefIndex();
    if (0 > mcPointId)
      throwError("getMcTruthIdForHitId: negative mcPointId.");
    FairMCPoint *myPoint = (FairMCPoint *)(fFtsMcPoints->At(mcPointId));
    if (0 == myPoint)
      throwError("getMcTruthIdForHitId: Could not get point belonging to hit.");
    Int_t mcTrackId = myPoint->GetTrackID();
    if (mcTrackId < 0)
      throwError("getMcTruthIdForHitId: negative mcTrackId.");
    return mcTrackId;
  }
  /** @brief Returns pointer to the FTS tube corresponding to input FTS hit.
   * @param[in] myHit: FTS hit for which the tube should be returned.
   * @return Pointer to tube corresponding to myHit.
   */
  const PndFtsTube *GetFtsTube(const PndFtsHit *const myHit) const
  {
    Int_t tubeID = myHit->GetTubeID();
    const PndFtsTube *tube = (PndFtsTube *)fFtsTubeArray->At(tubeID);
    return tube;
  }
  /** @brief Returns the position error (based on FTS straw geometry) for the hit with index hitId in the FTS hit array.
   * @param[in] pointer to hit for which the error should be returned.
   * @return Error in cm.
   */
  const TVector3 GetFtsHitPosErrors(const PndFtsHit *const ftsHit) const;
  /** @brief Returns the position covariance matrix (based on FTS straw geometry) for the hit with index hitId in the FTS hit array.
   * @param[in] ftsHit pointer to hit for which the error should be returned.
   * @return Error in cm.
   */
  const TMatrixT<Double_t> GetFtsHitCovMatrix(const PndFtsHit *const ftsHit) const;
  /**@brief Returns detector Id of FTS. Try not to use it.
   *
   * @see GetFtsHit
   * @see GetNHits
   * @return Detector Id of FTS.
   */
  Int_t getFtsBranchId() const { return fFtsBranchId; };
  /**@brief Returns pointer to the hit array in which FTS hits are saved as PndFtsHit. Try not to use it.
   *
   * @see GetFtsHit
   * @see GetNHits
   * @return Pointer to the hit array in which hits are saved as PndFtsHit.
   */
  TClonesArray *getFtsHitArrayPtr() const { return fFtsHitArray; };

  //-----------
  // DATA ACCESS TO B FIELD
  //-----------
  FairField *getMagneticFieldPtr() const { return fField; }; ///< @brief Returns pointer to the B field.

  FairLogger *fLogger; ///< @brief For output handling.

 protected:
  //  for writing out histograms for debugging
  //	void InitOutFileForDebugging();
  //	void AddNewEventToOutFileForDebugging(UInt_t eventNr);
  //	TFile* fOutFile;

  //--------
  // General
  //--------

  //--------
  // Input
  //--------
  Int_t fFtsBranchId;         ///< @brief Detector Id of FTS.
  TClonesArray *fFtsHitArray; ///< @brief Input array of PndFtsHit
  TClonesArray *fFtsMcPoints; ///< @brief Input array of McPoints

  /** @brief Needed for FTS map creator.
   *
   * I don't really know what that does...
   */
  PndGeoFtsPar *fFtsParameters;
  /** @brief Input array of PndFtsTube (map of FTS tubes).
   *
   * Is filled by map creator
   */
  TClonesArray *fFtsTubeArray;

  /// @brief For B field access.
  FairField *fField;

  //--------
  // Output
  //--------
  TString fTracksArrayName;  ///< @brief Branch name where to store the Track candidates
  TClonesArray *fTrackCands; ///< @brief Array of found track candidates in PndTrackCand (for output)
  TClonesArray *fTracks;     ///< @brief Array of found tracks in PndTrack (for output)
  //	TClonesArray *fHoughTrackCands; ///< @brief Array of found track cands in PndFtsHoughTrackCand (only for debugging)
  /* @brief Not used.
   *
   * The idea was to use this in order to write out the Hough spaces to the root file.
   * Due to bad performance this is not used. Instead, I write it out as pictures directly.
   */
  // TClonesArray *fHoughSpaces;

  //-------
  // Debug
  //-------
  Int_t fSaveDebugInfo; ///< @brief Debug information will be created if >0.
  UInt_t fEventNr;      ///< @brief Event number for debugging purposes.
  /** @brief For error reporting */
  void throwError(const TString s) const { throw std::runtime_error(s.Data()); };

  void CheckForDuplicateFtsHits()
  {
    for (int iHit1 = 0; iHit1 < GetNFtsHits(); ++iHit1) {
      const PndFtsHit *const myHit1 = GetFtsHit(iHit1);

      const Int_t tubeIdHit1 = myHit1->GetTubeID();

      for (int iHit2 = iHit1 + 1; iHit2 < GetNFtsHits(); ++iHit2) {
        const PndFtsHit *const myHit2 = GetFtsHit(iHit2);

        const Int_t tubeIdHit2 = myHit2->GetTubeID();

        if (tubeIdHit1 == tubeIdHit2)
          std::cout << "Event " << GetEventNr() << "tubeID1=" << tubeIdHit1 << " tubeID2=" << tubeIdHit2 << ": HitIdx " << iHit1 << " and HitIdx " << iHit2 << " are duplicate!\n";
      }

    } // for loop over all hits
  };

  // TODO: I don't think I need the copy constructor and the operator=
  PndFtsHoughTrackerTask(const PndFtsHoughTrackerTask &);
  PndFtsHoughTrackerTask operator=(const PndFtsHoughTrackerTask &);

  ClassDef(PndFtsHoughTrackerTask, 1);
};

#endif
