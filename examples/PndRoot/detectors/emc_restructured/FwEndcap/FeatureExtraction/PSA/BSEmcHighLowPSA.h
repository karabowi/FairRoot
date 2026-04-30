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

//-----------------------------------------------------------
// Description:
//     	Wrapping class to manage two independent PSAs
//     	for high, resp. low gain
//      Philipp Mahlberg
//-----------------------------------------------------------

#ifndef BSEMCHIGHLOWPSA_HH
#define BSEMCHIGHLOWPSA_HH

#include <utility>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsPSA.h"

class BSEmcPSAFPGASampleAnalyser;
class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Wrapping class to manage two independent PSAs for high, resp. low gain
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcHighLowPSA : public BSEmcAbsPSA {
 public:
  BSEmcHighLowPSA(Int_t t_verbose = 0);
  virtual void
  Init(BSEmcPSAFPGASampleAnalyser *t_highgainPSA, BSEmcPSAFPGASampleAnalyser *t_lowgainPSA, Double_t t_overflowThreshold, Int_t t_highgainWfIndex = 0, Int_t t_lowgainWfIndex = 1);
  virtual ~BSEmcHighLowPSA() {}

  void SetVerbose(Int_t t_verbose = 0) { fVerbose = t_verbose; };
  virtual Int_t Process(const BSEmcWaveform *t_waveform) /*override*/;
  virtual void Reset() /*override*/;
  virtual void GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time) /*override*/;
  virtual void GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time, Int_t &t_PileupType);

  virtual Int_t GetWaveformIdx(Int_t t_idx);

 private:
  BSEmcPSAFPGASampleAnalyser *fHighgainPSA{nullptr};
  BSEmcPSAFPGASampleAnalyser *fLowgainPSA{nullptr};

  Int_t fIdx_high{0};
  Int_t fIdx_low{0};

  std::vector<std::pair<BSEmcPSAFPGASampleAnalyser *, Int_t>> fHitsInFE{};
  Double_t fOverflowThreshold{0};

  Int_t fVerbose{0};

  ClassDef(BSEmcHighLowPSA, 1)
};

#endif /*BSEMCHIGHLOWPSA_HH*/
