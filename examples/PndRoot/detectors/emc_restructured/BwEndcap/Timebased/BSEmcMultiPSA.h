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
//     	Wrapping class to manage multiple independent PSAs
//      Guang Zhao
//-----------------------------------------------------------

#ifndef BSEMCMULTIPSA_HH
#define BSEMCMULTIPSA_HH

#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsPSA.h"

class BSEmcPSACombinator;
class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Wrapping class to manage multiple independent PSAs
 *
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup BwEndcap
 */
class BSEmcMultiPSA : public BSEmcAbsPSA {
 public:
  BSEmcMultiPSA(BSEmcPSACombinator *);
  void AddPSA(BSEmcAbsPSA *);

  virtual ~BSEmcMultiPSA();

  virtual Int_t Process(const BSEmcWaveform *t_waveform) /*override*/;
  virtual void Reset() /*override*/;
  virtual void GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time) /*override*/;
  Int_t GetAPDGainIndex() { return fAPDGainIndex; }

 private:
  std::vector<BSEmcAbsPSA *> fPSA{};
  std::vector<Int_t> fNhit{};
  BSEmcPSACombinator *fCombinator{nullptr}; // combinator to combine multi-PSA outputs to a signle output
  Int_t fAPDGainIndex{-1}; // 0 for hg, 1 for lg //TODO: Create a more meaningfull and expressive enum GainMode especially sind APDGainIndex is unset until Process is run
  std::vector<Double_t> fEnergyList{};
  std::vector<Double_t> fTimeList{};

  ClassDef(BSEmcMultiPSA, 1)
};

#endif /*BSEMCMULTIPSA_HH*/
