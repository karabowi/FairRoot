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

#ifndef BSEMCFWENDCAPONLINEFEATUREEXTRACTION_HH
#define BSEMCFWENDCAPONLINEFEATUREEXTRACTION_HH

#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcDigi.h"
#include "BSEmcOnlineFeatureExtractionProcess.h"

class BSEmcPSAFPGASampleAnalyser;
class BSEmcFwEndcapDigiPar;
class BSEmcWaveform;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Process to extract digis from FwEndcap waveforms. Online like, but not actually online usable.
 *
 * Instantiates and sets a PulseShapeAnalysis object and which parses a waveform and extracts each maximum energy and time.
 * Those information are then used to create BSEmcDigis.
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>, process changes -> Ben Salisbury <salisbury@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapOnlineFeatureExtraction : public BSEmcOnlineFeatureExtractionProcess {
 public:
  BSEmcFwEndcapOnlineFeatureExtraction();
  virtual ~BSEmcFwEndcapOnlineFeatureExtraction();

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

 protected:
  /**
   * @brief Define PSA
   *
   */
  virtual void DefinePSA() /*override*/;
  virtual BSEmcDigi::eGAIN GetGainType(const BSEmcWaveform *t_waveform, Int_t t_hit) const /*override*/;

 private:
  BSEmcFwEndcapDigiPar *fDigiPar{nullptr};
  BSEmcPSAFPGASampleAnalyser *fHighgainPSA{nullptr};
  BSEmcPSAFPGASampleAnalyser *fLowgainPSA{nullptr};

  ClassDef(BSEmcFwEndcapOnlineFeatureExtraction, 1);
};

#endif /*BSEMCFWENDCAPONLINEFEATUREEXTRACTION_HH*/
