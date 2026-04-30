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
// -----            PndGemIdealHitProducer header file                 -----
// -----            Created 17/03/2009 by R. Karabowicz                -----
// -------------------------------------------------------------------------

/** \class PndGemIdealHitProducer
 *  \author R.Karabowicz <r.karabowicz@gsi.de>
 *  \date 17/03/2009
 *  \brief Class for conversion points to hits
 *
 * is a dummy hit reconstruction task, which converts each point
 * into a hit (1:1 conversion) with a resolution defined by strip
 * pitches and angles taken from the parameter file
 **/

#ifndef PNDGEMIDEALHITPRODUCER_H
#define PNDGEMIDEALHITPRODUCER_H 1

// Pnd includes
#include "FairTask.h"

// Root includes
#include "TStopwatch.h"

class TClonesArray;
class PndGemDigiPar;
class PndGemSensor;
class PndGemStation;

class PndGemIdealHitProducer : public FairTask {

 public:
  /** Default constructor **/
  PndGemIdealHitProducer();

  /** Constructor **/
  PndGemIdealHitProducer(const char *name, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndGemIdealHitProducer();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  /** Public method AddHit(...)
   ** adds another hit to the hit array
   **/
  //	PndGemHit* AddHit(const PndGemPoint* point, Int_t idx);

 private:
  PndGemDigiPar *fDigiPar;

  Int_t fTNofEvents;
  Int_t fTNofPoints;
  Int_t fTNofHits;

  TClonesArray *fPointArray; ///< Input array of PndGemMCPoints
  TClonesArray *fHitArray;   ///< Output array of PndGemHits

  /** Private method Smear(...)
   **    smears the position vector
   **/
  //	void Smear(TVector3& pos, const TVector3 dpos) const;

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Virtual method Finish **/
  virtual void Finish();

  ClassDef(PndGemIdealHitProducer, 1);
};

#endif
