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

#ifndef BSEMCFWENDCAPDIGI_HH
#define BSEMCFWENDCAPDIGI_HH

#include <set>
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "fairlogger/Logger.h"

#include "BSEmcAbsPSA.h"
#include "BSEmcDigi.h"
#include "BSEmcExtractDigisFromWaveforms.h"
#include "BSEmcFwEndcapDigiPar.h"
#include "BSEmcHighLowPSA.h"
#include "BSEmcPSAFPGASampleAnalyser.h"

class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;
//#include <vector>

class BSEmcAbsPSA;
class BSEmcAbsPulseshape;

/**
 * @brief Task to create digis from waveforms.
 *
 * This is an alternative to BSEmcWaveformToDigi, which despite its name could be
 * used for all the EMC, not just FwEndcap.
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapDigi : public BSEmcExtractDigisFromWaveforms<BSEmcFwEndcapDigiPar> {
 public:
  // Constructors
  BSEmcFwEndcapDigi(Bool_t t_storedigis = kTRUE);
  // Destructor
  virtual ~BSEmcFwEndcapDigi();

  void SetPSAAlgorithms(BSEmcAbsPSA *t_highgain, BSEmcAbsPSA *t_lowgain)
  {
    fHighgainPSA = dynamic_cast<BSEmcPSAFPGASampleAnalyser *>(t_highgain);
    fLowgainPSA = dynamic_cast<BSEmcPSAFPGASampleAnalyser *>(t_lowgain);

    if (fHighgainPSA == nullptr || fLowgainPSA == nullptr) {
      LOG(error) << "BSEmcFwEndcapDigi::SetPSAAlgorithms: PSA must be a BSEmcPSAFPGASampleAnalyser. External PSA setting has no effect";
    }
  }

 protected:
  /** Get parameter containers **/
  virtual void DefinePSA() /*override*/;
  virtual BSEmcDigi::eGAIN GetGainType(BSEmcWaveform *t_waveform, Int_t t_hit) const /*override*/;

 private:
  BSEmcPSAFPGASampleAnalyser *fHighgainPSA{nullptr};
  BSEmcPSAFPGASampleAnalyser *fLowgainPSA{nullptr};

  ClassDef(BSEmcFwEndcapDigi, 1);
};

#endif /*BSEMCFWENDCAPDIGI_HH*/
