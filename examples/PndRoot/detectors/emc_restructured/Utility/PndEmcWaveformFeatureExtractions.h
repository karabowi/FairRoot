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

#ifndef BSEMCWAVEFORMDIGITIZERS_HH
#define BSEMCWAVEFORMDIGITIZERS_HH

#include "PndPersistencyTask.h"

class BSEmcBarrelWaveformFeatureExtraction : public PndPersistencyTask {
 public:
  BSEmcBarrelWaveformFeatureExtraction();
  ClassDef(BSEmcBarrelWaveformFeatureExtraction, 1);
};

class BSEmcBwEndcapWaveformFeatureExtraction : public PndPersistencyTask {
 public:
  BSEmcBwEndcapWaveformFeatureExtraction();
  ClassDef(BSEmcBwEndcapWaveformFeatureExtraction, 1);
};

class BSEmcFwEndcapWaveformFeatureExtraction : public PndPersistencyTask {
 public:
  BSEmcFwEndcapWaveformFeatureExtraction();
  ClassDef(BSEmcFwEndcapWaveformFeatureExtraction, 1);
};

class BSEmcShashlikWaveformFeatureExtraction : public PndPersistencyTask {
 public:
  BSEmcShashlikWaveformFeatureExtraction();
  ClassDef(BSEmcShashlikWaveformFeatureExtraction, 1);
};

class BSEmcWaveformFeatureExtraction : public PndPersistencyTask {
 public:
  BSEmcWaveformFeatureExtraction();
  ClassDef(BSEmcWaveformFeatureExtraction, 1);
};

#endif /*BSEMCWAVEFORMDIGITIZERS_HH*/
