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
 * PndSdsTimeWalkCorr.h
 *
 * Description:
 *
 *  Created on: Aug 26, 2010
 *      Author: Simone Esch
 *      Abstract Base Class to calculate the TimeWalkCorrection from the TOT time
 */

#include "TObject.h"

#ifndef PNDSDSTIMEWALKCORR_H_
#define PNDSDSTIMEWALKCORR_H_

class PndSdsTimeWalkCorr : public TObject {
 public:
  PndSdsTimeWalkCorr();

  virtual ~PndSdsTimeWalkCorr();
  virtual Double_t CorrectionTimeWalk(Double_t tot) = 0;
  virtual Double_t GetCharge() = 0;

 private:
  ClassDef(PndSdsTimeWalkCorr, 1);
};

#endif /* PNDSDSTIMEWALKCORR_H_ */
