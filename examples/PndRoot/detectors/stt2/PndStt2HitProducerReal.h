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
 
#ifndef PNDSTT2HITPRODUCERREAL_H
#define PNDSTT2HITPRODUCERREAL_H

#include "PndPersistencyTask.h"

#include "TVector3.h"
//#include "TRandom.h"

class TClonesArray;
class PndStt2GeoHandler;
class PndStt2DigiPar;
//class PndStt2GeoPar;
class PndGeoSttPar;
class PndStt2Digitizer;

class PndStt2HitProducerReal : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndStt2HitProducerReal();

  /** Destructor **/
  ~PndStt2HitProducerReal();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }

 private:

  void SetParContainers();

  /** Input array of PndStt2Digi **/
  TClonesArray *fDigiArray;

  /** Output array of PndStt2Hit **/
  TClonesArray *fHitArray;

  //PndStt2GeoPar  *fSttGeoPar; 
  PndGeoSttPar  *fSttGeoPar; 
  PndStt2DigiPar *fSttDigiPar; 
  PndStt2Digitizer *fDigitizer;  //! Digitizer tool for t_drift <-> r_isochrone
  PndStt2GeoHandler *fSttGeoH;   //! the STT geo handler

  ClassDef(PndStt2HitProducerReal, 1);
};

#endif
