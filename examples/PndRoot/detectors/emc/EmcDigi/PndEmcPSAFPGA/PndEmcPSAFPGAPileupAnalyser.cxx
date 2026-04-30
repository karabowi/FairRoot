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

#include "PndEmcPSAFPGAPileupAnalyser.h"
#include "PndEmcWaveform.h"
#include "PndEmcPSAFPGAFilterCF.h"
#include "PndEmcPSAFPGAFilterMWD.h"
#include "PndEmcPSAFPGAFilterMA.h"
#include "PndEmcPSAFPGAFilterDelay.h"
#include "PndEmcPSAFPGALinFitter.h"
//
#include "TRandom.h"
#include "TF1.h"
#include <fstream>
#include <iostream>
#include "FairLogger.h"

PndEmcPSAFPGAPileupAnalyser::PndEmcPSAFPGAPileupAnalyser()
  : PndEmcPSAFPGASampleAnalyser(), fVerbose(0), Int_thres(nullptr), Int_mean(nullptr), MWD_filter1(nullptr), MA_filter1(nullptr), MWD_filter2(nullptr), CF_prev(nullptr),
    func_defined(false)
{
}

PndEmcPSAFPGAPileupAnalyser::~PndEmcPSAFPGAPileupAnalyser ()
{
  if (Int_thres != nullptr)
    delete Int_thres;
  if (Int_mean != nullptr)
    delete Int_mean;
  if (MWD_filter1 != nullptr)
    delete MWD_filter1;
  if (MA_filter1 != nullptr)
    delete MA_filter1;
  if (MWD_filter2 != nullptr)
    delete MWD_filter2;
  if (CF_prev != nullptr)
    delete CF_prev;
}

void
PndEmcPSAFPGAPileupAnalyser::reset ()
{				//reset for every waveform to process
	if (fVerbose >= 7)
	{
    LOG(info) << " PileupAnalyser::reset - new waveform";
  }
	local_time = 0;
	//rough_pulse_timing = 0;
	Number_of_hits = 0;
	Square = 0;
	t = 0;
	max = 0;

	Number_of_puls = 0;
	Number_of_sample = 0;
	z[Number_of_puls] = 1;
	Number_of_puls_mwd = 0;
	Number_of_sample_mwd = 0;
	z_mwd[Number_of_puls_mwd] = 1;
	Number_of_puls_mwd2 = 0;
	Number_of_sample_mwd2 = 0;
	max_mwd2[Number_of_puls_mwd2]=0;
	t_mwd=0;
	t_mwd2[Number_of_puls_mwd2] = 0;
	z_mwd2[Number_of_puls_mwd2] = 1;
	saved_local_time[Number_of_puls] = 0;
	saved_local_time_mwd[Number_of_puls_mwd] = 0;
	saved_local_time_mwd2[Number_of_puls_mwd2] = 0;
	CF_filter->resetToZero ();
	CF_filter2->resetToZero ();
	MA_triggering->resetToZero ();
	MWD_filter1->resetToZero ();
	MWD_filter2->resetToZero ();
	MA_filter1->resetToZero ();
	CF_delay->resetToZero ();
	pulse_detected = false;
	pulse_detected_mwd = false;
	pulse_detected_mwd2 = false;
	BaselineSum = 0;
	return;
}


void
PndEmcPSAFPGAPileupAnalyser::InitParameters (const std::vector < Double_t >
&params)
{

	//define parameter initialization, otherwise conflict with clock_unit and iafactor parameters, which have been added to PndEmcPSAFPGASampleAnalyser
	//

	const Int_t size = params.size ();
	if (size != 14)
	{
		std::
		cerr <<
		"-W PndEmcPSAFPGAPileupAnalyser::InitParameters Mismatch in number of parameters ("
		<< size << "!=14). Filling missing parameter values up with zeros..."
		<< std::endl;
	}
	std::
	cout << "-I- PileupAnalyser::Init..params ..and..SampleAnalyser" << std::
	endl;
	SampleAnalyserParams newParams;
	newParams.ma_trig_M = (size > 0) ? (int) params.at (0) : 0;
	newParams.hit_threshold = (size > 1) ? params.at (1) : 0;
	newParams.cf_delay = (size > 2) ? (int) params.at (2) : 0;
	newParams.cf_ratio = (size>3) ? params.at(3): 0;
	//newParams.cf_ratio = 1 / 4;
	newParams.cf_fitter_length = (size > 4) ? (int) params.at (4) : 0;
	newParams.cf_fit_offset = (size > 5) ? (int) params.at (5) : 0;
	newParams.mwd_length = (size>6) ? (int) params.at(6): 0;
	newParams.mwd_tau = (size>7) ? params.at(7): 0;
	//newParams.mwd_length = 32;
	//newParams.mwd_tau = 25;
	newParams.mwd2_length = 10;
	newParams.mwd2_tau = 24.5;
	//newParams.mwd2_length = (size>8) ? (int) params.at(8): 0;
	//newParams.mwd2_tau = (size>9) ? params.at(9): 0;
	newParams.mwd3_length = (size > 10) ? (int) params.at (10) : 0;
	newParams.mwd3_tau = (size > 11) ? params.at (11) : 0;
	newParams.sig_delay = (size > 12) ? params.at (12) : 0;
	newParams.rough_timing_corr = (size > 13) ? params.at (13) : 0;

	PndEmcPSAFPGASampleAnalyser::init (newParams);
}

void
PndEmcPSAFPGAPileupAnalyser::Init (const std::vector < Double_t > &params,
		TF1 * r_thres, TF1 * r_mean,
		float extBaselineValue)
{
	std::cout << "-I- PileupAnalyser::Init_extBaselineValue_params " << std::
			endl;
	Init2 (params, r_thres, r_mean);
	setBaseline (extBaselineValue);

}

void
PndEmcPSAFPGAPileupAnalyser::Init (const std::vector < Double_t > &params,
		TF1 * r_thres, TF1 * r_mean,
		unsigned int baselineStartSample,
		unsigned int baselineStopSample)
{
  LOG(info) << " PileupAnalyser::Init_baselineStartSample";
  Init2 (params, r_thres, r_mean);
	setBaselineWindow (baselineStartSample, baselineStopSample);

}


void
PndEmcPSAFPGAPileupAnalyser::Init2 (const std::vector < Double_t > &params,
		TF1 * r_thres, TF1 * r_mean)
{
	std::cout << "-I- PileupAnalyser::Init2" << "  hit_threshold " << params.
			at (1) << std::endl;
	InitParameters (params);
	Signal_delay->set (analyserParams.sig_delay);

  if (MWD_filter1 != nullptr)
    delete MWD_filter1;
	MWD_filter1 = new PndEmcPSAFPGAFilterMWD ();
  if (MA_filter1 != nullptr)
    delete MA_filter1;
	MA_filter1 = new PndEmcPSAFPGAFilterMA ();
  if (MWD_filter2 != nullptr)
    delete MWD_filter2;
	MWD_filter2 = new PndEmcPSAFPGAFilterMWD ();
  if (CF_prev != nullptr)
    delete CF_prev;
	CF_prev = new PndEmcPSAFPGAFilterDelay ();

	MWD_filter1->set (analyserParams.mwd_tau, analyserParams.mwd_length);
	MA_filter1->set (analyserParams.mwd_length/2);
	MWD_filter2->set (analyserParams.mwd2_tau, analyserParams.mwd2_length);
	CF_prev->set (2);
	// std::cout << "-I- analyserParams.mwd_tau " <<analyserParams.mwd_tau<<" mvd2 "<<analyserParams.mwd2_tau<<" mvd3 "<<analyserParams.mwd3_tau<<std::endl;

  if (r_mean != nullptr && r_thres != nullptr) {
    func_defined = true;

    if (Int_thres != nullptr)
      delete Int_thres;
		Int_thres =
				new TF1 ("intThres", "(" + r_thres->GetExpFormula () + ")*x",
						r_thres->GetXmin (), r_thres->GetXmax ());
		Int_thres->SetParameters (r_thres->GetParameters ());
		Int_thres->SetNpx (1000);

    if (Int_mean != nullptr)
      delete Int_mean;
		Int_mean =
				new TF1 ("intMean", "(" + r_mean->GetExpFormula () + ")*x",
						r_mean->GetXmin (), r_mean->GetXmax ());
		Int_mean->SetParameters (r_mean->GetParameters ());
		Int_mean->SetNpx (1000);

  } else {
    std::
		cerr <<
		"-W PndEmcPSAFPGAPileupAnalyser:Init2 no pileup separation function defined. All hits will be treated as single pulses"
		<< std::endl;
  }
}

//TODO: Baseline follower (baselineMode = kFollow)

void
PndEmcPSAFPGAPileupAnalyser::setBaseline (float newBaseline)
{
	PndEmcPSAFPGASampleAnalyser::setBaseline (newBaseline);
	baselineMode = kExtern;
}

void
PndEmcPSAFPGAPileupAnalyser::setBaselineWindow (unsigned int startSample,
		unsigned int stopSample)
{
	// std::cout << "-I------  BaselineWindow----------I- " << std::endl;

	BaselineStartSample = startSample;
	BaselineStopSample = stopSample;
	baselineMode = kWindow;
	// std::cout << "-I-  BaselineStartSample " << BaselineStartSample<< std::endl;
	//     std::cout << "-I-  BaselineStopSample " << BaselineStopSample<< std::endl;
}


void
PndEmcPSAFPGAPileupAnalyser::GetHit (Int_t i, Double_t & Energy,
		Double_t & Time)
{
	Energy = sampleAmplitude (i);
	Time = sampleTime (i);
	//Energy = sampleIntegral(i);
	if (fVerbose >= 5)  std::cout << "-I- PndEmcPSAFPGAPileupAnalyser::GetHit   "<< std::endl;
}


void
PndEmcPSAFPGAPileupAnalyser::GetHit2 (Int_t i, Double_t & Energy,
		Double_t & Time, Int_t & PileupType)
{
	GetHit (i, Energy, Time);
	if (i < nHits () && i >= 0)
	{
		PileupType = pileups[i];
		if (fVerbose >= 5)  std::cout << "-I- PndEmcPSAFPGAPileupAnalyser::GetHit2   "<<pileups[i]<< std::endl;
	}
	else
	{
		PileupType = kInvalid;
	}
}


void
PndEmcPSAFPGAPileupAnalyser::GetEnergyMeasures (Int_t i, Double_t & Amplitude,
		Double_t & Integral)
{
	Amplitude = sampleAmplitude (i);
	Integral = sampleIntegral (i);
}

void
PndEmcPSAFPGAPileupAnalyser::put (float valueToStore)
{

	if (baselineMode == kWindow && local_time <= BaselineStopSample)
	{
		if (local_time >= BaselineStartSample)
		{
			BaselineSum += valueToStore;
		}
		if (local_time == BaselineStopSample)
		{
			baseline_value =
					BaselineSum / (BaselineStopSample - BaselineStartSample + 1);
		}
	}
	else
	{
		float signal = valueToStore - baseline_value;

		if (fVerbose >= 7)
		{
			if (analyserParams.hit_threshold <= 40)
			{
				std::
				cout << "-I- PndEmcPSAFPGAPileupAnalyser::put Low Gain   " <<
				"valueToStore  " << valueToStore << " local time  " <<
				local_time << std::endl;
			}
			else
			{
				std::
				cout << "-I- PndEmcPSAFPGAPileupAnalyser::put High Gain  " <<
				"valueToStore  " << valueToStore << " local time  " <<
				local_time << std::endl;
			}
		}
		float mwd_value = MWD_filter1->put (signal);
		float ma_value = MA_filter1->put (mwd_value);
		float mwd2_value = MWD_filter2->put (signal);
		float cf_value = CF_filter->put (ma_value);
		float cf_value2 =  CF_filter2->put (mwd2_value);
		/* if (analyserParams.hit_threshold<=50) {
	    Ratio.open("Lowgain_fw", std::ofstream::app); if(valueToStore==9999){ Ratio<<"End"<<endl;}else { Ratio<<cf_value2<<endl;}
	   // MWD.open("LowgainMWD+MA_fw", std::ofstream::app); if(valueToStore==9999){ MWD<<"End"<<endl;}else { MWD<<ma_value<<endl;}
           // MWD2.open("LowgainMWD2_fw", std::ofstream::app); if(valueToStore==9999) {MWD2<<"End"<<endl;}else { MWD2<<mwd2_value<<endl;}
                          Ratio.close();//MWD.close();MWD2.close();
		  }
             else {
            Ratio.open("Highgain_fw", std::ofstream::app);  if(valueToStore==9999){ Ratio<<"End"<<endl;}else { Ratio<<cf_value2<<endl;}
	  //  MWD.open("HighgainMWD+MA_fw", std::ofstream::app); if(valueToStore==9999){ MWD<<"End"<<endl;}else { MWD<<ma_value<<endl;}
          // MWD2.open("HighgainMWD2_fw", std::ofstream::app);  if(valueToStore==9999){ MWD2<<"End"<<endl;}else { MWD2<<mwd2_value<<endl;}
            	     Ratio.close();//MWD.close();MWD2.close();
			}*/
		float   cf_value_prev = CF_prev_val;
		float   cf_value_prev2 = CF_prev_val2;
		status = kUndefined;

		//////...................MWD2........................................////
		if (!pulse_detected_mwd2)
		{
			if (mwd2_value > analyserParams.hit_threshold)
			{			// new pulse detected..reset pulse specific quantities
				energy_finished_mwd2 = false;
				//timing_finished_mwd2 = false;
				pulse_detected_mwd2 = true;

				if (fVerbose >= 7)
				{
					std::cout << std::endl;
					std::
					cout << "           [........first time over thres: " <<
					local_time << ".....][.....ThresholdMWD2 = " <<
					analyserParams.
					hit_threshold << "...] " << " [....valueMWD2= " <<
					mwd2_value << "  ...]" << std::endl;
					std::cout << std::endl;
				}
			}
			else
			{

				saved_local_time_mwd2[Number_of_puls_mwd2] = local_time;
				sav_mwd2 = mwd2_value;

			}
		}
		///......................MWD2........................
		if (pulse_detected_mwd2)
		{
			if( (local_time>=analyserParams.cf_delay) && (local_time<5*analyserParams.cf_delay+
					saved_local_time_mwd2[Number_of_puls_mwd2]) ) {
				if((cf_value_prev2 < 0.0) && (cf_value2 >= 0.0)) { 		// new zero crossing                                       
					CF_Fitter->reset();
					CF_Fitter->putPoint(local_time-1, cf_value_prev2); 
					CF_Fitter->putPoint(local_time, cf_value2);
					CF_Fitter->fit();

					double a = CF_Fitter->offset();
					double k = CF_Fitter->slope();
					if (fVerbose >= 7)
					{ 
						std::cout << "Constant Fraction  prev_mwd2  " <<cf_value_prev2<<"  current_mwd2 "<<cf_value2<< std::endl;}
					if(k>0){

						cfZero2[Number_of_puls_mwd2] = -a/k;
					}
				}
			}
			if (mwd2_value < analyserParams.hit_threshold / 4)
			{
				if (fVerbose >= 7)
				{
					std::
					cout << "-I- Start_of_puls_mwd2  " <<
					saved_local_time_mwd2[Number_of_puls_mwd2] << std::endl;
				}
				max_mwd2[Number_of_puls_mwd2]=0;
				t_mwd2[Number_of_puls_mwd2]=0;
				pulse_detected_mwd2 = false;
				int d_mwd2 = 0;
				Square_mwd2 = 0;
				for (int i = 0; i < z_mwd2[Number_of_puls_mwd2]; i++)
				{
					Square_mwd2 =
							signal_buffer_mwd2[i][Number_of_puls_mwd2] + Square_mwd2;
					if (fVerbose >= 7)
					{
						if (analyserParams.hit_threshold <= 40)
						{
							std::
							cout <<
							"-I- PndEmcPSAFPGAPileupAnalyser::put Low Gain   " <<
							"signal_buffer_mwd2[" << i << "] " <<
							signal_buffer_mwd2[i][Number_of_puls_mwd2] << std::
							endl;
						}
						else
						{
							std::
							cout <<
							"-I- PndEmcPSAFPGAPileupAnalyser::put High Gain  " <<
							"signal_buffer_mwd2[" << i << "] " <<
							signal_buffer_mwd2[i][Number_of_puls_mwd2] << std::
							endl;
						}
					}
					if (max_mwd2[Number_of_puls_mwd2] <
							signal_buffer_mwd2[i][Number_of_puls_mwd2])
						max_mwd2[Number_of_puls_mwd2] =
								signal_buffer_mwd2[i][Number_of_puls_mwd2];
				}
				float R_mwd2 = Square_mwd2 / max_mwd2[Number_of_puls_mwd2];
				//std::cout << std::endl;
				t_mwd2[Number_of_puls_mwd2] = cfZero2[Number_of_puls_mwd2];
				for (int i = 1; i <= z_mwd2[Number_of_puls_mwd2]; i++)
				{
					if (t_mwd2[Number_of_puls_mwd2] > 0)
					{
						if (fVerbose >= 7)
						{
							if (analyserParams.hit_threshold <= 40)
							{
								MWD2.open ("LowgainMWD2", std::ofstream::app);
								if (d_mwd2 == 0)
								{
									MWD2 <<
											signal_buffer_mwd2[0][Number_of_puls_mwd2] <<
											endl;
									d_mwd2++;
								}
								if (i != 1)
								{
									MWD2 << signal_buffer_mwd2[i -
															   1]
															   [Number_of_puls_mwd2] << endl;
									if (i == z_mwd2[Number_of_puls_mwd2])
									{
										MWD2 << "End" << R_mwd2 << endl;
									}
								}
								MWD2.close ();
							}
							else
							{
								MWD2.open ("HighgainMWD2", std::ofstream::app);
								if (d_mwd2 == 0)
								{
									MWD2 <<
											signal_buffer_mwd2[0][Number_of_puls_mwd2] <<
											endl;
									d_mwd2++;
								}
								if (i != 1)
								{
									MWD2 << signal_buffer_mwd2[i -
															   1]
															   [Number_of_puls_mwd2] << endl;
									if (i == z_mwd2[Number_of_puls_mwd2])
									{
										MWD2 << "End" << R_mwd2 << endl;
									}
								}
								MWD2.close ();
							}
						}
					}
				}
				for(int i = 0; i < z_mwd2[Number_of_puls_mwd2]; i++)
				{ signal_buffer_mwd2[i][Number_of_puls_mwd2]=0;}
				if (t_mwd2[Number_of_puls_mwd2] > 0)
				{
					//Ratio.open("max_mwd2.txt", std::ofstream::app);
					//Ratio<<max_mwd2<<endl;
					// Ratio.close();
					Number_of_puls_mwd2++;
				}
				z_mwd2[Number_of_puls_mwd2] = 1;
			}

			signal_buffer_mwd2[0][Number_of_puls_mwd2] = sav_mwd2;
			////...............MWD2 Waveform..........................
			if (mwd2_value > analyserParams.hit_threshold / 4)
			{
				signal_buffer_mwd2[z_mwd2[Number_of_puls_mwd2]]
								   [Number_of_puls_mwd2] = mwd2_value;
				if (fVerbose >= 7)
				{
					std::
					cout <<
					"   signal_buffer_mwd2[z_mwd2[Number_of_puls_mwd2]]   " <<
					signal_buffer_mwd2[z_mwd2[Number_of_puls_mwd2]]
									   [Number_of_puls_mwd2] << "   [z_mwd2[Number_of_puls_mwd2]]   "
									   << z_mwd2[Number_of_puls_mwd2] << "   Number_of_puls_mwd2  "
									   << Number_of_puls_mwd2 << std::endl;
				}
				z_mwd2[Number_of_puls_mwd2]++;
			}
		}
		//////////////MWD2END/////////////////////
		//#ifdef KILLMWD
		//////...................MWD........................................////
		if (!pulse_detected_mwd)
		{
			if (ma_value > analyserParams.hit_threshold)
			{			// new pulse detected..reset pulse specific quantities
				energy_finished = false;
				timing_finished = false;
				pulse_detected_mwd = true;
				pileups[Number_of_puls_mwd]=0;
				if (fVerbose >= 7)
				{
					std::cout << std::endl;
					std::
					cout << "           [........first time over thres: " <<
					local_time << ".....][.....ThresholdMWD = " <<
					analyserParams.
					hit_threshold << "...] " << " [....valueMWD= " << ma_value
					<< "  ...]" << std::endl;
					std::cout << std::endl;
				}
			}
			else
			{

				saved_local_time_mwd[Number_of_puls_mwd] = local_time;
				sav_mwd = ma_value;
				status = kBaseline;
			}
		}
		///......................MWD........................
		if (pulse_detected_mwd)
		{
			status = kPulseDetected;
			if( ((local_time>=analyserParams.cf_delay) && (local_time<5*analyserParams.cf_delay+
					saved_local_time_mwd[Number_of_puls_mwd])) && (!timing_finished) ) {
				if((cf_value_prev < 0.0) && (cf_value >= 0.0)) { 		// new zero crossing                                       
					CF_Fitter->reset();
					CF_Fitter->putPoint(local_time-1, cf_value_prev); 
					CF_Fitter->putPoint(local_time, cf_value);
					CF_Fitter->fit();

					double a = CF_Fitter->offset();
					double k = CF_Fitter->slope();
					if (fVerbose >= 7)
					{ 
						std::cout << "Constant Fraction  prev  " <<cf_value_prev<<"  current "<<cf_value<< std::endl;}
					if(k>0){
						//	timing_finished = true;
						cfZero[Number_of_puls_mwd] = -a/k;
					}
				}
			}
			if (ma_value < analyserParams.hit_threshold / 4)
			{
				if (fVerbose >= 7)
				{
					std::
					cout << "-I- Start_of_puls_mwd  " <<
					saved_local_time_mwd[Number_of_puls_mwd] << std::endl;
				}
				max_mwd=0;
				pulse_detected_mwd = false;
				int d_mwd = 0;
				t_mwd=0;
				float Square_mwd = 0;
				for (int i = 0; i < z_mwd[Number_of_puls_mwd]; i++)
				{
					Square_mwd =
							signal_buffer_mwd[i][Number_of_puls_mwd] + Square_mwd;
					if (fVerbose >= 7)
					{
						if (analyserParams.hit_threshold <= 40)
						{
							std::
							cout <<
							"-I- PndEmcPSAFPGAPileupAnalyser::put Low Gain   " <<
							"signal_buffer_mwd[" << i << "] " <<
							signal_buffer_mwd[i][Number_of_puls_mwd] << std::endl;
						}
						else
						{
							std::
							cout <<
							"-I- PndEmcPSAFPGAPileupAnalyser::put High Gain  " <<
							"signal_buffer_mwd[" << i << "] " <<
							signal_buffer_mwd[i][Number_of_puls_mwd] << std::endl;
						}
					}
					if (max_mwd < signal_buffer_mwd[i][Number_of_puls_mwd])
						max_mwd = signal_buffer_mwd[i][Number_of_puls_mwd];
				}
				R_mwd = Square_mwd / max_mwd;
				t_mwd = cfZero[Number_of_puls_mwd];
				if (fVerbose >= 7)
				{
					Ratio.open("Ratio.txt", std::ofstream::app);
					Ratio<<R_mwd<<endl;
					Ratio.close();
				}
				for (int i = 1; i <= z_mwd[Number_of_puls_mwd]; i++)
				{
					if (t_mwd > 0)
					{
						if (fVerbose >= 7)
						{
							if (analyserParams.hit_threshold <= 40)
							{
								MWD.open ("LowgainMWD", std::ofstream::app);
								if (d_mwd == 0)
								{
									MWD << signal_buffer_mwd[0][Number_of_puls_mwd]
																<< endl;
									d_mwd++;
								}
								if (i != 1)
								{
									MWD << signal_buffer_mwd[i -
															 1][Number_of_puls_mwd]
																<< endl;
									if (i == z_mwd[Number_of_puls_mwd])
									{
										MWD << "End" << R_mwd << endl;
									}
								}
								MWD.close ();
							}
							else
							{
								MWD.open ("HighgainMWD", std::ofstream::app);
								if (d_mwd == 0)
								{
									MWD << signal_buffer_mwd[0][Number_of_puls_mwd]
																<< endl;
									d_mwd++;
								}
								if (i != 1)
								{
									MWD << signal_buffer_mwd[i -
															 1][Number_of_puls_mwd]
																<< endl;
									if (i == z_mwd[Number_of_puls_mwd])
									{
										MWD << "End" << R_mwd << endl;
									}
								}
								MWD.close ();
							}
						}
					}
				}
				for(int i = 0; i < z_mwd[Number_of_puls_mwd]; i++)
				{ signal_buffer_mwd[i][Number_of_puls_mwd]=0;}
				if (t_mwd > 0)
				{
					if (fVerbose >= 7)
					{
						std::cout << " Ratio_MA " << R_mwd << std::endl;
					}

					if (R_mwd > 24)
					{
						if (fVerbose >= 7)
						{
							std::
							cout << "-------------Pile-up------------- " << std::
							endl;
						}
						for (int m = Number_of_puls_mwd; m <Number_of_puls_mwd2; m++)
						{
							/*AMPLITUDE*/ amplitude[m] = max_mwd2[m]*0.903;
							/*TIME*/ time[m] = t_mwd2[m];
							/*Pile-up index*/ pileups[m]=2;
							status = kPulseFinished;
							Number_of_puls_mwd++;
						}
						energy_finished = true;
						timing_finished = true;
					}
					else
					{
						/*INTEGRAL*/ integral[Number_of_puls_mwd] = Square_mwd;
						/*AMPLITUDE*/amplitude[Number_of_puls_mwd]=max_mwd*0.903;
						/*TIME*/ time[Number_of_puls_mwd]=t_mwd;
						/*Pile-up index*/ pileups[Number_of_puls_mwd]=1;
						timing_finished = true;
						energy_finished = true;
						Number_of_puls_mwd++;
						Number_of_puls_mwd2=Number_of_puls_mwd;
					}
				}

				z_mwd[Number_of_puls_mwd] = 1;
			}
			signal_buffer_mwd[0][Number_of_puls_mwd] = sav_mwd;
			////...............MWD Waveform..........................
			if (ma_value > analyserParams.hit_threshold / 4)
			{
				signal_buffer_mwd[z_mwd[Number_of_puls_mwd]][Number_of_puls_mwd]
															 = ma_value;
				if (fVerbose >= 7)
				{
					std::
					cout << "   signal_buffer_mwd[z_mwd[Number_of_puls_mwd]]   "
					<<
					signal_buffer_mwd[z_mwd[Number_of_puls_mwd]]
									  [Number_of_puls_mwd] << "   [z_mwd[Number_of_puls_mwd]]   " <<
									  z_mwd[Number_of_puls_mwd] << "   Number_of_puls_mwd  " <<
									  Number_of_puls_mwd << std::endl;
				}
				z_mwd[Number_of_puls_mwd]++;
			}
		}

		if (energy_finished&&timing_finished )
		{
			Number_of_hits = Number_of_puls_mwd;
			/*Just for print */ integral[Number_of_puls_mwd] = Square;
			/*Just for print */ amplitude[Number_of_puls_mwd] = max_mwd;
			/*Just for print */ time[Number_of_puls_mwd] = t_mwd;

			if (fVerbose >= 7)
			{
				std::
				cout << "Finaliyzing " << Number_of_puls_mwd << " hit"
				<< std::endl;
				std::
				cout << "  time:" << time[Number_of_puls_mwd] << std::
				endl;
				std::
				cout << "  amplitude:" <<
				amplitude[Number_of_puls_mwd] << "\t";
				std::
				cout << "  integral: " << integral[Number_of_puls_mwd]
												   << std::endl;
			}
			energy_finished = false;
			status = kPulseFinished;
		}
		CF_prev_val = cf_value;
		CF_prev_val2 = cf_value2;
	}
	local_time++;
}


//}

ClassImp (PndEmcPSAFPGAPileupAnalyser)

