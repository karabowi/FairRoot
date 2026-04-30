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
// -----                   PndGemFindTracks header file                -----
// -----                  Created 19.03.2009  by R. Karabowicz         -----
// -----                 according to the PndDchFindTracks             -----
// -------------------------------------------------------------------------

/** \class PndGemFindTracks
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief Task class for track finding in the Gem
 *
 *   Uses as track finding algorithm classes derived from PndGemTrackFinder.
 *	 Input:  TClonesArray of PndGemHit or
 *	 \return TClonesArray of PndGemTrack
 **/

#ifndef PNDGEMFINDTRACKS_H
#define PNDGEMFINDTRACKS_H 1

#include "TStopwatch.h"

#include "FairTask.h"

class PndGemTrackFinder;
class PndGemDigiPar;
class TClonesArray;

class PndGemFindTracks : public FairTask {

 public:
  /** Default constructor **/
  PndGemFindTracks();

  /** Standard constructor
   *  \param name   Name of class
   *  \param title  Task title
   *  \param finder Finder algorithm
   **/
  PndGemFindTracks(const char *name, const char *title = "PndTask", PndGemTrackFinder *finder = nullptr);

  /** Destructor **/
  virtual ~PndGemFindTracks();

  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t *opt);

  /** Finish at the end of each event **/
  virtual void Finish();

  /** SetParContainers **/
  virtual void SetParContainers();

  /** Accessors **/
  PndGemTrackFinder *GetFinder() { return fFinder; };
  Int_t GetNofTracks() const { return fNofTracks; };
  TString GetUseHitOrDigi() const { return fUseHitOrDigi; };

  /** Set concrete track finder **/
  void UseFinder(PndGemTrackFinder *finder) { fFinder = finder; };

  /** Set use digi or hits
   ** Option: "hit/chit/digi", Default: "hit"
   **/
  void SetUseHitOrDigi(TString useHitOrDigi = "hit") { fUseHitOrDigi = useHitOrDigi; };

 private:
  PndGemDigiPar *fDigiPar; /** Digitisation parameters **/

  PndGemTrackFinder *fFinder;       ///< Pointer to TrackFinder concrete class
  TClonesArray *fGemHitOrDigiArray; //
  TClonesArray *fTrackArray;        ///< Output array of PndTracks
  TClonesArray *fTrackCandArray;    ///< Output array of PndTrackCands
  Int_t fNofTracks;                 ///< Number of created tracks
  TString fUseHitOrDigi;            ///< Choose use hits or digis, default: hits

  TStopwatch fTimer;
  Int_t fTNofEvents;
  Int_t fTNofTracks;
  Double_t fTTime;

  ClassDef(PndGemFindTracks, 1);
};

#endif
