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
// Description: Definition of photo sensors
//-------------------------------------------------------------------------

#ifndef DISCDIRC_PHOTODETECTOR_HH
#define DISCDIRC_PHOTODETECTOR_HH

#include "PndDiscSensorGridPhotodetector.h"

#include <Math/Interpolator.h>

class DiscDIRC_Photodetector : public SensorGrid::SensorGridPhotodetector {
 public:
  enum DesignID { DESIGN_SIPM = 1, DESIGN_LRD };

  DiscDIRC_Photodetector();
  DiscDIRC_Photodetector(DesignID id);
  virtual ~DiscDIRC_Photodetector(){};

  void SetPDE(int n_entries, const double *wavelength_nm, const double *pde) { pde_interpolator.SetData(n_entries, wavelength_nm, pde); }

  virtual double GetPDE(const double &wavelength_nm) const { return pde_interpolator.Eval(wavelength_nm); }

 protected:
  void Init();
  void Init(DesignID design_id);

 private:
  ROOT::Math::Interpolator pde_interpolator;
};

#endif // DISCDIRC_PHOTODETECTOR_HH
