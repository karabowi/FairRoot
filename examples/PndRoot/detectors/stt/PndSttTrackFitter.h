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
// -----                   PndSttTrackFitter header file               -----
// -----                  Created 18/02/05  by V. Friese               -----
// -------------------------------------------------------------------------

/** PndSttTrackFitter
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Abstract base class for concrete STT track fitting algorithm.
 ** Each derived class must implement the method DoFit. This has
 ** to operate on an object of type PndSttTrack and fill the
 ** parameters fPidHypo, fParamFirst, fParamLast, fFlag and fChi2.
 **/

#ifndef PNDSTTTRACKFITTER
#define PNDSTTTRACKFITTER 1

#include "TObject.h"
#include "FairTrackParam.h"

class PndSttTrack;
class PndTrackCand;
class TClonesArray;

class PndSttTrackFitter : public TObject {

 public:
  virtual void AddHitCollection(TClonesArray *){};

  /** Default constructor **/
  PndSttTrackFitter(){};

  /** Destructor **/
  virtual ~PndSttTrackFitter(){};

  /** Virtual method Init. If needed, to be implemented in the
   ** concrete class. Else no action.
   **/
  virtual void Init(){};

  /** Abstract method DoFit. To be implemented in the concrete class.
   ** Task: Make a fit to the hits attached to the track by the track
   ** finder. Fill the track parameter member variables.
   **
   *@param pTrackCand  Pointer to PndTrackCand (input)
   *@param pTrack      Pointer to PndSttTrack  (output)
   *@param pidHypo     PID hypothesis for the fit. Default is pion.
   **/
  virtual Int_t DoFit(PndTrackCand *pTrackCand, PndSttTrack *pTrack, Int_t pidHypo = 211) = 0;

  /** Abstract method Extrapolate. Gives track parameters at a given r
   ** position.
   **
   *@param track  Pointer to SttTrack
   *@param r      r position
   *@param param  (return value) SttTrackParam at r
   **/
  virtual void Extrapolate(PndSttTrack *track, Double_t r, FairTrackParam *param) = 0;

  /** CHECK added **/
  virtual void SetTubeArray(TClonesArray *tubeArray) = 0;

  ClassDef(PndSttTrackFitter, 1);
};

#endif
