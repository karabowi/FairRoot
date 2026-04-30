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

// -------------------------------------------------------------------------
// -----                     PndGemSensor header file                  -----
// -----                  Created 12/02/2009  by R. Karabowicz         -----
// -------------------------------------------------------------------------

/** PndGemSensor.h
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 12/02/2009
 *@version 1.0
 **
 ** This class describes the digitisation scheme for a sensor of the GEM.
 ** The sensor shape is circulare with inner and outer radius.
 ** The sensor center is defined in 3-D space.
 ** The rotation angle is defined as angle around the beam axis.
 ** In the sensor two views of strip orientation are defined
 ** by specifing the angle of rotation and the smallest pitch.
 ** The strip rotation angle should be understood as follows:
 ** - rotation angle  0 - radial strips
 ** - rotation angle 90 - concentric strips
 ** - rotation angle 60 - tilted radial strips by angle of 60deg at inner radius,
 **                       the strips are straight, which means that the angle between
 **                       the tilted strips and radial strips reduces as the radius grows
 ** - rotation angle -60 - same as above, but the start angle is -60deg
 ** The strip pitch for concentric strips is the strip width.
 ** The strip pitch for radial and tilted strips is defined as the distance between
 ** strip borders at the inner radius.
 **/

#ifndef PNDGEMSENSOR_H
#define PNDGEMSENSOR_H 1

#include "TNamed.h"
#include "PndDetectorList.h"

#include <map>
#include <list>
#include <set>
#include <vector>

class PndGemSensor : public TNamed {

 public:
  /** Default constructor **/
  PndGemSensor();

  /** Standard constructor
   *@param fName     Unique sensor name
   *@param detId     Unique detector identifier
   *@param iType     Sensor type (1,2,3)
   *@param pos[]     sensor centre coordinate in global c.s [cm]
   *@param rotation  rotation in global c.s. [rad]
   *@param innerRadius     sensor inner radius in [cm]
   *@param outerRadius     sensor outer radius in [cm]
   *@param d         sensor thickness in [cm]
   *@param stripAngle[]  strip angle, if 0  - radial strips, measuring theta,
                                      if 60 - modified radial strips, joining inner ring with outer rotated by 60 deg.
                                      if 90 - concentric strips, measuring radius
   *@param pitch[]   readout radial/angle strip pitch or pixel width in x/y [cm]
   **/
  PndGemSensor(TString tempName, Int_t detId, Int_t iType, Double_t x0, Double_t y0, Double_t z0, Double_t rotation, Double_t innerRad, Double_t outerRad, Double_t d,
               Double_t stripAngle0, Double_t stripAngle1, Double_t pitch0, Double_t pitch1);
  PndGemSensor(TString tempName, Int_t stationNr, Int_t sectorNr, Int_t iType, Double_t x0, Double_t y0, Double_t z0, Double_t rotation, Double_t innerRad, Double_t outerRad,
               Double_t d, Double_t stripAngle0, Double_t stripAngle1, Double_t pitch0, Double_t pitch1);
  PndGemSensor(const PndGemSensor &tempSensor);

  /** Destructor **/
  virtual ~PndGemSensor();

  /** Accessors **/
  TString GetDetectorName() const { return fName.Data(); }
  void SetDetectorId(Int_t stationNr, Int_t sensorNr) { fDetectorId = static_cast<int>(PndDetectorId::kGEM) << 27 | 0 << 21 | stationNr << 8 | sensorNr << 6; }
  Int_t GetDetectorId() const { return fDetectorId; }
  Int_t GetSystemId() const { return ((fDetectorId & (31 << 27)) >> 27); }
  Int_t GetStationNr() const { return ((fDetectorId & (8191 << 8)) >> 8); }
  Int_t GetSensorNr() const
  { // sensor number within station
    return ((fDetectorId & (3 << 6)) >> 6);
  }
  Int_t GetType() const { return fType; }
  Double_t GetX0() const { return fPosition[0]; }
  Double_t GetY0() const { return fPosition[1]; }
  Double_t GetZ0() const { return fPosition[2]; }
  Double_t GetRotation() const { return fRotation; }
  Double_t GetInnerRadius() const { return fInnerRadius; }
  Double_t GetOuterRadius() const { return fOuterRadius; }
  Double_t GetD() const { return fD; }
  Double_t GetStripAngle(Int_t index) const { return fStripAngle[index]; }
  Double_t GetPitch(Int_t index) const { return fPitch[index]; }
  Double_t GetSigmaX() const { return fSigmaX; }
  Double_t GetSigmaY() const { return fSigmaY; }
  Double_t GetSigmaXY() const { return fSigmaXY; }
  Int_t GetNChannels() const { return fNChannelsFront + fNChannelsBack; }
  Int_t GetNChannelsFront() const { return fNChannelsFront; }
  Int_t GetNChannelsBack() const { return fNChannelsBack; }

  Int_t GetSideChannels(Int_t si) const
  {
    if (si == 0)
      return fNChannelsFront;
    return fNChannelsBack;
  } // will return NChFront for si==0, and NChBack for anything else

  /** Calculate channel number for a coordinate pair.
   *@param x      x coordinate in global c.s. [cm]
   *@param y      y coordinate in global c.s. [cm]
   *@param iSide  0 = front side, 1 = back side
   *@value iChan  channel number. -1 if point is outside sensor.
   ** Will return the same for both sides in case of pixel sensor.
   **/
  Int_t GetChannel(Double_t x, Double_t y, Int_t iSide);

  Int_t GetChannel2(Double_t x, Double_t y, Int_t iSide, Double_t &feeDist);
  // Double_t GetChannel(Double_t x, Double_t y, Int_t iSide, Double_t& stripWidth);

  /** Test whether a coordinate pair (x,y) in global coordinates is
   ** inside the sensor **/
  Bool_t Inside(Double_t x, Double_t y);
  Bool_t Inside(Double_t radius);

  /** Activate the channels corresponding to a MCPoint.
   *@param ipt   Index of MCPoint
   *@param x     x coordinate of point (global c.s.)
   *@param y     y coordinate of point (global c.s.)
   *@value  kTRUE if the point is inside the sensor, else kFALSE
   **/
  // Bool_t ActivateChannels(Int_t ipt, Double_t x, Double_t y);  // not implemented

  Double_t GetStripOrientation(Double_t x, Double_t y, Int_t iSide);
  Double_t GetDistance(Int_t iSide, Double_t chan1, Double_t chan2);
  Int_t GetDistance(Int_t iSide, Int_t chanMin, Int_t chanMax, Int_t chanTest);
  Double_t GetDistance2(Int_t iSide, Double_t chan1, Double_t chan2);

  // returns the part of sensor in which chan is located.
  // for most sensors it should return -1
  // only for the sensors where the channel order is broken
  Int_t GetSensorPart(Int_t iSide, Int_t chan);

  Double_t GetMeanChannel(Int_t iSide, Double_t chan1, Double_t weight1, Double_t chan2, Double_t weight2);

  Int_t GetNeighbours(Int_t iSide, Int_t iChan, Int_t &nChan1, Int_t &nChan2, Int_t &nChan3);

  /** Calculates the coordinates of the intersections of front strip i
   ** with back strip j in the global coordinate system
   *@param iFStrip   Front strip number
   *@param iBStrip   Back strip number
   *@param xCross    Vector of x coordinates of crossings [cm]
   *@param yCross    Vector of y coordinates of crossings [cm]
   *@value           Number of intersections
   **/
  // Int_t Intersect(Int_t iFStrip, Int_t iBStrip, std::vector<Double_t>& xCross, std::vector<Double_t>& yCross);  // not implemented

  Int_t Intersect(Double_t iFStrip, Double_t iBStrip, Double_t &xCross, Double_t &yCross, Double_t &zCross);

  Int_t Intersect(Double_t iFStrip, Double_t iBStrip, Double_t &xCross, Double_t &yCross, Double_t &zCross, Double_t &dr, Double_t &dp);

  Int_t Intersect(Double_t iFStrip, Double_t iBStrip, Double_t &xCross, Double_t &yCross, Double_t &zCross, Double_t &dx, Double_t &dy, Double_t &dr, Double_t &dp);

  // Int_t IntersectClusters(Double_t fChan, Double_t bChan, Double_t& xCross, Double_t& yCross, Double_t& zCross);  // not implemented

  /** The index of the MCPoint that has caused a combination of
   ** front and back strip to be fired. Returns -1 for combinations
   ** of strips fired by different points (fake hits)
   *@param iFStrip   Front strip number
   *@param iBStrip   Back strip number
   *@value Index of MCPoint
   **/
  // Int_t PointIndex(Int_t iFStrip, Int_t IBStrip);  // not implemented

  /** Clear the maps of fired strips **/
  void Reset();

  /** Screen output  **/
  void Print();

 private:
  /** -------------   Data members   --------------------------**/

  Int_t fDetectorId;       // Unique detector ID
  Int_t fType;             // Sensor type
  Double_t fPosition[3];   // Coordinates of the sensor centre [cm]
  Double_t fRotation;      // Rotation angle in global c.m. [rad]
  Double_t fInnerRadius;   // Inner radius of the sensor [cm]
  Double_t fOuterRadius;   // Outer radius of the sensor [cm]
  Double_t fD;             // thickness of the sensor [cm]
  Double_t fStripAngle[2]; // Strips angle
  Double_t fPitch[2];      // Strip readout pitch or pixel size in x/y

  /** Number of channels in front and back plane **/
  Int_t fNChannelsFront;
  Int_t fNChannelsBack;

  /** Errors of hit coordinates. For pixel sensor: size/sqrt(12),
   ** for strip sensors RMS of overlap of front and back side strip **/
  // Logically, this belongs to the HitFinder, but it is here
  // for performance reasons: to be executed once per sensor, not for
  // each MCPoint
  Double_t fSigmaX;  // RMS in x, global c.s. [cm]
  Double_t fSigmaY;  // RMS in y, global c.s. [cm]
  Double_t fSigmaXY; // Covariance in global c.s. [cm**2]

  /** -------------   Private methods   ------------------------**/

  /** Strip number of a point in the front plane.
   ** Returns -1 if the point is outside the sensor
   *@param x  x coordinate of point (global c.s.)
   *@param y  y coordinate of point (global c.s.)
   *@value  Number of strip hitted by the point
   **/
  Int_t FrontStripNumber(Double_t x, Double_t y) const;

  /** Strip number of a point in the back plane.
   ** Returns -1 if the point is outside the sensor
   *@param x  x coordinate of point (global c.s.)
   *@param y  y coordinate of point (global c.s.)
   *@value  Number of strip hitted by the point
   **/
  Int_t BackStripNumber(Double_t x, Double_t y) const;

  /** Calculate coordinates in internal coordinate system
   *@param x     x coordinate in global system
   *@param y     y coordinate in global system
   *@param xint  internal x coordinate (return)
   *@param yint  internal y coordinate (return)
   *@value kTRUE if point is inside the sensor
   **/
  Bool_t IntCoord(Double_t x, Double_t y, Double_t z, Double_t &xint, Double_t &yint, Double_t &zint) const;

  /** Check whether a point is inside the sensor
   *@param xpt  x coordinate of point (internal system)
   *@param ypt  y coordinate of point (internal system)
   *@value kTRUE if inside sensor, else kFALSE
   **/
  Bool_t IsInside(Double_t xint, Double_t yint) const;

  ClassDef(PndGemSensor, 1);
};

#endif
