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
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Class to handle tiled sensors
//-------------------------------------------------------------------------

#include "PndDiscSensorGridPhotodetector.h"
#include "PndDiscSensorGrid.h"

#include <TRandom.h>
#include <fstream>

#include <FairLogger.h>

namespace SensorGrid {

struct SensorGridPhotodetector::PixelTraits {
  double efficiency;  // pixel efficiency (additional to pde)
  double noise_rate;  // pixel noise rate
  double time_res_ns; // per pixel time resolution
};

SensorGridPhotodetector::SensorGridPhotodetector(SensorGridBase *sensor_grid_, bool per_pixel_traits_, double const &efficiency_init, double const &noise_rate_init,
                                                 double const &time_res_init)
  : per_pixel_traits(per_pixel_traits_), use_inhomogenity_factor(false), sensor_grid(sensor_grid_)
{
  Init(sensor_grid_, per_pixel_traits_, efficiency_init, noise_rate_init, time_res_init);
}

/// Initialization - can also be used in derived class ctors (avoid code duplication).
void SensorGridPhotodetector::Init(SensorGridBase *sensor_grid_, bool per_pixel_traits_, double const &efficiency_init, double const &noise_rate_init, double const &time_res_init)
{
  sensor_grid = sensor_grid_;
  sensor_grid->LockGrid(true);
  per_pixel_traits = per_pixel_traits_;
  if (per_pixel_traits) {
    pixel_traits = new PixelTraits[sensor_grid->GetNumberOfPixels()];
    // if(pixel_traits == nullptr)
    //    throw ...
    for (int i = 0; i < sensor_grid->GetNumberOfPixels(); ++i) {
      pixel_traits[i].efficiency = efficiency_init;
      pixel_traits[i].noise_rate = noise_rate_init;
      pixel_traits[i].time_res_ns = time_res_init;
    }
  } else {
    pixel_traits = new PixelTraits();
    pixel_traits->efficiency = efficiency_init;
    pixel_traits->noise_rate = noise_rate_init;
    pixel_traits->time_res_ns = time_res_init;
  }
}

//#define DETECT_DEBUG

#ifdef DETECT_DEBUG
std::ofstream ofs_detect_debug("detect_debug.txt");
#endif

/// Detection of a hit
int SensorGridPhotodetector::Detect(double const &hit_pos_x, double const &hit_pos_y, double const &hit_time_ns, double const &wavelength_nm, PixelInfo &pixel_info,
                                    double &smeared_time_ns) const
{
#ifdef DETECT_DEBUG
  ofs_detect_debug << "X: " << hit_pos_x << " Y: " << hit_pos_y;
#endif
  if (!sensor_grid->PositionToPixel(hit_pos_x, hit_pos_y, pixel_info)) {
#ifdef DETECT_DEBUG
    ofs_detect_debug << " not detected\n";
#endif
    return -1; // no pixel (geometric efficiency)
  }
#ifdef DETECT_DEBUG
  ofs_detect_debug << "Pixel: " << pixel_info.pixel_number << " Grid: " << pixel_info.grid_id << " Column/row: " << pixel_info.column_on_grid << " / " << pixel_info.row_on_grid
                   << std::endl;
#endif

  double global_pde = GetPDE(wavelength_nm);
  if (use_inhomogenity_factor)
    global_pde *= GetInhomegenityFactor(hit_pos_x, hit_pos_y);

  if (per_pixel_traits) {
    if (gRandom->Uniform() > pixel_traits[pixel_info.pixel_number].efficiency * global_pde)
      return -1; // global PDE and per pixel efficiency (e.g. due to bad gain uniformity)
    smeared_time_ns = gRandom->Gaus(hit_time_ns, pixel_traits[pixel_info.pixel_number].time_res_ns);
  } else {
    if (gRandom->Uniform() > pixel_traits->efficiency * global_pde)
      return -1;
    smeared_time_ns = gRandom->Gaus(hit_time_ns, pixel_traits->time_res_ns);
  }
  return pixel_info.pixel_number;
}

/// Generate noise on the sensor
void SensorGridPhotodetector::GenerateNoise(double const &time_start_ns, double const &time_window_ns, std::vector<std::pair<int, double>> &hits) const
{
  int i, n_hits, n_pixels = sensor_grid->GetNumberOfPixels();
  double avg_n_hits;
  std::pair<int, double> hit;

  // loop over all pixel numbers
  for (hit.first = 0; hit.first < n_pixels; ++hit.first) {
    // average number of noise events per pixel:
    if (per_pixel_traits)
      avg_n_hits = pixel_traits[hit.first].noise_rate * time_window_ns * 1E-9;
    else
      avg_n_hits = pixel_traits->noise_rate * time_window_ns * 1E-9;

    n_hits = gRandom->Poisson(avg_n_hits); // compute number of hits on this pixel
    // generate the hits in the given time window:
    for (i = 0; i < n_hits; ++i) {
      hit.second = time_start_ns + gRandom->Uniform(time_window_ns); // distribute hit times in window
      hits.push_back(hit);                                           // append hits to vector
    }
  }
}

/// Reinit noise iterator and return number of noise hits.
int SensorGridPhotodetector::GenerateNoise(double const &time_start_ns, double const &time_window_ns)
{
  noisegen_pixel_number = -1;
  noisegen_n_hits = 0;
  noisegen_current_hit = 0;
  noisegen_time_start_ns = time_start_ns;
  noisegen_time_window_ns = time_window_ns;

  return noisegen_n_hits;
}

/// Noise hit generator function which has to be called after GenerateNoise() to retrieve the noise hits.
bool SensorGridPhotodetector::GetNoiseHit(int &pixel_number, double &hit_time_ns, double &smeared_time_ns)
{
  if (noisegen_pixel_number >= sensor_grid->GetNumberOfPixels())
    return false;

  // after iterating over all hits, switch to next pixel.
  if (noisegen_current_hit >= noisegen_n_hits) {
    do {
      // next pixel:
      noisegen_pixel_number++;
      if (noisegen_pixel_number >= sensor_grid->GetNumberOfPixels())
        return false;

      // average number of noise events on this pixel:
      double avg_n_hits = 0.0;
      if (per_pixel_traits) {
        avg_n_hits = pixel_traits[pixel_number].noise_rate * noisegen_time_window_ns * 1E-9;
        noisegen_current_time_sigma = pixel_traits[pixel_number].time_res_ns;
      } else {
        avg_n_hits = pixel_traits->noise_rate * noisegen_time_window_ns * 1E-9;
        noisegen_current_time_sigma = pixel_traits->time_res_ns;
      }

      // number of hits on this pixel:
      noisegen_n_hits = gRandom->Poisson(avg_n_hits); // compute number of hits on this pixel
    } while (noisegen_n_hits == 0);

    noisegen_current_hit = 0;
    LOG(info) << Form("Number of generated hits on pixel %d are: %d\n", noisegen_pixel_number, noisegen_n_hits);
  }

  pixel_number = noisegen_pixel_number;
  hit_time_ns = noisegen_time_start_ns + gRandom->Uniform(noisegen_time_window_ns); // distribute hit times in window
  smeared_time_ns = gRandom->Gaus(hit_time_ns, noisegen_current_time_sigma);
  noisegen_current_hit++;

  return true;
}

/// Can be used to smear the time of noise entries. Noise is returned with
/// its true time stamp for dead-time handling and an additional smeared time
/// would increase the array, so it appears to me that it is more handy to
/// smear afterwards. A better way might be a noise iterator which generates
/// one hit at a time (-> implemented, will be tested).
double SensorGridPhotodetector::GetSmearedTime(double const &hit_time_ns, PixelInfo const &pixel_info) const
{
  double smeared_time_ns;
  if (per_pixel_traits) {
    smeared_time_ns = gRandom->Gaus(hit_time_ns, pixel_traits[pixel_info.pixel_number].time_res_ns);
  } else {
    smeared_time_ns = gRandom->Gaus(hit_time_ns, pixel_traits->time_res_ns);
  }
  return smeared_time_ns;
}

/// Will set the DCR for all pixels to dcr_Hz.
/// The current implementation does not provide the functionality
/// to model statistical fluctuations in the DCR.
void SensorGridPhotodetector::SetDCR(double const &dcr_Hz)
{
  if (per_pixel_traits) {
    int n_pixels = sensor_grid->GetNumberOfPixels();
    for (int i = 0; i < n_pixels; ++i) {
      pixel_traits[i].noise_rate = dcr_Hz;
    }
  } else
    pixel_traits->noise_rate = dcr_Hz;
}

} // namespace SensorGrid
