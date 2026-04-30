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

#ifndef BSEMCABSPSA_HH
#define BSEMCABSPSA_HH

#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"

class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Baseclass for pulseshapeanalysis ( featureextraction )
 * @author Dima Melnychuk
 * @ingroup EmcBase
 **/
class BSEmcAbsPSA {
 public:
  BSEmcAbsPSA(){};
  virtual ~BSEmcAbsPSA() { ; };

  /**
   * @brief Find Hits in Waveform
   *
   * @param BSEmcWaveform
   *
   * @return Return number of hits, negative if an error occurred
   **/
  virtual Int_t Process(const BSEmcWaveform *t_waveform) = 0;

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
  virtual void GetHit(Int_t t_i, Double_t &t_energy, Double_t &t_time) = 0;

  /**
   * @brief Deprecated. Do not use.
   *
   * @param BSEmcWaveform
   * @param Double_t
   * @param Double_t
   *
   * @return
   **/

  virtual void Process(const BSEmcWaveform *t_waveform, Double_t &t_amplitude, Double_t &t_time);
  // deprecated do not use

  ClassDef(BSEmcAbsPSA, 1);
};

// ClassImp(BSEmcAbsPSA);
#endif /*BSEMCABSPSA_HH*/
