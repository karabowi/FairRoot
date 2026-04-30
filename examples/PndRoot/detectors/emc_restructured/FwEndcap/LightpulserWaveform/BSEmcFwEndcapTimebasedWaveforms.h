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
//----------------------------------------------------------------------

//#pragma once
#ifndef BSEMCFWENDCAPTIMEBASEDWAVEFORMS_HH
#define BSEMCFWENDCAPTIMEBASEDWAVEFORMS_HH

#include "BSEmcFwEndcapWaveformGenPar.h"
#include "BSEmcMCDepositToTimebasedWaveforms.h"

/**
 * @brief Taks to create waveforms from hits.
 *
 * This is an alternative to BSEmcHitsToWaveform, which despite its name could be
 * used for all the EMC, not just FwEndcap.
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapTimebasedWaveforms : public BSEmcMCDepositToTimebasedWaveforms<BSEmcFwEndcapWaveformGenPar> {
 public:
  // Constructors
  BSEmcFwEndcapTimebasedWaveforms(Bool_t t_storewaves = kFALSE);
  // Destructor
  virtual ~BSEmcFwEndcapTimebasedWaveforms();

 protected:
  /** Get parameter containers **/
  virtual void SetupSimulator() /*override*/;

  ClassDef(BSEmcFwEndcapTimebasedWaveforms, 2);
};

#endif /*BSEMCFWENDCAPTIMEBASEDWAVEFORMS_HH*/
