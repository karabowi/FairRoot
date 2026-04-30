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

/*
 * PndLmdDim.h
 *
 * this class gives you methods to retrieve parameters of the geometry
 * dimensions are in cm and radian. this is a singleton and must be called
 * with PndLmdDim::Instance()
 *
 * Global comments:
 *
 * TODO
 *
 *
 *  Created on: Oct 5, 2012
 *      Author: promme
 *
 *      To use transformation functions you have to call Read_Transformation_matrices
 *
 *		as of 2015 maintained by Roman Klasen, klasen@kph.uni-mainz.de

 */

#ifndef PNDLMDDIM_H_
#define PNDLMDDIM_H_

#include <cmath>
#include <TRandom.h>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>
#include <TVector3.h>
#include <TMatrixT.h>
#include <TH2Poly.h>
#include <TMultiGraph.h>
#include <TGraph.h>
#include <stdlib.h>
#include <cctype>

// work with DB
/* #include<PndLmdContFact.h> */
/* #include<TList.h> */
#include <LmdTools/PndLmdAlignPar.h>
#include <TPolyLine.h>
/* #include "FairRuntimeDb.h" */
/* #include "FairRunAna.h" */
/* #include "FairRun.h" */

typedef TMatrixT<double> TMatrixD; // hmm funny, should be declared in TMatrixT.h

using namespace std;

// simple class used for a fast particle track propagation
// based on a matrix fit to simulated data
// the representation of the basis is
// x, y, z, w, px/pz, py/pz, pw
// w und pw are weights in order to
// provide homogeneous coordinates
class PndLmdDimPropMat {
 public:
  double mom;
  double M[7][7];
  PndLmdDimPropMat()
  {
    mom = 0;
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 7; j++) {
        M[i][j] = 0;
      }
    }
  }
  // set a matrix which is given as an array
  void Set(const double *matrix)
  {
    for (int i = 0; i < 7 * 7; i++) {
      M[i / 7][i % 7] = matrix[i];
    }
  }
  // set a matrix with fitted hardcoded values
  // up to now only 1.5 GeV/c and 15 GeV/c are available
  // to produce more, please modify and use trafo_matrix_fit.C
  PndLmdDimPropMat(double mom_init)
  {
    mom = mom_init;
    if (mom == 1.5) {
      double matrix_init[49] = {1.012,      1.22,       5.216e-05,  0.2599,     1.096,     -0.1589,   -0.0001459, -0.5851,    1.042,      0.001092,
                                -3.991e-06, 0.3103,     1.064,      -3.991e-06, -0.04447,  -0.04501,  0.001764,   11.24,      -0.04385,   0.007135,
                                0.0001733,  -1.36e-15,  1.816e-14,  3.031e-15,  1,         1.513e-16, -2.272e-16, 7.904e-17,  0.006668,   1.356,
                                -0.0001811, 0.000127,   0.9794,     -0.1215,    0.4001,    -0.6638,   0.04068,    0.001168,   -7.627e-06, 0.2952,
                                0.9388,     -7.627e-06, -1.003e-15, -3.248e-15, 9.487e-17, 5.594e-18, 2.41e-16,   -1.086e-16, 1};
      Set(matrix_init);
    } else {
      if (mom == 15) {
        double matrix_init[49] = {0.9633,    0.2086,    -0.01173,   0.2598,     1.123,      -0.03217,   -0.0002463, -0.104,     0.97,      -0.006443,
                                  5.119e-06, 0.06427,   1.12,       5.119e-06,  -0.03828,   0.005853,   0.004478,   11.24,      -0.0447,   0.001794,
                                  2.613e-06, 2.376e-06, -0.0003806, -2.583e-07, 1,          -1.647e-05, -1.871e-05, -3.862e-07, -0.01744,  0.1777,
                                  -0.01384,  0.0001758, 0.9997,     -0.02581,   0.4002,     -0.09394,   -0.05056,   -0.002047,  3.624e-05, 0.06091,
                                  0.994,     3.624e-05, 2.376e-06,  -0.0003806, -2.583e-07, -3.862e-07, -1.647e-05, -1.871e-05, 1};
        Set(matrix_init);
      } else { // identity matrix
        cout << " Warning in PndLmdDimPropMat: loading identity matrix only " << endl;
        double matrix_init[49] = {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
                                  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1};
        Set(matrix_init);
      }
    }
  }
  // transform the position and momentum at the IP
  // to a position and momentum at the first plane of the LMD
  void Propagate(TVector3 &pos, TVector3 &momdir)
  {
    // TVector3 dir = mom.Unit();
    double mommag = momdir.Mag();
    // position transformation was evaluated for meter and angles * 10.
    double xip[7] = {pos.X() / 100., pos.Y() / 100., pos.Z() / 100., 1, momdir.X() / momdir.Z() * 10., momdir.Y() / momdir.Z() * 10., 1};
    double xlmd[7] = {0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 7; j++) {
        xlmd[i] += M[i][j] * xip[j];
      }
    }
    pos.SetXYZ(xlmd[0] * 100., xlmd[1] * 100., xlmd[2] * 100.);
    momdir.SetXYZ(xlmd[4] / 10., xlmd[5] / 10., 1.); // not an exact calculation ;)
    momdir = momdir.Unit() * mommag;
  }
};

class Tkey {
 public:
  signed char half;
  signed char plane;
  signed char module;
  signed char side;
  signed char die;
  signed char sensor;
  bool operator<(const Tkey &comp) const
  {
    if (half < comp.half)
      return true;
    if (half > comp.half)
      return false;
    if (plane < comp.plane)
      return true;
    if (plane > comp.plane)
      return false;
    if (module < comp.module)
      return true;
    if (module > comp.module)
      return false;
    if (side < comp.side)
      return true;
    if (side > comp.side)
      return false;
    if (die < comp.die)
      return true;
    if (die > comp.die)
      return false;
    if (sensor < comp.sensor)
      return true;
    if (sensor >= comp.sensor)
      return false;
    return false;
  }

  bool operator==(const Tkey &comp) const
  {
    return (half == comp.half) && (plane == comp.plane) && (module == comp.module) && (side == comp.side) && (die == comp.die) && (sensor == comp.sensor);
  }

  Tkey(const Tkey &copy)
  {
    half = copy.half;
    plane = copy.plane;
    module = copy.module;
    side = copy.side;
    die = copy.die;
    sensor = copy.sensor;
  }

  Tkey(int ihalf, int iplane, int imodule, int iside, int idie, int isensor)
  {
    half = ihalf;
    plane = iplane;
    module = imodule;
    side = iside;
    die = idie;
    sensor = isensor;
  }

  Tkey(string key)
  {
    int sign(1);
    int ikey(0); // 0,1,2,3,4,5 = ihalf, iplane, imodule, iside, idie, isensor
    int number;
    for (unsigned int ichar = 0; ichar < key.size(); ichar++) {
      if (key[ichar] == '-') {
        sign = -1;
      }
      if (isdigit(key[ichar])) {
        number = (key[ichar] - '0') * sign;
        sign = 1;
        if (ikey == 0)
          half = number;
        if (ikey == 1)
          plane = number;
        if (ikey == 2)
          module = number;
        if (ikey == 3)
          side = number;
        if (ikey == 4)
          die = number;
        if (ikey == 5)
          sensor = number;
        ikey++;
      }
    }
    if (ikey != 6)
      cout << " Error in Generate_Tkey: key string " << key << " is not valid " << endl;
  }
  Tkey() {}
};

class PndLmdDim {
 private:
  // in case you change anything in the geometry
  // you must increase this version number in the corresponding .cxx file!
  // it may affect the consistency between the geometry and the transformation matrices
  static int geometry_version;

  static PndLmdDim *pinstance;
  TGeoManager *fgGeoMan;
  PndLmdDim();
  PndLmdDim(const PndLmdDim &instance);
  PndLmdDim &operator=(const PndLmdDim &instance)
  {
    this->box_size_x = instance.box_size_x; // to get rid from pedantic warnings
    return *this;
  }
  ~PndLmdDim();
  // the navigation paths for the detector geometry are stored here
  vector<string> nav_paths;

 public:
  static PndLmdDim &Get_instance();
  static PndLmdDim *Instance();

  // this offset in the sensor id is introduced by
  // pandaroot when assembling several detector components
  // the lmd alone would have a sensor id range between 0 and n total sensors
  // A detector loaded before the lmd will introduce an offset in
  // that ID which can be determined from a loaded root geometry by calling
  // Set_sensIDoffset(-1);
  // TODO: add everywhere that sensID offset
  unsigned int sensIDoffset;

  // set the sensIDoffset
  // in case offset is < 0 the offset is
  // tried to be determined from a possibly loaded root geometry
  bool Set_sensIDoffset(int offset = -1);

  //	FairRun* ana;
  //	FairRuntimeDb* rtdb;
  // pi
  double pi;
  // number of detector planes
  unsigned int n_planes;
  // number of modules per plane half
  unsigned int nmodules;
  // position of planes where the first plane defines the origin
  double *plane_pos_z;
  // ****************************** plane half array *****************************
  // x in lmd reference frame
  double half_offset_x;
  // y in lmd reference frame
  double half_offset_y;
  // z in lmd reference frame
  double half_offset_z;
  // phi rotation in lmd reference frame
  double half_tilt_phi;
  // theta rotation in lmd reference frame
  double half_tilt_theta;
  // psi rotation in lmd reference frame
  double half_tilt_psi;
  // ****************************** plane half supports ******************************
  // x in the detector half reference frame
  double plane_half_offset_x;
  // y in the detector half reference frame
  double plane_half_offset_y;
  // z in the detector half reference frame
  double plane_half_offset_z;
  // phi rotation in the detector half reference frame
  double plane_half_tilt_phi;
  // theta rotation in the detector half reference frame
  double plane_half_tilt_theta;
  // psi rotation in the detector half reference frame
  double plane_half_tilt_psi;

  // ****************************** cvd cooling support discs ************************
  // cvd_diamond is cut out of 79.5 mm discs of 200 micron thickness
  // inner min. radius due to beam pipe + a safety margin
  double inner_rad;
  // not used yet but should be the outer acceptance
  double outer_rad;
  // number of CVD diamond discs per plane
  int n_cvd_discs;
  // radius of a CVD diamond disc
  double cvd_disc_rad;
  // the half of the diamond thickness
  double cvd_disc_thick_half;
  // even and odd discs in a plane will be shifted in z in order to prevent
  // mechanical damage during assembly
  double cvd_disc_even_odd_offset;
  // angle from the division of a circle into n_cvd_discs
  double delta_phi;
  // a polygon of n_cvd_discs sides fitting a radius of inner_rad
  // has a side length pol_side_lg of
  double pol_side_lg_half;
  // the minimum distance to the center of the polygone is given by
  double pol_side_dist_min;
  // the cvd disc has to be placed such that the disc crosses
  // the inner ring at an angle of 0 and delta_phi
  // this defines the distance to the center according to pythagoras
  double cvd_disc_dist;

  // the mechanical alignment precision is defined as an offset of and tilt around
  // the middle of the cvd diamond.
  // Values are standard deviation.
  // first comes translation than rotation

  // x is radial to the beam pipe when support is aligned
  double cvd_offset_x;
  // y is tangent to the beam pipe when support is aligned
  double cvd_offset_y;
  // z is along the beam pipe when support is aligned
  double cvd_offset_z;
  // phi rotation in the reference frame of the plane half support
  double cvd_tilt_phi;
  // theta rotation in the reference frame of the plane half support
  double cvd_tilt_theta;
  // psi rotation in the reference frame of the plane half support
  double cvd_tilt_psi;
  // thickness of the kapton flexible circuits to the sensors
  // (chosen to be thicker to simulate also the influence of the
  //  printed circuits themselves)
  double kapton_disc_thick_half;
  // *********************************** one side on a CVD disc *************************************
  // x is radial to the beam pipe when cvd is aligned
  double side_offset_x;
  // y is tangent to the beam pipe when cvd is aligned
  double side_offset_y;
  // z is along the beam pipe when cvd is aligned
  double side_offset_z; // should not be used due to clashing volumes with the support!
  // phi rotation in the reference frame of the cvd support
  double side_tilt_phi; // should not be used due to clashing volumes with the support!
  // theta rotation in the reference frame of the cvd support
  double side_tilt_theta;
  // psi rotation in the reference frame of the cvd support
  double side_tilt_psi; // should not be used due to clashing volumes with the support!
  // *********************************** HV-MAPS *************************************
  //
  //            left   right
  //
  //  |---------|----------|----------| top
  //  ||------|-||-------|-||-------|-|
  //  ||      | ||       | ||       | |
  //  ||  1   | ||   2   | ||   3   | |     row 1
  //  ||	    | || active| ||       | |
  //  ||------|-||-------|-||-------|-|
  //  |         | passive  |          |
  //  |---------|----------|----------| bottom
  //  gap
  //            |----------|----------| top
  //            | passive  |          |
  //            ||-------|-||-------|-|
  //            ||       | ||       | |
  //            ||   2   | ||   3   | |     row 2
  //            || active| ||       | |
  //            ||-------|-||-------|-|
  //            |----------|----------| bottom
  //
  //            left   right
  //
  //                A   y; maps_n_row; height
  //                |
  //                |
  //                --> x; maps_n_col; width
  //
  // the current design foresees a rotation of the first parameters

  // even when several maps are placed on one die
  // those will be placed in the simulation next to
  // each other as separate detectors
  // assumption: sensor 1 and 2 are on one die, the rest is separated
  int maps_n_col;
  int maps_n_row;
  // enabled [row][col]
  bool **enabled;
  // number of sensors per side
  int n_sensors;
  // NOTE: MOST of the following VARIABLES are HALF of it
  // due to geometry construction in GEANT
  double maps_thickness;
  double maps_passive_top;
  double maps_passive_bottom;
  double maps_passive_left;
  double maps_passive_right;
  double maps_active_width;
  double maps_active_height;

  double maps_active_pixel_size; // pixel size NOT half of it

  double maps_width;
  double maps_height;

  double maps_active_offset_x;
  double maps_active_offset_y;

  double die_gap; // (cm)

  double maps_die_width;
  double maps_die_height;

  // the mechanical alignment precision is defined as an offset of and tilt around
  // the middle of the cvd diamond.
  // Values are standard deviation.
  // first comes translation than rotation
  // translations along z as well as rotations around x and y are
  // negligible for dies glued on a cvd diamond
  // rotation around z is not working yet

  // x is along the edge of the cvd disc
  double die_offset_x;
  // y is orthogonal to the edge of the cvd disc
  double die_offset_y;
  // z is along the beam pipe
  double die_offset_z; // should not be used -> crashing volumes;
  // x is a rotation around the edge of the cvd disc
  double die_tilt_phi; // please do not use yet
  // y is a rotation around the orthogonal component of the edge of the cvd disc
  double die_tilt_theta; // please do not use yet
  // z is a rotation around an axis parallel to the along the beam pipe
  double die_tilt_psi; // please do not use yet;
  // *********************************** one sensor *************************************
  // x is mostly radial to the beam pipe
  double sensor_offset_x;
  // y is mostly tangent to the beam pipe
  double sensor_offset_y;
  // z is along the beam pipe
  double sensor_offset_z; // should not be used due to clashing volumes with cvd and flex prints
  // phi rotation in the reference frame of the sensor
  double sensor_tilt_phi;
  // theta rotation in the reference frame of the sensor
  double sensor_tilt_theta; // should not be used due to clashing volumes with cvd and flex prints
  // psi rotation in the reference frame of the sensor
  double sensor_tilt_psi; // should not be used due to clashing volumes with cvd and flex prints
  //*********************************** lumi box parameters ***********************************
  // see CAD files for details
  // https://edms.cern.ch/nav/P:FAIR-000000719:V0/P:FAIR-000000726:V0/TAB3
  // width
  double box_size_x;
  // height
  double box_size_y;
  // length
  double box_size_z;
  // thickness of the V2A steel plates
  double box_thickness;
  // position of the inner rib
  double pos_rib;
  // beam pipe radius at entrance
  double rad_entrance;
  // beam pipe radius at exit
  double rad_exit;
  // beam pipe separating non interacting paricles
  double rad_pipe;
  // beam pipe thickness;
  double pipe_thickness;
  // cone height of the transition region
  double length_transision;
  // length of the inner pipe
  double length_pipe;
  // position of the first detector plane
  double pos_plane_0;
  //*********************************** global parameters *************************************
  // where bending starts with
  double end_seg_upstream;
  // the bending radius
  double r_bend;
  // and the angle of the circle path
  double phi_bend;
  // the point where both tangents of the straight beam pipe tubes meet is
  double pos_rot_z;
  // z position of the lmd
  double pos_z; //(cm)
  double end_seg_bend;
  // x position of the lmd
  double pos_x;
  // y position of the lmd
  double pos_y;
  double rot_x;
  double rot_y;
  double rot_z;

  // returns false when one of the variables exceeds design values
  bool Is_valid_idcall(int ihalf, int iplane = 0, int imodule = 0, int iside = 0, int idie = 0, int isensor = 0)
  {
    if (ihalf < 0 || ihalf >= 2)
      return false;
    if (iplane < 0 || (unsigned)iplane >= n_planes)
      return false;
    if (imodule < 0 || imodule >= n_cvd_discs)
      return false;
    if (iside < 0 || iside >= 2)
      return false;
    if (idie < 0 || idie >= 2)
      return false;
    // allow to count the non existing inner sensor at die 2
    if (isensor < 0 || isensor >= n_sensors + 1)
      return false;
    return true;
  }

  // get the sensor id for a sensor on a given side, module and plane
  int Get_sensor_id(int ihalf, int iplane, int imodule, int iside, int idie, int isensor)
  {
    if (idie == 1)
      isensor += 2; // the parallel sensor to sensor 0 is not there!
    int result = isensor + (iside + (imodule + (iplane + ihalf * n_planes) * nmodules) * 2) * n_sensors;
    return result + sensIDoffset;
  }

  // get the sensor position by it's id in terms of plane module and side
  void Get_sensor_by_id(const int sensor_id, int &ihalf, int &iplane, int &imodule, int &iside, int &idie, int &isensor)
  {
    int _sensor_id = sensor_id - sensIDoffset;
    isensor = _sensor_id % n_sensors;
    idie = 0;
    if (isensor > 2) {
      idie = 1;
      isensor -= 2; // add the first but non existing sensor at die 2
    }
    _sensor_id /= n_sensors;
    iside = _sensor_id % 2;
    _sensor_id /= 2;
    imodule = _sensor_id % nmodules;
    _sensor_id /= nmodules;
    iplane = _sensor_id % n_planes;
    _sensor_id /= n_planes;
    ihalf = _sensor_id % 2;
    if (!Is_valid_idcall(ihalf, iplane, imodule, iside, idie, isensor)) {
      ihalf = 0;
      iplane = 0;
      imodule = 0;
      iside = 0;
      idie = 0;
      isensor = 0;
      cout << "Error in PndLmdDim::Get_sensor_by_id: " << sensor_id << " is not a valid sensor id!" << endl;
    }
  }

  // when calling for a displaced module, the generated
  // value must be kept same for the cvd diamond as well
  // as the sensors sitting on the diamond it self
  // the storage is realized by a map
  // the vector contains offsets in x, y, z, rotphi, rottheta, rotpsi;
  map<Tkey, vector<double>> offsets;
  map<Tkey, vector<double>>::iterator itoffset;
  /* replaced by an ugly but faster version below
  string Generate_key(int ihalf, int iplane, int imodule, int iside, int idie, int isensor){
    stringstream keystream;
    keystream << ihalf << iplane << imodule << iside << idie << isensor;
    return keystream.str();
  }*/

  /**
   * C++ version 0.4 char* style "itoa":
   * Written by Lukás Chmela
   * Released under GPLv3.
   */
  char *itoa(int value, char *result, int base)
  {
    // check that the base if valid
    char *last_char;
    if (base < 2 || base > 36) {
      *result = '\0';
      return result;
    }
    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;
    do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);
    // Apply negative sign
    if (tmp_value < 0)
      *ptr++ = '-';
    last_char = ptr;
    *ptr-- = '\0';
    // cout << last_char << endl;
    while (ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr-- = *ptr1;
      *ptr1++ = tmp_char;
    }
    return last_char;
  }

  string Generate_key(int ihalf, int iplane, int imodule, int iside, int idie, int isensor)
  {
    char key[100];
    char *ptr;
    ptr = itoa(ihalf, key, 10);
    ptr = itoa(iplane, ptr, 10);
    ptr = itoa(imodule, ptr, 10);
    ptr = itoa(iside, ptr, 10);
    ptr = itoa(idie, ptr, 10);
    ptr = itoa(isensor, ptr, 10);
    string result(key);
    // stringstream keystream;
    // keystream << ihalf << iplane << imodule << iside << idie << isensor;
    return result;
  }

  // generate a unique integer key not same as the string above since there negative numbers are allowed
  // so in case of adding -1's key is not unique!
  // moreover numbers should be kept 1 digit long
  int Generate_keynumber(unsigned int ihalf = 0, unsigned int iplane = 0, unsigned int imodule = 0, unsigned int iside = 0, unsigned int idie = 0, unsigned int isensor = 0)
  {
    stringstream keystream;
    keystream << ihalf << iplane << imodule << iside << idie << isensor;
    int key;
    key = atoi(keystream.str().c_str());
    return key;
  }

  // same structure as for offsets is used for the transformation matrices
  // stored are matrix operations
  // global -> local lumi
  //   key: ihalf = -1 iplane = -1 imodule = -1 iside = -1 idie = -1 isensor = -1
  // local lumi -> local side on cvd disc
  //   key: ihalf >=0 iplane >= 0 imodule >=0 iside = -1 idie = -1 isensor = -1
  // local side on cvd disc -> local sensor
  //   key: all variable
  // map<string, TGeoMatrix* > transformation_matrices;
  // map<string, TGeoMatrix* > transformation_matrices_aligned; // alternative aligned detector description
  // map<string, TGeoMatrix* >::iterator it_transformation_matrices;
  // to increase the performance by a factor of 5-6 a struct is used for the key now
  map<Tkey, TGeoMatrix *> transformation_matrices;
  map<Tkey, TGeoMatrix *> transformation_matrices_aligned; // alternative aligned detector description
  map<Tkey, TGeoMatrix *>::iterator it_transformation_matrices;

  // cleanup some maps containing only references
  void Cleanup();

  // read transformation matrices from a given file
  // aligned and not aligned are two separate maps
  // containing the description of the detector positions
  // if not filename is specified matrices are searched in
  // VMCWORKDIR/input/matrices.txt
  // you may overwrite the version number if necessary
  // VMCWORKDIR/geometry folder is used if no filename is specified
  // ATTENTION! aligned matrices means perfect geometry, set to false if using misaligned geometry
  void Read_transformation_matrices(string filename = "", bool aligned = true, int version_number = geometry_version);

  // write transformation matrices from a given file
  // aligned and not aligned are two separate maps
  // containing the description of the detector positions
  // you may overwrite the version number if necessary
  // version == 0 is reserved for backward compatibility!
  // VMCWORKDIR/geometry folder is used if no filename (e.g. "") is specified
  // warning overwrites existing trafo_matrices_lmd.dat!
  void Write_transformation_matrices(string filename, bool aligned = true, int version_number = geometry_version);

  // read transformation matrices from a loaded geometry
  // aligned and not aligned are two separate maps
  // containing the description of the detector positions
  // the geometry must be loaded otherwise matrices cannot be read
  // version number will be set according to the geometry version number
  // ToDo: multiply also matrices on the way to the key matrices
  //        in case those are not unity matrices
  bool Read_transformation_matrices_from_geometry(bool aligned = true);

  // apply transformation matrices to a loaded geometry
  // aligned and not aligned are two separate maps
  // containing the description of the detector positions
  // the geometry must be loaded otherwise matrices cannot be read
  // version number will be set according to the geometry version number
  // IMPORTANT: you may choose which PndLmdDim matrices you want to use
  // but a ROOT Geometry can always be only aligned. The original
  // matrix stays untouched!
  // TODO: multiply also matrices on the way to the key matrices
  //        in case those are not unity matrices
  // TODO: Find out how to store the aligned geometry as a default
  //        one to pandaroot parameter files
  bool Write_transformation_matrices_to_geometry(bool aligned = true);

  // get a list of sensor paths in the geometry navigation model
  // returns the path to the lmd top volume
  // It is a recursive search, call it once with the default found_lmd variable
  // in case first_call then gGeoManager->CdTop(); is executed to get to the top node
  // of a geometry
  // The geometry must be loaded
  string Get_List_of_Sensors(vector<string> &list_of_sensors, bool found_lmd = false, bool first_call = true);

  // check a list of sensor paths for validity
  // result is true if tests were sucessful
  // offset is the offset in the sensor number which may be
  // not 0 if the geometry was not created in the first place
  bool Test_List_of_Sensors(vector<string> list_of_sensors, int &offset);

  // Get an offset for a volume, if not existent and random
  // a random offset is generated and stored
  // Is used during generation of geometries when calling
  void Get_offset(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, double &x, double &y, double &z, double &rotphi, double &rottheta, double &rotpsi,
                  bool random = false);

  // set an offset for example for the case of existent alignment
  // values, to generate with Get_offset a geometry that matches
  // those offsets
  void Set_offset(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, double x, double y, double z, double rotphi, double rottheta, double rotpsi);

  // read alignment constants from DB
  // Feb 2013: currently DB is ASCII file
  void Read_DB_offsets(PndLmdAlignPar *lmdalignpar);

  // correct transformation matrices by align
  void Correct_transformation_matrices();
  void reCreate_transformation_matrices();
  // several functions returning the position and orientation of
  // the luminosity detector

  // the global system is defined by PANDA
  // returns the position and rotation of the lmd detector
  // the origin is PANDA interaction point
  // first comes translation than rotation
  void Get_pos_lmd_global(double &x, double &y, double &z, double &rotx, double &roty, double &rotz, bool misaligned = false)
  {
    rotx = rot_x;
    roty = rot_y; // phi_bend;
    rotz = rot_z;
    x = pos_x;
    y = pos_y;
    z = pos_z;
    if (misaligned)
      rotx += 0; // pedantic compiler fix
  }

  // decoding a digital hit by the position of the sensor given by the sensor ID
  // and the row and column entry
  // parameters of MC geometry input are used to determine the position of
  // the active area within the sensor volume
  // The hit point is returned in the panda global reference frame
  // needs transformation matrices to determine the position
  // column and row can be also the mean from a cluster and therefore
  // not an integer
  TVector3 Decode_hit(const int sensorID, const double column, const double row, const bool aligned = true, bool newVersion = false);

  // matrices for a fast prediction of a particle track to the lmd
  // depending on the momentum setting of the hesr which is the key
  map<double, PndLmdDimPropMat> propagation_matrices;

  // propagate a particle at the IP to the first plane of the lmd
  // the propagation is based on a transformation matrix which was
  // fit to GEANT4 propagated data from January 2015
  // only 1.5 GeV/c and 15 GeV/c are implemented properly
  // TODO: interpolation between matrices
  // back propagation not implemented yet since
  // Mathematica inverted matrices did not work
  void Propagate_fast_ip_to_lmd(TVector3 &pos, TVector3 &mom, double pbeam);

  // Find the corresponding sensor on the opposite side of a module
  // point is in the panda frame
  // point is transformed into the local lmd frame and
  // the projection is tested 1. to lie on the given sensor
  // 2. on all other sensors on the opposite side for intersection
  // returns true if the corresponding sensor was found
  // in that case input parameters are set to the corresponding sensor
  // otherwise those are set to -1!
  // do not use in time critical cases!
  bool Get_overlapping_sensor(const TVector3 &point, int &ihalf, int &iplane, int &imodule, int &iside, int &idie, int &isensor, bool aligned = true);

  // Test if a point lies on a specific sensor
  // The point in the panda frame is transformed into the sensor frame
  // the projection is tested to be within the boundaries of the active sensor area
  bool Is_on_Sensor(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // get the overlapping sensors by geometric constraints
  // hard coded overlapping areas are given sorted by the overlapping area
  // returned is the number of overlapping sensors
  // jdie and jsensor are vectors with the length of the returned number
  int Get_overlapping_sensor(int idie, int isensor, vector<int> &jdie, vector<int> &jsensor);

  // get the transformation matrix from the PANDA global reference frame to the
  // Luminosity reference frame
  TGeoHMatrix Get_transformation_global_to_lmd_local(bool aligned = true);

  // get the transformation matrix from lmd local reference frame to the
  // cvd disc surface reference frame
  TGeoHMatrix Get_transformation_lmd_local_to_module_side(int ihalf, int iplane, int imodule, int iside, bool aligned = true);

  // get the transformation matrix from lmd cvd disc surface frame to the
  // sensor reference frame
  TGeoHMatrix Get_transformation_module_side_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // get the transformation matrix from PANDA global reference frame to the
  // sensor reference frame
  TGeoHMatrix Get_transformation_global_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // get the transformation matrix from lumi reference frame to the
  // sensor reference frame
  TGeoHMatrix Get_transformation_lmd_local_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // get the inverse transformation matrix from the PANDA global reference frame to the
  // Luminosity reference frame
  TGeoHMatrix Get_transformation_lmd_local_to_global(bool aligned = true);

  // get the inverse transformation matrix from lmd local reference frame to the
  // cvd disc surface reference frame
  TGeoHMatrix Get_transformation_module_side_to_lmd_local(int ihalf, int iplane, int imodule, int iside, bool aligned = true);

  // get the inverse transformation matrix from lmd cvd disc surface frame to the
  // sensor reference frame
  TGeoHMatrix Get_transformation_sensor_to_module_side(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // get the inverse transformation matrix from PANDA global reference frame to the
  // sensor reference frame
  TGeoHMatrix Get_transformation_sensor_to_global(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // get the inverse transformation matrix from lmd local reference frame to the
  // sensor reference frame
  TGeoHMatrix Get_transformation_sensor_to_lmd_local(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // get the transformation matrix from a ideal sensor to the aligned one
  TGeoHMatrix Get_transformation_sensor_to_sensor_aligned(int ihalf, int iplane, int imodule, int iside, int idie, int isensor);

  // get the transformation matrix from a aligned sensor to the ideal one
  TGeoHMatrix Get_transformation_sensor_aligned_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor);

  //************************** 3d vector transformations ******************************

  // Transform from the PANDA global reference frame to the
  // Luminosity reference frame
  TVector3 Transform_global_to_lmd_local(const TVector3 &point, bool isvector = false, bool aligned = true);

  // Transform from lmd local reference frame to the
  // cvd disc surface reference frame
  TVector3 Transform_lmd_local_to_module_side(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, bool isvector = false, bool aligned = true);

  // Transform from lmd cvd disc surface frame to the
  // sensor reference frame
  TVector3 Transform_module_side_to_sensor(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false, bool aligned = true);

  // Transform from PANDA global reference frame to the
  // sensor reference frame
  TVector3 Transform_global_to_sensor(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false, bool aligned = true);

  // Transform from lmd local reference frame to the
  // sensor reference frame
  TVector3 Transform_lmd_local_to_sensor(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false, bool aligned = true);

  // Transform from the PANDA global reference frame to the
  // Luminosity reference frame
  TVector3 Transform_lmd_local_to_global(const TVector3 &point, bool isvector = false, bool aligned = true);

  // Transform from lmd local reference frame to the
  // cvd disc surface reference frame
  TVector3 Transform_module_side_to_lmd_local(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, bool isvector = false, bool aligned = true);

  // Transform from lmd cvd disc surface frame to the
  // sensor reference frame
  TVector3 Transform_sensor_to_module_side(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false, bool aligned = true);

  // Transform from PANDA global reference frame to the
  // sensor reference frame
  TVector3 Transform_sensor_to_global(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false, bool aligned = true);

  // Transform from lmd local reference frame to the
  // sensor reference frame
  TVector3 Transform_sensor_to_lmd_local(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false, bool aligned = true);

  // Transform from a ideal sensor to the aligned one
  TVector3 Transform_sensor_to_sensor_aligned(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false);

  // Transform from a aligned sensor to the ideal one
  TVector3 Transform_sensor_aligned_to_sensor(const TVector3 &point, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false);

  // Transform from one sensor to an other
  TVector3 Transform_sensor_to_sensor(const TVector3 &point, int ihalf_from, int iplane_from, int imodule_from, int iside_from, int idie_from, int isensor_from, int ihalf_to,
                                      int iplane_to, int imodule_to, int iside_to, int idie_to, int isensor_to, bool isvector = false, bool aligned = true);

  // ************************* 3d matrix rotations ***************************************

  // Transform from the PANDA global reference frame to the
  // Luminosity reference frame
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_global_to_lmd_local(const TMatrixD &matrix, bool aligned = true);

  // Transform from lmd local reference frame to the
  // cvd disc surface reference frame
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_lmd_local_to_module_side(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, bool aligned = true);

  // Transform from lmd local reference frame to the
  // sensor reference frame
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_lmd_local_to_sensor(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // Transform from lmd cvd disc surface frame to the
  // sensor reference frame
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_module_side_to_sensor(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // Transform from PANDA global reference frame to the
  // sensor reference frame
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_global_to_sensor(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // Transform from the PANDA global reference frame to the
  // Luminosity reference frame
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_lmd_local_to_global(const TMatrixD &matrix, bool aligned = true);

  // Transform from lmd local reference frame to the
  // cvd disc surface reference frame
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_module_side_to_lmd_local(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, bool aligned = true);

  // Transform from lmd cvd disc surface frame to the
  // sensor reference frame
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_sensor_to_module_side(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // Transform from sensor reference frame to the
  // lmd local reference frame
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_sensor_to_lmd_local(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // Transform from PANDA global reference frame to the
  // sensor reference frame
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_sensor_to_global(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // Transform from a ideal sensor to the aligned one
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_sensor_to_sensor_aligned(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor);

  // Transform from a aligned sensor to the ideal one
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_sensor_aligned_to_sensor(const TMatrixD &matrix, int ihalf, int iplane, int imodule, int iside, int idie, int isensor);

  // Transform from one sensor to an other
  // treats only 3 x 3 matrices representing the space
  TMatrixD Transform_sensor_to_sensor(const TMatrixD &matrix, int ihalf_from, int iplane_from, int imodule_from, int iside_from, int idie_from, int isensor_from, int ihalf_to,
                                      int iplane_to, int imodule_to, int iside_to, int idie_to, int isensor_to, bool aligned = true);

  // *************************

  // get a pointer to the requested matrices with checks
  // returns nullptr if no matrices available
  // do not delete!
  map<Tkey, TGeoMatrix *> *Get_matrices(bool aligned = true);

  // get a pointer to the requested matrix with checks
  // returns nullptr if no matrix available
  // do not delete or modify unless you know why you want to
  // load the two files for matrices first
  TGeoMatrix *Get_matrix(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  // get the transformation matrix for an path in an existing root geometry
  // no checks are performed in advance
  // result may be 0!
  // if (aligned) the matrix after a possible alignment is returned
  // in that case details to the matrix must be provided in form
  // of ihalf ... isensor
  // TODO: get rid of path and do it only on the basis of ihalf ... isensor
  // if (!aligned) the original matrix is returned
  TGeoHMatrix *Get_matrix(string path, bool aligned = true, int ihalf = -1, int iplane = -1, int imodule = -1, int iside = -1, int idie = -1, int isensor = -1);

  // set the transformation matrix for an path in an existing root geometry
  // A matrix can be only aligned,
  // therefore by default original matrices are not touched!
  // since a key must be created for a node
  // details to it must be provided in form of
  // ihalf ... isensor
  // TODO: get rid of path and do it only on the basis of ihalf ... isensor
  bool Set_matrix(string path, TGeoHMatrix *matrix, int ihalf = -1, int iplane = -1, int imodule = -1, int iside = -1, int idie = -1, int isensor = -1); //, bool aligned = true);

  // get the difference between two matrices of the aligned and misaligned branch
  // in terms of displacement and euler angles
  // all 0 in case of troubles and result is false
  // load the two files for matrices first
  // NOTE: output values are formatted to µm and µrad!!!
  bool Get_matrix_difference(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, double &dx, double &dy, double &dz, double &dphi, double &dtheta, double &dpsi);

  // calculates the differences of matrices between loaded
  // aligned and misaligned ones
  // As an output a table and some histograms are generated
  // load the two files for matrices first
  void Calc_matrix_offsets();

  // see get matrix
  TGeoMatrix *Get_matrix_global_to_lmd_local(bool aligned = true);

  // see get matrix
  TGeoMatrix *Get_matrix_lmd_local_to_module_side(int ihalf, int iplane, int imodule, int iside, bool aligned = true);

  // see get matrix
  TGeoMatrix *Get_matrix_module_side_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

  //	TGeoMatrix* Get_matrix_global_to_sector_local(int ihalf, int iplane, int imodule, bool aligned=true);

  // x, y, z coordinate transformation from the PANDA global reference frame to the
  // local reference frame of the luminosity monitor
  void Transform_global_to_lmd_local(double &x, double &y, double &z, bool aligned = true);

  // x, y, z vector transformation from the PANDA global reference frame to the
  // local reference frame of the luminosity monitor
  void Transform_global_to_lmd_local_vect(double &x, double &y, double &z, bool aligned = true);

  // x, y, z coordinate transformation from the local sensor reference frame to the
  // local reference frame of the luminosity monitor
  // void transform_sensor_local_to_lmd_local(const int sensor_id, double& x, double& y, double& z, bool misaligned = false);

  // x, y, z coordinate transformation from the local luminosity reference frame to the
  // global reference frame of panda
  // void transform_local_lmd_to_global(const int sensor_id, double& x, double& y, double& z, bool misaligned = false);

  // x, y, z coordinate transformation between the local sensor reference frames
  // of misaligned and aligned sensors
  // void transform_local_sensor();

  // Generates the luminosity monitor geometry into the mother volume
  // Please make sure that mother volume is large enough or that it
  // is an assembly volume
  // in addition rotation and translation matrices are calculated
  // to store those into a file please use
  // Write_transformation_matrices(filename, false);
  // in case of misaligned, offsets are retrieved via get_offset
  // and included into the geometry
  void Generate_rootgeom(TGeoVolume &mothervol, bool misaligned = false);

  // returns false
  // if version number of the geometry could not be retrieved from a loaded geometry
  // geometry must be available via the root geometry manager
  bool Retrieve_version_number();

  // small function to test some transformation matrices and methods
  void Test_matrices();

  // Draw the Sensors as overlays to an active root pad
  // Projection in XY is used
  // lmd_frame == true : The Lumi reference frame is used
  void Draw_Sensors(int iplane, bool aligned = true, bool lmd_frame = true, int glside = 2); // by default draw both sides
  // Get one Sensor as a polyline to be drawn to an active root pad
  // Projection in XY is used
  // Important -> Do not delete the PolyLine until you are sure that you are
  // finished with displaying it!
  // lmd_frame == true : The Lumi reference frame is used
  TPolyLine *Get_Sensor_Shape(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true, bool lmd_frame = true);

  // Get one Sensor as a vector of graphs
  // Projection in XY is used
  // lmd_frame == true : The Lumi reference frame is used
  // else the sensor reference frame
  // if pixel_subdivision: several graphs are returned
  // representing the passive area and the
  // active area subdivided into several pixels
  // please do not call several times with same
  // parameters if previous result and it's contents
  // are not deleted
  vector<TGraph *>
  Get_Sensor_Graph(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true, bool lmd_frame = true, bool pixel_subdivision = true);

  // Get a TH2Poly histogram where bins matching the x_y projected shapes
  // of the sensors in one plane
  // The active area can be subdivided further
  // the active pixels: NOT RECOMMENDED due to heavy ram usage
  // Please rename it if you intend to call the function
  // several times with same parameters
  TH2Poly *Get_histogram_Plane(int iplane, int iside, bool aligned = true, bool lmd_frame = true, bool pixel_subdivision = false);

  // Get a TH2Poly histogram for one module side
  // in the reference frame of the lumi
  // on request in the panda reference frame
  // The active area can be subdivided further
  // into the active pixels: WARNING heavy ram usage
  // Please rename it if you intend to call the function
  // several times with same parameters
  TH2Poly *Get_histogram_Moduleside(int ihalf, int iplane, int imodule, int iside, bool aligned = true, bool lmd_frame = true, bool pixel_subdivision = true);

  // Get a TH2Poly histogram for one sensor
  // in the reference frame of the lumi
  // on request in the panda reference frame
  // The active area can be subdivided into
  // and the active pixels
  // Please rename it if you intend to call the function
  // several times with same parameters
  TH2Poly *Get_histogram_Sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true, bool lmd_frame = true);

  // gets all ids that correspond to an overlapping area
  std::vector<int> getAvailableOverlapIDs();
  int makeOverlapID(int firstSensorId, int secondSensorId);
  int getID1fromOverlapID(int overlapID);
  int getID2fromOverlapID(int overlapID);

  // this is a legacy function and should not be called anymore
  int makeModuleID(int overlapID);
};

#endif /* PNDLMDDIM_H_ */
