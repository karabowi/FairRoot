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

#ifndef BSEMCBWENDCAPONLINEFEATUREEXTRACTION_HH
#define BSEMCBWENDCAPONLINEFEATUREEXTRACTION_HH

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

#include "BSEmcOnlineFeatureExtractionProcess.h"

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
 * @brief Process to extract digis from BwEndcap waveforms.
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup BwEndcap
 */
class BSEmcBwEndcapOnlineFeatureExtraction : public BSEmcOnlineFeatureExtractionProcess {
 public:
  BSEmcBwEndcapOnlineFeatureExtraction();
  virtual ~BSEmcBwEndcapOnlineFeatureExtraction();

  /**
   *
   * @brief Sets the detectorname and fills the parameterlist with names of all required parameter
   *
   * @param t_detectorName
   */
  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;

  /**
   * @brief Fetches the parameteres requested in SetDetectorName
   *
   * @param t_parameterRegister
   */
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;

  void SingleAPDMode(Bool_t t_mode = kTRUE) { fSingleAPDMode = t_mode; }

  void SetVerbose(Int_t iVerbose) { fVerbose = iVerbose; }

 protected:
  Int_t fVerbose{0};
  /**
   * @brief Define PSA
   *
   */
  virtual void DefinePSA() /*override*/;
  virtual BSEmcDigi::eGAIN GetGainType(const BSEmcWaveform *t_waveform, Int_t t_hit) const /*override*/;
  virtual Double_t GetTimeStamp(const BSEmcWaveform * /*t_waveform*/, Double_t t_digi_time) const /*override*/ { return t_digi_time; }

 private:
  BSEmcBwEndcapDigiPar *fDigiPar{nullptr};

  BSEmcPSAOverflowCombinator *fCombinator{nullptr};
  BSEmcPSATmaxAnalyser *fHighgainPSA{nullptr};
  BSEmcPSATmaxAnalyser *fLowgainPSA{nullptr};
  Bool_t fSingleAPDMode{kFALSE};

  ClassDef(BSEmcBwEndcapOnlineFeatureExtraction, 1);
};

#endif /*BSEMCBWENDCAPONLINEFEATUREEXTRACTION_HH*/
