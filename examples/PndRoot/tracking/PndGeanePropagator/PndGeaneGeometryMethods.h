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

#ifndef PNDGEANEGEOMETRYMETHODS_H
#define PNDGEANEGEOMETRYMETHODS_H

#include "TNamed.h"
#include "TVector3.h"
#include "PndPropagator.h"

class PndGeaneGeometryMethods : public TNamed
{ 
 public:
  PndGeaneGeometryMethods()
    : TNamed("PndGeaneGeometryMethods", "Helper class for geometry methods used in PndGeanePro.")
    {};
  virtual ~PndGeaneGeometryMethods() {};

  /**
   * @brief Closest approach to a line from 2 GEANE points.
   * METHOD: The nearest points on the two lines x1,x2 and w1,w2 is found.
   *         The method is described in:
   *         http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
   *         http://www.geometrictools.com/Documentation/DistanceLine3Line3.pdf.
   * @param x1 first closest approach GEANE point
   * @param x2 second closest approach GEANE point
   * @param w1 first anchor point of the line (wire) to approach
   * @param w2 second anchor point of the line (wire) to approach
   * @return PCAOutputStruct containing:
   *         - point of closest approach on track,
   *         - point of closest apporach on wire,
   *         - distance between Pfinal and w1
   *         - track length to add to the GEANE length of x1,
   *         - PCAStatusFlag
   *            = 0 if successful
   *            = 1 if Pwire is outside [w1,w2]:
   *                In this case when w1 and w2 are the extremes of the wire
   *                the user could rerun the procedure by calling Track3ToPoint or Track2ToPoint
   *                where the point is w1 or w2.
   *            = 2 if the two lines are parallel and the solution does not exists.
   * Authors: Andrea Fontana and Alberto Rotondi 20 May 2007
   */
  PndProp::PCAOutputStruct Track2ToLine(TVector3 x1,
                                        TVector3 x2,
                                        TVector3 w1,
                                        TVector3 w2);

  /**
   * @brief Closest approach to a point from 2 GEANE points.
   * METHOD: The nearest point to w1 on the line x1, x2 is found.
   *         Elementary vector calculus is used.
   * @param x1 first closest approach GEANE point
   * @param x2 second closest approach GEANE point
   * @param point point to approach
   * @return PCAOutputStruct containing:
   *         - point of closest approach on track,
   *         - distance between Pfinal and w1,
   *         - track length to add to the GEANE length of x1,
   *         - PCAStatusFlag
   *           = 0 if successful
   *           = 1 if points are on straight line
   * Authors: Andrea Fontana and Alberto Rotondi May 2007
   */
  PndProp::PCAOutputStruct Track2ToPoint(TVector3 x1,
                                         TVector3 x2,
                                         TVector3 point);

  /**
   * @brief Find the closest approach points between a curve (helix) and a line (wire).
   * METHOD: The classical Eberly method is used, see
   *         http://www.geometrictools.com/Documentation/DistanceLine3Circle.pdf.
   *         See also www.geometrictools.com for the other formulae used in this interface.
   *         The 4-degree polynomial resulting for the line parameter t is solved with
   *         the efficient SolveQuartic Root routine. The minimal distance solution is found
   *         by using our Track3ToPoint routine.
   * @param x1 first closest approach GEANE point
   * @param x2 second closest approach GEANE point
   * @param x3 third closest approach GEANE point
   * @param w1 first anchor point of the line (wire) to approach
   * @param w2 second anchor point of the line (wire) to approach
   * @return PCAOutputStruct containing: 
   *         - point of closest approach on track,
   *         - point of closest approach on wire,
   *         - distance between Pfinal and w1,
   *         - track length to add to the GEANE length of x1,
   *         - radius of circle,
   *         - PCAStatusFlag:
   *            = 1 if the points are on a straight line
   *                within the precision of the method (20 micron).
   *                In this case the user should call Track2ToPoint.
   *            = 2 if Pwire is outside [w1,w2].
   *                In this case when w1 and w2 are the extremes
   *                of the wire the user could redo the procedure
   *                by calling Track3ToPoint where the Point is w1 or w2.
   *            = 3 if both condition 1 and 2 are encountered.
   *            = 4 if the method failed for mathematical reasons.
   *                In this case the user should use Track2ToLine where
   *                x1 = x1 and x2 = x3.
   * Authors: Andrea Fontana and Alberto Rotondi 20 June 2007
   */
  PndProp::PCAOutputStruct Track3ToLine(TVector3 x1,
                                        TVector3 x2,
                                        TVector3 x3,
                                        TVector3 w1,
                                        TVector3 w2);

  /**
   * @brief Closest approach to a point from 3 GEANE points.
   * METHOD: First, the coordinate systems is transformed to the circle plane
   *         to have x1=(0,0), x2=(x2,0), x3=(x3,y3).
   *         Then, the point on the circle is found as the intersection between
   *         the circle and the line joining the circle center and
   *         the projection of the point on the circle plane. The 3D distance
   *         is found between w1 and this point on the circle.
   * @param x1 first closest approach GEANE point
   * @param x2 second closest approach GEANE point
   * @param x3 third closest approach GEANE point
   * @return PCAOutputStruct containing: 
   *         - point of closest approach on track,
   *         - distance between Pfinal and w1,
   *         - track length to add to the GEANE length of x1,
   *         - radius of circle,
   *         - PCAStatusFlag:
   *            = 1 if the points are on a straight line
   *                within the precision of the method (20 micron).
   *                In this case the user should call Track2ToPoint.
   *            = 2 if mathematical errors occure.
   * Authors: Andrea Fontana and Alberto Rotondi 20 June 2007
   */
  PndProp::PCAOutputStruct Track3ToPoint(TVector3 x1,
                                         TVector3 x2,
                                         TVector3 x3,
                                         TVector3 w1);

  ClassDef(PndGeaneGeometryMethods, 1);
};

#endif //PNDGEANEGEOMETRYMETHODS_H