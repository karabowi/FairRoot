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
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Class to emulate noise for the readout electronics
//------------------------------------------------------------------------

#ifndef PNDDISCNOISEGENERATION_H
#define PNDDISCNOISEGENERATION_H

#include "TClonesArray.h"

class PndDiscDigitizedHit;

class PndDiscNoiseGeneration {
 public:
  PndDiscNoiseGeneration();
  ~PndDiscNoiseGeneration();

  // Initialization:

  /// Set the number of sensors for the whole apparatus
  void SetNumberOfSensors(int n_detectors_, int n_readout_modules_, int n_sensors_);
  /// Set the pixel grid on the sensors
  void SetSensorGrid(double sensor_width_, double sensor_height_, int n_pixel_x_, int n_pixel_y_);
  /// Set a logical tiling (used for dead time handling)
  void SetSensorTiling(int sensor_tiles_x, int sensor_tiles_y);
  void SetPerCellDCR(double dcr_Hz) { dcr_per_pixel = dcr_Hz * 1E-9; }

  // Noise generation and access to noise digits:
  int GenerateNoise(double t_start_ns, double t_end_ns); ///< Generate noise pattern in time window given by t_start, t_end
  PndDiscDigitizedHit *GetNextNoiseDigit();              ///< Iterate over all generated hits.
  void Clear();                                          ///< Clears the digit buffer

 private:
  bool GetPixelPosition(int n_pixel, double &pos_x, double &pos_y);

 protected:
  TClonesArray *noise_digits; ///< Memory pool to store the digits
  int current_index;          ///< current index in pool (used in iteration)

  int n_detectors;       ///< number of detectors
  int n_readout_modules; ///< number of readout modules on a detector
  int n_sensors;         ///< number of sensors (untiled) at a readout module
  int n_sensor_tiles_x;  ///< number of sensor tiles = number of dies
  int n_sensor_tiles_y;  ///< number of sensor tiles = number of dies

  double half_sensor_width;
  double half_sensor_height;
  int n_pixel_x;
  int n_pixel_y;
  int n_pixel_total;

  double pitch_x;
  double pitch_y;

  double dcr_per_pixel; // pixel means a single SPAD here - unit: dark counts / ns
};

#endif // PNDDISCNOISEGENERATION_HH_
