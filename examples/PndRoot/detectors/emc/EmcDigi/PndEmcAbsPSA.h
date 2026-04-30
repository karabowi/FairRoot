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

//-----------------------------------------------------------
// Description:
//      Abstract interface for EMC Pulse Shape Analyzer
//      Dima Melnychuk
//-----------------------------------------------------------

#ifndef EMCABSPSA_HH
#define EMCABSPSA_HH

// Base Class Headers ----------------
#include <vector>
#include "TObject.h"

class PndEmcWaveform;

/**
 * @brief Baseclass for pulseshapeanalysis ( featureextraction )
 *
 * @ingroup PndEmc
 **/
class PndEmcAbsPSA {
 public:
  PndEmcAbsPSA(){};
  virtual ~PndEmcAbsPSA() { ; };

  /**
   * @brief Find Hits in Waveform
   *
   * @param PndEmcWaveform
   *
   * @return Return number of hits, negative if an error occurred
   **/
  virtual Int_t Process(const PndEmcWaveform *waveform) = 0;

  /**
   * @brief reset found hits
   *
   * @return
   **/
  // Constructors/Destructors ---------
  virtual void Reset(){};

  /**
   * @brief Get energy and time of hit
   *
   * @param Int_t  Index of Hit
   * @param Double_t  Energy of Hit
   * @param Double_t  Time of Hit
   *
   * @return
   **/
  virtual void GetHit(Int_t i, Double_t &energy, Double_t &time) = 0;

  /**
   * @brief Deprecated. Do not use.
   *
   * @param PndEmcWaveform
   * @param Double_t
   * @param Double_t
   *
   * @return
   **/

  virtual void Process(const PndEmcWaveform *waveform, Double_t &amplitude, Double_t &time);
  // deprecated do not use

  ClassDef(PndEmcAbsPSA, 1);
};

// ClassImp(PndEmcAbsPSA);
#endif
