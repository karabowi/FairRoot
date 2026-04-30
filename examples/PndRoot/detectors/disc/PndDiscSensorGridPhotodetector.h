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

#ifndef SensorGridPhotodetector_HH
#define SensorGridPhotodetector_HH

#include <vector>

namespace SensorGrid {

class SensorGridBase;
struct PixelInfo;

class SensorGridPhotodetector {
 private:
  struct PixelTraits;

 public:
  SensorGridPhotodetector(SensorGridBase *sensor_grid_, bool per_pixel_traits_, double const &pde_init, double const &noise_rate_init, double const &time_res_init);

 protected:
  SensorGridPhotodetector(){}; // for use in derived classes
  virtual ~SensorGridPhotodetector(){};
  void Init(SensorGridBase *sensor_grid_, bool per_pixel_traits_, double const &efficiency_init, double const &noise_rate_init, double const &time_res_init);

 public:
  // Per pixel data initialization:
  void SetPixel(int pixel_id, double efficiency, double noise_rate, double time_res_ns);
  void UseInhomogenityFactor(bool flag) { use_inhomogenity_factor = flag; }

  /// Handle photon detection:
  int Detect(double const &hit_pos_x, double const &hit_pos_y, double const &hit_time_ns, double const &wavelength_nm, PixelInfo &pixel_info, double &smeared_time_ns) const;

  /// Generate noise hits
  void GenerateNoise(double const &time_start_ns, double const &time_window_ns, std::vector<std::pair<int, double>> &hits) const;

  /// Derived classes should override this function and return the average pde of the sensor.

  /// Per pixel deviations of the PDE can be taken into account by including them in
  /// a individual pixel efficiency.
  virtual double GetPDE(const double &) const { return 1.0; } // FIXME // wavelength_nm //[R.K.03/2017] unused variable(s)

  /// Derived classes should override this function to account for spatial PDE deviations (e.g. due to gain inhomgenity)

  /// This function is used to implement inhomogenitites over the whole sensor, which cannot
  /// be treated as single pixel efficiencies, e.g. if the pixels are large compared to the
  /// spatial deviation of the inhomogenity.
  virtual double GetInhomegenityFactor(double const &, double const &) const { return 1.0; } // FIXME//hit_pos_x  hit_pos_y//[R.K.03/2017] unused variable(s)

  /// Apply time smearing
  double GetSmearedTime(double const &time_value, PixelInfo const &pixel_info) const;

  /// Reinit noise iterator and return number of noise hits.
  int GenerateNoise(double const &time_start_ns, double const &time_window_ns);

  /// Noise hit generator function which has to be called after GenerateNoise() to retrieve the noise hits.
  bool GetNoiseHit(int &pixel_number, double &hit_time_ns, double &smeared_time_ns);

  /// Set the dark count rate for all pixels to dcr_Hz
  void SetDCR(double const &dcr_Hz);

  const SensorGridBase *GetGrid() { return sensor_grid; }

 protected:
  bool per_pixel_traits;
  bool use_inhomogenity_factor;
  PixelTraits *pixel_traits;
  SensorGridBase *sensor_grid;
  // state of noise generator:
  int noisegen_pixel_number;
  double noisegen_time_start_ns;
  double noisegen_time_window_ns;
  int noisegen_n_hits;
  int noisegen_current_hit;
  double noisegen_current_time_sigma;
};

} // namespace SensorGrid

#endif // SensorGridPhotodetector_HH
