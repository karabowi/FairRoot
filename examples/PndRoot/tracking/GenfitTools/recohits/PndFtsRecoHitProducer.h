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

#ifndef PNDFTSRECOHITPRODUCER_H
#define PNDFTSRECOHITPRODUCER_H

#include <vector>
#include <map>
#include <assert.h>
#include <iostream>

#include "TClonesArray.h"
#include "GFException.h"
#include "GFRecoHitProducer.h"

class GFAbsRecoHit;
class TClonesArray;

template <class hit_T, class recoHit_T>
class PndFtsRecoHitProducer : public GFAbsRecoHitProducer {

 private:
  TClonesArray *hitArrayTClones;
  TClonesArray *tubeArrayTClones;

 public:
  PndFtsRecoHitProducer(TClonesArray *, TClonesArray *);
  virtual ~PndFtsRecoHitProducer();

  virtual GFAbsRecoHit *produce(int index);
};

template <class hit_T, class recoHit_T>
PndFtsRecoHitProducer<hit_T, recoHit_T>::PndFtsRecoHitProducer(TClonesArray *theArr, TClonesArray *theTubeArr)
{
  hitArrayTClones = theArr;
  tubeArrayTClones = theTubeArr;
}

template <class hit_T, class recoHit_T>
PndFtsRecoHitProducer<hit_T, recoHit_T>::~PndFtsRecoHitProducer()
{
}

template <class hit_T, class recoHit_T>
GFAbsRecoHit *PndFtsRecoHitProducer<hit_T, recoHit_T>::produce(int index)
{
  assert(hitArrayTClones != nullptr);
  assert(tubeArrayTClones != nullptr);
  if (hitArrayTClones->At(index) == 0) {
    GFException e("In PndFtsRecoHitProducer: index for hit in TClonesArray out of bounds", __LINE__, __FILE__);
    e.setFatal();
    throw e;
  }

  return (new recoHit_T((hit_T *)hitArrayTClones->At(index), tubeArrayTClones));
}

#endif
