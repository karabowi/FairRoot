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

#include "PndDiscPhotodetector.h"
#include "PndDiscSensorGrid.h"

#include "FairLogger.h"

DiscDIRC_Photodetector::DiscDIRC_Photodetector()
{
  Init(DESIGN_SIPM);
}

DiscDIRC_Photodetector::DiscDIRC_Photodetector(DesignID design_id)
{
  Init(design_id);
}

void DiscDIRC_Photodetector::Init(DesignID design_id)
{
  using namespace SensorGrid;

  if (design_id == DESIGN_SIPM) {
    // 210 stripes SiPM sensor (acceptance gap due to bonding is covered with stripes !).
    MultipleGrids *new_sensor_grid = new MultipleGrids();
    if (new_sensor_grid == 0)
      LOG(fatal) << "Allocation of sensor failed.";

    BasicGrid *grid = nullptr;

    grid = new BasicGrid(-1.5900 / 2.0, 1.5900 / 2.0, 1.0, 1, -0.672, 0.0064, 0.0064, 105);
    if (grid == 0)
      LOG(fatal) << "Allocation of sensor failed.";

    new_sensor_grid->AddGrid(grid);

    grid = new BasicGrid(0.0, 1.5900 / 2.0, 1.0, 1, -0.672, 0.0064, 0.0064, 105);
    if (grid == 0)
      LOG(fatal) << "Allocation of sensor failed.";

    grid->SetUserColumnOffset(1);
    new_sensor_grid->AddGrid(grid);

    grid = new BasicGrid(-1.5900 / 2.0, 1.5900 / 2.0, 1.0, 1, 0.0, 0.0064, 0.0064, 105);
    if (grid == 0)
      LOG(fatal) << "Allocation of sensor failed.";

    grid->SetUserRowOffset(105);
    new_sensor_grid->AddGrid(grid);

    grid = new BasicGrid(0.0, 1.5900 / 2.0, 1.0, 1, 0.0, 0.0064, 0.0064, 105);
    if (grid == 0)
      LOG(fatal) << "Allocation of sensor failed.";

    grid->SetUserColumnOffset(1);
    grid->SetUserRowOffset(105);
    new_sensor_grid->AddGrid(grid);

    double active_cell_fraction = 0.9;
    double SPAD_DCR = 25.0; // Hz
    SensorGrid::SensorGridPhotodetector::Init(new_sensor_grid, false, active_cell_fraction, 256. * SPAD_DCR, 0.060);
  } else if (design_id == DESIGN_LRD) {
    // Implement Low Rate Design ...
    BasicGrid *grid = nullptr;

    // digitization grid 3 x 100 for 50 x 50 mm^2 active area (Planacon)
    grid = new BasicGrid(-2.55, 1.6, 1.7, 3,     // X: 1 mm gap between the 16 mm wide stripes
                         -2.5, 0.05, 0.05, 100); // Y: 100 stripes without gap
    if (grid == 0)
      LOG(fatal) << "Allocation of sensor failed.";

    SensorGrid::SensorGridPhotodetector::Init(grid, false, 0.7, 1.0, 0.050);
  }
}
