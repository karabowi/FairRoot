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

// modified by elisabetta Prencipe 19/05/2014

#ifndef PNDFTSRECOHITPRODUCER2_H
#define PNDFTSRECOHITPRODUCER2_H

#include <vector>
#include <map>
#include <assert.h>
#include <iostream>

#include "TClonesArray.h"
#include "Exception.h"
#include "MeasurementProducer.h"
#include "WireMeasurement.h"
#include "TrackCandHit.h"

template <class hit_T, class measurement_T>
class PndFtsRecoHitProducer2 : public genfit::AbsMeasurementProducer<genfit::AbsMeasurement> {

 private:
  TClonesArray *hitArrayTClones;
  TClonesArray *tubeArrayTClones;

 public:
  PndFtsRecoHitProducer2(TClonesArray *, TClonesArray *);
  virtual ~PndFtsRecoHitProducer2();

  virtual genfit::WireMeasurement *produce(int index, const genfit::TrackCandHit *hit);
};

template <class hit_T, class measurement_T>
PndFtsRecoHitProducer2<hit_T, measurement_T>::PndFtsRecoHitProducer2(TClonesArray *theArr, TClonesArray *theTubeArr)
{
  hitArrayTClones = theArr;
  tubeArrayTClones = theTubeArr;
}

template <class hit_T, class measurement_T>
PndFtsRecoHitProducer2<hit_T, measurement_T>::~PndFtsRecoHitProducer2()
{
  delete (tubeArrayTClones);
}

template <class hit_T, class measurement_T>
genfit::WireMeasurement *PndFtsRecoHitProducer2<hit_T, measurement_T>::produce(int index, const genfit::TrackCandHit *hit)
{
  assert(hitArrayTClones != nullptr);
  assert(tubeArrayTClones != nullptr);
  if (hitArrayTClones->At(index) == 0) {
    genfit::Exception e("In PndFtsRecoHitProducer2: index for hit in TClonesArray out of bounds", __LINE__, __FILE__);
    e.setFatal();
    throw e;
  }

  return (new measurement_T((hit_T *)hitArrayTClones->At(index), hit, tubeArrayTClones));
}

#endif
