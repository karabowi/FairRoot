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

#ifndef BSEMCSHASHLYKDIGI_H_
#define BSEMCSHASHLYKDIGI_H_

#include "BSEmcExtractDigisFromWaveforms.h"
#include "BSEmcShashlykDigiPar.h"

class BSEmcHit;
class BSEmcTwoCoordIndex;
class BSEmcWaveform;

class TClonesArray;
class BSEmcRecoPar;
class BSEmcGeoPar;
class BSEmcAbsPSA;
class BSEmcAbsPulseshape;
class BSEmcPSAOptimalFilterAnalyser;

/**
 * @brief Task to create digis from waveforms.
 *
 * This is an alternative to BSEmcWaveformToDigi, which despite its name could be
 * used for all the EMC, not just Shashlik.
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup PndEmc
 */
class BSEmcShashlykDigi : public BSEmcExtractDigisFromWaveforms<BSEmcShashlykDigiPar> {
 public:
  // Constructors
  BSEmcShashlykDigi(const std::string &t_detectorname = "Shashlik", Bool_t t_storedigis = kTRUE);
  // Destructor
  virtual ~BSEmcShashlykDigi();

 protected:
  virtual void DefinePSA() /*override*/;
  virtual BSEmcDigi::eGAIN GetGainType(BSEmcWaveform *t_waveform, Int_t t_hit) const /*override*/;

 private:
  ClassDef(BSEmcShashlykDigi, 1)
};

#endif /*BSEMCSHASHLYKDIGI_H_*/
