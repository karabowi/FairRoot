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
 

#ifndef PNDSTT2DIGITASK_H
#define PNDSTT2DIGITASK_H

#include "PndPersistencyTask.h"
#include "PndStt2DigiPar.h"
//#include "PndStt2GeoPar.h"
#include "PndGeoSttPar.h"

#include "TVector3.h"
#include "TClonesArray.h"
//#include "TRandom.h"

//class TClonesArray;
//class PndGeoHandling;
class PndStt2GeoHandler;
class PndStt2Digitizer;

class PndStt2DigiTask : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndStt2DigiTask();

  /** Destructor **/
  ~PndStt2DigiTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }

 private:
  /** Private method GetClostestApproachToWire;
   **
   ** Returns the measured values of the straw, radius and z position
   *@param closestDistanceinPos the radial position of the hit in straw frame
   *@param zPosInStrawFrame the z-position of the hit in straw frame
   *@param inPos entry point of the track into the straw
   *@param outPos exit point of the track into the straw
   **/
  //Double_t GetDistToWire(TVector3 lPosIn, TVector3 lPosOut);
  Double_t GetDistToWire(Int_t tubeID, TVector3 gPosIn, TVector3 gPosOut );
  void SetParContainers();

  /** Input array of PndSttPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndSttHits **/
  TClonesArray *fDigiArray;

  ///** from parameters array of PndSttTube **/ //  CHECK added
  //TClonesArray *fTubeArray;
  //TmIntTubePtr      fTubeMap;
  PndStt2DigiPar    *fSttDigiPar; //  CHECK added
  PndGeoSttPar      *fSttGeoPar;  //  CHECK added
  //PndStt2GeoPar     *fSttGeoPar;  //  CHECK added
  //PndGeoHandling   *fGeoH;  //! Gives Access to the Path info of a hit
  PndStt2GeoHandler *fSttGeoH;    // the STT geo handler
  PndStt2Digitizer  *fDigitizer;  //! Digitizer tool for t_drift <-> r_isochrone
  

  ClassDef(PndStt2DigiTask, 1);
};

#endif
