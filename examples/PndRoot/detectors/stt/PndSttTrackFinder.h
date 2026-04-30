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
// -----                   PndSttTraer. Please consider using one of the 32 headers found in section 17.4.1.2 of the C++ standard. Examples include substickFinder header file -----
// -----                  Created 28/03/06  by R. Castelijns           -----
// -------------------------------------------------------------------------

/** PndSttTrackFinder
 *@author R.Castelijns <r.castelijns@fz-jeulich.de>
 **
 ** Abstract base class for concrete STT track finding algorithm.
 ** Each derived class must implement the method DoFind. This has
 ** to operate on the two TClonesArrays of pixel and strip hits
 ** and to fill the PndSttTrackArray.
 **/

#ifndef PNDSTTTRACKFINDER
#define PNDSTTTRACKFINDER 1

#include "TObject.h"
#include "TList.h"
#include <iostream>

class TClonesArray;

class PndSttTrackFinder : public TObject {
 public:
  /** Default constructor **/
  PndSttTrackFinder() : fVerbose(0), fHelixHitProduction(kFALSE){};

  /** Destructor **/
  virtual ~PndSttTrackFinder(){};

  /** Virtual method Init. If needed, to be implemented in the
   ** concrete class. Else no action.
   **/
  virtual void Init(){};

  /** Abstract method DoFind. To be implemented in the concrete class.
   ** Task: Read the pixel and strip hit arrays and fill the track array,
   ** pointers to which are given as arguments
   *@value Number of tracks created
   **/
  virtual Int_t DoFind(TClonesArray *trackArray, TClonesArray *helixHitArray) = 0;
  virtual Int_t DoFind(TClonesArray *trackArray, TClonesArray *trackCandArray, TClonesArray *helixHitArray) = 0; // CHECK

  /** Virtual method Finish. If needed, to be implemented in the concrete
   ** class. Executed at the end of the run.
   **/
  virtual void Finish() = 0;

  virtual void AddHitCollection(TClonesArray *mHitArray, TClonesArray *mPointArray) = 0;

  /** Set verbosity
   *@param verbose   Verbosity level
   **/
  void SetVerbose(Int_t verbose) { fVerbose = verbose; };

  /** set the helix hit production flag true or false **/
  void SetHelixHitProduction(Bool_t hhprod) { fHelixHitProduction = hhprod; };

  /** CHECK added **/
  virtual void SetTubeArray(TClonesArray *tubeArray) = 0;

  virtual void SetInputBranchName(char *string1) = 0;

 private:
 protected:
  Int_t fVerbose; // Verbosity level
  /** production to file of helix hit from PR or not **/
  Bool_t fHelixHitProduction;

  ClassDef(PndSttTrackFinder, 1);
};

#endif
