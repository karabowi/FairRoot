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

#ifndef BSEMCBWENDCAPDIGI_HH
#define BSEMCBWENDCAPDIGI_HH

#include <PndPersistencyTask.h>
#include <set>
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "BSEmcBwEndcapDigiPar.h"
#include "BSEmcDigi.h"
#include "BSEmcExtractDigisFromWaveforms.h"
#include "BSEmcPSATmaxAnalyser.h"

class BSEmcAbsPSA;
class BSEmcAbsPulseshape;
class BSEmcMultiPSA;
class BSEmcPSAOverflowCombinator;
class BSEmcPSATmaxAnalyser;
class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Task to create digis from waveforms.
 *
 * This is an alternative to BSEmcWaveformToDigi, which despite its name could be
 * used for all the EMC, not just BwEndcap.
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup BwEndcap
 */
class BSEmcBwEndcapDigi : public BSEmcExtractDigisFromWaveforms<BSEmcBwEndcapDigiPar> {
 public:
  // Constructors
  BSEmcBwEndcapDigi(const std::string &t_detectorname = "BwEndcap", Bool_t t_storedigis = kTRUE);
  // Destructor
  virtual ~BSEmcBwEndcapDigi();

  void SingleAPDMode(Bool_t t_mode = kTRUE) { fSingleAPDMode = t_mode; }

 protected:
  virtual void DefinePSA() /*override*/;
  virtual Double_t GetTimeStamp(BSEmcWaveform * /*t_waveform*/, Double_t t_digi_time) const /*override*/ { return t_digi_time; }
  virtual BSEmcDigi::eGAIN GetGainType(BSEmcWaveform *t_waveform, Int_t t_hit) const /*override*/;

 private:
  BSEmcPSAOverflowCombinator *fCombinator{nullptr};
  BSEmcPSATmaxAnalyser *fHighgainPSA{nullptr};
  BSEmcPSATmaxAnalyser *fLowgainPSA{nullptr};
  Bool_t fSingleAPDMode{kFALSE};

  ClassDef(BSEmcBwEndcapDigi, 1)
};

#endif /*BSEMCBWENDCAPDIGI_HH*/
