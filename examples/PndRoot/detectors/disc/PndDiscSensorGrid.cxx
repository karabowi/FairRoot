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
// Description: Definition of sensor grid
//-------------------------------------------------------------------------

#include "PndDiscSensorGrid.h"

#include <math.h>
#include <iostream>

#define OMERLE_SENSORGRID_DEBUG

namespace SensorGrid {

/// Lock the grid:
void SensorGridBase::LockGrid(bool lock)
{
  if (lock == true) {
    // Enumerate this and all child grids:
    int grid_id = 1;
    int pixel_offset = number_of_pixels;
    EnumerateGrids(grid_id, pixel_offset);
    LockChilds(true);

    // and set lock flag
    locked = true;
  } else
    locked = false;
}

/// Return information of the next pixel (pixel iteration)

/// The pixel_info.pixel_number will be increased by one and the pixel_info structure
/// is updated to the corresponding values. The position of the pixel center is also
/// returned using arguments x, y.
bool SensorGridBase::NextPixel(double &x, double &y, PixelInfo &pixel_info) const
{
  // simple method that will work with all derived classes.
  // !!! maybe slow !!! -> derived classes should implement caching of last subgrid.
  if (pixel_info.pixel_number < 0)
    pixel_info.pixel_number = 0; // start enum
  else
    ++pixel_info.pixel_number; // run enum
  return PixelToPosition(pixel_info, x, y);
}

// -------------------------------------------------------------------------------------------------------
// BasicGrid
// -------------------------------------------------------------------------------------------------------

BasicGrid::BasicGrid() {}

BasicGrid::BasicGrid(const double &x_min_, const double &x_width_, const double &x_pitch_, int n_x_, const double &y_min_, const double &y_width_, const double &y_pitch_, int n_y_)
  : SensorGridBase(), x_min(x_min_), x_width(x_width_), x_pitch(x_pitch_), y_min(y_min_), y_width(y_width_), y_pitch(y_pitch_), n_x(n_x_), n_y(n_y_)
{
  ///\todo: throw here
  x_width_over_two_pitch = x_width / (2. * x_pitch);
  y_width_over_two_pitch = y_width / (2. * y_pitch);
  x_max = x_min + double(n_x) * x_pitch;
  y_max = y_min + double(n_y) * y_pitch;
  number_of_pixels = n_x * n_y;
}

BasicGrid::~BasicGrid() {}

void BasicGrid::EnumerateGrids(int &id, int &pixel_offset)
{
#ifdef OMERLE_SENSORGRID_DEBUG
  std::cout << "BasicGrid::EnumerateGrids\n";
#endif
  // Set grid id and pixel number offset
  grid_number = id++;
  pixel_number_offset = pixel_offset;
  pixel_offset += number_of_pixels;
}

void BasicGrid::SetXDivision(const double &x_min_, const double &x_width_, const double &x_pitch_, int n_x_)
{
  if (locked)
    return; // todo: throw error
  x_min = x_min_;
  x_width = x_width_;
  x_pitch = x_pitch_;
  n_x = n_x_;
  x_width_over_two_pitch = x_width / (2. * x_pitch);
  x_max = x_min + double(n_x) * x_pitch;
  number_of_pixels = n_x * n_y;
}

void BasicGrid::SetYDivision(const double &y_min_, const double &y_width_, const double &y_pitch_, int n_y_)
{
  if (locked)
    return; // todo: throw error
  y_min = y_min_;
  y_width = y_width_;
  y_pitch = y_pitch_;
  n_y = n_y_;
  y_width_over_two_pitch = y_width / (2. * y_pitch);
  y_max = y_min + double(n_y) * y_pitch;
  number_of_pixels = n_x * n_y;
}

/// Convert a hit position to a pixel number
bool BasicGrid::PositionToPixel(const double &x, const double &y, PixelInfo &pixel_info) const
{
  if (x < x_min || x > x_max || y < y_min || y > y_max)
    return false;

  double x_coord_fp = (x - x_min) / x_pitch;
  double x_coord = floor(x_coord_fp);
  if (x_width_over_two_pitch < fabs(0.5 - (x_coord_fp - x_coord)))
    return false;

  double y_coord_fp = (y - y_min) / y_pitch;
  double y_coord = floor(y_coord_fp);
  if (y_width_over_two_pitch < fabs(0.5 - (y_coord_fp - y_coord)))
    return false;

  pixel_info.column_on_grid = (int)x_coord;
  pixel_info.row_on_grid = (int)y_coord;
  pixel_info.pixel_number = pixel_info.column_on_grid + pixel_info.row_on_grid * n_x + pixel_number_offset;

  pixel_info.column_on_grid += user_col_offset;
  pixel_info.row_on_grid += user_row_offset;

  pixel_info.grid_id = grid_number;
  return true;
}

/// Compute the center-position of a pixel on this grid and update the pixel_info structure
bool BasicGrid::PixelToPosition(PixelInfo &pixel_info, double &x, double &y) const
{
  // std::cout << pixel_number_offset << "\t" << number_of_pixels << std::endl;
  int local_pixel_num = pixel_info.pixel_number - pixel_number_offset;
  if (pixel_info.pixel_number < pixel_number_offset || local_pixel_num >= number_of_pixels)
    return false;

  pixel_info.column_on_grid = local_pixel_num % n_x;
  pixel_info.row_on_grid = local_pixel_num / n_x;
  pixel_info.grid_id = grid_number;

  x = x_min + (0.5 + (double)(pixel_info.column_on_grid)) * x_pitch;
  y = y_min + (0.5 + (double)(pixel_info.row_on_grid)) * y_pitch;

  pixel_info.column_on_grid += user_col_offset;
  pixel_info.row_on_grid += user_row_offset;
  return true;
}

// -------------------------------------------------------------------------------------------------------
// MultipleGrids
// -------------------------------------------------------------------------------------------------------

MultipleGrids::MultipleGrids() {}

MultipleGrids::~MultipleGrids()
{
  std::vector<SensorGridBase *>::const_iterator it;
  for (it = grids.begin(); it != grids.end(); ++it)
    delete *it;
}

void MultipleGrids::AddGrid(SensorGridBase *grid)
{
  grids.push_back(grid);
}

void MultipleGrids::EnumerateGrids(int &id, int &pixel_offset)
{
#ifdef OMERLE_SENSORGRID_DEBUG
  std::cout << "MultipleGrids::EnumerateGrids\n";
#endif
  grid_number = id++;
  pixel_number_offset = pixel_offset;
  std::vector<SensorGridBase *>::const_iterator it;
  for (it = grids.begin(); it != grids.end(); ++it) {
    EnumerateGridsProxy(*it, id, pixel_offset);
#ifdef OMERLE_SENSORGRID_DEBUG
    std::cout << "After Enum cycle" << id << "\t" << pixel_offset << std::endl;
#endif
  }
  number_of_pixels = pixel_offset;
}

bool MultipleGrids::PositionToPixel(const double &x, const double &y, PixelInfo &pixel_info) const
{
  std::vector<SensorGridBase *>::const_iterator it;
  for (it = grids.begin(); it != grids.end(); ++it) {
    if ((*it)->PositionToPixel(x, y, pixel_info))
      return true;
  }
  return false;
}

void MultipleGrids::LockChilds(bool lock)
{
  std::vector<SensorGridBase *>::const_iterator it;
  for (it = grids.begin(); it != grids.end(); ++it)
    LockChildsProxy(*it, lock);
}

bool MultipleGrids::PixelToPosition(PixelInfo &pixel_info, double &x, double &y) const
{
  std::vector<SensorGridBase *>::const_iterator it;
  for (it = grids.begin(); it != grids.end(); ++it) {
    if ((*it)->PixelToPosition(pixel_info, x, y))
      return true;
  }
  return false;
}

} // namespace SensorGrid
