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

#ifndef PNDEMCFPGADIGITALFILTERANALYSER_HH
#define PNDEMCFPGADIGITALFILTERANALYSER_HH

#include "PndEmcPSAFPGASampleAnalyser.h"
#include <string>

class PndEmcPSAFPGAFilterMWD;

#define MAX_NUMBER_OF_HITS 100

class PndEmcPSAFPGADigitalFilterAnalyser : public PndEmcPSAFPGASampleAnalyser {

 public:
  PndEmcPSAFPGADigitalFilterAnalyser();
  PndEmcPSAFPGADigitalFilterAnalyser(const std::string &fname);
  ~PndEmcPSAFPGADigitalFilterAnalyser();

  virtual void setBaselineInterval(int anl_start, int bl_stop);
  virtual float baseline();
  virtual void init(PndEmcPSAFPGASampleAnalyser::SampleAnalyserParams &params);
  virtual void reset();
  virtual void put(float valueToStore);
  virtual void enable_mwd_filter(bool flag);

  virtual Int_t Process(const PndEmcWaveform *waveform);

 private:
  unsigned int analysis_start_position;
  unsigned int baseline_stop_position;
  float baseline_value_calculated;
  float ma_value_delay;
  int increasement;
  int decreasement;
  bool useMWDFilter;
  bool pulse_finished;
  int ith_pulse;
  int rising_count;

  PndEmcPSAFPGAFilterMWD *MWD_filter;

  ClassDef(PndEmcPSAFPGADigitalFilterAnalyser, 1);
};

#endif
