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

#ifndef PndTrkCTGeometryCalculations_H
#define PndTrkCTGeometryCalculations_H 1

// Root includes
#include "TROOT.h"

class PndTrkCTGeometryCalculations : public TObject {

 public:
  /** Default constructor **/
  PndTrkCTGeometryCalculations(){};

  /** Destructor **/
  ~PndTrkCTGeometryCalculations(){};

  Double_t CalculateArcLength(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge,
                              Double_t *Xcross, // entrance-exit point
                              Double_t *Ycross  // entrance-exit point
  );

  bool CalculateCircleThru3Points(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t x3, Double_t y3, Double_t *o_x, Double_t *o_y, Double_t *r_r);

  void calculateintersections(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t C0x, Double_t C0y, Double_t C0z, Double_t r, Double_t vx, Double_t vy, Double_t vz, Int_t *STATUS,
                              Double_t *POINTS);

  void CalculateSandZ(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t skewnum, Double_t info[][7], Double_t *WDX, Double_t *WDY, Double_t *WDZ, Double_t S[2], Double_t Z[2],
                      Double_t Zdrift[2], Double_t Zerror[2]);

  void CalculateSandZ2(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t skewnum, Double_t info[][7], Double_t *WDX, Double_t *WDY, Double_t *WDZ, Double_t S[2], Double_t Sdrift[2],
                       Double_t Z[2], Double_t Zdrift[2], Double_t Zerror[2]);

  void ChooseEntranceExitbis(Double_t Oxx, Double_t Oyy, Short_t Charge, Double_t FiStart, Short_t nIntersections, Double_t *XintersectionList, Double_t *YintersectionList,
                             Double_t Xcross[2], // output
                             Double_t Ycross[2]  // output
  );

  void ChooseEntranceExit3(Double_t Oxx, Double_t Oyy, Short_t Charge, Double_t FiStart, Short_t nIntersections,
                           Double_t *XintersectionList, // input and output;
                           Double_t *YintersectionList, // input and output;
                           Double_t *FiOrderedList      // output;
  );

  Double_t Dist_SZ(Double_t Rr, Double_t KAPPA, Double_t FI0, Double_t ZED, Double_t S, Int_t *nrounds);

  Double_t Dist_SZ_bis(Double_t Rr,              // input
                       Double_t KAPPA,           // input
                       Double_t FI0,             // input
                       Double_t ZED,             // input
                       Double_t S,               // input
                       Short_t n_allowed_rounds, // input, number of maximum allowed turns. That means that the number of turns
                                                 // can go from 0 to  n_allowed_rounds.  It can be negative, in that case
                                                 // the number of allowed turns go from n_allowed_rounds to 0;
                       Double_t signPz,          // input, it indicates if the tracks goes forward (Pz>0) or backwords (Pz<0);
                       Double_t &chosenS         // output, the S position corrisponding to the point minimizing the distance;
                                                 // this S can be < 0 or > 2PI;
  );

  Double_t FindDistance(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t tanlow, Double_t tanmid, Double_t tanup, Double_t alfa, Double_t beta, Double_t gamma);

  void FindingParallelTrackAngularRange(Double_t oX, Double_t oY, Double_t Rr, Short_t Charge,
                                        Double_t *Fi_low_limit, // Fi (in XY Helix frame) lower limit using
                                                                // the Stt detector minimum/maximum radius
                                                                // Fi_low_limit is ALWAYS between 0. and 2PI
                                        Double_t *Fi_up_limit,  // Fi (in XY Helix frame) upper limit using
                                                                // the Stt detector maximum/minimum radius
                                                                // Fi_up_limit is ALWAYS > Fi_low_limit and
                                                                // possibly > 2PI.
                                        Short_t *status,
                                        Double_t Rmin, // Rmin of cylindrical volume intersected by track;
                                        Double_t Rmax  // Rmax of cylindrical volume intersected by track;
  );

  void FindingParallelTrackAngularRange2(Double_t oX,            // input;
                                         Double_t oY,            // input;
                                         Double_t Rma,           // Rmax of cylindrical volume intersected by track;
                                         Double_t Rmi,           // Rmin of cylindrical volume intersected by track;
                                         Double_t Rr,            // input;
                                         Double_t *Fi_low_limit, // output; Fi (in XY Helix frame) lower limit using
                                                                 // the Stt detector minimum/maximum radius
                                                                 // Fi_low_limit is ALWAYS between 0. and 2PI;
                                         Double_t *Fi_up_limit,  // output;	// Fi (in XY Helix frame) upper limit using
                                                                 // the Stt detector maximum/minimum radius
                                                                 // Fi_up_limit is ALWAYS > Fi_low_limit and
                                                                 // possibly > 2PI;
                                         Short_t *status         // output;
  );

  Short_t FindIntersectionsOuterCircle(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t RMax, Double_t Xcross[2], Double_t Ycross[2]);

  Short_t FindTrackEntranceExitbiHexagonLeft(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                             Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                             Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2]);

  Short_t FindTrackEntranceExitbiHexagonLeft2(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                              Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                              Double_t ApotemaMax, Double_t XintersectionList[16], Double_t YintersectionList[16], Double_t FiOrderedList[16]);

  Short_t FindTrackEntranceExitbiHexagonRight(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                              Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                              Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2]);

  Short_t FindTrackEntranceExitbiHexagonRight2(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                               Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                               Double_t ApotemaMax, Double_t XintersectionList[16], Double_t YintersectionList[16], Double_t FiOrderedList[16]);

  Short_t FindTrackEntranceExitHexagonCircleLeft(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                 Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                 Double_t ApotemaMax, Double_t GAP, Double_t Xcross[2], Double_t Ycross[2]);

  Short_t FindTrackEntranceExitHexagonCircleLeft2(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                  Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                  Double_t ApotemaMax, Double_t GAP, Double_t XintersectionList[12], Double_t YintersectionList[12], Double_t FiOrderedList[12]);

  Short_t FindTrackEntranceExitHexagonCircleRight(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                  Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                  Double_t ApotemaMax, Double_t GAP, Double_t Xcross[2], Double_t Ycross[2]);

  Short_t FindTrackEntranceExitHexagonCircleRight2(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                   Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                   Double_t ApotemaMax, Double_t GAP, Double_t XintersectionList[12], Double_t YintersectionList[12], Double_t FiOrderedList[12]);

  bool IntersectionCircle_Segment(Double_t a, // coefficients implicit equation.
                                  Double_t b, // of segment : a*x + b*y + c =0.
                                  Double_t c,
                                  Double_t P1x, // point delimiting the segment.
                                  Double_t P2x, // point delimiting the segment.
                                  Double_t P1y, // point delimiting the segment.
                                  Double_t P2y, // point delimiting the segment.
                                  Double_t Oxx, // center of circle.
                                  Double_t Oyy,
                                  Double_t Rr, // Radius of circle.
                                  Short_t *Nintersections, Double_t XintersectionList[2], Double_t YintersectionList[2], Double_t *distance);

  bool IntersectionCircle_Segment_forScitil(Double_t a, // coefficients implicit equation.
                                            Double_t b, // of segment : a*x + b*y + c =0.
                                            Double_t c,
                                            Double_t P1x, // point delimiting the segment.
                                            Double_t P2x, // point delimiting the segment.
                                            Double_t P1y, // point delimiting the segment.
                                            Double_t P2y, // point delimiting the segment.
                                            Double_t Oxx, // center of circle.
                                            Double_t Oyy,
                                            Double_t Rr,     // Radius of circle.
                                            Double_t factor, // to take into account errors in
                                                             // the determination of the circumference;
                                            Short_t *Nintersections, Double_t XintersectionList[2], Double_t YintersectionList[2], Double_t *distance);

  bool IntersectionSciTil_Circle(Double_t posizSciTilx, Double_t posizSciTily,
                                 Double_t Oxx, // center of circle.
                                 Double_t Oyy,
                                 Double_t Rr, // Radius of circle.
                                 Short_t *Nintersections, Double_t XintersectionList[2], Double_t YintersectionList[2]);

  Short_t IntersectionsWithClosedbiHexagonLeft(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr,
                                               Double_t Ami, // Apotema min of inner Hexagon;
                                               Double_t Ama, // Apotema max of outer Hexagon;

                                               //-------- outputs
                                               Short_t *nIntersections, Double_t *XintersectionList, Double_t *YintersectionList);

  Short_t IntersectionsWithClosedbiHexagonRight(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr,
                                                Double_t Ami, // Apotema min of inner Hexagon;
                                                Double_t Ama, // Apotema max of outer Hexagon;

                                                //-------- outputs
                                                Short_t *nIntersections, Double_t *XintersectionList, Double_t *YintersectionList);

  Short_t IntersectionsWithClosedPolygon(Double_t Oxx, Double_t Oyy, Double_t Rr,
                                         Double_t Rmi, // Rmin of cylindrical volume intersected by track;
                                         Double_t Rma, // Rmax of cylindrical volume intersected by track;

                                         //-------- outputs
                                         Short_t nIntersections[2], Double_t XintersectionList[][2], Double_t YintersectionList[][2]);

  Short_t IntersectionsWithGapSemicircle(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t gap, bool left, Double_t Rma, Double_t *XintersectionList, Double_t *YintersectionList);

  Short_t IntersectionsWithOpenPolygon(Double_t Oxx,   // Track parameter
                                       Double_t Oyy,   // Track parameter
                                       Double_t Rr,    // Track parameter
                                       Short_t nSides, // input, n. of Sides of open Polygon.
                                       Double_t *a,    //  coefficient of formula :  aX + bY + c = 0 defining
                                       Double_t *b,    //  the Polygon sides.
                                       Double_t *c,
                                       Double_t *side_x, // X,Y coordinate of the Sides vertices (in sequence, following
                                       Double_t *side_y, // the Polygon along.
                                       //-------- outputs
                                       Double_t *XintersectionList, // XintersectionList
                                       Double_t *YintersectionList  // YintersectionList.
  );

  bool IsInsideArc(Double_t Oxx, Double_t Oyy, Short_t Charge, Double_t Xcross[2], Double_t Ycross[2], Double_t Spoint);

  bool IsInMvdMiniDisk1_97to1_99(Double_t X, //  X coordinate of point;
                                 Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk1_97to1_99withMargin(Double_t X,       //  X coordinate of point;
                                           Double_t Y,       //  Y coordinate of point;
                                           Double_t xmargin, //  safety margin in X coordinate;
                                           Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk2_41to2_43(Double_t X, //  X coordinate of point;
                                 Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk2_41to2_43withMargin(Double_t X,       //  X coordinate of point;
                                           Double_t Y,       //  Y coordinate of point;
                                           Double_t xmargin, //  safety margin in X coordinate;
                                           Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk3_97to3_99(Double_t X, //  X coordinate of point;
                                 Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk3_97to3_99withMargin(Double_t X,       //  X coordinate of point;
                                           Double_t Y,       //  Y coordinate of point;
                                           Double_t xmargin, //  safety margin in X coordinate;
                                           Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk4_41to4_43(Double_t X, //  X coordinate of point;
                                 Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk4_41to4_43withMargin(Double_t X,       //  X coordinate of point;
                                           Double_t Y,       //  Y coordinate of point;
                                           Double_t xmargin, //  safety margin in X coordinate;
                                           Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk6_97to6_99(Double_t X, //  X coordinate of point;
                                 Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk6_97to6_99withMargin(Double_t X,       //  X coordinate of point;
                                           Double_t Y,       //  Y coordinate of point;
                                           Double_t xmargin, //  safety margin in X coordinate;
                                           Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk7_41to7_43(Double_t X, //  X coordinate of point;
                                 Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk7_41to7_43withMargin(Double_t X,       //  X coordinate of point;
                                           Double_t Y,       //  Y coordinate of point;
                                           Double_t xmargin, //  safety margin in X coordinate;
                                           Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk9_97to9_99(Double_t X, //  X coordinate of point;
                                 Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk9_97to9_99withMargin(Double_t X,       //  X coordinate of point;
                                           Double_t Y,       //  Y coordinate of point;
                                           Double_t xmargin, //  safety margin in X coordinate;
                                           Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk10_41to10_43(Double_t X, //  X coordinate of point;
                                   Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk10_41to10_43withMargin(Double_t X,       //  X coordinate of point;
                                             Double_t Y,       //  Y coordinate of point;
                                             Double_t xmargin, //  safety margin in X coordinate;
                                             Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk14_77to14_79(Double_t X, //  X coordinate of point;
                                   Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk14_77to14_79withMargin(Double_t X,       //  X coordinate of point;
                                             Double_t Y,       //  Y coordinate of point;
                                             Double_t xmargin, //  safety margin in X coordinate;
                                             Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk15_21to15_23(Double_t X, //  X coordinate of point;
                                   Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk15_21to15_23withMargin(Double_t X,       //  X coordinate of point;
                                             Double_t Y,       //  Y coordinate of point;
                                             Double_t xmargin, //  safety margin in X coordinate;
                                             Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk21_77to21_79(Double_t X, //  X coordinate of point;
                                   Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk21_77to21_79withMargin(Double_t X,       //  X coordinate of point;
                                             Double_t Y,       //  Y coordinate of point;
                                             Double_t xmargin, //  safety margin in X coordinate;
                                             Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInMvdMiniDisk22_21to22_23(Double_t X, //  X coordinate of point;
                                   Double_t Y  //  Y coordinate of point;
  );

  bool IsInMvdMiniDisk22_21to22_23withMargin(Double_t X,       //  X coordinate of point;
                                             Double_t Y,       //  Y coordinate of point;
                                             Double_t xmargin, //  safety margin in X coordinate;
                                             Double_t ymargin  //  safety margin in Y coordinate;
  );

  bool IsInTargetPipe(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t fi0, Double_t kappa, Short_t charge, Double_t gap);

  bool IsInternal(Double_t Px, // point
                  Double_t Py, Double_t Xtraslation, Double_t Ytraslation, Double_t Theta);

  void ListAxialSectorsCrossedbyTrack_and_Hits(Double_t Ox,              // input;
                                               Double_t Oy,              // input;
                                               Double_t R,               // input;
                                               Double_t Charge,          // input;
                                               Short_t nHits,            // input;
                                               Short_t *ListHits,        // input;
                                               Double_t info[][7],       // input;
                                               Short_t &nArcs_populated, // output; # Arcs of trajectory populated by at least 1 axial hit; this is <= 56;
                                               Short_t nHitsInArc[56],   // output; number of hits in each Sector; if the maximun # of Intersected Sector is 56,
                                                                         //  than the maximum # of Arcs is 28;

                                               Short_t (*ListHitsInArc)[56] // output; ordered list of hits in each Arc (from first to last
                                                                            // according to the charge of the particle;if the maximum
                                                                            //  # of Intersected Sector is 56, than the maximum # of Arcs is 28;
  );

  ClassDef(PndTrkCTGeometryCalculations, 1);
};

#endif
