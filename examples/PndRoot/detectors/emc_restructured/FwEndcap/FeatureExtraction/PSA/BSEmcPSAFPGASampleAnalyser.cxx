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

#include "BSEmcPSAFPGASampleAnalyser.h"

#include <string>

#include "BSEmcPSAFPGAFilterCF.h"
#include "BSEmcPSAFPGAFilterDelay.h"
#include "BSEmcPSAFPGAFilterMA.h"
#include "BSEmcPSAFPGALinFitter.h"
#include "BSEmcWaveform.h"

/*
#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>

namespace pod = boost::program_options::detail;
*/
BSEmcPSAFPGASampleAnalyser::BSEmcPSAFPGASampleAnalyser() : fStatus(kUndefined)
{
  fCF_filter = new BSEmcPSAFPGAFilterCF();
  fCF_filter2 = new BSEmcPSAFPGAFilterCF();
  fMA_triggering = new BSEmcPSAFPGAFilterMA();
  fMA_delay = new BSEmcPSAFPGAFilterDelay();
  fCF_delay = new BSEmcPSAFPGAFilterDelay();
  fSignal_delay = new BSEmcPSAFPGAFilterDelay();
  fSignal_buffer = new BSEmcPSAFPGAFilterDelay();
  fCF_Fitter = new BSEmcPSAFPGALinFitter();
  fBaseline_value = 7000.;
}

BSEmcPSAFPGASampleAnalyser::BSEmcPSAFPGASampleAnalyser(const std::string &t_fname) : fStatus(kUndefined)
{
  fCF_filter = new BSEmcPSAFPGAFilterCF();
  fCF_filter2 = new BSEmcPSAFPGAFilterCF();
  fMA_triggering = new BSEmcPSAFPGAFilterMA();
  fMA_delay = new BSEmcPSAFPGAFilterDelay();
  fCF_delay = new BSEmcPSAFPGAFilterDelay();
  fSignal_delay = new BSEmcPSAFPGAFilterDelay();
  fSignal_buffer = new BSEmcPSAFPGAFilterDelay();
  fCF_Fitter = new BSEmcPSAFPGALinFitter();
  fBaseline_value = 7000.;
  initFromFile(t_fname);
}

BSEmcPSAFPGASampleAnalyser::~BSEmcPSAFPGASampleAnalyser() {}

void BSEmcPSAFPGASampleAnalyser::init(BSEmcPSAFPGASampleAnalyser::SampleAnalyserParams &t_params)
{
  fAnalyserParams.hit_threshold = t_params.hit_threshold;
  fAnalyserParams.ma_trig_M = t_params.ma_trig_M;
  fAnalyserParams.cf_delay = t_params.cf_delay;
  fAnalyserParams.cf_ratio = t_params.cf_ratio;
  fAnalyserParams.cf_fitter_length = t_params.cf_fitter_length;
  fAnalyserParams.cf_fit_offset = t_params.cf_fit_offset;
  fAnalyserParams.mwd_length = t_params.mwd_length;
  fAnalyserParams.mwd_tau = t_params.mwd_tau;
  fAnalyserParams.mwd2_length = t_params.mwd2_length;
  fAnalyserParams.mwd2_tau = t_params.mwd2_tau;
  fAnalyserParams.mwd3_length = t_params.mwd3_length;
  fAnalyserParams.mwd3_tau = t_params.mwd3_tau;
  fAnalyserParams.sig_delay = t_params.sig_delay;
  fAnalyserParams.rough_timing_corr = t_params.rough_timing_corr;
  fAnalyserParams.clock_unit = t_params.clock_unit;
  fAnalyserParams.iafactor = t_params.iafactor;

  fCF_filter->set(fAnalyserParams.cf_delay, fAnalyserParams.cf_ratio);
  fCF_filter2->set(fAnalyserParams.cf_delay / 2, fAnalyserParams.cf_ratio);
  fSignal_delay->set(fAnalyserParams.cf_delay + fAnalyserParams.cf_delay / 2);
  fMA_triggering->set(fAnalyserParams.ma_trig_M);
  fCF_delay->set(fAnalyserParams.cf_fit_offset);

  return;
}

void BSEmcPSAFPGASampleAnalyser::initFromFile(const std::string & /*unused*/)
{ // fname //[R.K.03/2017] unused variable(s)
  SampleAnalyserParams newParams;
  newParams.hit_threshold = 200;
  newParams.ma_trig_M = 30;
  newParams.cf_delay = 5;
  newParams.cf_ratio = 0.15;
  newParams.cf_fitter_length = 5;
  newParams.cf_fit_offset = 2;
  newParams.mwd_length = 90;
  newParams.mwd_tau = 2500;
  newParams.clock_unit = 10; // ns
  newParams.iafactor = 1.;   // ns

  /*  std::ifstream ifile(fname.c_str());
    if(!ifile) {
      std::cerr << "Can not open input file: " << fname << "\n";
      exit(1);
    }

    std::set<std::string> options;
    options.insert("MWD_Tau");
    options.insert("MWD_Length");
    options.insert("TR_Thresh");
    options.insert("CFD_Ratio");
    options.insert("CFD_Delay");
    options.insert("Fit_ofset");
    options.insert("Fitter_L");
    options.insert("MA_trig");

    //parser
    for (pod::config_file_iterator i(ifile, options), e ; i != e; ++i) {
      if (i->string_key == "MWD_Tau") {
        newParams.mwd_tau = atof((i->value[0]).c_str());
      }
      if (i->string_key == "MWD_Length") {
        newParams.mwd_length = atoi((i->value[0]).c_str());
      }
      if (i->string_key == "TR_Thresh") {
        newParams.hit_threshold = atof((i->value[0]).c_str());
      }
      if (i->string_key == "CFD_Ratio") {
        newParams.cf_ratio = atof((i->value[0]).c_str());
      }
      if (i->string_key == "CFD_Delay") {
        newParams.cf_delay = atoi((i->value[0]).c_str());
      }
      if (i->string_key == "Fit_ofset") {
        newParams.cf_fit_offset = atoi((i->value[0]).c_str());
      }
      if (i->string_key == "Fitter_L") {
        newParams.cf_fitter_length = atoi((i->value[0]).c_str());
      }
      if (i->string_key == "MA_trig") {
        newParams.ma_trig_M = atoi((i->value[0]).c_str());
      }
    }

    ifile.close();*/
  init(newParams);
  return;
}
void BSEmcPSAFPGASampleAnalyser::Init(const std::vector<Double_t> &t_params)
{
  SampleAnalyserParams newParams;
  newParams.ma_trig_M = (Int_t)t_params.at(0);
  newParams.hit_threshold = t_params.at(1);
  newParams.cf_delay = (Int_t)t_params.at(2);
  newParams.cf_ratio = t_params.at(3);
  newParams.cf_fitter_length = (Int_t)t_params.at(4);
  newParams.cf_fit_offset = (Int_t)t_params.at(5);
  newParams.mwd_length = (Int_t)t_params.at(6);
  newParams.mwd_tau = t_params.at(7);
  newParams.clock_unit = t_params.at(8);
  newParams.iafactor = t_params.at(9);
  /*
  std::cout << newParams.ma_trig_M << std::endl;
  std::cout << newParams.hit_threshold << std::endl;
  std::cout << newParams.cf_delay << std::endl;
  std::cout << newParams.cf_ratio << std::endl;
  std::cout << newParams.cf_fitter_length << std::endl;
  std::cout << newParams.cf_fit_offset << std::endl;
  std::cout << newParams.mwd_length << std::endl;
  std::cout << newParams.mwd_tau << std::endl;
*/
  init(newParams);
  return;
}

float BSEmcPSAFPGASampleAnalyser::sampleTime(Int_t t_index)
{
  if (t_index < nHits() && t_index >= 0) {
    return fTime[t_index];
  }
  return 0.0;
}

float BSEmcPSAFPGASampleAnalyser::sampleAmplitude(Int_t t_index)
{
  if (t_index < nHits() && t_index >= 0) {
    return fAmplitude[t_index];
  }
  return 0.0;
}

float BSEmcPSAFPGASampleAnalyser::sampleIntegral(Int_t t_index)
{
  if (t_index < nHits() && t_index >= 0) {
    return fIntegral[t_index];
  }
  return 0.0;
}

Int_t BSEmcPSAFPGASampleAnalyser::samplePileup(Int_t t_index)
{
  if (t_index < nHits() && t_index >= 0) {
    return fPileups[t_index];
  }
  return 0.0;
}

Int_t BSEmcPSAFPGASampleAnalyser::Process(const BSEmcWaveform *t_waveform)
{
  // std::cout<<"BSEmcPSAFPGASampleAnalyser::Process#"<<waveform->GetTimeStamp()
  //	<<", "<<waveform->GetActiveTime()<<std::endl;
  reset();
  std::vector<double> signal = t_waveform->GetSignal();
  std::vector<double>::iterator it;
  for (it = signal.begin(); it < signal.end(); it++) {
    this->put(*it);
  }
  return this->nHits();
}

void BSEmcPSAFPGASampleAnalyser::GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time)
{
  t_energy = this->sampleIntegral(t_idx);
  t_energy = this->sampleAmplitude(t_idx);
  t_time = this->sampleTime(t_idx);
}

void BSEmcPSAFPGASampleAnalyser::GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time, Int_t &t_PileupType)
{
  t_energy = this->sampleIntegral(t_idx);
  t_energy = this->sampleAmplitude(t_idx);
  t_time = this->sampleTime(t_idx);
  t_PileupType = this->samplePileup(t_idx);
}

ClassImp(BSEmcPSAFPGASampleAnalyser);
