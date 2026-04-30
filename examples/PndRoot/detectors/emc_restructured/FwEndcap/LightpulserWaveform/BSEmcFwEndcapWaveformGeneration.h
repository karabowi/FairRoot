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

//----------------------------------------------------------------------
// Author List:
//      Phil Strother                  	Original author
// 	Dima Melnichuk 			- adaption for PANDA
// 	Philipp Mahlberg		- integration in timebased simulation concept
//----------------------------------------------------------------------

#ifndef BSEMCFWENDCAPWAVEFORMGENERATION_HH
#define BSEMCFWENDCAPWAVEFORMGENERATION_HH

#include <string>

#include "Rtypes.h"

#include "BSEmcWaveformGenerationProcess.h"

class BSEmcFwEndcapWaveformGenPar;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Process to create Waveforms out of BSEmcMCDeposits based on
 * BSEmcFwEndcapDigi
 *
 * Specific implementation of the Waveformsimulator for the FwEndcap then used
 * in BSEmcWaveformGenerationProcess to transform BSEmcMCDeposits into
 * BSEmcMultiWaveforms
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapWaveformGeneration : public BSEmcWaveformGenerationProcess {
 public:
  BSEmcFwEndcapWaveformGeneration();
  virtual ~BSEmcFwEndcapWaveformGeneration();

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
   * @brief Defines the waveform simulator
   *
   */
  virtual void SetupSimulator();
  std::string fWaveformGenerationParName{""};
  BSEmcFwEndcapWaveformGenPar *fWaveformGenerationPar{nullptr};
  ClassDef(BSEmcFwEndcapWaveformGeneration, 1);
};

#endif /*BSEMCFWENDCAPWAVEFORMGENERATION_HH*/
