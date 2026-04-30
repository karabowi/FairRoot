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

//-------------------------------------------------------------------------
// Author:      Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Changes:
// Date:        30.11.2015
// Description: Particle Identification
//-------------------------------------------------------------------------

#include "PndDiscTaskReconstruction.h"
#include "PndDiscDigitizedHit.h"
#include "PndDiscParticleMCPoint.h"
#include "PndDiscSensorMCPoint.h"
#include "PndDiscReconResult.h"
#include "PndDiscTaskPID.h"
#include "PndDiscPID.h"

#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairEventHeader.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TString.h"
#include "TRandom.h"
#include "TMath.h"

#include <cstdlib>

double PndDiscTaskPID::gauss(int x, double mean_value, double rms)
{
  return 1 / sqrt(2 * TMath::Pi() * rms * rms) * exp(-0.5 * ((x - mean_value) / rms) * ((x - mean_value) / rms));
}

//---------------------------------------------------------------------------

double PndDiscTaskPID::mean(std::vector<double> values)
{
  double average = 0;

  for (size_t i = 0; i < values.size(); i++) {
    average += values[i];
  }

  return average / double(values.size());
}

//---------------------------------------------------------------------------

double PndDiscTaskPID::deviation(double mean_value, std::vector<double> values)
{
  double average = 0;

  for (size_t i = 0; i < values.size(); i++) {
    average += (values[i] - mean_value) * (values[i] - mean_value);
  }

  average = sqrt(1 / (double(values.size()) - 1) * average);

  return average;
}

//---------------------------------------------------------------------------

PndDiscTaskPID::PndDiscTaskPID() : PndPersistencyTask("PndDiscTaskPID")
{
  branch_name_digits = "DiscPatternPrediction";
  folder_name_digits = "DiscDIRC";
}

//---------------------------------------------------------------------------

PndDiscTaskPID::~PndDiscTaskPID() {}

//--------------------------------------------------------------------------

InitStatus PndDiscTaskPID::ReInit()
{
  LOG(info) << "PndDiscTaskPID::ReInit()";
  return kSUCCESS;
}

//--------------------------------------------------------------------------

InitStatus PndDiscTaskPID::Init()
{
  LOG(info) << "PndDiscTaskPID::Init()";

  // Get IO manager instance
  FairRootManager *io_manager = FairRootManager::Instance();
  if (!io_manager) {
    LOG(fatal) << "FairRootManager instance is nullptr !!!";
    return kFATAL;
  }

  // Get branch with digitized hits
  tclarr_digits = (TClonesArray *)io_manager->GetObject("DiscDigitizedHit");
  if (!tclarr_digits) {
    LOG(error) << "Branch " << branch_name_digits.Data() << " is not accessible through FairRootManager.";
    return kERROR;
  }

  // Get branch with hit pattern prediction
  tclarr_recon_results = (TClonesArray *)io_manager->GetObject("DiscPatternPrediction");
  if (!tclarr_recon_results) {
    LOG(error) << "No DiscPatternPrediction collection registered with FairRootManager";
    return kFATAL;
  }

  // Get branch with primary particle information
  tclarr_particles_in = (TClonesArray *)io_manager->GetObject("DiscRealTracks");
  if (!tclarr_particles_in) {
    LOG(error) << "No DiscRealTracks collection registered with FairRootManager";
    return kFATAL;
  }

  tclarr_pid_results = new TClonesArray("PndDiscPID");

  io_manager->Register("DiscPID", "DiscDIRC_Detector", tclarr_pid_results, GetPersistency());

  static bool recon_initialized = false;
  if (recon_initialized)
    return kSUCCESS;

  return kSUCCESS;
}

//----------------------------------------------------------------------

void PndDiscTaskPID::Exec(Option_t *)
{
  // Reading out entries
  int particles = tclarr_particles_in->GetEntries();
  int entries = tclarr_recon_results->GetEntries();
  int digits = tclarr_digits->GetEntries();

  std::cout << "Number of particles: " << particles << std::endl;

  // Information of theoretical hit pattern
  std::vector<std::vector<std::vector<int>>> pixel_prediction;   // Predicted hit pattern
  std::vector<std::vector<std::vector<double>>> time_prediction; // Predicted time of hit
  std::vector<std::vector<std::vector<double>>> dtime;           // Time difference

  // Information of simulated hit pattern
  std::vector<std::vector<int>> pixel_hit;
  std::vector<std::vector<double>> tdc_time;

  // Information of primary particle
  std::vector<double> x0, y0, px, py, pz, p;
  std::vector<double> theta, phi;
  std::vector<double> t;

  std::vector<std::vector<double>> mean_time;
  std::vector<std::vector<double>> deviation_time;

  // Creating matrix for hitpattern
  for (int nparticle = 0; nparticle < particles; nparticle++) {
    pixel_prediction.push_back(std::vector<std::vector<int>>());
    time_prediction.push_back(std::vector<std::vector<double>>());
    dtime.push_back(std::vector<std::vector<double>>());
    mean_time.push_back(std::vector<double>());
    deviation_time.push_back(std::vector<double>());
    pixel_hit.push_back(std::vector<int>());
    tdc_time.push_back(std::vector<double>());

    for (int nhypo = 0; nhypo < 3; nhypo++) {
      pixel_prediction[nparticle].push_back(std::vector<int>());
      time_prediction[nparticle].push_back(std::vector<double>());
      dtime[nparticle].push_back(std::vector<double>());
    }
  }

  for (int i = 0; i < particles; i++) {
    PndDiscParticleMCPoint *particle_mc_point = (PndDiscParticleMCPoint *)tclarr_particles_in->At(i);

    double time = particle_mc_point->GetTime();

    t.push_back(time);
  }

  for (int i = 0; i < entries; i++) {
    PndDiscReconResult *recon_result = (PndDiscReconResult *)tclarr_recon_results->At(i);

    int particle = recon_result->particle;
    int hypothesis = recon_result->hypothesis;
    int pixel = recon_result->pixel;
    double time = recon_result->time;

    pixel_prediction[particle][hypothesis].push_back(pixel);
    time_prediction[particle][hypothesis].push_back(time);
  }

  //-------------------------------------------------------------------------
  // Difference between predicted and simulated time
  //-------------------------------------------------------------------------

  for (int i = 0; i < particles; i++) {
    for (int j = 0; j < digits; j++) {
      PndDiscDigitizedHit *sensor_mc_point = (PndDiscDigitizedHit *)tclarr_digits->At(j);

      int detector_id = sensor_mc_point->GetDetectorID();
      int sensor_id = 27 * detector_id + sensor_mc_point->GetReadoutID(); // Sensor ID of hit
      // int pixel = sensor_mc_point->GetPixelNumber(); // Pixel number of hit //[R.K. 01/2017] unused variable?
      double tdc = sensor_mc_point->GetTdcTime();

      for (int k = 0; k < 3; k++) {
        double propagation = tdc - t[i];                            // Calculation of photon propagation time
        double dt = propagation - time_prediction[i][k][sensor_id]; // Calculation of time difference
        if (dt < 3 && dt > -3) {
          dtime[i][k].push_back(dt);
        } else {
          dtime[i][k].push_back(-1);
        }
      }
    }

    // Calculating mean of time difference and standard deviation
    for (int k = 0; k < 3; k++) {
      mean_time[i].push_back(mean(dtime[i][k]));
      deviation_time[i].push_back(deviation(mean_time[i][k], dtime[i][k]));
      std::cout << "Mean time difference: " << mean_time[i][k] << ", deviation: " << deviation_time[i][k] << std::endl;
    }
    std::cout << std::endl;
  }

  PndDiscPID *pid_result = new PndDiscPID();

  for (int i = 0; i < particles; i++) {
    double prob[3] = {0};

    for (int j = 0; j < digits; j++) {
      PndDiscDigitizedHit *sensor_mc_point = (PndDiscDigitizedHit *)tclarr_digits->At(j);

      int detector_id = sensor_mc_point->GetDetectorID();
      int sensor_id = 27 * detector_id + sensor_mc_point->GetReadoutID(); // Sensor ID of hit
      int pixel = sensor_mc_point->GetPixelNumber();                      // Pixel number of hit
      // double tdc = sensor_mc_point->GetTdcTime(); //[R.K. 01/2017] unused variable?

      for (int k = 0; k < 3; k++) {
        // std::cout << "Time differences: " << tdc - t[i] - time_prediction[i][k][sensor_id] - mean_time[i][k] << std::endl;
        // std::cout << "Pixel difference: " << pixel_prediction[i][k][sensor_id] - pixel << std::endl;

        if (pixel_prediction[i][0][sensor_id] != -1 && dtime[i][0][j] != -1 && pixel_prediction[i][1][sensor_id] != -1 && dtime[i][1][j] != -1 &&
            pixel_prediction[i][2][sensor_id] != -1 && dtime[i][2][j] != -1) {
          if ((pixel - pixel_prediction[i][0][sensor_id] < 5 && pixel - pixel_prediction[i][0][sensor_id] > -5) ||
              (pixel - pixel_prediction[i][1][sensor_id] < 5 && pixel - pixel_prediction[i][1][sensor_id] > -5) ||
              (pixel - pixel_prediction[i][2][sensor_id] < 5 && pixel - pixel_prediction[i][2][sensor_id] > -5)) {
            // double propagation = tdc - t[i]; //Calculation of photon propagation time //[R.K. 01/2017] unused variable
            // double dt = propagation - time_prediction[i][k][sensor_id]; //Calculation of time difference //[R.K. 01/2017] unused variable?
            prob[k] += log(gauss(pixel, pixel_prediction[i][k][sensor_id], 4)); // Likelihood calculation
          }
        }
      }
    }

    std::cout << std::endl;
    std::cout << "Log Likelihood: " << std::endl;
    std::cout << "Pion: " << (prob[0]) << std::endl;
    std::cout << "Kaon: " << (prob[1]) << std::endl;
    std::cout << "Proton: " << (prob[2]) << std::endl << std::endl;

    pid_result->loglikepion = prob[0];
    pid_result->loglikekaon = prob[1];
    pid_result->loglikeproton = prob[2];

    // Save identified particle

    if (prob[0] < prob[1] && prob[0] < prob[2]) {
      pid_result->pion = 1;
      pid_result->kaon = 0;
      pid_result->proton = 0;
      new ((*tclarr_pid_results)[tclarr_pid_results->GetEntriesFast()]) PndDiscPID(*pid_result);
    }
    if (prob[1] < prob[0] && prob[1] < prob[2]) {
      pid_result->pion = 0;
      pid_result->kaon = 1;
      pid_result->proton = 0;
      new ((*tclarr_pid_results)[tclarr_pid_results->GetEntriesFast()]) PndDiscPID(*pid_result);
    }
    if (prob[2] < prob[0] && prob[2] < prob[1]) {
      pid_result->pion = 0;
      pid_result->kaon = 0;
      pid_result->proton = 1;
      new ((*tclarr_pid_results)[tclarr_pid_results->GetEntriesFast()]) PndDiscPID(*pid_result);
    }
  }

  std::cout << "PndDiscTaskPID::Exec" << std::endl;
}

void PndDiscTaskPID::FinishEvent()
{
  // called after all Tasks did their Exec() and the data is copied to the file

  if (tclarr_pid_results != nullptr)
    tclarr_pid_results->Clear();

  FinishEvents();
}

void PndDiscTaskPID::FinishTask() {}

ClassImp(PndDiscTaskPID)
