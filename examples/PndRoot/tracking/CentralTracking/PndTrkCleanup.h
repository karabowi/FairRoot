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

#ifndef PndTrkCleanup_H
#define PndTrkCleanup_H 1

#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkConstants.h"

// Root includes
#include "TROOT.h"

class PndTrkCleanup : public TObject {

 public:
  /** Default constructor **/
  PndTrkCleanup(){};
  /** Destructor **/
  ~PndTrkCleanup(){};

  bool BadTrack_ParStt(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t Stawradius, Short_t Charge,
                       Double_t Xcross[2], // Xcross[0]=point of entrance;
                                           //  Xcross[1]=point of exit.
                       Double_t Ycross[2], Short_t nHits, Short_t *ListHits, Double_t info[][7], int istampa, Double_t cut, Short_t maxnum,
                       Short_t islack // uncertainty allowed as far as
                                      // the n. of hits that should be present in a given section of the Stt track.
  );

  bool GoodTrack(Double_t info[][7],            // input
                 bool farthest_hit_is_boundary, // input
                 Double_t Ox,                   // input; center of the current track;
                 Double_t Oy,                   // input; center of the current track;
                 Double_t R,                    // input; Radius of the current track;
                 Short_t Charge,                // input; Charge of the current track;
                 Short_t nHits,                 // input
                 Short_t *ListHits,             // input
                 Short_t *StrawCode,            // first straw boundary code (a straw can belong to 2 boundaries);
                 Short_t *StrawCode2,
                 Short_t *TubeID,                // input
                 Short_t *nParContiguous,        // input
                 Short_t ListParContiguous[][6], // input
                 Double_t *xTube,                // input
                 Double_t *yTube,                // input
                 Double_t *zTube,                // input
                 Double_t *xxyyTube,             // input

                 Short_t &holes // input and output

  );

  bool IsThereMvdHitInBarrel(Double_t Xintersect, // input, X position of the point of crossing as calculated from the track trajectory;
                             Double_t Yintersect, // input, Y position of the point of crossing as calculated from the track trajectory;
                             Double_t Zintersect, // input, Z position of the point of crossing as calculated from the track trajectory;

                             Short_t nPixelHitsinTrack,        // number of Mvd Pixel hits in this track;
                             Short_t *ListMvdPixelHitsinTrack, // ... and their list;
                             Double_t *XMvdPixel,              // list of the X positions of ALL Mvd hits of the event;
                             Double_t *YMvdPixel,              // list of the Y positions of ALL Mvd hits of the event;
                             Double_t *ZMvdPixel,              // list of the Z positions of ALL Mvd hits of the event;
                             Short_t nStripHitsinTrack,        // number of Mvd Strip hits in this track;
                             Short_t *ListMvdStripHitsinTrack, // ... and their list;
                             Double_t *XMvdStrip,              // list of the X positions of ALL Mvd hits of the event;
                             Double_t *YMvdStrip,              // list of the Y positions of ALL Mvd hits of the event;
                             Double_t *ZMvdStrip               // list of the Z positions of ALL Mvd hits of the event;
  );

  bool IsThereHitInMvdMiniDisk(Double_t ZLayerBegin,             // Z of the beginning of the layer (end of layer = + 0.02);
                               Short_t nPixelHitsinTrack,        // number of Mvd Pixel hits in this track;
                               Short_t *ListMvdPixelHitsinTrack, // ... and their list;
                               Double_t *XMvdPixel, Double_t *YMvdPixel, Double_t *ZMvdPixel,

                               Short_t nStripHitsinTrack,        // number of Mvd Strip hits in this track;
                               Short_t *ListMvdStripHitsinTrack, // ... and their list;
                               Double_t *XMvdStrip, Double_t *YMvdStrip, Double_t *ZMvdStrip,

                               PndTrkCTGeometryCalculations *GeometryCalculator // pointer to
                                                                                // the class doing the geometrical calculations;

  );

  bool MvdCleanup(Double_t Ox, Double_t Oy, Double_t R, Double_t fi0, Double_t kappa, Double_t charge,
                  Double_t *XMvdPixel,       // list of the X positions of ALL Mvd hits of the event;
                  Double_t *XMvdStrip,       // list of the X positions of ALL Mvd hits of the event;
                  Double_t *YMvdPixel,       // list of the Y positions of ALL Mvd hits of the event;
                  Double_t *YMvdStrip,       // list of the Y positions of ALL Mvd hits of the event;
                  Double_t *ZMvdPixel,       // list of the Z positions of ALL Mvd hits of the event;
                  Double_t *ZMvdStrip,       // list of the Z positions of ALL Mvd hits of the event;
                  Short_t nPixelHitsinTrack, // number of Mvd Pixel hits in this track;
                  Short_t *ListMvdPixelHitsinTrack,
                  Short_t nStripHitsinTrack, // number of Mvd Strip hits in this track;
                  Short_t *ListMvdStripHitsinTrack, Double_t extra_distance, Double_t extra_distance_Z, PndTrkCTGeometryCalculations *GeomCalculator);

  bool MvdCleanup_prova(Double_t Ox, Double_t Oy, Double_t R, Double_t fi0, Double_t kappa, Double_t charge, Double_t semiverticalgap, Short_t nMvdHits,
                        PndTrkCTGeometryCalculations *GeomCalculator);

  void SeparateInnerOuterParallel(

    // input
    Short_t nHits, Short_t *ListHits, Double_t info[][7], Double_t RStrawDetInnerParMax,

    // output
    Short_t *nInnerHits, Short_t *ListInnerHits, Short_t *nOuterHits, Short_t *ListOuterHits,

    Short_t *nInnerHitsLeft, Short_t *ListInnerHitsLeft, Short_t *nInnerHitsRight, Short_t *ListInnerHitsRight,

    Short_t *nOuterHitsLeft, Short_t *ListOuterHitsLeft, Short_t *nOuterHitsRight, Short_t *ListOuterHitsRight);

  void SeparateInnerOuterRightLeftAxialStt(

    // input
    Double_t info[][7], Short_t *ListHits, Short_t nHits, Double_t RStrawDetInnerParMax,

    // output

    Short_t *ListInnerHitsLeft, Short_t *ListInnerHitsRight, Short_t *ListOuterHitsLeft, Short_t *ListOuterHitsRight, Short_t *nInnerHitsLeft, Short_t *nInnerHitsRight,
    Short_t *nOuterHitsLeft, Short_t *nOuterHitsRight);

  bool SttParalCleanup(Double_t ApotemaInnerParMax, Double_t ApotemaMinOuterPar, Short_t Charge, Double_t FI0, Double_t FiLimitAdmissible, Double_t GAP, Double_t info[][7],
                       int istampa, int IVOLTE, Short_t *Listofhits, Short_t nHits, Double_t Oxx, Double_t Oyy, Double_t Rr,
                       Double_t RStrawDetMax, // radius of circle encompassing ALL
                                              // the straw detector;
                       Double_t RStrawDetMin, Double_t Start[3], Double_t Strawradius);

  bool SttSkewCleanup(Double_t ApotemaMaxSkew, Double_t ApotemaMinSkew, Short_t Charge,
                      Double_t cut, // cut distance (in cm).
                      Double_t FI0, Double_t FiLimitAdmissible, Double_t GAP, Double_t info[][7], int istampa, int IVOLTE, Short_t *Listofhits,
                      Short_t maxnum, // max number allowed of failures to pass the cut.
                      int MAXSTTHITS, Short_t nHits, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t RStrawDetMax, Double_t *S, Double_t Start[3], Double_t Strawradius);

  bool TrackCleanup(Double_t ApotemaMaxInnerPar, Double_t ApotemaMaxSkew, Double_t ApotemaMinOuterPar, Double_t ApotemaMinSkew, Double_t *auxS, Short_t Charge, Double_t FI0,
                    Double_t GAP, Double_t info[][7], int istampa, int IVOLTE, Double_t KAPPA, Short_t *ListHitsPar, Short_t *ListHitsSkew, int MAXSTTHITS,
                    Short_t nHitsPar,  // n. hits parall Stt
                    Short_t nHitsSkew, // n. hits parall Stt
                    Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t RStrawDetMax, Double_t RStrawDetMin, Double_t Start[3], Double_t Strawradius);

  bool Track_Crosses_MvdBarrelFullAzimuthalCoverage(Double_t Ox,     // track trajectory center;
                                                    Double_t Oy,     // track trajectory center;
                                                    Double_t R,      // track trajectory radius;
                                                    Double_t fi0,    // FI0 of the Helix of the particle trajectory;
                                                    Double_t kappa,  // KAPPA of the Helix of the particle trajectory;
                                                    Double_t charge, // charge of the particle;

                                                    const Double_t Zlow,                              // Z low limit of this barrel;
                                                    const Double_t Zup,                               // Z upper limit of this barrel;
                                                    Double_t RBarrel,                                 // R of this barrel at which the intersection of the particle
                                                                                                      // trajectory is calculated;
                                                    PndTrkCTGeometryCalculations *GeometryCalculator, // pointer
                                                    Double_t extra_distance_Z,                        // in cm; extra distance allowed during decision
                                                                                                      // if there should be an hit in a Mvd sensitive layer;
                                                    Double_t &Xintersect,                             // output, X position of the point of crossing;
                                                    Double_t &Yintersect,                             // output, Y position of the point of crossing;
                                                    Double_t &Zintersect                              // output, Z position of the point of crossing;
  );

  bool Track_Crosses_MvdBarrelPartialAzimuthalCoverage(

    // in this function it is assumed to deal with an Mvd Barrel section composed of an Inner Barrel with
    // RMin radius and an Outer Barrel with RMax radius.
    // Both the Inner and Outer Barrel have their own azimuthal (partial) coverage defined by a number of
    // azimuthal gaps ( ngapInner and ngapOuter respectively, maximum 4 gaps) with a certain range in Fi
    // defined in the arrays   :    gap_lowInner - gap_upInner  and   gap_lowOuter - gap_upOuter respectively;

    Double_t Ox,     // track trajectory center;
    Double_t Oy,     // track trajectory center;
    Double_t R,      // track trajectory radius;
    Double_t fi0,    // FI0 of the Helix of the particle trajectory;
    Double_t kappa,  // KAPPA of the Helix of the particle trajectory;
    Double_t charge, // charge of the particle;

    Double_t Zlow, // Z low limit of this barrel;
    Double_t Zup,  // Z upper limit of this barrel;

    Double_t RInnerBarrel,        // R Minimum of this barrel at which the intersection of the particle
                                  // trajectory is calculated;
    int ngapInner,                // number of gaps in the azimuthal coverage;
    const Double_t *gap_lowInner, // array of low limits of the range of the azimuthal gaps (radians);
    const Double_t *gap_upInner,  // array of upper limits of the range of the azimuthal gaps (radians);

    Double_t ROuterBarrel,        // R Minimum of this barrel at which the intersection of the particle
                                  // trajectory is calculated;
    int ngapOuter,                // number of gaps in the azimuthal coverage;
    const Double_t *gap_lowOuter, // array of low limits of the range of the azimuthal gaps (radians);
    const Double_t *gap_upOuter,  // array of upper limits of the range of the azimuthal gaps (radians);

    PndTrkCTGeometryCalculations *GeometryCalculator, // pointer
    Double_t extra_distance_Z,                        // in cm; extra distance allowed during decision
                                                      // if there should be an hit in a Mvd sensitive layer;

    Double_t *Xintersect, // output, X position of the point of crossing track-Inner Barrel
                          // and track-Outer Barrel;
    Double_t *Yintersect, // output, Y position of the point of crossing;
    Double_t *Zintersect  // output, Z position of the point of crossing;
  );

  bool Track_Crosses_MvdMiniDisk_withMargin(Double_t ZLayerBegin, // Z of the beginning of the layer (end of layer = + 0.02);
                                            Double_t xmargin,     //  safety margin in X coordinate;
                                            Double_t ymargin,     //  safety margin in Y coordinate;

                                            Double_t Ox,     // track trajectory center;
                                            Double_t Oy,     // track trajectory center;
                                            Double_t R,      // track trajectory radius;
                                            Double_t fi0,    // FI0 of the Helix of the particle trajectory;
                                            Double_t kappa,  // KAPPA of the Helix of the particle trajectory;
                                            Double_t charge, // charge of the particle;

                                            PndTrkCTGeometryCalculations *GeometryCalculator // pointer to
                                                                                             // the class doing the geometrical calculations;
  );

  bool XYCleanup(
    // general infos about the axial Straws;
    int istampa, Double_t info[][7], Short_t (*ListParContiguous)[6], Short_t *nParContiguous, Short_t *StrawCode, Short_t *StrawCode2, Short_t *TubeID, Double_t *xTube,
    Double_t *yTube, Double_t *zTube, Double_t *xxyyTube,
    // the following are the info of the track under scrutiny;
    Double_t Ox, Double_t Oy, Double_t R, Short_t Charge, Short_t *ListHits, Short_t nHits, Double_t RStrawDetInnerParMax,
    Short_t nScitilHitsInTrack,     // input, # of SciTil hits in the current track;
    Short_t *ListSciTilHitsinTrack, // input, list of SciTil hits in the current track;
    Double_t posizSciTil[][3]       // input, info on all the SciTil position;
  );

  ClassDef(PndTrkCleanup, 1);
};

#endif
