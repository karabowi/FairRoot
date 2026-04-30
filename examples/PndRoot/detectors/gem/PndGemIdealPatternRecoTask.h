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
// -----                   PndGemIdealPatternRecoTask header file      -----
// -----                based on PndDchFindTracks header file          -----
// -----                  Created 27.01.2008  by A. Bubak              -----
// -----                 according to the CbmStsFindTracks             -----
// -------------------------------------------------------------------------

/** \class PndGemIdealPatternRecoTask
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 27.01.2008
 *  \brief Task class for track finding
 *
 *   Ideal Pattern reco task
 **/

#ifndef PNDGEMIDEALPATTERNRECOTASK_H
#define PNDGEMIDEALPATTERNRECOTASK_H

#include "FairTask.h"

class PndDchTrackFinder;
class TClonesArray;

class PndGemIdealPatternRecoTask : public FairTask {

 public:
  /** Default constructor **/
  PndGemIdealPatternRecoTask();

  /** Destructor **/
  virtual ~PndGemIdealPatternRecoTask();

  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t *opt);

  /** SetParContainers **/
  virtual void SetParContainers();

  /** Accessors **/

  /** Set concrete track finder **/

  /** Set use digi or hits
   ** Option: "hit/chit/digi", Default: "hit"
   **/

 private:
  TClonesArray *fGemHitArr;
  TClonesArray *fMCTrackArr;
  TClonesArray *fGemPointArr;
  TClonesArray *fPndTrackCandArr;

  ClassDef(PndGemIdealPatternRecoTask, 1);
};

#endif
