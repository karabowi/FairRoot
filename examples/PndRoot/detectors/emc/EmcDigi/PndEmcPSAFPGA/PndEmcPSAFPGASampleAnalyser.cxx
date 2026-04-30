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

#include "PndEmcPSAFPGASampleAnalyser.h"
#include "PndEmcWaveform.h"

#include "PndEmcPSAFPGAFilterCF.h"
#include "PndEmcPSAFPGAFilterMA.h"
#include "PndEmcPSAFPGAFilterDelay.h"
#include "PndEmcPSAFPGALinFitter.h"

#include "FairLogger.h"
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <sstream>
/*
#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>

namespace pod = boost::program_options::detail;
*/
PndEmcPSAFPGASampleAnalyser::PndEmcPSAFPGASampleAnalyser() : status(kUndefined) {
  LOG(info) << " SampleAnalyser::status(kUndefined)";
  CF_filter = new PndEmcPSAFPGAFilterCF();
  CF_filter2 = new PndEmcPSAFPGAFilterCF();
  MA_triggering = new PndEmcPSAFPGAFilterMA();
  MA_delay = new PndEmcPSAFPGAFilterDelay();
  CF_delay = new PndEmcPSAFPGAFilterDelay();
  Signal_delay = new PndEmcPSAFPGAFilterDelay();
  Signal_buffer= new PndEmcPSAFPGAFilterDelay();
  CF_Fitter = new PndEmcPSAFPGALinFitter();
  baseline_value = 7000.;

}

PndEmcPSAFPGASampleAnalyser::PndEmcPSAFPGASampleAnalyser(const std::string &fname) : status(kUndefined) {
  LOG(info) << " SampleAnalyser::status(kUndefined)..file";
  CF_filter = new PndEmcPSAFPGAFilterCF();
  CF_filter2 = new PndEmcPSAFPGAFilterCF();
  MA_triggering = new PndEmcPSAFPGAFilterMA();
  MA_delay = new PndEmcPSAFPGAFilterDelay();
  CF_delay = new PndEmcPSAFPGAFilterDelay();
  Signal_buffer= new PndEmcPSAFPGAFilterDelay();
  Signal_delay = new PndEmcPSAFPGAFilterDelay();
  CF_Fitter = new PndEmcPSAFPGALinFitter();
  baseline_value = 7000.;
  initFromFile(fname);
 
}

PndEmcPSAFPGASampleAnalyser::~PndEmcPSAFPGASampleAnalyser() {
}

void PndEmcPSAFPGASampleAnalyser::init(PndEmcPSAFPGASampleAnalyser::SampleAnalyserParams &params) {
  LOG(info) << " SampleAnalyser::....init......";
  analyserParams.hit_threshold = params.hit_threshold;
  analyserParams.ma_trig_M = params.ma_trig_M;
  analyserParams.cf_delay = params.cf_delay;
  analyserParams.cf_ratio = params.cf_ratio;
  analyserParams.cf_fitter_length = params.cf_fitter_length;
  analyserParams.cf_fit_offset = params.cf_fit_offset;
  analyserParams.mwd_length = params.mwd_length;
  analyserParams.mwd_tau = params.mwd_tau;
  analyserParams.mwd2_length = params.mwd2_length;
  analyserParams.mwd2_tau = params.mwd2_tau;
  analyserParams.mwd3_length = params.mwd3_length;
  analyserParams.mwd3_tau = params.mwd3_tau;
  analyserParams.sig_delay = params.sig_delay;
  analyserParams.rough_timing_corr = params.rough_timing_corr;
  analyserParams.clock_unit= params.clock_unit;
  analyserParams.iafactor= params.iafactor;

  CF_filter->set(analyserParams.cf_delay, analyserParams.cf_ratio);
  CF_filter2->set(analyserParams.cf_delay/2, analyserParams.cf_ratio);
  Signal_delay->set(analyserParams.cf_delay + analyserParams.cf_delay/2);
  MA_triggering->set(analyserParams.ma_trig_M);
  CF_delay->set(analyserParams.cf_fit_offset);

  return;
}

void PndEmcPSAFPGASampleAnalyser::initFromFile(const std::string &) {// fname //[R.K.03/2017] unused variable(s)
  LOG(info) << " SampleAnalyser::.init..from..file...";
  SampleAnalyserParams newParams;
  newParams.hit_threshold = 200;
  newParams.ma_trig_M = 30;
  newParams.cf_delay = 5;
  newParams.cf_ratio = 0.15;
  newParams.cf_fitter_length = 5; 
  newParams.cf_fit_offset = 2;
  newParams.mwd_length = 90;
  newParams.mwd_tau = 2500;
  newParams.clock_unit = 10;//ns
  newParams.iafactor= 1.;//ns

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
void PndEmcPSAFPGASampleAnalyser::Init(const std::vector<Double_t> &params){
  LOG(info) << " FPGASampleAnalyser::Init";
  SampleAnalyserParams newParams;
	newParams.ma_trig_M = (int) params.at(0);
	newParams.hit_threshold = params.at(1);
	newParams.cf_delay = (int) params.at(2);
	newParams.cf_ratio = params.at(3);
	newParams.cf_fitter_length = (int) params.at(4);
	newParams.cf_fit_offset = (int) params.at(5);
	newParams.mwd_length = (int) params.at(6);
	newParams.mwd_tau = params.at(7);
	newParams.clock_unit = params.at(8);
	newParams.iafactor = params.at(9);
        

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


float PndEmcPSAFPGASampleAnalyser::sampleTime(int index) {
  if (index < nHits() && index >= 0){ 
  /* if (analyserParams.hit_threshold <= 100)
		                        {  Ratio.open("Hit_low", std::ofstream::app); 
                                          Ratio<<time[index]<<std::endl;
					Ratio.close();
                     				 } else{
						Ratio.open("Hit_high", std::ofstream::app); 
                      				 Ratio<<time[index]<<std::endl;
                      				 Ratio.close();
                      				   }*/
    return time[index];}
  return 0.0;
}

float PndEmcPSAFPGASampleAnalyser::sampleAmplitude(int index) {
  if(index < nHits() && index >= 0){
      /* if (analyserParams.hit_threshold <= 100)
		                        {  Ratio.open("Hit_low", std::ofstream::app); 
                                          Ratio<<amplitude[index]<<" ";
					Ratio.close();
                     				 } else{
						Ratio.open("Hit_high", std::ofstream::app); 
                      				 Ratio<<amplitude[index]<<" ";
                      				 Ratio.close();
                      				   }*/
    return amplitude[index];}
  return 0.0;
}

float PndEmcPSAFPGASampleAnalyser::sampleIntegral(int index) {
  if (index < nHits() && index >= 0)
    return integral[index];
  return 0.0;
}
Int_t PndEmcPSAFPGASampleAnalyser::samplePileup(int index) {
  if (index < nHits() && index >= 0)
    return pileups[index];
  return 0.0;
}
Int_t PndEmcPSAFPGASampleAnalyser::Process(const PndEmcWaveform *waveform){
	std::cout<<"PndEmcPSAFPGASampleAnalyser::Process#"<<waveform->GetTimeStamp()
		<<", "<<waveform->GetActiveTime()<<std::endl;
	reset();
	std::vector<double> signal = waveform->GetSignal();
	std::vector<double>::iterator it =signal.end();
            std::cout << "it:  "<< *it<< std::endl;
	for(it = signal.begin(); it < signal.end();it++){
		this->put(*it);
	}
        std::cout<<"PndEmcPSAFPGASampleAnalyser::Finish"<<std::endl;
	return this->nHits();
}

void PndEmcPSAFPGASampleAnalyser::GetHit(Int_t i,Double_t &energy, Double_t &t){
	energy = this->sampleIntegral(i);
	energy = this->sampleAmplitude(i);
	t = this->sampleTime(i);
 
     
}
void PndEmcPSAFPGASampleAnalyser::GetHit2(Int_t i,Double_t &energy, Double_t &t, Int_t &PileupType ){
	energy = this->sampleIntegral(i);
	energy = this->sampleAmplitude(i);
	t = this->sampleTime(i);
        PileupType = this->samplePileup(i);
}

ClassImp(PndEmcPSAFPGASampleAnalyser);
