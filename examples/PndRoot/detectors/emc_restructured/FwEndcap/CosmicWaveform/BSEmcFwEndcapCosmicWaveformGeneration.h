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

#ifndef BSEMCFWENDCAPCOSMICWAVEFORMGENERATION_HH
#define BSEMCFWENDCAPCOSMICWAVEFORMGENERATION_HH

#include <string>

#include "Rtypes.h"

#include "BSEmcWaveformGenerationProcess.h"

class BSEmcFwEndcapCosmicWaveformGenPar;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Process to create Cosmic Waveforms out of BSEmcMCDeposits based on
 * BSEmcFwEndcapDigi
 *
 * Specific implementation of the Waveformsimulator for the FwEndcap then used
 * in BSEmcWaveformGenerationProcess to transform BSEmcMCDeposits into
 * BSEmcMultiWaveforms
 * New Cosmic Waveform based on near final setup measurements (APD-submodule) at
 * -25 degree (2021/2022)
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>, HISKP Bonn
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @author Celina Frenkel <frenkel@hiskp.uni-bonn.de>, HISKP Bonn
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapCosmicWaveformGeneration : public BSEmcWaveformGenerationProcess {
 public:
  BSEmcFwEndcapCosmicWaveformGeneration();
  virtual ~BSEmcFwEndcapCosmicWaveformGeneration();

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

  /**
   * @brief Set the Waveform Generation Parameter Name
   *
   * @param t_waveformGenParName
   */
  void SetWaveformGenParName(const std::string &t_waveformGenParName) { fWaveformGenerationParName = t_waveformGenParName; }

  /**
   * @brief Set the Waveform Generation Parameter Name
   *
   * @param t_waveformGenParName
   */
  void SetPulseshapeLUTName(const std::string &t_pulseshapeLUTName) { fPulseshapeLUT = t_pulseshapeLUTName; }

 protected:
  /**
   * @brief Defines the waveform simulator
   *
   */
  virtual void SetupSimulator();

  std::string fPulseshapeLUT{"LUTCosmicWaveformSADC2022FwEC_APD.txt"};
  std::string fWaveformGenerationParName{""};
  BSEmcFwEndcapCosmicWaveformGenPar *fWaveformGenerationPar{nullptr};
  ClassDef(BSEmcFwEndcapCosmicWaveformGeneration, 1);
};

#endif /*BSEMCFWENDCAPCOSMICWAVEFORMGENERATION_HH*/
