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

#ifndef BSEMCLUTBASEDPULSESHAPE_HH
#define BSEMCLUTBASEDPULSESHAPE_HH

#include <vector>

#include "BSEmcAbsPulseshape.h"

/**
 * @brief Look-Up-Table based Pulseshape
 *
 * Using a Look-Up-Table consisting of 120 samples(each sample corresponds to 12.5 ns) describing the peak and tail as I failed to  perfectly described the peak with analytical
 * functions.
 *
 * To be used with the BSEmcFwEndcapSimpleCosmicWaveformGeneration
 */
class BSEmcLUTBasedPulseshape : public BSEmcAbsPulseshape {

 public:
  BSEmcLUTBasedPulseshape();
  virtual ~BSEmcLUTBasedPulseshape(){};

  virtual Double_t operator()(const Double_t t_time, const Double_t t_amp, const Double_t t_timeOffset) const;
  void SetLUTFile(const std::string &t_lutfile)
  {
    fLUTFileName = t_lutfile;
    ReadLUT(fLUTFileName);
  }

  UInt_t GetWaveformLength() const { return fLUT.size(); }

 private:
  void ReadLUT(const std::string &t_lutfile);

  std::vector<Double_t> fLUT{};
  std::string fLUTFileName{};
  ClassDef(BSEmcLUTBasedPulseshape, 1)
};

#endif /*BSEMCLUTBASEDPULSESHAPE_HH*/
