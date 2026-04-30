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

/*
 * PndLmdDigiProducer.h
 *
 *  Created on: Aug 10, 2009
 *      Author: huagen
 */

#ifndef PNDLMDDIGIPRODUCER_H_
#define PNDLMDDIGIPRODUCER_H_

#include "PndLmdMCPoint.h"
#include "PndLmdCalStrip.h"
#include "PndLmdDigiPara.h"
#include "PndLmdDigiStrips.h"
#include "PndLmdGeoHandling.h"
#include "PndLmdContFact.h"
#include "PndDetectorList.h"

#include "FairTask.h"

#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <iostream>
#include "TClonesArray.h"

class TClonesArray;

// this class shall implement the event digitization process
// by means of calling the related class
class PndLmdDigiProducer : public FairTask {
 public:
  // default constructor
  PndLmdDigiProducer();
  // constructor which will take the parameters from Digi_Par file, for the two segments sensor
  PndLmdDigiProducer(Int_t k, SensorSegment segment = kLEFT);
  // constructor for the curved strip sensor
  PndLmdDigiProducer(Double_t k, SensorSide side = SensorSide::kTOP);
  // destructor
  ~PndLmdDigiProducer();

  virtual void SetParContainers();
  // virtual void Init methods
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  // virtual method Exec()
  virtual void Exec(Option_t *opt);

  void AddDigi(Int_t &iStrip, Int_t iPoint, Int_t detID, TString detName, Int_t fe, Int_t channel, Double_t charge);
  void SetParamSet();

 private:
  // digitization parameters
  PndLmdDigiPara *fDigiParTrap;
  PndLmdDigiPara *fCurrentDigiPar;

  PndLmdDigiStrips *temp;

  // calculator objects
  PndLmdCalStrip *fStripCalTrapLeft;
  PndLmdCalStrip *fStripCalTrapRight;
  PndLmdCalStrip *fCurrentStripCalLeft;
  PndLmdCalStrip *fCurrentStripCalRight;

  PndLmdCalStrip *fStripCalTrapCircle;
  PndLmdCalStrip *fCurrentStripCalCircle;

  SensorSegment fSegment;
  SensorSide fSide;

  TString fBranchName;

  // input array of MC points
  TClonesArray *fPointArray;
  // output array of Digis
  TClonesArray *fStripArray;

  void Register();
  void ReSet();
  void ProduceHits();

  Bool_t SelectSensorParams(TString detname);

  PndLmdGeoHandling *fGeoH; // convert the point coordinates between local and global

  ClassDef(PndLmdDigiProducer, 5);
};

#endif /* PNDLMDDIGIPRODUCER_H_ */
