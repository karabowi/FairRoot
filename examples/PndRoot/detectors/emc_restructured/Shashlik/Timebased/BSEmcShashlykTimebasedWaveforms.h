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
//  Guang Zhao          - implementation for shashlyk
//----------------------------------------------------------------------

//#pragma once
#ifndef BSEMCSHASHLYKTIMEBASEDWAVEFORMS_H
#define BSEMCSHASHLYKTIMEBASEDWAVEFORMS_H

#include "BSEmcMCDepositToTimebasedWaveforms.h"

class BSEmcMapper;
class BSEmcShashlykDigiPar;
class BSEmcGeoPar;
class BSEmcWaveformBuffer;
class BSEmcAbsWaveformSimulator;
class BSEmcFullStackedWaveformSimulator;
class BSEmcAbsWaveformModifier;
class BSEmcAbsPulseshape;
class TClonesArray;

#define MULTI

/**
 * @brief Taks to create waveforms from hits.
 *
 * This is an alternative to BSEmcHitsToWaveform, which despite its name could be
 * used for all the EMC, not just BwEndcap.
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup PndEmc
 */
class BSEmcShashlykTimebasedWaveforms : public BSEmcMCDepositToTimebasedWaveforms<BSEmcShashlykDigiPar> {
 public:
  // Constructors
  BSEmcShashlykTimebasedWaveforms(const std::string &t_detectorname = "BwEndcap", Bool_t t_storewaves = kFALSE);
  // Destructor
  virtual ~BSEmcShashlykTimebasedWaveforms();

 protected:
  virtual void SetupSimulator() /*override*/;

 private:
  ClassDef(BSEmcShashlykTimebasedWaveforms, 1)
};

#endif /*BSEMCSHASHLYKTIMEBASEDWAVEFORMS_H*/
