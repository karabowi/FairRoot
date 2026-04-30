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

// modified by Elisabetta Prencipe 19/5/2014

#ifndef PNDSTTRECOHITPRODUCER2_H
#define PNDSTTRECOHITPRODUCER2_H

#include <vector>
#include <map>
#include <assert.h>
#include <iostream>

#include "TClonesArray.h"
#include "Exception.h"
#include "TrackCandHit.h"
#include "MeasurementProducer.h"
#include "AbsMeasurement.h"

template <class hit_T, class measurement_T>
class PndSttRecoHitProducer2 : public genfit::AbsMeasurementProducer<genfit::AbsMeasurement> {

 private:
  TClonesArray *hitArrayTClones;
  TClonesArray *tubeArrayTClones;

 public:
  PndSttRecoHitProducer2(TClonesArray *, TClonesArray *);
  virtual ~PndSttRecoHitProducer2();

  virtual genfit::AbsMeasurement *produce(int index, const genfit::TrackCandHit *hit);
};

template <class hit_T, class measurement_T>
PndSttRecoHitProducer2<hit_T, measurement_T>::PndSttRecoHitProducer2(TClonesArray *theArr, TClonesArray *theTubeArr)
{
  hitArrayTClones = theArr;
  tubeArrayTClones = theTubeArr;
}

template <class hit_T, class measurement_T>
PndSttRecoHitProducer2<hit_T, measurement_T>::~PndSttRecoHitProducer2()
{
  delete (tubeArrayTClones);
}

template <class hit_T, class measurement_T>
genfit::AbsMeasurement *PndSttRecoHitProducer2<hit_T, measurement_T>::produce(int index, const genfit::TrackCandHit *hit)
{
  assert(hitArrayTClones != nullptr);
  // assert(tubeArrayTClones!=nullptr);
  if (hitArrayTClones->At(index) == 0) {
    genfit::Exception e("In PndSttRecoHitProducer2: index for hit in TClonesArray out of bounds", __LINE__, __FILE__);
    e.setFatal();
    throw e;
  }

  return (new measurement_T((hit_T *)hitArrayTClones->At(index), hit, tubeArrayTClones));
}

#endif
