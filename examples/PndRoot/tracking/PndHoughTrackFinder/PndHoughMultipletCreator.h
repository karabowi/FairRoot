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

/////////////////////////////////////////////////////////////
//  PndHoughMultipletCreator
//  Creates Multiplets: all possible hit combination
/////////////////////////////////////////////////////////////////

/** PndHoughMultipletCreator
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 29.10.2018
 *@updated 02.02.2021
 *@version 3.0
 **
 ** PANDA class for creating all possible hit combinations of two or three hits in a track candidate
 ** Task level RECO
 **/

#ifndef PndHoughMultipletCreator_H_
#define PndHoughMultipletCreator_H_

#include "FairTask.h"
#include "PndTrackCand.h"

class PndHoughMultipletCreator {
 public:
  PndHoughMultipletCreator() {}

  virtual ~PndHoughMultipletCreator(){};
  /** @brief: creates doublets or triplets from a track Canddidate. */
  std::vector<std::vector<FairLink>> multiplets_creator(PndTrackCand trackCandWithoutSkewed, int num);

 private:
  ClassDef(PndHoughMultipletCreator, 1);
};

#endif /*PndHoughMultipletCreator_H_*/
