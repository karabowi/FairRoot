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

//#pragma once
#ifndef BSEMCSHAPINGNOISEADDER_HH
#define BSEMCSHAPINGNOISEADDER_HH

#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsWaveformModifier.h"

class BSEmcWaveform;
class BSEmcAbsWaveformSimulator;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief waveform modifier to add noise to waveform
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcShapingNoiseAdder : public BSEmcAbsWaveformModifier {
 public:
  BSEmcShapingNoiseAdder();
  BSEmcShapingNoiseAdder(Double_t t_shapingTime, Double_t t_sampleRate, Double_t t_noiseWidth, Double_t t_inputScale);

  virtual void Modify(BSEmcWaveform *t_wf) /*override*/;

 protected:
 private:
  Double_t fShapingTime{0};
  Double_t fNoiseWidth{0};
  Double_t fInputScale{0};

  Double_t fSingleNoiseWidth{0};

  class MA { // moving Average Buffer
   private:
    std::vector<Double_t> fBuffer{};
    UInt_t fBPointer{0};
    Double_t fSum{0};

   public:
    MA(UInt_t t_size) : fBuffer(t_size, 0.0), fBPointer(0), fSum(0){};

    Double_t operator()(Double_t t_xN)
    {
      fSum += t_xN - fBuffer[fBPointer];
      fBuffer[fBPointer] = t_xN;
      fBPointer++;
      fBPointer %= fBuffer.size();

      return fSum / fBuffer.size();
    }
    UInt_t GetSize() { return fBuffer.size(); };
  } fMA_buffer;

  ClassDef(BSEmcShapingNoiseAdder, 0)
};

#endif /*BSEMCSHAPINGNOISEADDER_HH*/
