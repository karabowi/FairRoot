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

#ifndef SENSOR_GRID_HH_
#define SENSOR_GRID_HH_

#include <vector>
//#include <tr1/memory>

namespace SensorGrid {

// ======================
// =    Next steps:     =
// ======================
//
// - add similar counting like pixel number, but for dead time entity  ... 0%
// - add pixel iteration (return number and position)                  ... to test
//

// Compute number of pixels in
// a Grid and number them as:
// pixel_number_offset + ix + iy*cells_per_row

struct PixelInfo {
  int pixel_number;
  int grid_id;
  int column_on_grid;
  int row_on_grid;
};

/// Common base class for sensor grids

/// The sensor grid is meant to store the spatial topology of the sensor structure
/// and not metadata like PDE, DCR, gain. Latter should be stored in a external
/// map (pixel_number -> metadata). Pixel enumeration via NextPixel can be used
/// to implement noise generators.
class SensorGridBase {
 public:
  SensorGridBase() : pixel_number_offset(0), user_row_offset(0), user_col_offset(0), number_of_pixels(0), locked(false) {}
  virtual ~SensorGridBase() {}
  void SetUserColumnOffset(int ofs) { user_col_offset = ofs; }
  void SetUserRowOffset(int ofs) { user_row_offset = ofs; }
  void LockGrid(bool lock); // Function for the user to lock the root grid - will trigger enumeration.
  bool IsLocked() { return locked; }

  int GetNumberOfPixels() { return number_of_pixels; }
  virtual bool PositionToPixel(const double &x, const double &y, PixelInfo &pixel_info) const = 0;
  virtual bool PixelToPosition(PixelInfo &pixel_info, double &x, double &y) const = 0;

  virtual bool NextPixel(double &x, double &y, PixelInfo &pixel_number) const; // Pixel enumeration

 protected:
  virtual void EnumerateGrids(int &id, int &pixel_offset) = 0; // used to enumerate grids and pixel offsets.
  virtual void LockChilds(bool) {}                             // used to forward lock to children //  lock//[R.K.03/2017] unused variable(s)
  void SetPixelNumberOffset(int offset) { pixel_number_offset = offset; }

  // Proxys to method in siblings through base class:
  void EnumerateGridsProxy(SensorGridBase *b, int &grid_id, int &pixel_number) { b->EnumerateGrids(grid_id, pixel_number); }
  void LockChildsProxy(SensorGridBase *b, bool lock) { b->LockChilds(lock); }

 protected:
  int grid_number;
  int pixel_number_offset; /// The pixel number of the first pixel in this grid (or its childs)
  int user_row_offset;     /// user prowided offset to add to row on grid value in PixelInfo
  int user_col_offset;     /// user prowided offset to add to column on grid value in PixelInfo
  int number_of_pixels;    /// The total number of pixels in this grid (or its childs)
  bool locked;             /// True if grid islocked - no changed can be applied
};

/// A generic regular pixel grid with dead space between cells
class BasicGrid : public SensorGridBase {
 public:
  BasicGrid();
  BasicGrid(const double &x_min, const double &x_width, const double &x_pitch, int n_x, const double &y_min, const double &y_width, const double &y_pitch, int n_y);
  virtual ~BasicGrid();

  void SetXDivision(const double &x_min, const double &x_width, const double &x_pitch, int n_x);
  void SetYDivision(const double &y_min, const double &y_width, const double &y_pitch, int n_y);

  virtual bool PositionToPixel(const double &x, const double &y, PixelInfo &pixel_info) const;
  virtual bool PixelToPosition(PixelInfo &pixel_info, double &x, double &y) const;

  virtual void EnumerateGrids(int &id, int &pixel_offset);

 protected:
  double x_min, x_width, x_pitch, x_width_over_two_pitch;
  double y_min, y_width, y_pitch, y_width_over_two_pitch;
  int n_x, n_y;
  double x_max, y_max;
};

/// A grid to group other grids or to create nested grids.
class MultipleGrids : public SensorGridBase {
 public:
  MultipleGrids();
  virtual ~MultipleGrids();

  void AddGrid(SensorGridBase *grid); // Policy: this instance will take over the ownership of grid. Do not use the grid in userspace.

  virtual void EnumerateGrids(int &id, int &pixel_offset);

  virtual bool PositionToPixel(const double &x, const double &y, PixelInfo &pixel_info) const;
  virtual bool PixelToPosition(PixelInfo &pixel_number, double &x, double &y) const;

 protected:
  virtual void LockChilds(bool lock);

  std::vector<SensorGridBase *> grids;
};

} // namespace SensorGrid
#endif
