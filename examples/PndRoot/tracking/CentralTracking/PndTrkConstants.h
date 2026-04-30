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

#ifndef PndTrkConstants_H
#define PndTrkConstants_H 1

// definition of constants used in PndTrk.... classes;

// integer constants
const int MAXHITSINCELL = 20,
          // the following MAXHITSINFIT cannot be too large because the fit
          // crashes 'silently' for too much memory consumption in the character arrays
          // or takes too long time;

  TIMEOUT = 60;

// B field in Tesla
const Double_t BFIELD = 2.,
               CVEL = 2.99792, //  velocity of light
                               // DELTAnR = range of nR in TrkAssociatedParallelHitsToHelixquater
  DELTAnR = 2.,                // range of nR in TrkAssociatedParallelHitsToHelixquater
  PMAX = 100.,
               VERTICALGAP = 4.; // (cm) gap between Left and Right sections of detector.

//----------------------------- Mvd constants, according to Mvd design;
const Double_t ERRORPIXEL = 0.02611, ERRORSTRIP = 0.02611, ERRORSQPIXEL = 0.00068175, ERRORSQSTRIP = 0.00068175;

// averge radius of the various layers of the barrel part;
const int MAXMVDMCPOINTS = 2000;

//--------------------------------------- MVD Barrel, full azimuthal coverage;
const int MVD_BARREL_LAYERS_FULL_AZIMUTH = 3; // number of barrel layers with 360 degree azimuthal coverage;
const Double_t MVD_BARREL_FULL_AZIMUTH_MAX_RADIUS[3] = {12.35, 9., 12.35}, MVD_BARREL_FULL_AZIMUTH_Z_LOW[3] = {-16.917, -13.376, -10.048},
               MVD_BARREL_FULL_AZIMUTH_Z_UP[3] = {-10.248, -6.707, -3.379};

//--------------------------------------- MVD Barrel, partial azimuthal coverage;

const int MVD_BARREL_LAYERS_PARTIAL_AZIMUTH = 11, // number barrel layers with less than 360 degree azimuthal coverage;

  // in the geometry of the Mvd it is possible to assume that the number of gaps is
  // not necessarily the same for the Inner and Outer barrels of any Mvd  Section;
  // therefore in   MVD_BARREL_PARTIAL_AZIMUTH_NGAP[i][j]  i is the index indicating the
  // i-th Barrel having Partial Azimuth Coverage;  j is the index indicating
  //   the INNER (j=0) and the OUTER (j=1) number of azimuthal gaps;

  MVD_BARREL_PARTIAL_AZIMUTH_NGAP[11][2] = {13, 10, 3, 5, 3, 0, 4, 4, 4, 5, 2, 0, 13, 10, 2, 2, 3, 3, 2, 2, 3, 3};
const Double_t
  // in practice there are never more than 13 gaps in azimuthal angle in radians from 0 to 2*pi;
  MVD_BARREL_PARTIAL_AZIMUTH_GAP_LOW_INNER[11][13] =
    {

      0.,     0.347,  0.701,  1.2437, 2.141, 2.590, 3.039, 3.489, 3.843, 4.385, 5.282, 5.7311, 6.18, // first Barrel
      0.,     1.693,  4.834,  0.,     0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,     0.,   // second Barrel
      0.,     1.827,  4.969,  0.,     0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,     0.,   // third Barrel
      0.,     1.2687, 2.167,  4.858,  0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,     0.,   // fourth Barrel
      0.372,  3.066,  3.9615, 5.758,  0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,     0.,   // fifth Barrel
      1.1525, 4.555,  0.,     0.,     0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,     0.,   // sixth Barrel
      0.,     0.347,  0.701,  1.2437, 2.141, 2.590, 3.039, 3.489, 3.843, 4.385, 5.282, 5.7311, 6.18, // seventh Barrel
      1.2,    4.34,   0.,     0.,     0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,     0.,   // eighth Barrel
      0.,     1.828,  4.9696, 0.,     0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,     0.,   // ninth Barrel
      1.2,    4.341,  0.,     0.,     0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,     0.,   // tenth Barrel
      0.,     1.827,  4.969,  0.,     0.,    0.,    0.,    0.,    0.,    0.,    0.,    0.,     0.    // eleventh Barrel
},
  MVD_BARREL_PARTIAL_AZIMUTH_GAP_UP_INNER[11][13] =
    {
      0.103, 0.5519, 1.001, 1.898, 2.4406, 2.7946, 3.2446, 3.6935, 4.1426, 5.0393, 5.582, 5.936, 6.283, // first Barrel
      1.449, 4.59,   6.283, 0.,    0.,     0.,     0.,     0.,     0.,     0.,     0.,    0.,    0.,    // second Barrel
      1.162, 4.303,  6.283, 0.,    0.,     0.,     0.,     0.,     0.,     0.,     0.,    0.,    0.,    // third Barrel
      0.75,  1.646,  4.34,  6.283, 0.,     0.,     0.,     0.,     0.,     0.,     0.,    0.,    0.,    // fourth Barrel
      2.545, 3.444,  5.237, 6.136, 0.,     0.,     0.,     0.,     0.,     0.,     0.,    0.,    0.,    // fifth Barrel
      1.877, 5.019,  0.,    0.,    0.,     0.,     0.,     0.,     0.,     0.,     0.,    0.,    0.,    // sixth Barrel
      0.103, 0.5519, 1.001, 1.898, 2.4406, 2.7946, 3.2446, 3.6935, 4.1426, 5.0393, 5.582, 5.936, 6.283, // seventh Barrel
      1.789, 4.931,  0.,    0.,    0.,     0.,     0.,     0.,     0.,     0.,     0.,    0.,    0.,    // eighth Barrel
      1.161, 4.302,  6.283, 0.,    0.,     0.,     0.,     0.,     0.,     0.,     0.,    0.,    0.,    // ninth Barrel
      1.789, 4.931,  0.,    0.,    0.,     0.,     0.,     0.,     0.,     0.,     0.,    0.,    0.,    // tenth Barrel
      1.162, 4.303,  6.283, 0.,    0.,     0.,     0.,     0.,     0.,     0.,     0.,    0.,    0.     // eleventh Barrel
},

  MVD_BARREL_PARTIAL_AZIMUTH_GAP_LOW_OUTER[11][13] =
    {
      0.1104, 0.559,  1.008, 2.353, 2.8026, 3.252, 3.7, 4.149, 5.495, 5.944, 0., 0., 0., // first Barrel
      0.,     1.460,  1.9,   4.6,   5.045,  0.,    0.,  0.,    0.,    0.,    0., 0., 0., // second Barrel
      0.,     0.,     0.,    0.,    0.,     0.,    0.,  0.,    0.,    0.,    0., 0., 0., // third Barrel
      0.,     1.666,  4.356, 5.256, 0.,     0.,    0.,  0.,    0.,    0.,    0., 0., 0., // fourth Barrel
      0.,     0.763,  2.559, 3.46,  6.1486, 0.,    0.,  0.,    0.,    0.,    0., 0., 0., // fifth Barrel
      0.,     0.,     0.,    0.,    0.,     0.,    0.,  0.,    0.,    0.,    0., 0., 0., // sixth Barrel
      0.1104, 0.559,  1.008, 2.353, 2.8026, 3.252, 3.7, 4.149, 5.495, 5.944, 0., 0., 0., // seventh Barrel
      1.413,  4.555,  0.,    0.,    0.,     0.,    0.,  0.,    0.,    0.,    0., 0., 0., // eighth Barrel
      0.,     1.8964, 5.038, 0.,    0.,     0.,    0.,  0.,    0.,    0.,    0., 0., 0., // ninth Barrel
      1.4135, 4.555,  0.,    0.,    0.,     0.,    0.,  0.,    0.,    0.,    0., 0., 0., // tenth Barrel
      0.,     1.896,  5.038, 0.,    0.,     0.,    0.,  0.,    0.,    0.,    0., 0., 0.  // eleventh Barrel
},
  MVD_BARREL_PARTIAL_AZIMUTH_GAP_UP_OUTER[11][13] =
    {
      0.339, 0.788,  2.133, 2.582, 3.0312, 3.489, 3.929, 5.275, 5.724, 6.173, 0., 0., 0., // first Barrel
      1.238, 1.683,  4.383, 4.825, 6.283,  0.,    0.,    0.,    0.,    0.,    0., 0., 0., // second Barrel
      0.,    0.,     0.,    0.,    0.,     0.,    0.,    0.,    0.,    0.,    0., 0., 0., // third Barrel
      1.252, 3.947,  4.843, 6.283, 0.,     0.,    0.,    0.,    0.,    0.,    0., 0., 0., // fourth Barrel
      0.352, 2.148,  3.046, 5.739, 6.283,  0.,    0.,    0.,    0.,    0.,    0., 0., 0., // fifth Barrel
      0.,    0.,     0.,    0.,    0.,     0.,    0.,    0.,    0.,    0.,    0., 0., 0., // sixth Barrel
      0.339, 0.788,  2.133, 2.582, 3.0312, 3.489, 3.929, 5.275, 5.724, 6.173, 0., 0., 0., // seventh Barrel
      1.876, 5.018,  0.,    0.,    0.,     0.,    0.,    0.,    0.,    0.,    0., 0., 0., // eighth Barrel
      1.393, 4.5346, 6.283, 0.,    0.,     0.,    0.,    0.,    0.,    0.,    0., 0., 0., // ninth Barrel
      1.876, 5.018,  0.,    0.,    0.,     0.,    0.,    0.,    0.,    0.,    0., 0., 0., // tenth Barrel
      1.393, 4.535,  6.283, 0.,    0.,     0.,    0.,    0.,    0.,    0.,    0., 0., 0.  // eleventh Barrel
},
  // radius of INNER and OUTER layers of a Barrel; in case the Barrel can be described by one
  // radius only then MVD_BARREL_PARTIAL_AZIMUTH_RADIUS_OUTER is set at -1.;

  MVD_BARREL_PARTIAL_AZIMUTH_RADIUS_INNER[11] = {4.7624, 4.7654, 9.343, 2.262, 2.255, 12.63, 4.7624, 9.342, 9.314, 9.315, 9.3435},
  MVD_BARREL_PARTIAL_AZIMUTH_RADIUS_OUTER[11] = {5.2624, 5.25, -1., 2.84, 2.84, -1., 5.2624, 12.61, 12.61, 13.11, 12.5725},

  MVD_BARREL_PARTIAL_AZIMUTH_Z_LOW[11] = {-7.98, -7.48, -6.507, -3.98, -3.54, -3.179, -1.02, 0.362, 3.69, 7.231, 10.559},
  MVD_BARREL_PARTIAL_AZIMUTH_Z_UP[11] = {-1.18, -1.82, -3.166, -1.74, 0.98, 0.162, 5.78, 7.031, 10.359, 13.9, 13.9};

// old stuff;------------------------------------------------------------------------------------
// 0 --> innermost layer;  3 --> outermost layer;

const int MVD_BARREL_LAYERS = 4; // number of barrel layers;

// Radia of the Barrel Mvd layers;
// MVD_BARREL_AVERAGE_RADIUS[i] == AVERAGE Radius of i-th layer;
// MVD_BARREL_ZLIMITS[0][i] = Minimum Z of i-th layer,
// MVD_BARREL_ZLIMITS[1][i] = Maximum Z of i-th layer,
// MVD_BARREL_NOZONE_X[0 or 1] = X lower and upper limits of the zone of the beam pipe;
// MVD_BARREL_NOZONE_X[0 or 1] = Z lower and upper limits of the zone of the beam pipe;

const Double_t MVD_BARREL_AVERAGE_RADIUS[4] = {(2.18 + 2.858) / 2., (4.73 + 5.282) / 2., (8.972 + 9.686) / 2., (12.32 + 12.994) / 2.},
               MVD_BARREL_RADIASQMean[4] = {6.4762, 25.15, 87.15, 160.31}, MVD_BARREL_RADIASQDifference[4] = {3.5, 5.5, 13.32, 17.06}, MVD_BARREL_NOZONE_X[2] = {-2., 2.},
               MVD_BARREL_NOZONE_Z[2] = {-2., 2.}, MVD_BARREL_ZLIMITS[2][4] = {-3.98, -7.9, -13.376, -16.917, 13.9, 13.9, 13.9, 13.9};

// end of old stuff ---------------------------------------------------------------------------------

//---------------------------------------- MVD MiniDisks geometry infos;

// this disks are positioned perpendicularly to the Z direction; the thickness of the sensitive layer is 0.02 ;
const int MVD_MINIDISK_LAYERS = 12; // number of Mvd MiniDisks;
//  Z positions;
const Double_t MVD_Z_LAYER_BEGIN[12] = {1.97, 2.41, 3.97, 4.41, 6.97, 7.41, 9.97, 10.41, 14.77, 15.21, 21.77, 22.21},
               MVD_Z_LAYER_END[12] = {1.99, 2.43, 3.99, 4.43, 6.99, 7.43, 9.99, 10.43, 14.79, 15.23, 21.79, 22.23};

//---------------------------------------- MVD Disks

// 0 --> most upstream layer;  1 --> most downstream layer;
const int MVD_DISK_LAYERS = 2; // number of Mvd disks;
//  Z positions; in reality there are to disks very close to each other
//  then take the average Z to describe both;
const Double_t MVD_DISK_Z[2] = {16.265, 20.735},

               // each layer has a maximum radius and a minimum radius;
  MVD_DISK_MIN_RADIUS[2] = {7.5, 7.5}, MVD_DISK_MAX_RADIUS[2] = {13.11, 13.11};

//-----------------------------

//----------------------------- SciTil detector constants
//  RADIUSSCITIL = radius of the cylinder tangent to the SciTil tiles in the middle of each tile;
const Double_t RADIUSSCITIL = 49.27, DIMENSIONSCITIL = 2.85;
//-----------------------------

//----------------------------- STT detector constants
// maximum radius of the Stt detector in  cm; this is the radius CIRCUMSCRIBED to the outer Hexagon
// delimiting the STT detector;
const Double_t RSTRAWDETECTORMAX = 40.73,
               // APOTEMA means the radius of a circle INSCRIBED in a Hexagon (this is the same as the
               // mathematical definition); otherwise the name Radius is used instead;
  APOTEMAMAXINNERPARSTRAW = 23.246827,
               APOTEMAMAXSKEWSTRAW = 31.517569, // delimitation of the skew area
  APOTEMAMINOUTERPARSTRAW = 31.863369,
               APOTEMAMINSKEWSTRAW = 23.246827, // delimitation of the skew area
  APOTEMASTRAWDETECTORMIN = 16.119;             // minimum APOTEMA of the Stt detector in  cm
                                                // this is the Radius of the circle INSCRIBED in the
                                                // smaller Hexagon delimiting the STT detector;
const int MAXFOUNDCLUSTERS = 100,               // maximum allowed clusters in the event;
  MAXHITSINCLUSTER = 200,                       // maximum allowed hits in a cluster;
  MAXHITSINFIT = 50, MAXSKEWHITSINFIT = 8, MAXSTTHITSINTRACK = 40,

          MINIMUMMVDHITSPERTRACK = 1, MINIMUMSTTMHITSPERTRACK = 2, MINOUTERHITSPERTRACK = 5,
          MAX_NOT_CONNECTED = 1; // maximum # of STT "holes" in the hits of a track;

const Double_t DIAMETERSTRAWTUBE = 1., DIAMETERSTRAWTUBE2 = 1.,
               //  semilength of a axial straw;
  SEMILENGTH_STRAIGHT = 75., STRAWRADIUS = 0.5, STRAWRESOLUTION = 0.015, STRAW_SKEW_INCLINATION = 3.,
               STTDRIFTVEL = 0.0025, //   in cm/nsec
                                     // Z position of the center of the axial straw;
  ZCENTER_STRAIGHT = 35.;

//-----------------------------

const Double_t PI = 3.141592654, TWO_PI = 6.283185307;

#endif
