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

//------------------------------------------------------------------------
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:	Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Class to emulate noise for the readout electronics
//------------------------------------------------------------------------

#include "PndDiscNoiseGeneration.h"
#include "PndDiscDigitizedHit.h"

#include "TString.h"

#include "TGeoManager.h"
#include "TRandom.h"

#include <cstdlib>

PndDiscNoiseGeneration::PndDiscNoiseGeneration()
  : current_index(0), n_detectors(0), n_readout_modules(0), n_sensors(0), n_sensor_tiles_x(1), n_sensor_tiles_y(1), half_sensor_width(1.0), half_sensor_height(1.0), n_pixel_x(1),
    n_pixel_y(1), pitch_x(1.0), pitch_y(1.0), dcr_per_pixel(0.0)
{
  noise_digits = new TClonesArray("PndDiscDigitizedHit");
}

PndDiscNoiseGeneration::~PndDiscNoiseGeneration()
{
  if (noise_digits != nullptr)
    delete noise_digits;
}

void PndDiscNoiseGeneration::SetNumberOfSensors(int n_detectors_, int n_readout_modules_, int n_sensors_)
{
  // note: a sensor can also be tiled - this is actually the sensitive area at a focusing element.
  n_detectors = n_detectors_;
  n_readout_modules = n_readout_modules_;
  n_sensors = n_sensors_;
}

void PndDiscNoiseGeneration::SetSensorGrid(double sensor_width_, double sensor_height_, int n_pixel_x_, int n_pixel_y_)
{
  half_sensor_width = 0.5 * sensor_width_;
  half_sensor_height = 0.5 * sensor_height_;
  n_pixel_x = n_pixel_x_;
  n_pixel_y = n_pixel_y_;

  pitch_x = sensor_width_ / double(n_pixel_x);
  pitch_y = sensor_height_ / double(n_pixel_y);

  n_pixel_total = n_pixel_x * n_pixel_y;
}

void PndDiscNoiseGeneration::SetSensorTiling(int sensor_tiles_x, int sensor_tiles_y)
{
  n_sensor_tiles_x = sensor_tiles_x;
  n_sensor_tiles_y = sensor_tiles_y;
}

bool PndDiscNoiseGeneration::GetPixelPosition(int n_pixel, double &pos_x, double &pos_y)
{
  if (n_pixel >= n_pixel_total)
    return false;
  pos_x = -half_sensor_width + (double(n_pixel % n_pixel_x) + 0.5) * pitch_x;
  pos_y = -half_sensor_height + (double(n_pixel / n_pixel_x) + 0.5) * pitch_y;
  return true;
}

int PndDiscNoiseGeneration::GenerateNoise(double t_start_ns, double t_end_ns)
{
  Clear(); // clear buffer and reset the counter

  // fill buffer with new noise digits
  double n_hits_avg = dcr_per_pixel * (t_end_ns - t_start_ns); // average number of hits in time window

  // compute the tile pitch
  double tile_pitch_x = half_sensor_width * 2.0 / double(n_sensor_tiles_x);
  double tile_pitch_y = half_sensor_height * 2.0 / double(n_sensor_tiles_y);

  // loop over all sensors:
  int n_pixel, n_hits, hit, n_total_hits = 0;
  int sensor_nx, sensor_ny, detector_id, readout_id, sensor_id, sensor_vid;
  double hit_time, pos_x, pos_y;

  for (detector_id = 0; detector_id < n_detectors; detector_id++) {
    for (readout_id = 0; readout_id < n_readout_modules; readout_id++) {
      for (sensor_vid = 0; sensor_vid < n_sensors; sensor_vid++) {
        // loop over all pixels of the sensor:
        for (n_pixel = 0; GetPixelPosition(n_pixel, pos_x, pos_y); n_pixel++) {
          // compute the sensor identifier (tile grouping for dead time handling):
          sensor_nx = (int)(pos_x + half_sensor_width) / tile_pitch_x;
          sensor_ny = (int)(pos_y + half_sensor_height) / tile_pitch_y;
          // sensor_nx = (pos_x < 0 ? 0 : 1);
          // sensor_ny = (pos_y < 0 ? 0 : 1);
          sensor_id = sensor_vid * (n_sensor_tiles_x * n_sensor_tiles_y) + sensor_ny * (n_sensor_tiles_x) + sensor_nx + 1;

          // compute number of hits and distribute these in the time window:
          n_hits = gRandom->Poisson(n_hits_avg); // hits on this pixel in the given time window
          for (hit = 0; hit < n_hits; hit++) {
            hit_time = gRandom->Uniform(t_start_ns, t_end_ns);
            // store noise hit:
            new ((*noise_digits)[n_total_hits++]) PndDiscDigitizedHit(FairLink(), detector_id, readout_id, sensor_id, sensor_id, n_pixel / n_pixel_x, pos_y, hit_time, hit_time, 1);
          }
        }
      }
    }
  }
  return noise_digits->GetEntries();
}

PndDiscDigitizedHit *PndDiscNoiseGeneration::GetNextNoiseDigit()
{
  if (current_index < noise_digits->GetEntries())
    return (PndDiscDigitizedHit *)(*noise_digits)[current_index++];
  else
    return nullptr;
}

void PndDiscNoiseGeneration::Clear()
{
  noise_digits->Clear();
  current_index = 0;
}

ClassImp(PndDiscNoiseGeneration);
