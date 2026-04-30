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

#include "PndEmcPSAFPGADigitalFilterAnalyser.h"

#include "PndEmcPSAFPGAFilterCF.h"
#include "PndEmcPSAFPGAFilterMA.h"
#include "PndEmcPSAFPGAFilterMWD.h"
#include "PndEmcPSAFPGAFilterDelay.h"
#include "PndEmcPSAFPGALinFitter.h"
#include "PndEmcWaveform.h"
#include <iostream>
#include <fstream>
#include <string>

PndEmcPSAFPGADigitalFilterAnalyser::PndEmcPSAFPGADigitalFilterAnalyser() : PndEmcPSAFPGASampleAnalyser()
{
  MWD_filter = new PndEmcPSAFPGAFilterMWD();
  analysis_start_position = 0;
  baseline_stop_position = 19;
  increasement = 0;
  ma_value_delay = 0.;
  useMWDFilter = false;
  baseline_value = 0.;
  pulse_finished = true;
  ith_pulse = 0;
}

PndEmcPSAFPGADigitalFilterAnalyser::PndEmcPSAFPGADigitalFilterAnalyser(const std::string &fname) : PndEmcPSAFPGASampleAnalyser(fname)
{
  MWD_filter = new PndEmcPSAFPGAFilterMWD();
  analysis_start_position = 0;
  baseline_stop_position = 19;
  increasement = 0;
  ma_value_delay = 0.;
  useMWDFilter = false;
  baseline_value = 0.;
  pulse_finished = true;
  ith_pulse = 0;
}

PndEmcPSAFPGADigitalFilterAnalyser::~PndEmcPSAFPGADigitalFilterAnalyser()
{
  delete MWD_filter;
}

void PndEmcPSAFPGADigitalFilterAnalyser::enable_mwd_filter(bool flag)
{
  useMWDFilter = flag;
}
void PndEmcPSAFPGADigitalFilterAnalyser::init(PndEmcPSAFPGASampleAnalyser::SampleAnalyserParams &params)
{
  PndEmcPSAFPGASampleAnalyser::init(params);
  MWD_filter->set(analyserParams.mwd_tau, analyserParams.mwd_length);
  return;
}

void PndEmcPSAFPGADigitalFilterAnalyser::setBaselineInterval(int anl_start, int bl_stop)
{
  analysis_start_position = anl_start;
  baseline_stop_position = bl_stop;
  return;
}

float PndEmcPSAFPGADigitalFilterAnalyser::baseline()
{
  return baseline_value_calculated / (baseline_stop_position - analysis_start_position);
}

void PndEmcPSAFPGADigitalFilterAnalyser::reset()
{
  local_time = 0;
  Number_of_hits = 0;
  baseline_value_calculated = 0.0;
  CF_filter->resetToZero();
  MA_triggering->resetToZero();
  Signal_delay->resetToZero();
  CF_delay->resetToZero();
  MWD_filter->resetToZero();
  pulse_detected = false;

  pulse_finished = true;
  ith_pulse = 0;
  rising_count = 0;
  return;
}
Int_t PndEmcPSAFPGADigitalFilterAnalyser::Process(const PndEmcWaveform *waveform)
{
  // std::cout<<"PndEmcPSAFPGADigitalFilterAnalyser::Process# MWD used = "<<useMWDFilter<<std::endl;
  reset();
  std::vector<double> signal = waveform->GetSignal();
  std::vector<double>::iterator it;
  // read directly
  baseline_value = waveform->GetBaseline();
  // std::cout<<"PndEmcPSAFPGADigitalFilterAnalyser:: Process, baseline_value#"<<baseline_value<<std::endl;
  for (it = signal.begin(); it < signal.end(); it++) {
    this->put(*it);
  }
  return this->nHits();
}
// new version put
/*void PndEmcPSAFPGADigitalFilterAnalyser::put(float valueToStore)
  {
//std::cout<<"local_time#"<<local_time<<", val#"<<valueToStore<<std::endl;
float mwd_val = valueToStore;
if(useMWDFilter)
mwd_val = MWD_filter->put(valueToStore);

float signal = mwd_val - baseline_value;
float signal_delayed = Signal_delay->put(signal);
float cf_value = CF_filter->put(signal);
//float raw_signal = valueToStore - baseline_value;
//float cf_value = CF_filter->put(raw_signal);
float cf_value_delayed = CF_delay->put(cf_value);
float ma_value = MA_triggering->put(signal);

int dT = local_time - rough_pulse_timing;
//bool pulse_conditon = (cf_value > cf_value_delayed);

if((cf_value > cf_value_delayed ) && (ma_value > 2*analyserParams.hit_threshold))
{
++ rising_count;
if(!pulse_detected && rising_count>=3 ){
//	cout<<"Detect pulse # time#"<<local_time<<", value#"<<cf_value<<endl;
rising_count = 0;
pulse_detected = true;
pulse_finished = false;
CF_Fitter->reset();
CF_Fitter->putPoint(local_time, cf_value_delayed);
cf_zero_crossing_points = 1;
rough_pulse_timing = local_time;
++Number_of_hits;
integral[Number_of_hits-1] = 0.;
amplitude[Number_of_hits-1] = 0.;
}
}
if(pulse_detected){
if(( dT < 3*analyserParams.cf_delay) && (signal_delayed > amplitude[Number_of_hits-1])){
amplitude[Number_of_hits-1] = signal_delayed;
//time[Number_of_hits -1 ] = local_time;
}
if(cf_zero_crossing_points < analyserParams.cf_fitter_length) {
CF_Fitter->putPoint(local_time, cf_value_delayed);
cf_zero_crossing_points++;
//  std::cout<<" after pulse_detected , local_time #"<<local_time<<", cf#"<<cf_value<<std::endl;
}
if((cf_value < cf_value_delayed)&&(dT > 3*analyserParams.cf_delay) ){
//	cout<<"Begin next search, time#"<<local_time<<", dT#"<<dT<<", 3*CF_WIDTH#"<<3*analyserParams.cf_delay<<endl;
pulse_detected = false;
rising_count = 0;
CF_Fitter->fit();
double a = CF_Fitter->offset();
double k = CF_Fitter->slope();
double average = CF_Fitter->average();
cf_zero_crossing_points = 0;
if(k > 0)
time[Number_of_hits-1] = (average-a)/k;
else
-- Number_of_hits;
}
}
if(!pulse_finished){
if( (dT > analyserParams.cf_delay) && (ma_value < signal_delayed) && (ma_value < analyserParams.hit_threshold))
{
++ ith_pulse;
pulse_finished = true;
//	cout<<"Finish pulse #"<<local_time<<", time diff#"<<dT<<", sig_delay_value#"<<signal_delayed<<endl;
//recalculate results;
//	cout<<"iafactor = "<<analyserParams.iafactor<<", Integ = "<<integral[0]<<endl;
if(ith_pulse == 1 && Number_of_hits>=2){
amplitude[1] = integral[0]/analyserParams.iafactor - amplitude[0];
}
}
integral[ith_pulse] += signal_delayed;
}
local_time++;
}*/

// orginal put
void PndEmcPSAFPGADigitalFilterAnalyser::put(float valueToStore)
{
  // float mwd_val = MWD_filter->put(valueToStore);
  float mwd_val = valueToStore;
  if (useMWDFilter)
    mwd_val = MWD_filter->put(valueToStore);

  if (local_time < analysis_start_position) {
    local_time++;
    return;
  }
  if (local_time < baseline_stop_position) {
    baseline_value_calculated += mwd_val;
    //
    // uncomment lines below for automatic baseline calculation; does not work for high rates
    baseline_value = baseline_value_calculated / (baseline_stop_position - analysis_start_position);
    local_time++;
    return;
  }
  float signal = mwd_val - baseline_value;
  float signal_delayed = Signal_delay->put(signal);
  float cf_value = CF_filter->put(signal);
  float cf_value_delayed = CF_delay->put(cf_value);
  float ma_value = MA_triggering->put(signal);
  // cout<<"local_time #"<<local_time<<", raw#"<<valueToStore<<", mwd#"<<signal<<", ma#"<<ma_value<<", cfd#"<<cf_value<<endl;
  int dT = local_time - rough_pulse_timing;
  if (!pulse_detected) {
    if ((cf_value_delayed < 0.0) && (cf_value > 0.0) && (ma_value > analyserParams.hit_threshold)) {
      // cout<<"detect pulse #"<<local_time<<", value#"<<cf_value <<endl;
      // hit detected
      // CF
      // cout<<"hit ["<<Number_of_hits<<"] # idx"<<cf_zero_crossing_points<<", #"<<local_time<<", v#"<<cf_value_delayed<<endl;
      CF_Fitter->reset();
      CF_Fitter->putPoint(local_time, cf_value_delayed);
      cf_zero_crossing_points = 1;
      pulse_detected = true;
      rough_pulse_timing = local_time;
      integral[Number_of_hits] = 0.0;
      amplitude[Number_of_hits] = 0.0;
    }
  } else {
    // CF timing
    if (cf_zero_crossing_points < analyserParams.cf_fitter_length) {
      // cout<<"hit ["<<Number_of_hits<<"] # idx"<<cf_zero_crossing_points<<", #"<<local_time<<", v#"<<cf_value_delayed<<endl;
      CF_Fitter->putPoint(local_time, cf_value_delayed);
      cf_zero_crossing_points++;
    }
    // Integral
    integral[Number_of_hits] += signal_delayed;
    // Amplitude
    if ((dT < 2 * analyserParams.cf_delay) && (signal_delayed > amplitude[Number_of_hits])) {
      // cout<<"update amp t#"<<local_time<<", a#"<<signal_delayed<<endl;
      amplitude[Number_of_hits] = signal_delayed;
    }
    // Finalize pulse detection
    // cout<<"dT#"<<dT<<", cf_delay#"<<analyserParams.cf_delay;
    // cout<<"ma #"<<ma_value<<", signal_delayed #"<<signal_delayed<<", analyserParams.hit_threshold#"<<analyserParams.hit_threshold<<endl;
    if ((dT > 2 * analyserParams.cf_delay) && (signal_delayed < analyserParams.hit_threshold)) {
      // cout<<"finish pulse #"<<local_time<<", cf_value_delayed#"<<cf_value_delayed<<", dT#"<<dT<<endl;
      /// analyserParams.ma_trig_M))
      cf_zero_crossing_points = 0;
      pulse_detected = false;
      CF_Fitter->fit();
      double a = CF_Fitter->offset();
      double k = CF_Fitter->slope();
      time[Number_of_hits] = 0.0;
      // if(k > 0){
      //	if(-a/k < 0 || -a/k > 3*analyserParams.cf_delay ) time[Number_of_hits] = CF_Fitter->averageX();
      //	else
      //		time[Number_of_hits] = -a/k;
      //	Number_of_hits++;
      //}
      if (k > 0 && -a / k > 0 && -a / k < 3 * analyserParams.cf_delay) {
        time[Number_of_hits] = -a / k;
        Number_of_hits++;
      }
    }
  }

  local_time++;
}

ClassImp(PndEmcPSAFPGADigitalFilterAnalyser);
