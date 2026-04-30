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
// -----                PndLmdNoiseProducer header file                -----
// -----                  Created 05.2015  by P. Jasinski                   -----
// -------------------------------------------------------------------------

/** PndLmdNoiseProducer.h
 * based on PndMvdNoiseProducer with adaptation to lmd specific calls
 *original @author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The Noise Producer adds fake hits to silicon sensor channels (strips and
 ** Pixels)
 ** updated: 17/07/2015 by A.Karavdina
 **/

#ifndef PNDLMDNOISEPRODUCER_H
#define PNDLMDNOISEPRODUCER_H

#include "PndMvdNoiseProducer.h"
//#include "FairTask.h"

class PndLmdNoiseProducer : public PndMvdNoiseProducer {
 public:
  PndLmdNoiseProducer() : PndMvdNoiseProducer(){};

  virtual InitStatus Init();

  void FillSensorLists();

  void SetParContainers();

  void Exec(Option_t *opt);

  Double_t CalcReadoutCycles(Double_t clock);
  void AddDigiPixel(Int_t &noisies, Int_t iPoint, Int_t sensorID, Int_t fe, Int_t col, Int_t row, Double_t charge);

 private:
  std::vector<Int_t> fPixelIds;

  ClassDef(PndLmdNoiseProducer, 1);
};

#endif
