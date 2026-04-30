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

#ifndef BSEMCMULTIWAVEFORMSIMULATOR_HH
#define BSEMCMULTIWAVEFORMSIMULATOR_HH

#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcFullStackedWaveformSimulator.h"

class BSEmcAbsPulseshape;
class BSEmcMultiWaveform;
class TClonesArray;
class BSEmcAbsWaveformModifier;
class BSEmcWaveform;
class BSEmcWaveformData;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Simulator to create BSEmcMultiWaveform
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup EmcBase
 */
class BSEmcMultiWaveformSimulator : public BSEmcFullStackedWaveformSimulator {
 public:
  BSEmcMultiWaveformSimulator();
  BSEmcMultiWaveformSimulator(Double_t t_sampleRate, BSEmcAbsPulseshape *t_pulseshape, Int_t t_wfMultiplicity);

  /*! wfIndex characterizes wf, the modifier will apllied on*
   *  If the modifier itself will work on the multiwaveform itself, pass a -1. In this case, the Modifier is considered to scale all waveforms equally.
   */
  virtual void AddModifier(BSEmcAbsWaveformModifier *t_wfModifier, Int_t t_wfIndex);

  virtual Double_t GetTotalScale(Int_t t_wfIndex);

 protected:
  virtual BSEmcWaveform *CallModifiers(BSEmcWaveform *t_wf);
  virtual BSEmcWaveform *MakeWaveform(BSEmcWaveformData *t_wfData, TClonesArray *t_arrayToStore = nullptr) /*override*/;
  virtual BSEmcWaveform *MakeSingleWaveform(Double_t t_depositEnergy, Double_t t_depositTime, TClonesArray *t_arrayToStore = nullptr, Int_t t_detId = -1, Int_t t_trackId = -1,
                                            Int_t t_depositIndex = -1) /*override*/;

 private:
  Int_t fWfMultiplicity{-1};
  std::vector<Int_t> fIndexList{};

  BSEmcMultiWaveform *MultiplyWaveform(const BSEmcWaveform *t_singleWaveform, TClonesArray *t_arrayToStore);

  ClassDef(BSEmcMultiWaveformSimulator, 1)
};

#endif /*BSEMCMULTIWAVEFORMSIMULATOR_HH*/
