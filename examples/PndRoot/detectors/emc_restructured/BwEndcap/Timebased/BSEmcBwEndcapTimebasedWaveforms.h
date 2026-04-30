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
// 	Philippp Mahlberg		- integrtion in timebased simulation concept
//  Guang Zhao          - implementation for bwec
//----------------------------------------------------------------------

//#pragma once
#ifndef BSEMCBWENDCAPTIMEBASEDWAVEFORMS_HH
#define BSEMCBWENDCAPTIMEBASEDWAVEFORMS_HH

#include "BSEmcMCDepositToTimebasedWaveforms.h"

class BSEmcBwEndcapDigiPar;

/**
 * @brief Taks to create waveforms from hits.
 *
 * This is an alternative to BSEmcHitsToWaveform, which despite its name could be
 * used for all the EMC, not just BwEndcap.
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup BwEndcap
 */
class BSEmcBwEndcapTimebasedWaveforms : public BSEmcMCDepositToTimebasedWaveforms<BSEmcBwEndcapDigiPar> {
 public:
  // Constructors
  BSEmcBwEndcapTimebasedWaveforms(const std::string &t_detectorname = "BwEndcap", Bool_t t_storewaves = kFALSE);
  // Destructor
  virtual ~BSEmcBwEndcapTimebasedWaveforms();

  void SingleAPDMode(Bool_t t_mode = kTRUE) { fSingleAPDMode = t_mode; }
  void NoiseMode(Int_t t_mode = 1) { fNoiseMode = t_mode; } // 1 for reduced ifft, 0 for full ifft
  void RunTestMode(Double_t t_energy = 0.01)
  {
    fTestMode = kTRUE;
    fTestEnergy = t_energy;
  }

 protected:
  virtual void SetupSimulator() /*override*/;

 private:
  // don't allow copying (-Weffc++)
  BSEmcBwEndcapTimebasedWaveforms(const BSEmcBwEndcapTimebasedWaveforms &);            // no implementation
  BSEmcBwEndcapTimebasedWaveforms &operator=(const BSEmcBwEndcapTimebasedWaveforms &); // no implementation

 private:
  Bool_t fSingleAPDMode{kFALSE};
  Int_t fNoiseMode{1};
  Bool_t fTestMode{kFALSE};
  Double_t fTestEnergy{0};

  ClassDef(BSEmcBwEndcapTimebasedWaveforms, 1)
};

#endif /*BSEMCBWENDCAPTIMEBASEDWAVEFORMS_HH*/
