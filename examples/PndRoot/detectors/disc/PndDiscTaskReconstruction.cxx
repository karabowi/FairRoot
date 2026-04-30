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
// Description: Track Reconstruction
//-------------------------------------------------------------------------

#include "PndDiscTaskReconstruction.h"
#include "PndDiscDigitizedHit.h"
#include "PndDiscParticleMCPoint.h"
#include "PndDiscSensorMCPoint.h"
#include "PndDiscReconResult.h"

#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairEventHeader.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TString.h"
#include "TRandom.h"
#include "TMath.h"

#include <cstdlib>

double pos_fel_x[108] = {3.200016,    8.995571,   14.791126,  20.586681,  26.382236,  32.17779,    37.973345,   43.7689,     49.564455,   55.109197,   59.355735,   63.602272,
                         67.84881,    72.095348,  76.341886,  80.588423,  84.834961,  89.081499,   92.318168,   93.869134,   95.420099,   96.971064,   98.52203,    100.072995,
                         101.623961,  103.174926, 104.725891, 104.742558, 103.189644, 101.63673,   100.083816,  98.530901,   96.977987,   95.425073,   93.872158,   92.319244,
                         89.121382,   84.882642,  80.643902,  76.405162,  72.166422,  67.927682,   63.688942,   59.450202,   55.211462,   49.635225,   43.839148,   38.043072,
                         32.246995,   26.450918,  20.654841,  14.858764,  9.062688,   3.266611,    -3.200016,   -8.995571,   -14.791126,  -20.586681,  -26.382236,  -32.17779,
                         -37.973345,  -43.7689,   -49.564455, -55.109197, -59.355735, -63.602272,  -67.84881,   -72.095348,  -76.341886,  -80.588423,  -84.834961,  -89.081499,
                         -92.318168,  -93.869134, -95.420099, -96.971064, -98.52203,  -100.072995, -101.623961, -103.174926, -104.725891, -104.742558, -103.189644, -101.63673,
                         -100.083816, -98.530901, -96.977987, -95.425073, -93.872158, -92.319244,  -89.121382,  -84.882642,  -80.643902,  -76.405162,  -72.166422,  -67.927682,
                         -63.688942,  -59.450202, -55.211462, -49.635225, -43.839148, -38.043072,  -32.246995,  -26.450918,  -20.654841,  -14.858764,  -9.062688,   -3.266611};

double pos_fel_y[108] = {104.742558, 103.189644, 101.63673,   100.083816,  98.530901,   96.977987,   95.425073,   93.872158,   92.319244,  89.121382,   84.882642,  80.643902,
                         76.405162,  72.166422,  67.927682,   63.688942,   59.450202,   55.211462,   49.635225,   43.839148,   38.043072,  32.246995,   26.450918,  20.654841,
                         14.858764,  9.062688,   3.266611,    -3.200016,   -8.995571,   -14.791126,  -20.586681,  -26.382236,  -32.17779,  -37.973345,  -43.7689,   -49.564455,
                         -55.109197, -59.355735, -63.602272,  -67.84881,   -72.095348,  -76.341886,  -80.588423,  -84.834961,  -89.081499, -92.318168,  -93.869134, -95.420099,
                         -96.971064, -98.52203,  -100.072995, -101.623961, -103.174926, -104.725891, -104.742558, -103.189644, -101.63673, -100.083816, -98.530901, -96.977987,
                         -95.425073, -93.872158, -92.319244,  -89.121382,  -84.882642,  -80.643902,  -76.405162,  -72.166422,  -67.927682, -63.688942,  -59.450202, -55.211462,
                         -49.635225, -43.839148, -38.043072,  -32.246995,  -26.450918,  -20.654841,  -14.858764,  -9.062688,   -3.266611,  3.200016,    8.995571,   14.791126,
                         20.586681,  26.382236,  32.17779,    37.973345,   43.7689,     49.564455,   55.109197,   59.355735,   63.602272,  67.84881,    72.095348,  76.341886,
                         80.588423,  84.834961,  89.081499,   92.318168,   93.869134,   95.420099,   96.971064,   98.52203,    100.072995, 101.623961,  103.174926, 104.725891};

// Angles of tilted Sensors
double angle_sensor[108] = {
  75,     75,     75,     75,     75,     75,     75,     75,     75,     45,     45,     45,     45,     45,     45,     45,     45,     45,     14.98,  14.98,  14.98,  14.98,
  14.98,  14.98,  14.98,  14.98,  14.98,  345,    345,    345,    345,    345,    345,    345,    345,    345,    315,    315,    315,    315,    315,    315,    315,    315,
  315,    284.98, 284.98, 284.98, 284.98, 284.98, 284.98, 284.98, 284.98, 284.98, 255,    255,    255,    255,    255,    255,    255,    255,    255,    225.05, 225.05, 225.05,
  225.05, 225.05, 225.05, 225.05, 225.05, 225.05, 194.98, 194.98, 194.98, 194.98, 194.98, 194.98, 194.98, 194.98, 194.98, 165,    165,    165,    165,    165,    165,    165,
  165,    165,    135.05, 135.05, 135.05, 135.05, 135.05, 135.05, 135.05, 135.05, 135.05, 104.98, 104.98, 104.98, 104.98, 104.98, 104.98, 104.98, 104.98, 104.98,
};

// Rest masses of pions, kaons and protons in MeV
double E0[3] = {134.976, 493.677, 938.272};

//---------------------------------------------------------------------------

double PndDiscTaskReconstruction::gauss(int x, double mean_value, double rms)
{
  return 1 / sqrt(2 * TMath::Pi() * rms * rms) * exp(-0.5 * ((x - mean_value) / rms) * ((x - mean_value) / rms));
}

//---------------------------------------------------------------------------

double PndDiscTaskReconstruction::mean(std::vector<double> values)
{
  double average = 0;

  for (size_t i = 0; i < values.size(); i++) {
    average += values[i];
  }

  return average / double(values.size());
}

//---------------------------------------------------------------------------

double PndDiscTaskReconstruction::deviation(double mean_value, std::vector<double> values)
{
  double average = 0;

  for (size_t i = 0; i < values.size(); i++) {
    average += (values[i] - mean_value) * (values[i] - mean_value);
  }

  average = sqrt(1 / (double(values.size()) - 1) * average);

  return average;
}

//---------------------------------------------------------------------------

PndDiscTaskReconstruction::PndDiscTaskReconstruction()
  : PndPersistencyTask("DiscDircTaskReconstruction"), tclarr_digits(nullptr), tclarr_particles(nullptr), tclarr_tracks(nullptr), is_time_based(kTRUE), flag_export_patterns(kFALSE),
    average_wl(420.), minimum_wl(385.)
{
  branch_name_digits = "DiscDigit";
  folder_name_digits = "DiscDIRC";
}

//---------------------------------------------------------------------------

PndDiscTaskReconstruction::~PndDiscTaskReconstruction() {}

//--------------------------------------------------------------------------

InitStatus PndDiscTaskReconstruction::ReInit()
{
  LOG(info) << "DiscDircTaskReconstruction::ReInit()";
  return kSUCCESS;
}

//--------------------------------------------------------------------------

InitStatus PndDiscTaskReconstruction::Init()
{
  LOG(info) << "PndDiscTaskReconstruction::Init()";

  // Get IO manager instance:
  FairRootManager *io_manager = FairRootManager::Instance();
  if (!io_manager) {
    LOG(fatal) << "FairRootManager instance is nullptr !!!";
    return kFATAL;
  }

  // Get branch with digitized hits:
  tclarr_digits = (TClonesArray *)io_manager->GetObject(branch_name_digits);
  if (!tclarr_digits) {
    LOG(error) << "Branch " << branch_name_digits.Data() << " is not accessible through FairRootManager.";
    return kERROR;
  }

  tclarr_particles = (TClonesArray *)io_manager->GetObject("DiscMCTruthTracks");
  if (!tclarr_particles) {
    LOG(error) << "No DiscDIRC_ParticleMCPoint collection registered with FairRootManager";
    return kFATAL;
  }

  tclarr_recon_results = new TClonesArray("PndDiscReconResult");
  tclarr_digits_out = new TClonesArray("PndDiscDigitizedHit");
  tclarr_particles_out = new TClonesArray("PndDiscParticleMCPoint");

  io_manager->Register("DiscPatternPrediction", "DiscDircDetector", tclarr_recon_results, GetPersistency());
  io_manager->Register("DiscDigitizedHit", "DiscDircDetector", tclarr_digits_out, GetPersistency());
  io_manager->Register("DiscRealTracks", "DiscDircDetector", tclarr_particles_out, GetPersistency());

  static bool recon_initialized = false;
  if (recon_initialized)
    return kSUCCESS;

  return kSUCCESS;
}

//----------------------------------------------------------------------

/// Use particle track array as driving quantity
/// and retrieve pattern digits by using time-based-simulation.
void PndDiscTaskReconstruction::Exec(Option_t *)
{
  // Reading out entries
  int particles = tclarr_particles->GetEntries();
  int entries = tclarr_digits->GetEntries();

  // time window around track time:
  // const double time_window_t1 =  -5.0; //[R.K. 01/2017] unused variable?
  // const double time_window_t2 =  40.0; //[R.K. 01/2017] unused variable?

  // Information of primary particle
  std::vector<double> x0, y0, px, py, pz, p;
  std::vector<double> theta, phi;
  std::vector<double> t;

  std::vector<std::vector<double>> mean_time;
  std::vector<std::vector<double>> deviation_time;

  // double average; //[R.K. 01/2017] unused variable?

  //-----------------------------------------------------------------------
  // Reading out primary particle information
  //-----------------------------------------------------------------------

  for (int i = 0; i < particles; i++) {
    PndDiscParticleMCPoint *particle_mc_point = (PndDiscParticleMCPoint *)tclarr_particles->At(i);

    x0.push_back(particle_mc_point->GetX());
    y0.push_back(particle_mc_point->GetY());

    px.push_back(particle_mc_point->GetPx());
    py.push_back(particle_mc_point->GetPy());
    pz.push_back(particle_mc_point->GetPz());

    p.push_back(sqrt(px[i] * px[i] + py[i] * py[i] + pz[i] * pz[i]));

    t.push_back(particle_mc_point->GetTime());

    theta.push_back(acos(pz[i] / sqrt(px[i] * px[i] + py[i] * py[i] + pz[i] * pz[i])));

    std::cout << "Primary particles:" << std::endl;
    std::cout << "Particle pos: x = " << x0[i] << std::endl;
    std::cout << "Particle pos: y = " << y0[i] << std::endl;
    std::cout << "Particle mom: px = " << px[i] << std::endl;
    std::cout << "Particle mom: py = " << py[i] << std::endl;
    std::cout << "Particle mom: pz = " << pz[i] << std::endl;
    std::cout << "Particle abs mom: p = " << p[i] << std::endl;
    std::cout << "Particle time: t = " << t[i] << std::endl;
    std::cout << "Particle angle: theta[deg] = " << theta[i] * 180 / TMath::Pi() << std::endl;
    std::cout << std::endl;
  }

  //-------------------------------------------------------------------------
  // Caculating theoretical hit pattern for pion, kaon, proton
  //-------------------------------------------------------------------------

  PndDiscReconResult *recon_result = new PndDiscReconResult();

  for (int i = 0; i < particles; i++) {
    for (int j = 0; j < 3; j++) {
      std::cout << "Particle: " << j << std::endl;
      for (int k = 0; k < 108; k++) {
        double E = sqrt(p[i] * 1000 * p[i] * 1000 + E0[j] * E0[j]);
        double gamma = E / E0[j];
        double beta = sqrt(1 - 1 / (gamma * gamma));
        double n = 1.47;
        double cherenkov = acos(1 / (n * beta));

        double dx = pos_fel_x[k] - x0[i];
        double dy = pos_fel_y[k] - y0[i];

        double phirel = acos((dx * px[i] + dy * py[i]) / (sqrt(dx * dx + dy * dy) * sqrt(px[i] * px[i] + py[i] * py[i])));

        double angle;

        if (div(k, 27).quot == 0 || div(k, 27).quot == 3) {
          angle = angle_sensor[k] * TMath::Pi() / 180;
        }
        if (div(k, 27).quot == 1 || div(k, 27).quot == 2) {
          angle = 2 * TMath::Pi() - angle_sensor[k] * TMath::Pi() / 180;
        }

        double alpha = acos(dx / (sqrt(dx * dx + dy * dy))) - angle;

        double delta = sin(theta[i]) * cos(phirel);
        double lambda = delta * delta + cos(theta[i]) * cos(theta[i]);
        double varphi = acos(delta * cos(cherenkov) / lambda + sqrt((cos(theta[i]) * cos(theta[i]) - cos(cherenkov) * cos(cherenkov)) / lambda +
                                                                    (delta * cos(cherenkov) / lambda) * (delta * cos(cherenkov) / lambda)));
        double varphidash = atan(tan(varphi) / cos(alpha));

        int pixel = (TMath::Pi() / 2 - varphidash - 0.8530931655) / 0.0035321867;

        double time = (sqrt(dx * dx + dy * dy)) / 100 / 3e8 * n / cos(varphidash) * 1e9;

        // Results are only valid for pixels between 0 and 100
        if (pixel >= 0 && pixel <= 100) {

          std::cout << "Sensor: " << k << " Pixel: " << pixel << " Time: " << time << std::endl;

          recon_result->hypothesis = j;
          recon_result->sensor = k;
          recon_result->pixel = pixel;
          recon_result->time = time;
          recon_result->particle = i;

          new ((*tclarr_recon_results)[tclarr_recon_results->GetEntriesFast()]) PndDiscReconResult(*recon_result);
        } else {

          recon_result->hypothesis = j;
          recon_result->sensor = k;
          recon_result->pixel = -1;
          recon_result->time = time;
          recon_result->particle = i;

          new ((*tclarr_recon_results)[tclarr_recon_results->GetEntriesFast()]) PndDiscReconResult(*recon_result);
        }
      }
      std::cout << std::endl;
    }
  }

  //-------------------------------------------------------------------------
  // Copying digitized hits and particle information
  //-------------------------------------------------------------------------

  for (int i = 0; i < entries; i++) {
    PndDiscDigitizedHit *digits = (PndDiscDigitizedHit *)tclarr_digits->At(i);
    digits = new ((*tclarr_digits_out)[tclarr_digits_out->GetEntriesFast()]) PndDiscDigitizedHit(*digits);
  }

  for (int i = 0; i < particles; i++) {
    PndDiscParticleMCPoint *particles_out = (PndDiscParticleMCPoint *)tclarr_particles->At(i);
    particles_out = new ((*tclarr_particles_out)[tclarr_particles_out->GetEntriesFast()]) PndDiscParticleMCPoint(*particles_out);
  }

  // Reading out hit information and calculating Cherenkov angle
  for (int i = 0; i < entries; i++) {
    PndDiscDigitizedHit *sensor_mc_point = (PndDiscDigitizedHit *)tclarr_digits->At(i);

    int detector_id = sensor_mc_point->GetDetectorID();
    int sensor_id = 27 * detector_id + sensor_mc_point->GetReadoutID(); // Sensor ID of hit
    int pixel = sensor_mc_point->GetPixelNumber();                      // Pixel number of hit

    double time = sensor_mc_point->GetTdcTime();

    // Difference vector between particle and sensor element
    // double dx = pos_fel_x[sensor_id] - x0[0];
    // double dy = pos_fel_y[sensor_id] - y0[0];

    // double angle = 0; //[R.K. 01/2017] unused variable

    // if(detector_id == 0 || detector_id == 3) //[R.K. 01/2017] unused variable
    //{
    // angle = angle_sensor[sensor_id]*TMath::Pi()/180; //[R.K. 01/2017] unused variable
    //}
    // if(detector_id == 1 || detector_id == 2) //[R.K. 01/2017] unused variable
    //{
    // angle = 2*TMath::Pi() - angle_sensor[sensor_id]*TMath::Pi()/180; //[R.K. 01/2017] unused variable
    //}

    // double phirel = acos((dx*px[0]+dy*py[0])/(sqrt(dx*dx+dy*dy)*sqrt(px[0]*px[0]+py[0]*py[0])));
    // double alpha = acos(dx/(sqrt(dx*dx+dy*dy))) - angle;
    // double varphidash = TMath::Pi()/2-(0.00347196*pixel + 0.852934);
    // double varphi = atan(tan(varphidash)*cos(alpha));

    // double cherenkov = acos(sin(theta[0])*cos(phirel)*cos(varphi)+cos(theta[0])*sin(varphi)); //Reconstructed Cherenkov angle

    std::cout << "Hit informations:" << std::endl;
    std::cout << "TDC Time [ns]: " << time << std::endl;
    std::cout << "Detector ID: " << detector_id << std::endl;
    std::cout << "Sensor ID: " << sensor_id << std::endl;
    std::cout << "Sensor coor: x = " << pos_fel_x[sensor_id] << ", y = " << pos_fel_y[sensor_id] << std::endl;
    std::cout << "Pixel no: " << pixel << std::endl;
    std::cout << std::endl;
  }

  std::cout << "DiscDIRC_TaskReconstruction::Exec\n";
}

void PndDiscTaskReconstruction::FinishEvent()
{
  // called after all Tasks did their Exec() and the data is copied to the file
  if (tclarr_recon_results != nullptr)
    tclarr_recon_results->Clear();
  if (tclarr_digits_out != nullptr)
    tclarr_digits_out->Clear();
  if (tclarr_particles_out != nullptr)
    tclarr_particles_out->Clear();

  FinishEvents();
}

void PndDiscTaskReconstruction::FinishTask() {}

ClassImp(PndDiscTaskReconstruction)
