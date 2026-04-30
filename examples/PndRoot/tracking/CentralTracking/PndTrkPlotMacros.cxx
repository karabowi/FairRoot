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

#include "PndTrkPlotMacros.h"
#include "PndTrkCTGeometryCalculations.h"
#include "FairMCPoint.h"

#include "PndMCTrack.h"
#include "PndTrkVectors.h"

#include <iostream>
#include <cmath>

// Root includes
#include "TROOT.h"

using namespace std;

//----------end of function PndTrkPlotMacros::disegnaAssiXY
// inizio cambio_in_perl ;
void PndTrkPlotMacros::disegnaAssiXY(FILE *MACRO, double xmin, double xmax, double ymin, double ymax)
{
  // fine cambio_in_perl ;

  fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", xmin, 0., xmax, 0., xmin, xmax);
  fprintf(MACRO, "Assex->SetTitle(\"X\");\n");
  fprintf(MACRO, "Assex->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assex->Draw();\n");
  fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0., ymin, 0., ymax, ymin, ymax);
  fprintf(MACRO, "Assey->SetTitle(\"Y\");\n");
  fprintf(MACRO, "Assey->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assey->Draw();\n");
}

//----------end of function PndTrkPlotMacros::disegnaAssiXY

//----------begin of function PndTrkPlotMacros::disegnaSciTilHit

// inizio cambio_in_perl ;
void PndTrkPlotMacros::disegnaSciTilHit(int colorcode, // goes in the SetColor function of root;
                                        Double_t DIMENSIONSCITIL, FILE *MACRO, double posx, double posy, int ScitilHit,
                                        int tipo // if 0 then SciTil draw in XY; if 1 then SciTil draw in SZ; else
                                                 // SciTil draw in UV.
)
{
  // fine cambio_in_perl ;
  double x1, x2, y1, y2, L, Rr, RR;

  L = DIMENSIONSCITIL / 2.;

  if (tipo == 0) { // SciTil disegnate in XY.
    Rr = sqrt(posx * posx + posy * posy);
    x1 = posx + posy * L / Rr;
    x2 = posx - posy * L / Rr;
    y1 = posy - posx * L / Rr;
    y2 = posy + posx * L / Rr;
  } else if (tipo == 1) { // SciTil disegnate in SZ.
    x1 = posx - L;
    x2 = posx + L;
    y1 = posy;
    y2 = posy;
  } else { // SciTil disegnate in UV.
    RR = posx * posx + posy * posy;
    Rr = sqrt(RR);
    x1 = posx + posy * L / Rr;
    x1 /= RR;
    x2 = posx - posy * L / Rr;
    x2 /= RR;
    y1 = posy - posx * L / Rr;
    y1 /= RR;
    y2 = posy + posx * L / Rr;
    y2 /= RR;
  }
  fprintf(MACRO, "TLine *Tile%d = new TLine(%f,%f,%f,%f);\n", ScitilHit, x1, y1, x2, y2);
  fprintf(MACRO, "Tile%d->SetLineColor(%d);\n", ScitilHit, colorcode);

  if (tipo == 0) {
    // disegna in XY.
    fprintf(MACRO, "Tile%d->SetLineWidth(2);\n", ScitilHit);
  } else if (tipo == 1) {
    // disegna in SZ.
    fprintf(MACRO, "Tile%d->SetLineWidth(3);\n", ScitilHit);
  } else {
    // disegna in UV.
    fprintf(MACRO, "Tile%d->SetLineWidth(3);\n", ScitilHit);
  }

  fprintf(MACRO, "Tile%d->Draw();\n", ScitilHit);
}

//----------end of function PndTrkPlotMacros::disegnaSciTilHit

//----------begin of function PndTrkPlotMacros::DrawBiHexagonInMacro
// inizio cambio_in_perl ;

void PndTrkPlotMacros::DrawBiHexagonInMacro(Double_t vgap, FILE *MACRO, Double_t Ami, Double_t Ama, Short_t color, char *name)
{
  // fine cambio_in_perl ;
  Short_t iside;

  // these are the points defining the sides of the biexhagon on the left(inner).
  Double_t side_x[] = {-vgap / 2., -Ama, -Ama, -vgap / 2., -vgap / 2., -Ami, -Ami, -vgap / 2., -vgap / 2.},
           side_y[] = {(-0.5 * vgap + 2. * Ama) / sqrt(3.),  Ama / sqrt(3.),  -Ama / sqrt(3.), -(-0.5 * vgap + 2. * Ama) / sqrt(3.),
                       -(-0.5 * vgap + 2. * Ami) / sqrt(3.), -Ami / sqrt(3.), Ami / sqrt(3.),  (-0.5 * vgap + 2. * Ami) / sqrt(3.),
                       (-0.5 * vgap + 2. * Ama) / sqrt(3.)};

  //  Inner straws left
  for (iside = 0; iside < 8; iside++) {
    fprintf(MACRO, "TLine* %sL%d = new TLine(%f,%f,%f,%f);\n", name, iside, side_x[iside], side_y[iside], side_x[iside + 1], side_y[iside + 1]);
    fprintf(MACRO, "%sL%d->SetLineColor(%d);\n", name, iside, color);
    fprintf(MACRO, "%sL%d->SetLineStyle(2);\n", name, iside);
    fprintf(MACRO, "%sL%d->Draw();\n", name, iside);
  }

  //  Inner straws right
  for (iside = 0; iside < 8; iside++) {
    fprintf(MACRO, "TLine* %sR%d = new TLine(%f,%f,%f,%f);\n", name, iside, -side_x[iside], side_y[iside], -side_x[iside + 1], side_y[iside + 1]);
    fprintf(MACRO, "%sR%d->SetLineColor(%d);\n", name, iside, color);
    fprintf(MACRO, "%sR%d->SetLineStyle(2);\n", name, iside);
    fprintf(MACRO, "%sR%d->Draw();\n", name, iside);
  }
}
//----------end of function PndTrkPlotMacros::DrawBiHexagonInMacro

//----------begin of function PndTrkPlotMacros::DrawHexagonCircleInMacro
// inizio cambio_in_perl ;

void PndTrkPlotMacros::DrawHexagonCircleInMacro(Double_t GAP, FILE *MACRO, Double_t ApotemaMin, Double_t Rma, Short_t color, char *name)
{
  // fine cambio_in_perl ;
  Short_t iside;

  Double_t angle1, angle2;

  const double PI = 3.141592654;

  // these are the points defining the sides of the exhagon on the left(outer).
  Double_t side_x[] = {-GAP / 2., -GAP / 2., -ApotemaMin, -ApotemaMin, -GAP / 2., -GAP / 2.},
           side_y[] = {sqrt(Rma * Rma - GAP * GAP / 4.), (2. * ApotemaMin - 0.5 * GAP) / sqrt(3.),  ApotemaMin / sqrt(3.),
                       -ApotemaMin / sqrt(3.),           -(2. * ApotemaMin - 0.5 * GAP) / sqrt(3.), -sqrt(Rma * Rma - GAP * GAP / 4.)};

  //  Outer straws left
  for (iside = 0; iside < 5; iside++) {
    fprintf(MACRO, "TLine* %sL%d = new TLine(%f,%f,%f,%f);\n", name, iside, side_x[iside], side_y[iside], side_x[iside + 1], side_y[iside + 1]);
    fprintf(MACRO, "%sL%d->SetLineColor(%d);\n", name, iside, color);
    fprintf(MACRO, "%sL%d->SetLineStyle(2);\n", name, iside);
    fprintf(MACRO, "%sL%d->Draw();\n", name, iside);
  }

  //  Outer straws right
  for (iside = 0; iside < 5; iside++) {
    fprintf(MACRO, "TLine* %sR%d = new TLine(%f,%f,%f,%f);\n", name, iside, -side_x[iside], side_y[iside], -side_x[iside + 1], side_y[iside + 1]);
    fprintf(MACRO, "%sR%d->SetLineColor(%d);\n", name, iside, color);
    fprintf(MACRO, "%sR%d->SetLineStyle(2);\n", name, iside);
    fprintf(MACRO, "%sR%d->Draw();\n", name, iside);
  }

  // drawing the left circle.
  angle1 = atan2(side_y[0], side_x[0]) * 180. / PI;
  angle2 = 360. + atan2(side_y[4], side_x[4]) * 180. / PI;
  fprintf(MACRO, "TEllipse* %sCircleL = new TEllipse(0.,0.,%f,%f,%f,%f);\n", name, Rma, Rma, angle1, angle2);
  fprintf(MACRO, "%sCircleL->SetFillStyle(0);\n", name);
  fprintf(MACRO, "%sCircleL->SetLineColor(%d);\n", name, color);
  fprintf(MACRO, "%sCircleL->Draw(\"only\");\n", name);

  // drawing the right circle.
  angle2 = atan2(side_y[0], -side_x[0]) * 180. / PI;
  angle1 = atan2(side_y[4], -side_x[4]) * 180. / PI;
  fprintf(MACRO, "TEllipse* %sCircleR = new TEllipse(0.,0.,%f,%f,%f,%f);\n", name, Rma, Rma, angle1, angle2);
  fprintf(MACRO, "%sCircleR->SetFillStyle(0);\n", name);
  fprintf(MACRO, "%sCircleR->SetLineColor(%d);\n", name, color);
  fprintf(MACRO, "%sCircleR->Draw(\"only\");\n", name);
}
//----------end of function PndTrkPlotMacros::DrawHexagonCircleInMacro

//---------- begin of function PndTrkPlotMacros::SttInfoXYZParal
// inizio cambio_in_perl ;
void PndTrkPlotMacros::SttInfoXYZParal(Vec<Double_t> *info, Short_t infopar, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t KAPPA, Double_t FI0, Short_t Charge, Double_t *Posiz)
{
  // fine cambio_in_perl ;

  Double_t const PI = 3.141592654;

  Double_t fi, norm, vers[2];

  vers[0] = Oxx - info->at(infopar * 7 + 0);
  vers[1] = Oyy - info->at(infopar * 7 + 1);
  norm = sqrt(vers[0] * vers[0] + vers[1] * vers[1]);

  if (norm < 1.e-20) {
    Posiz[0] = -999999999.;
    return;
  }

  if (fabs(Rr - fabs(norm - info->at(infopar * 7 + 3))) // distance trajectory-drift radius
      < fabs(Rr - (norm + info->at(infopar * 7 + 3)))) {

    Posiz[0] = info->at(infopar * 7 + 0) + info->at(infopar * 7 + 3) * vers[0] / norm;
    Posiz[1] = info->at(infopar * 7 + 1) + info->at(infopar * 7 + 3) * vers[1] / norm;

  } else {

    Posiz[0] = info->at(infopar * 7 + 0) - info->at(infopar * 7 + 3) * vers[0] / norm;
    Posiz[1] = info->at(infopar * 7 + 1) - info->at(infopar * 7 + 3) * vers[1] / norm;

  } // end of if ( fabs( Rr - fabs( Distance - info->at(infopar*7+3) ) ).....

  //   Posiz[0] = info->at(infopar*7+0) + info->at(infopar*7+3)*vers[0]/norm;
  //   Posiz[1] = info->at(infopar*7+1) + info->at(infopar*7+3)*vers[1]/norm;

  if (fabs(KAPPA) < 1.e-20) {
    Posiz[2] = -888888888.;
    return;
  }

  fi = atan2(-vers[1], -vers[0]);
  if (fi < 0.)
    fi += 2. * PI;

  if (Charge > 0) {
    if (fi > FI0)
      FI0 += 2. * PI;
    //    Posiz[2] = (FI0-fi)/KAPPA;
  } else {
    if (fi < FI0)
      fi += 2. * PI;
  }
  Posiz[2] = (fi - FI0) / KAPPA;

  return;
}

//---------- end function PndTrkPlotMacros::SttInfoXYZParal

//----------begin of function PndTrkPlotMacros::WriteAllMacros

void PndTrkPlotMacros::WriteAllMacros(

  PndTrkPlotMacros_InputData In_Put)
{

  bool intersect;

  Short_t Nint;

  int i, j, k;

  double const PI = 3.141592654;

  FairMCPoint *puntator;

  //  marker1 per cambioperl;

  //---------------  trasformazione   in variabili locali;
  // int MAXMCTRACKS = In_Put.MAXMCTRACKS; //[R.K. 01/2017] unused variable?
  int MAXMVDPIXELHITS = In_Put.MAXMVDPIXELHITS;
  int MAXMVDPIXELHITSINTRACK = In_Put.MAXMVDPIXELHITSINTRACK;
  int MAXMVDSTRIPHITS = In_Put.MAXMVDSTRIPHITS;
  int MAXMVDSTRIPHITSINTRACK = In_Put.MAXMVDSTRIPHITSINTRACK;
  // int MAXSCITILHITS = In_Put.MAXSCITILHITS; //[R.K. 01/2017] unused variable?
  int MAXSCITILHITSINTRACK = In_Put.MAXSCITILHITSINTRACK;
  int MAXSTTHITS = In_Put.MAXSTTHITS;
  int MAXSTTHITSINTRACK = In_Put.MAXSTTHITSINTRACK;
  int MAXTRACKSPEREVENT = In_Put.MAXTRACKSPEREVENT;
  int dime = MAXSTTHITSINTRACK + MAXMVDPIXELHITSINTRACK + MAXMVDSTRIPHITSINTRACK + MAXSCITILHITSINTRACK;

  Double_t APOTEMAMAXINNERPARSTRAW = In_Put.apotemamaxinnerparstraw;
  Double_t APOTEMAMAXSKEWSTRAW = In_Put.apotemamaxskewstraw;
  Double_t APOTEMAMINOUTERPARSTRAW = In_Put.apotemaminouterparstraw;
  Double_t APOTEMAMINSKEWSTRAW = In_Put.apotemaminskewstraw;
  Double_t BFIELD = In_Put.bfield;

  Vec<Short_t> Charge(In_Put.Charge, In_Put.MAXTRACKSPEREVENT, "Charge");

  Double_t CVEL = In_Put.cvel;
  Vec<Short_t> daTrackFoundaTrackMC(In_Put.daTrackFoundaTrackMC, In_Put.MAXTRACKSPEREVENT, "daTrackFoundaTrackMC");

  Double_t DIMENSIONSCITIL = In_Put.dimensionscitil;
  bool doMcComparison = In_Put.doMcComparison;
  Vec<Double_t> FI0(In_Put.FI0, In_Put.MAXTRACKSPEREVENT, "FI0");

  TClonesArray *fMCTrackArray = In_Put.fMCTrackArray;
  TClonesArray *fSttPointArray = In_Put.fSttPointArray;

  Vec<Double_t> info(In_Put.info, In_Put.MAXSTTHITS * 7, "info");

  Vec<bool> InclusionListSciTil(In_Put.InclusionListSciTil, In_Put.MAXSCITILHITS, "InclusionListSciTil");
  Vec<bool> InclusionListStt(In_Put.InclusionListStt, In_Put.MAXSTTHITS, "InclusionListStt");
  int IVOLTE = In_Put.IVOLTE;
  Vec<Double_t> KAPPA(In_Put.KAPPA, In_Put.MAXTRACKSPEREVENT, "KAPPA");
  Vec<bool> keepit(In_Put.keepit, In_Put.MAXTRACKSPEREVENT, "keepit");
  // int istampa = In_Put.istampa; //[R.K. 01/2017] unused variable?

  Vec<Short_t> ListMvdPixelHitsinTrack(In_Put.ListMvdPixelHitsinTrack, In_Put.MAXTRACKSPEREVENT * In_Put.MAXMVDPIXELHITSINTRACK, "ListMvdPixelHitsinTrack");

  Vec<Short_t> ListMvdStripHitsinTrack(In_Put.ListMvdStripHitsinTrack, In_Put.MAXTRACKSPEREVENT * In_Put.MAXMVDSTRIPHITSINTRACK, "ListMvdStripHitsinTrack");

  Vec<Short_t> ListSciTilHitsinTrack(In_Put.ListSciTilHitsinTrack, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSCITILHITSINTRACK, "ListSciTilHitsinTrack");

  Vec<Short_t> ListSttParHitsinTrack(In_Put.ListSttParHitsinTrack, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITSINTRACK, "ListSttParHitsinTrack");

  Vec<Short_t> ListSttSkewHitsinTrack(In_Put.ListSttSkewHitsinTrack, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITSINTRACK, "ListSttSkewHitsinTrack");

  Vec<Short_t> ListTrackCandHit(In_Put.ListTrackCandHit,
                                In_Put.MAXTRACKSPEREVENT * (In_Put.MAXSTTHITSINTRACK + In_Put.MAXMVDPIXELHITSINTRACK + In_Put.MAXMVDSTRIPHITSINTRACK + In_Put.MAXSCITILHITSINTRACK),
                                "ListTrackCandHit");

  Vec<Short_t> ListTrackCandHitType(
    In_Put.ListTrackCandHitType,
    In_Put.MAXTRACKSPEREVENT * (In_Put.MAXSTTHITSINTRACK + In_Put.MAXMVDPIXELHITSINTRACK + In_Put.MAXMVDSTRIPHITSINTRACK + In_Put.MAXSCITILHITSINTRACK), "ListTrackCandHitType");

  Vec<Short_t> MCMvdPixelAloneList(In_Put.MCMvdPixelAloneList, In_Put.nTotalCandidates * In_Put.nMvdPixelHit, "MCMvdPixelAloneList");

  Vec<Short_t> MCMvdStripAloneList(In_Put.MCMvdStripAloneList, In_Put.nTotalCandidates * In_Put.nMvdStripHit, "MCMvdStripAloneList");

  Vec<Short_t> MCParalAloneList(In_Put.MCParalAloneList, In_Put.MAXTRACKSPEREVENT * In_Put.nSttHit, "MCParalAloneList");

  Vec<Short_t> MCSkewAloneList(In_Put.MCSkewAloneList, In_Put.MAXTRACKSPEREVENT * In_Put.nSttHit, "MCSkewAloneList");

  Vec<Double_t> MCSkewAloneX(In_Put.MCSkewAloneX, In_Put.MAXSTTHITS, "MCSkewAloneX");
  Vec<Double_t> MCSkewAloneY(In_Put.MCSkewAloneY, In_Put.MAXSTTHITS, "MCSkewAloneY");

  Vec<Short_t> MvdPixelCommonList(In_Put.MvdPixelCommonList, In_Put.nTotalCandidates * In_Put.MAXMVDPIXELHITSINTRACK, "MvdPixelCommonList");

  Vec<Short_t> MvdPixelSpuriList(In_Put.MvdPixelSpuriList, In_Put.nTotalCandidates * In_Put.MAXMVDPIXELHITSINTRACK, "MvdPixelSpuriList");

  Vec<Short_t> MvdStripCommonList(In_Put.MvdStripCommonList, In_Put.nTotalCandidates * In_Put.MAXMVDSTRIPHITSINTRACK, "MvdStripCommonList");

  Vec<Short_t> MvdStripSpuriList(In_Put.MvdStripSpuriList, In_Put.nTotalCandidates * In_Put.MAXMVDSTRIPHITSINTRACK, "MvdStripSpuriList");

  Vec<Short_t> nMCMvdPixelAlone(In_Put.nMCMvdPixelAlone, In_Put.nTotalCandidates, "nMCMvdPixelAlone");
  Vec<Short_t> nMCMvdStripAlone(In_Put.nMCMvdStripAlone, In_Put.nTotalCandidates, "nMCMvdStripAlone");

  Vec<Short_t> nMCParalAlone(In_Put.nMCParalAlone, In_Put.MAXTRACKSPEREVENT, "nMCParalAlone");
  Vec<Short_t> nMCSkewAlone(In_Put.nMCSkewAlone, In_Put.MAXTRACKSPEREVENT, "nMCSkewAlone");

  Short_t nMCTracks = In_Put.nMCTracks;

  Vec<Short_t> nMvdPixelCommon(In_Put.nMvdPixelCommon, In_Put.nTotalCandidates, "nMvdPixelCommon");
  Short_t nMvdPixelHit = In_Put.nMvdPixelHit;
  Vec<Short_t> nMvdPixelHitsinTrack(In_Put.nMvdPixelHitsinTrack, In_Put.MAXTRACKSPEREVENT, "nMvdPixelHitsinTrack");
  Vec<Short_t> nMvdPixelSpuriinTrack(In_Put.nMvdPixelSpuriinTrack, In_Put.MAXTRACKSPEREVENT, "nMvdPixelSpuriinTrack");

  Vec<Short_t> nMvdStripCommon(In_Put.nMvdStripCommon, In_Put.nTotalCandidates, "nMvdStripCommon");
  Short_t nMvdStripHit = In_Put.nMvdStripHit;
  Vec<Short_t> nMvdStripHitsinTrack(In_Put.nMvdStripHitsinTrack, In_Put.MAXTRACKSPEREVENT, "nMvdStripHitsinTrack");
  Vec<Short_t> nMvdStripSpuriinTrack(In_Put.nMvdStripSpuriinTrack, In_Put.nTotalCandidates, "nMvdStripSpuriinTrack");

  Vec<Short_t> nParalCommon(In_Put.nParalCommon, In_Put.MAXTRACKSPEREVENT, "nParalCommon");

  Short_t nSciTilHits = In_Put.nSciTilHits;

  Vec<Short_t> nSciTilHitsinTrack(In_Put.nSciTilHitsinTrack, In_Put.MAXTRACKSPEREVENT, "nSciTilHitsinTrack");

  Vec<Short_t> nSkewCommon(In_Put.nSkewCommon, In_Put.MAXTRACKSPEREVENT, "nSkewCommon");

  Vec<Short_t> nSpuriParinTrack(In_Put.nSpuriParinTrack, In_Put.MAXTRACKSPEREVENT, "nSpuriParinTrack");

  Int_t nSttHit = In_Put.nSttHit;
  Int_t nSttParHit = In_Put.nSttParHit;
  Vec<Short_t> nSttParHitsinTrack(In_Put.nSttParHitsinTrack, In_Put.MAXTRACKSPEREVENT, "nSttParHitsinTrack");
  Int_t nSttSkewHit = In_Put.nSttSkewHit;
  Vec<Short_t> nSttSkewHitsinTrack(In_Put.nSttSkewHitsinTrack, In_Put.MAXTRACKSPEREVENT, "nSttSkewHitsinTrack");

  Short_t nTotalCandidates = In_Put.nTotalCandidates;
  Vec<Short_t> nTrackCandHit(In_Put.nTrackCandHit, In_Put.MAXTRACKSPEREVENT, "nTrackCandHit");

  Vec<Double_t> Ox(In_Put.Ox, In_Put.MAXTRACKSPEREVENT, "Ox");
  Vec<Double_t> Oy(In_Put.Oy, In_Put.MAXTRACKSPEREVENT, "Oy");

  Vec<Short_t> ParalCommonList(In_Put.ParalCommonList, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITSINTRACK, "ParalCommonList");

  Vec<Short_t> ParSpuriList(In_Put.ParSpuriList, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITSINTRACK, "ParSpuriList");

  Vec<Double_t> posizSciTil(In_Put.posizSciTil, In_Put.MAXSCITILHITS * 3, "posizSciTil");

  Vec<Double_t> R(In_Put.R, In_Put.MAXTRACKSPEREVENT, "R");

  Double_t RSTRAWDETECTORMAX = In_Put.rstrawdetectormax;
  Double_t RSTRAWDETECTORMIN = In_Put.rstrawdetectormin;

  Vec<Double_t> sigmaXMvdPixel(In_Put.sigmaXMvdPixel, In_Put.MAXMVDPIXELHITS, "sigmaXMvdPixel");
  Vec<Double_t> sigmaXMvdStrip(In_Put.sigmaXMvdStrip, In_Put.MAXMVDSTRIPHITS, "sigmaXMvdStrip");
  Vec<Double_t> sigmaYMvdPixel(In_Put.sigmaYMvdPixel, In_Put.MAXMVDPIXELHITS, "sigmaYMvdPixel");
  Vec<Double_t> sigmaYMvdStrip(In_Put.sigmaYMvdStrip, In_Put.MAXMVDSTRIPHITS, "sigmaYMvdStrip");

  Vec<Double_t> SchosenSkew(In_Put.SchosenSkew, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITS, "SchosenSkew");

  Vec<Short_t> SkewCommonList(In_Put.SkewCommonList, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITSINTRACK, "SkewCommonList");

  Double_t VERTICALGAP = In_Put.verticalgap;
  Vec<Double_t> XMvdPixel(In_Put.XMvdPixel, In_Put.MAXMVDPIXELHITS, "XMvdPixel");
  Vec<Double_t> XMvdStrip(In_Put.XMvdStrip, In_Put.MAXMVDSTRIPHITS, "XMvdStrip");
  Vec<Double_t> YMvdPixel(In_Put.YMvdPixel, In_Put.MAXMVDPIXELHITS, "YMvdPixel");
  Vec<Double_t> YMvdStrip(In_Put.YMvdStrip, In_Put.MAXMVDSTRIPHITS, "YMvdStrip");
  Vec<Double_t> WDX(In_Put.WDX, In_Put.MAXSTTHITS, "WDX");
  Vec<Double_t> WDY(In_Put.WDY, In_Put.MAXSTTHITS, "WDY");
  Vec<Double_t> WDZ(In_Put.WDZ, In_Put.MAXSTTHITS, "WDZ");
  Vec<Double_t> ZMvdPixel(In_Put.ZMvdPixel, In_Put.MAXMVDPIXELHITS, "ZMvdPixel");
  Vec<Double_t> ZMvdStrip(In_Put.ZMvdStrip, In_Put.MAXMVDSTRIPHITS, "ZMvdStrip");

  //----------------------------

  //  marker2 per cambioperl;
  Double_t ultimoang[nTotalCandidates];
  Double_t primoang[nTotalCandidates];
  Double_t Posiz1[3];
  Double_t XintersectionList[2];
  Double_t YintersectionList[2];

  Vec<Double_t> ultimoangolo(ultimoang, nTotalCandidates, "ultimoangolo");
  Vec<Double_t> primoangolo(primoang, nTotalCandidates, "primoangolo");

  //  marker2 per cambioperl;

  // calcolo di S degli eventuali hits SciTil presenti nelle tracce trovate.

  PndTrkCTGeometryCalculations GeometryCalculator;

  for (i = 0, k = -1; i < nTotalCandidates; i++) {
    Double_t esseSciTil[nSciTilHitsinTrack.at(i)];
    if (!keepit.at(i))
      continue;
    for (j = 0; j < nSciTilHitsinTrack.at(i); j++) {

      intersect = GeometryCalculator.IntersectionSciTil_Circle(
        //			DIMENSIONSCITIL,
        posizSciTil.at(ListSciTilHitsinTrack.at(i * MAXSCITILHITSINTRACK + j) * 3 + 0), posizSciTil.at(ListSciTilHitsinTrack.at(i * MAXSCITILHITSINTRACK + j) * 3 + 1),
        Ox.at(i), // center of circle.
        Oy.at(i),
        R.at(i), // Radius of circle.
        &Nint, XintersectionList, YintersectionList);

      // reject case with no intersection of the SciTil with the circle trajectory.
      if (intersect) {
        // calculate S on the lateral face of the Helix.
        if (Nint == 1) { // the majority of the cases
          esseSciTil[j] = atan2(YintersectionList[0] - Oy.at(i), XintersectionList[0] - Ox.at(i));
        } else { // in this case Nint=2 (it should be a very rare case).
                 // do an average of the two positions.
          esseSciTil[j] = atan2(0.5 * (YintersectionList[0] + YintersectionList[1]) - Oy.at(i), 0.5 * (XintersectionList[0] + XintersectionList[1]) - Ox.at(i));
        } // end of  if ( Nint==1)
        if (esseSciTil[j] < 0.)
          esseSciTil[j] += 2. * PI;

      } // continuation of if(intersect)

    } // end of for(j....

    // }  // end of  for(  i= 0; i< nTotalCandidates; i++)
    //-----------------

    // calcolo di S degli eventuali hits SciTil 'Alone' delle tracce trovate.

    // Double_t esseSciTilAlone[MAXTRACKSPEREVENT][MAXSCITILHITS];

    // for(  i= 0, k=-1; i< nTotalCandidates; i++){
    Double_t esseSciTilAlone[In_Put.nMCSciTilAlone[i]];
    if (!keepit.at(i))
      continue;

    for (j = 0; j < In_Put.nMCSciTilAlone[i]; j++) {
      intersect = GeometryCalculator.IntersectionSciTil_Circle(
        //			DIMENSIONSCITIL,
        posizSciTil.at(In_Put.MCSciTilAloneList[i * nSciTilHits + j] * 3 + 0), posizSciTil.at(In_Put.MCSciTilAloneList[i * nSciTilHits + j] * 3 + 1),
        Ox.at(i), // center of circle.
        Oy.at(i),
        R.at(i), // Radius of circle.
        &Nint, XintersectionList, YintersectionList);
      // calculate S on the lateral face of the Helix.
      esseSciTilAlone[j] =
        atan2(posizSciTil.at(In_Put.MCSciTilAloneList[i * nSciTilHits + j] * 3 + 1) - Oy.at(i), posizSciTil.at(In_Put.MCSciTilAloneList[i * nSciTilHits + j] * 3 + 0) - Ox.at(i));
      if (esseSciTilAlone[j] < 0.)
        esseSciTilAlone[j] += 2. * PI;

    } // end of for(j....

    // }  // end of  for(  i= 0; i< nTotalCandidates; i++)
    //    for(  i= 0, k=-1; i< nTotalCandidates; i++){
    //	if(!keepit.at(i)) continue;

    k++;
    int npunti = -1 + nSttParHitsinTrack.at(i) + nSttSkewHitsinTrack.at(i) + nMvdPixelHitsinTrack.at(i) + nMvdStripHitsinTrack.at(i) + nSciTilHitsinTrack.at(i);

    if (ListTrackCandHitType.at(i * dime + npunti) == 0) { //  Mvd Pixel
      ultimoangolo[i] = atan2(YMvdPixel.at(ListTrackCandHit.at(i * dime + npunti)) - Oy.at(i), XMvdPixel.at(ListTrackCandHit.at(i * dime + npunti)) - Ox.at(i));
    } else if (ListTrackCandHitType.at(i * dime + npunti) == 1) { //  Mvd Strip
      ultimoangolo[i] = atan2(YMvdStrip.at(ListTrackCandHit.at(i * dime + npunti)) - Oy.at(i), XMvdStrip.at(ListTrackCandHit.at(i * dime + npunti)) - Ox.at(i));
    } else if (ListTrackCandHitType.at(i * dime + npunti) == 2) { // it is a parallel straw hit

      // inizio cambio_in_perl ;
      SttInfoXYZParal(&info, ListTrackCandHit.at(i * dime + npunti), Ox.at(i), Oy.at(i), R.at(i), KAPPA.at(i), FI0.at(i), Charge.at(i), Posiz1);

      // fine cambio_in_perl ;

      ultimoangolo[i] = atan2(Posiz1[1] - Oy.at(i), Posiz1[0] - Ox.at(i));
    } else if (ListTrackCandHitType.at(i * dime + npunti) == 3) { // it is a skew straw hit

      ultimoangolo[i] = SchosenSkew.at(i * MAXSTTHITS + ListTrackCandHit.at(i * dime + npunti));
    } else if (ListTrackCandHitType.at(i * dime + npunti) == 1001) { // SciTil hit.
      ultimoangolo[i] = atan2(posizSciTil.at(ListTrackCandHit.at(i * dime + npunti) * 3 + 1) - Oy.at(i), posizSciTil.at(ListTrackCandHit.at(i * dime + npunti) * 3 + 0) - Ox.at(i));
    }
    if (ultimoangolo[i] < 0.)
      ultimoangolo[i] += 2. * PI;

    double primo;
    primoangolo[i] = fmod(FI0.at(i), 2. * PI);
    if (Charge.at(i) > 0.) {
      if (ultimoangolo[i] > primoangolo[i])
        ultimoangolo[i] -= 2. * PI;
      primo = ultimoangolo[i] * 180. / PI;
      ultimoangolo[i] = primoangolo[i] * 180. / PI;
      primoangolo[i] = primo;

    } else {
      if (ultimoangolo[i] < primoangolo[i])
        ultimoangolo[i] += 2. * PI;
      ultimoangolo[i] = ultimoangolo[i] * 180. / PI;
      primoangolo[i] = primoangolo[i] * 180. / PI;
    }

    if (nSttParHitsinTrack.at(i) + nMvdPixelHitsinTrack.at(i) + nMvdStripHitsinTrack.at(i) > 0 && doMcComparison) {

      for (j = 0; j < nMCSkewAlone.at(i); j++) {
        puntator = (FairMCPoint *)fSttPointArray->At(MCSkewAloneList.at(i * nSttHit + j));
        MCSkewAloneX.at(MCSkewAloneList.at(i * nSttHit + j)) = puntator->GetX();
        MCSkewAloneY.at(MCSkewAloneList.at(i * nSttHit + j)) = puntator->GetY();
      }

      //  inizio cambio_in_perl ;

      WriteMacroSttParallelAssociatedHitsandMvdwithMC(In_Put, Ox.at(i), Oy.at(i), R.at(i), primoangolo[i], ultimoangolo[i], nSttParHitsinTrack.at(i), i,
                                                      k, // questo si usa solo per il nome della Macro.
                                                      daTrackFoundaTrackMC.at(i), nMvdPixelHitsinTrack.at(i), nMvdStripHitsinTrack.at(i), nSttSkewHitsinTrack.at(i));
      //  fine cambio_in_perl ;

    } // end of  if( nSttParHitsinTrack.at(i)+nMvdPixelHitsinTrack.at(i)+
      //			nMvdStripHitsinTrack.at(i)>0 &&  doMcComparison)

    if (nSttSkewHitsinTrack.at(i) + nMvdPixelHitsinTrack.at(i) + nMvdStripHitsinTrack.at(i) > 0 && doMcComparison) {
      WriteMacroSkewAssociatedHitswithMC(esseSciTil, esseSciTilAlone, In_Put,
                                         k, // questo si usa solo per il nome della Macro.
                                         i);
    } //  end of	if(  nSttSkewHitsinTrack.at(i)+nMvdPixelHitsinTrack.at(i)+
      //	nMvdStripHitsinTrack.at(i)>0 &&  doMcComparison)
  }   //   end of   for(  i= 0; i< nSttTrackCand; i++)

  //  inizio cambio_in_perl ;
  WriteMacroParallelHitsGeneral(APOTEMAMAXINNERPARSTRAW, APOTEMAMAXSKEWSTRAW, APOTEMAMINOUTERPARSTRAW, APOTEMAMINSKEWSTRAW, BFIELD, CVEL, DIMENSIONSCITIL, doMcComparison,
                                fMCTrackArray, nSttHit, &info, In_Put, IVOLTE, nMCTracks, nMvdPixelHit, nMvdStripHit, nSciTilHits, nTotalCandidates, &keepit, &FI0, &Ox, &Oy,
                                &posizSciTil, &primoangolo, &R, RSTRAWDETECTORMAX, RSTRAWDETECTORMIN, &sigmaXMvdPixel, &sigmaXMvdStrip, &sigmaYMvdPixel, &sigmaYMvdStrip,
                                &ultimoangolo, VERTICALGAP, &XMvdPixel, &XMvdStrip, &YMvdPixel, &YMvdStrip);

  // Macro nel piano conforme, senza gli hits Mvd;

  WriteMacroParallelHitsGeneralConformalwithMC(APOTEMAMAXINNERPARSTRAW, APOTEMAMAXSKEWSTRAW, APOTEMAMINOUTERPARSTRAW, APOTEMAMINSKEWSTRAW, BFIELD, CVEL, DIMENSIONSCITIL,
                                               doMcComparison, fMCTrackArray, nSttHit, &info, In_Put, IVOLTE, nMCTracks, nMvdPixelHit, nMvdStripHit, nSciTilHits, nTotalCandidates,
                                               &keepit, &FI0, &Ox, &Oy, &posizSciTil, &primoangolo, &R, RSTRAWDETECTORMAX, RSTRAWDETECTORMIN, &sigmaXMvdPixel, &sigmaXMvdStrip,
                                               &sigmaYMvdPixel, &sigmaYMvdStrip, &ultimoangolo, VERTICALGAP, &XMvdPixel, &XMvdStrip, &YMvdPixel, &YMvdStrip);

  // Macro nel piano conforme, con anche gli hits Mvd;

  WriteMacroParallel_MvdHitsGeneralConformalwithMC(APOTEMAMAXINNERPARSTRAW, APOTEMAMAXSKEWSTRAW, APOTEMAMINOUTERPARSTRAW, APOTEMAMINSKEWSTRAW, BFIELD, CVEL, DIMENSIONSCITIL,
                                                   doMcComparison, fMCTrackArray, nSttHit, &info, In_Put, IVOLTE, nMCTracks, nMvdPixelHit, nMvdStripHit, nSciTilHits,
                                                   nTotalCandidates, &keepit, &FI0, &Ox, &Oy, &posizSciTil, &primoangolo, &R, RSTRAWDETECTORMAX, RSTRAWDETECTORMIN, &sigmaXMvdPixel,
                                                   &sigmaXMvdStrip, &sigmaYMvdPixel, &sigmaYMvdStrip, &ultimoangolo, VERTICALGAP, &XMvdPixel, &XMvdStrip, &YMvdPixel, &YMvdStrip);

  //     la seguente e' da modificare per includere eventuali hits SciTil mai usati.
  WriteMacroAllHitsRestanti(APOTEMAMAXINNERPARSTRAW, APOTEMAMAXSKEWSTRAW, APOTEMAMINOUTERPARSTRAW, APOTEMAMINSKEWSTRAW, &InclusionListSciTil, &InclusionListStt, &info, IVOLTE,
                            &keepit, &ListTrackCandHit, &ListTrackCandHitType, MAXMVDPIXELHITS, MAXMVDPIXELHITSINTRACK, MAXMVDSTRIPHITS, MAXMVDSTRIPHITSINTRACK,
                            MAXSCITILHITSINTRACK, MAXSTTHITS, MAXSTTHITSINTRACK, MAXTRACKSPEREVENT, nMvdPixelHit, nMvdStripHit, nSciTilHits, nSttHit, nSttParHit, nSttSkewHit,
                            nTotalCandidates, &nTrackCandHit, &posizSciTil, RSTRAWDETECTORMAX, RSTRAWDETECTORMIN, VERTICALGAP, &XMvdPixel, &XMvdStrip, &YMvdPixel, &YMvdStrip);

  //  fine cambio_in_perl ;

  //---------------------  fine plottamenti --------------------------------------------

  return;
}

//  marker3 per cambioperl ;

//----------end of function PndTrkPlotMacros::WriteAllMacros

//----------begin of function PndTrkPlotMacros::WriteMacroAllHitsRestanti

//  inizio cambio_in_perl ;
void PndTrkPlotMacros::WriteMacroAllHitsRestanti(Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMAXSKEWSTRAW, Double_t APOTEMAMINOUTERPARSTRAW, Double_t APOTEMAMINSKEWSTRAW,
                                                 Vec<bool> *InclusionListSciTil, Vec<bool> *InclusionListStt, Vec<Double_t> *info, int IVOLTE, Vec<bool> *keepit,
                                                 Vec<Short_t> *ListTrackCandHit, Vec<Short_t> *ListTrackCandHitType, int MAXMVDPIXELHITS, int MAXMVDPIXELHITSINTRACK,
                                                 int MAXMVDSTRIPHITS, int MAXMVDSTRIPHITSINTRACK, int MAXSCITILHITSINTRACK, int MAXSTTHITS, int MAXSTTHITSINTRACK,
                                                 int /*MAXTRACKSPEREVENT*/,                                                                                //[R.K. 9/2018] unused
                                                 Short_t nMvdPixelHit, Short_t nMvdStripHit, Short_t nSciTilHits, Short_t nSttHit, Short_t /*nSttParHit*/, //[R.K. 9/2018] unused
                                                 Short_t /*nSttSkewHit*/,                                                                                  //[R.K. 9/2018] unused
                                                 Short_t nSttTrackCand, Vec<Short_t> *nTrackCandHit, Vec<Double_t> *posizSciTil, Double_t RSTRAWDETECTORMAX,
                                                 Double_t RSTRAWDETECTORMIN, Double_t VERTICALGAP, Vec<Double_t> *XMvdPixel, Vec<Double_t> *XMvdStrip, Vec<Double_t> *YMvdPixel,
                                                 Vec<Double_t> *YMvdStrip

)
{

  //  fine cambio_in_perl ;

  //	nSttHit = parallel+skew.

  bool exclusionStt[MAXSTTHITS], exclusionPixel[MAXMVDPIXELHITS], exclusionStrip[MAXMVDSTRIPHITS];

  char nome[300], nome2[300];

  int i, j; //,k; //[R.K. 01/2017] unused variable?

  Double_t // delta, //[R.K. 01/2017] unused variable?
           // deltax, //[R.K. 01/2017] unused variable?
           // deltay, //[R.K. 01/2017] unused variable?
    xmin,
    xmax, ymin, ymax;

  // trasformazioni in variabili locali;
  int dime = MAXSTTHITSINTRACK + MAXMVDPIXELHITSINTRACK + MAXMVDSTRIPHITSINTRACK + MAXSCITILHITSINTRACK;

  for (i = 0; i < nSttHit; i++) {
    exclusionStt[i] = false;
  }
  for (i = 0; i < nMvdPixelHit; i++) {
    exclusionPixel[i] = false;
  }
  for (i = 0; i < nMvdStripHit; i++) {
    exclusionStrip[i] = false;
  }

  for (i = 0; i < nSttTrackCand; i++) {
    if (!keepit->at(i))
      continue;

    for (j = 0; j < nTrackCandHit->at(i); j++) {

      switch (ListTrackCandHitType->at(i * dime + j)) {
      case 0: exclusionPixel[ListTrackCandHit->at(i * dime + j)] = true; break;
      case 1: exclusionStrip[ListTrackCandHit->at(i * dime + j)] = true; break;
      default: exclusionStt[ListTrackCandHit->at(i * dime + j)] = true; break;
      }
    } // end of  for(j=0;j<nTrackCandHit->at(i);j++)
  }   //  end of  for(i=0;i<nSttTrackCand;i++)

  xmin = -1.3 * RSTRAWDETECTORMAX;
  xmax = 1.3 * RSTRAWDETECTORMAX;
  ymin = -1.3 * RSTRAWDETECTORMAX;
  ymax = 1.3 * RSTRAWDETECTORMAX;

  sprintf(nome, "MacroSttMvdHitsRestantiEvent%d", IVOLTE);
  sprintf(nome2, "%s.C", nome);
  FILE *MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);
  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  disegnaAssiXY(MACRO, xmin, xmax, ymin, ymax);

  //	disegna il BiHexagon destro e sinistro delle inner parallel straws.
  char myname[100];

  sprintf(myname, "InnerPar");
  DrawBiHexagonInMacro(VERTICALGAP, MACRO, RSTRAWDETECTORMIN, APOTEMAMAXINNERPARSTRAW,
                       4, // color code, 4= blue.
                       myname);
  //--------------
  //	disegna il BiHexagon destro e sinistro delle skew straws.
  sprintf(myname, "Skew");
  DrawBiHexagonInMacro(VERTICALGAP, MACRO, APOTEMAMINSKEWSTRAW, APOTEMAMAXSKEWSTRAW,
                       2, // color code.
                       myname);
  //--------------
  //	disegna il BiHexagon destro e sinistro delle skew straws.
  sprintf(myname, "OuterPar");
  DrawHexagonCircleInMacro(VERTICALGAP, MACRO, APOTEMAMINOUTERPARSTRAW, RSTRAWDETECTORMAX,
                           4, // color code.
                           myname);
  //--------------

  for (i = 0; i < nSttHit; i++) {
    if ((!exclusionStt[i]) && InclusionListStt->at(i)) { // all straws
      if (info->at(i * 7 + 5) == 1) {                    // parallel straws
        fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n", i, info->at(i * 7 + 0), info->at(i * 7 + 1),
                info->at(i * 7 + 3), info->at(i * 7 + 3), i, i);
      } else { //  skew straws.
        fprintf(MACRO, "TMarker* SS%d = new TMarker(%f,%f,%d);\nSS%d->SetMarkerColor(1);\nSS%d->Draw();\n", i, info->at(i * 7 + 0), info->at(i * 7 + 1), 28, i, i);
      }
    }
  }
  for (i = 0; i < nMvdPixelHit; i++) {
    if (!exclusionPixel[i]) { // all Pixels
      fprintf(MACRO, "TMarker* Pixel%d = new TMarker(%f,%f,%d);\nPixel%d->SetMarkerColor(1);\nPixel%d->Draw();\n", i, XMvdPixel->at(i), YMvdPixel->at(i), 26, i, i);
    }
  }
  for (i = 0; i < nMvdStripHit; i++) {
    if (!exclusionStrip[i]) { // all Pixels
      fprintf(MACRO, "TMarker* Strip%d = new TMarker(%f,%f,%d);\nStrip%d->SetMarkerColor(1);\nStrip%d->Draw();\n", i, XMvdStrip->at(i), YMvdStrip->at(i), 25, i, i);
    }
  }

  for (i = 0; i < nSciTilHits; i++) {
    if (InclusionListSciTil->at(i)) { // all SciTil hit never used.
      fprintf(MACRO, "TMarker* SciT%d = new TMarker(%f,%f,%d);\nSciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n", i, posizSciTil->at(i * 3 + 0), posizSciTil->at(i * 3 + 1), 30, i,
              i);
    }
  }
  fprintf(MACRO, "}\n");
  fclose(MACRO);

  return;
}

//----------end of function PndTrkPlotMacros::WriteMacroAllHitsRestanti

//----------start of function PndTrkPlotMacros::WriteMacroParallelHitsGeneral
//  inizio cambio_in_perl ;

void PndTrkPlotMacros::WriteMacroParallelHitsGeneral(Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMAXSKEWSTRAW, Double_t APOTEMAMINOUTERPARSTRAW, Double_t APOTEMAMINSKEWSTRAW,
                                                     Double_t BFIELD, Double_t CVEL, Double_t DIMENSIONSCITIL, bool doMcComparison, TClonesArray *fMCTrackArray, Int_t Nhits,
                                                     Vec<Double_t> *info, PndTrkPlotMacros_InputData In_Put, int IVOLTE, Short_t nMCTracks, Short_t nMvdPixelHit,
                                                     Short_t nMvdStripHit, Short_t nSciTilHits, Short_t nTracksFoundSoFar, Vec<bool> *keepit,
                                                     Vec<Double_t> * /*FI0*/, //[R.K. 9/2018] unused
                                                     Vec<Double_t> *Ox, Vec<Double_t> *Oy, Vec<Double_t> *posizSciTil, Vec<Double_t> *primoangolo, Vec<Double_t> *R,
                                                     Double_t RSTRAWDETECTORMAX, Double_t RSTRAWDETECTORMIN, Vec<Double_t> *sigmaXMvdPixel, Vec<Double_t> *sigmaXMvdStrip,
                                                     Vec<Double_t> *sigmaYMvdPixel, Vec<Double_t> *sigmaYMvdStrip, Vec<Double_t> *ultimoangolo, Double_t VERTICALGAP,
                                                     Vec<Double_t> *XMvdPixel, Vec<Double_t> *XMvdStrip, Vec<Double_t> *YMvdPixel, Vec<Double_t> *YMvdStrip

)
{
  //  fine cambio_in_perl ;

  Int_t i, j, ii; //    nup, nlow,Kincl,index,i1, STATUS //[R.K. 01/2017] unused variable?

  Double_t xmin, xmax, ymin, ymax, // xl, xu, yl, yu, //[R.K. 01/2017] unused variable?
                                   // gamma, //[R.K. 01/2017] unused variable?
                                   // dx, dy, diff, d1, d2, //[R.K. 01/2017] unused variable?
                                   // delta, deltax, deltay, deltaz, deltaS, //[R.K. 01/2017] unused variable?
                                   // factor,ff, //[R.K. 01/2017] unused variable?
                                   // zmin, zmax, Smin, Smax, S1, S2, //[R.K. 01/2017] unused variable?
    y2, x2,                        // y1, x1,z1, z2,  //[R.K. 01/2017] unused variable?
    // vx1, vy1, vz1, C0x1, C0y1, C0z1, //[R.K. 01/2017] unused variable?
    aaa, bbb, rrr; // ccc, angle, minor, major, //[R.K. 01/2017] unused variable?
  // distance, Rx, Ry, LL, //[R.K. 01/2017] unused variable?
  // Aellipsis1, Bellipsis1,fi1, //[R.K. 01/2017] unused variable?
  // fmin, fmax, offset, step, //[R.K. 01/2017] unused variable?
  // SkewInclWithRespectToS, zpos, zpos1, zpos2, //[R.K. 01/2017] unused variable?
  // Tiltdirection1[2], //[R.K. 01/2017] unused variable?
  // zl[200],zu[200], //[R.K. 01/2017] unused variable?
  // POINTS1[6]; //[R.K. 01/2017] unused variable?

  char nome[300], nome2[300];

  const double PI = 3.141592654;

  //---------- parallel straws Macro now
  sprintf(nome, "MacroSttMvdAllHitsEvent%d", IVOLTE);
  sprintf(nome2, "%s.C", nome);
  FILE *MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);

  xmin = -1.3 * RSTRAWDETECTORMAX;
  xmax = 1.3 * RSTRAWDETECTORMAX;
  ymin = -1.3 * RSTRAWDETECTORMAX;
  ymax = 1.3 * RSTRAWDETECTORMAX;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  disegnaAssiXY(MACRO, xmin, xmax, ymin, ymax);

  //	disegna il BiHexagon destro e sinistro delle inner parallel straws.
  char myname[100];

  sprintf(myname, "InnerPar");
  DrawBiHexagonInMacro(VERTICALGAP, MACRO, RSTRAWDETECTORMIN, APOTEMAMAXINNERPARSTRAW,
                       4, // color code, 4= blue.
                       myname);
  //--------------
  //	disegna il BiHexagon destro e sinistro delle skew straws.
  sprintf(myname, "Skew");
  DrawBiHexagonInMacro(VERTICALGAP, MACRO, APOTEMAMINSKEWSTRAW, APOTEMAMAXSKEWSTRAW,
                       2, // color code.
                       myname);
  //--------------
  //	disegna il BiHexagon destro e sinistro delle skew straws.
  sprintf(myname, "OuterPar");
  DrawHexagonCircleInMacro(VERTICALGAP, MACRO, APOTEMAMINOUTERPARSTRAW, RSTRAWDETECTORMAX,
                           4, // color code.
                           myname);
  //--------------

  for (i = 0; i < Nhits; i++) {
    if (!In_Put.InclusionListStt[i])
      continue;
    if (info->at(i * 7 + 5) == 1) { // parallel straws
      fprintf(MACRO, "TEllipse* Paral%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nParal%d->SetFillStyle(0);\nParal%d->Draw();\n", i, info->at(i * 7 + 0), info->at(i * 7 + 1),
              info->at(i * 7 + 3), info->at(i * 7 + 3), i, i);
    } else { // skew straws.
      fprintf(MACRO, "TMarker* Skew%d = new TMarker(%f,%f,%d);\nSkew%d->SetMarkerColor(1);\nSkew%d->Draw();\n", i, info->at(i * 7 + 0), info->at(i * 7 + 1), 28, i, i);
    }
  }

  for (ii = 0; ii < nMvdStripHit; ii++) {
    // x1= XMvdStrip->at(ii)-sigmaXMvdStrip->at(ii); //[R.K. 01/2017] unused variable?
    x2 = XMvdStrip->at(ii) + sigmaXMvdStrip->at(ii);
    // y1= YMvdStrip->at(ii)-sigmaYMvdStrip->at(ii); //[R.K. 01/2017] unused variable?
    y2 = YMvdStrip->at(ii) + sigmaYMvdStrip->at(ii);
    fprintf(MACRO, "TMarker* Strip%d = new TMarker(%f,%f,%d);\nStrip%d->SetMarkerColor(1);\nStrip%d->Draw();\n", ii, XMvdStrip->at(ii), YMvdStrip->at(ii), 25, ii, ii);
  }
  for (ii = 0; ii < nMvdPixelHit; ii++) {
    // x1= XMvdPixel->at(ii)-sigmaXMvdPixel->at(ii); //[R.K. 01/2017] unused variable?
    x2 = XMvdPixel->at(ii) + sigmaXMvdPixel->at(ii);
    // y1= YMvdPixel->at(ii)-sigmaYMvdPixel->at(ii); //[R.K. 01/2017] unused variable?
    y2 = YMvdPixel->at(ii) + sigmaYMvdPixel->at(ii);
    fprintf(MACRO, "TMarker* Pixel%d = new TMarker(%f,%f,%d);\nPixel%d->SetMarkerColor(1);\nPixel%d->Draw();\n", ii, XMvdPixel->at(ii), YMvdPixel->at(ii), 26, ii, ii);
  }

  //---- disegna gli Scitil.

  for (i = 0; i < nSciTilHits; i++) {
    disegnaSciTilHit(1, // color code (the same as in SetColor of root);
                     DIMENSIONSCITIL, MACRO, posizSciTil->at(i * 3 + 0), posizSciTil->at(i * 3 + 1), i,
                     0 // if 0 then SciTil draw in XY; if 1 then SciTil draw in SZ; else
                       // SciTil draw in UV.
    );
  }
  //------------------------

  //-------------------------------   plotting all the tracks found

  for (i = 0, ii = 0; i < nTracksFoundSoFar; i++) {

    if (!keepit->at(i))
      continue;
    ii++;
    aaa = Ox->at(i);
    bbb = Oy->at(i);
    rrr = R->at(i);
    fprintf(MACRO, "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,%f,%f);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw(\"only\");\n", ii, aaa, bbb, rrr, rrr,
            primoangolo->at(i), ultimoangolo->at(i), ii, ii, ii);
  }

  // -----------

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  //------------------------------------------------------------------------------------------------------------

  if (!doMcComparison)
    return;

  //---------- parallel straws Macro now con anche le tracce MC

  sprintf(nome, "MacroSttMvdAllHitswithMCEvent%d", IVOLTE);
  sprintf(nome2, "%s.C", nome);
  MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  disegnaAssiXY(MACRO, xmin, xmax, ymin, ymax);

  //	disegna il BiHexagon destro e sinistro delle inner parallel straws.
  sprintf(myname, "InnerPar");
  DrawBiHexagonInMacro(VERTICALGAP, MACRO, RSTRAWDETECTORMIN, APOTEMAMAXINNERPARSTRAW,
                       4, // color code, 4= blue.
                       myname);
  //--------------
  //	disegna il BiHexagon destro e sinistro delle skew straws.
  sprintf(myname, "Skew");
  DrawBiHexagonInMacro(VERTICALGAP, MACRO, APOTEMAMINSKEWSTRAW, APOTEMAMAXSKEWSTRAW,
                       2, // color code.
                       myname);
  //--------------
  //	disegna il BiHexagon destro e sinistro delle skew straws.
  sprintf(myname, "OuterPar");
  DrawHexagonCircleInMacro(VERTICALGAP, MACRO, APOTEMAMINOUTERPARSTRAW, RSTRAWDETECTORMAX,
                           4, // color code.
                           myname);
  //--------------

  for (i = 0; i < Nhits; i++) {

    if (!In_Put.InclusionListStt[i])
      continue;
    if (info->at(i * 7 + 5) == 1) { // parallel straws
      fprintf(MACRO, "TEllipse* Paral%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nParal%d->SetFillStyle(0);\nParal%d->Draw();\n", i, info->at(i * 7 + 0), info->at(i * 7 + 1),
              info->at(i * 7 + 3), info->at(i * 7 + 3), i, i);
    } else { // skew straws.
      fprintf(MACRO, "TMarker* Skew%d = new TMarker(%f,%f,%d);\nSkew%d->SetMarkerColor(1);\nSkew%d->Draw();\n", i, info->at(i * 7 + 0), info->at(i * 7 + 1), 28, i, i);
    }
  }

  for (ii = 0; ii < nMvdStripHit; ii++) {
    // x1= XMvdStrip->at(ii)-sigmaXMvdStrip->at(ii); //[R.K. 01/2017] unused variable?
    x2 = XMvdStrip->at(ii) + sigmaXMvdStrip->at(ii);
    // y1= YMvdStrip->at(ii)-sigmaYMvdStrip->at(ii); //[R.K. 01/2017] unused variable?
    y2 = YMvdStrip->at(ii) + sigmaYMvdStrip->at(ii);
    fprintf(MACRO, "TMarker* Strip%d = new TMarker(%f,%f,%d);\nStrip%d->SetMarkerColor(1);\nStrip%d->Draw();\n", ii, XMvdStrip->at(ii), YMvdStrip->at(ii), 25, ii, ii);
  }
  for (ii = 0; ii < nMvdPixelHit; ii++) {
    // x1= XMvdPixel->at(ii)-sigmaXMvdPixel->at(ii); //[R.K. 01/2017] unused variable?
    x2 = XMvdPixel->at(ii) + sigmaXMvdPixel->at(ii);
    // y1= YMvdPixel->at(ii)-sigmaYMvdPixel->at(ii); //[R.K. 01/2017] unused variable?
    y2 = YMvdPixel->at(ii) + sigmaYMvdPixel->at(ii);
    fprintf(MACRO, "TMarker* Pixel%d = new TMarker(%f,%f,%d);\nPixel%d->SetMarkerColor(1);\nPixel%d->Draw();\n", ii, XMvdPixel->at(ii), YMvdPixel->at(ii), 26, ii, ii);
  }

  //---- disegna gli Scitil.

  for (i = 0; i < nSciTilHits; i++) {
    // inizio cambio_in_perl

    disegnaSciTilHit(1, // color code, the same as in SetColor of root;
                     DIMENSIONSCITIL, MACRO, posizSciTil->at(i * 3 + 0), posizSciTil->at(i * 3 + 1), i,
                     0 // if 0 then SciTil draw in XY; if 1 then SciTil draw in SZ;
                       // else SciTil draw in UV.
    );
  }

  // fine cambio_in_perl

  //------------------------

  //-------------------------------   plotting all the tracks found
  for (i = 0, ii = 0; i < nTracksFoundSoFar; i++) {

    if (!keepit->at(i))
      continue;
    ii++;
    aaa = Ox->at(i);
    bbb = Oy->at(i);
    rrr = R->at(i);
    fprintf(MACRO, "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,%f,%f);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw(\"only\");\n", ii, aaa, bbb, rrr, rrr,
            primoangolo->at(i), ultimoangolo->at(i), ii, ii, ii);
  }

  //----------------- ora le traccia MC
  for (i = 0; i < nMCTracks; i++) {
    Int_t icode;
    Double_t alfa0, newalfa, newx, newy, primo, ultimo;
    Double_t Rr, Oxx, Oyy, Cx, Cy, Px, Py, carica; // Dd, Fifi, //[R.K. 01/2017] unused variable?
    PndMCTrack *pMC;
    pMC = (PndMCTrack *)fMCTrackArray->At(i);
    TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
    if (pMC) {
      icode = pMC->GetPdgCode(); //   PDG code of track
      TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
      if (icode > 1000000000)
        carica = 1.;
      else
        carica = fParticle->Charge() / 3.; //   charge of track
      Oxx = pMC->GetStartVertex().X();     //   X of starting point track
      Oyy = pMC->GetStartVertex().Y();     //   Y of starting point track
      Px = pMC->GetMomentum().X();
      Py = pMC->GetMomentum().Y();
      aaa = sqrt(Px * Px + Py * Py);
      if (fabs(carica) > 0.1) {
        Rr = aaa * 1000. / (BFIELD * CVEL); // R(cm) of Helix of track projected in XY plane;
        // B = 2 Tesla
        Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
        Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);

        //  calcolo per plottare  solo la parte rilevante della traccia MC.
        primo = alfa0 = atan2(Oyy - Cy, Oxx - Cx);
        bool flaggo = true;
        for (j = 0; j < 90; j++) {
          newalfa = alfa0 - carica * j * PI / 45;
          newx = Cx + Rr * cos(newalfa);
          newy = Cy + Rr * sin(newalfa);
          if (newx > xmax || newx < xmin || newy > ymax || newy < ymin) {
            ultimo = newalfa;
            if (primo > ultimo) {
              primo = ultimo;
              ultimo = alfa0;
            };
            flaggo = false;
            break;
          }
        }
        if (flaggo) {
          primo = 0.;
          ultimo = 2. * PI;
        }

        fprintf(MACRO, "TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,%f,%f);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw(\"only\");\n", i, Cx, Cy, Rr, Rr,
                primo * 180. / PI, ultimo * 180. / PI, i, i, i);

      } else {    // continuation of  if( fabs(carica) > 0.1 )
        continue; //  per ora non plotto le neutre.
        // this is a neutral particle (green dashed straight line).
        if (Oxx < xmin || Oxx > xmax || Oyy < ymin || Oyy > ymax)
          continue;
        double time, time1, time2;
        if (fabs(Px) > 1.e-10) {
          time2 = (xmax - Oxx) / Px;
          time1 = (xmin - Oxx) / Px;
          if (time1 < 0. && time2 < 0.) {
            continue;
          } else if (time2 > 0. && time1 < 0.) {
            time = time2;
            x2 = xmax;
          } else if (time1 > 0. && time2 < 0.) {
            time = time1;
            x2 = xmin;
          } else {
            if (time2 > time1) {
              time = time2;
              x2 = xmax;
            } else {
              time = time1;
              x2 = xmin;
            }
          }
          y2 = time * Py + Oyy;
          fprintf(MACRO, "TLine* MCneut%d = new TLine(%f,%f,%f,%f);\n", i, Oxx, Oyy, x2, y2);
          fprintf(MACRO, "MCneut%d->SetLineStyle(2);\n", i);
          fprintf(MACRO, "MCneut%d->SetLineColor(3);\nMCneut%d->Draw(\"only\");\n", i, i);
        } else if (fabs(Px) > 1.e-10) {
          if (fabs(Py) > 1.e-10)
            continue;
          time2 = (ymax - Oyy) / Py;
          time1 = (ymin - Oyy) / Py;
          if (time1 < 0. && time2 < 0.) {
            continue;
          } else if (time2 > 0. && time1 < 0.) {
            y2 = ymax;
          } else if (time1 > 0. && time2 < 0.) {
            y2 = ymin;
          } else {
            y2 = time2 > time1 ? ymax : ymin;
          }
          fprintf(MACRO, "TLine* MCneut%d = new TLine(%f,%f,%f,%f);\n", i, Oxx, Oyy, Oxx, y2);
          fprintf(MACRO, "MCneut%d->SetLineStyle(2);\n", i);
          fprintf(MACRO, "MCneut%d->SetLineColor(3);\nMCneut%d->Draw(\"only\");\n", i, i);
        }

      } // end of  if( fabs(carica) > 0.1 )

    } // end of  if ( pMC )

  } //    end of for(i=0; i<nMCTracks;i++)
    //----------- fine parte del MC

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  //------------------------------------------------------------------------------------------------------------

  return;
}

//----------end of function PndTrkPlotMacros::WriteMacroParallelHitsGeneral

//----------start of function PndTrkPlotMacros::WriteMacroParallelHitsGeneralConformalwithMC
//  inizio cambio_in_perl ;

void PndTrkPlotMacros::WriteMacroParallelHitsGeneralConformalwithMC(
  Double_t /*APOTEMAMAXINNERPARSTRAW*/,                                                                                         //[R.K. 9/2018] unused
  Double_t /*APOTEMAMAXSKEWSTRAW*/,                                                                                             //[R.K. 9/2018] unused
  Double_t /*APOTEMAMINOUTERPARSTRAW*/,                                                                                         //[R.K. 9/2018] unused
  Double_t /*APOTEMAMINSKEWSTRAW*/,                                                                                             //[R.K. 9/2018] unused
  Double_t BFIELD, Double_t CVEL, Double_t /*DIMENSIONSCITIL*/,                                                                 //[R.K. 9/2018] unused
  bool doMcComparison, TClonesArray * /*fMCTrackArray*/,                                                                        //[R.K. 9/2018] unused
  Int_t Nhits, Vec<Double_t> *info, PndTrkPlotMacros_InputData In_Put, int IVOLTE, Short_t nMCTracks, Short_t /*nMvdPixelHit*/, //[R.K. 9/2018] unused
  Short_t /*nMvdStripHit*/,                                                                                                     //[R.K. 9/2018] unused
  Short_t nSciTilHits, Short_t nTracksFoundSoFar, Vec<bool> *keepit, Vec<Double_t> * /*FI0*/,                                   //[R.K. 9/2018] unused
  Vec<Double_t> *Oxxx, Vec<Double_t> *Oyyy, Vec<Double_t> *posizSciTil, Vec<Double_t> * /*primoangolo*/,                        //[R.K. 9/2018] unused
  Vec<Double_t> *R, Double_t RSTRAWDETECTORMAX, Double_t RSTRAWDETECTORMIN, Vec<Double_t> * /*sigmaXMvdPixel*/,                 //[R.K. 9/2018] unused
  Vec<Double_t> * /*sigmaXMvdStrip*/,                                                                                           //[R.K. 9/2018] unused
  Vec<Double_t> * /*sigmaYMvdPixel*/,                                                                                           //[R.K. 9/2018] unused
  Vec<Double_t> * /*sigmaYMvdStrip*/,                                                                                           //[R.K. 9/2018] unused
  Vec<Double_t> * /*ultimoangolo*/,                                                                                             //[R.K. 9/2018] unused
  Double_t /*VERTICALGAP*/,                                                                                                     //[R.K. 9/2018] unused
  Vec<Double_t> * /*XMvdPixel*/,                                                                                                //[R.K. 9/2018] unused
  Vec<Double_t> * /*XMvdStrip*/,                                                                                                //[R.K. 9/2018] unused
  Vec<Double_t> * /*YMvdPixel*/,                                                                                                //[R.K. 9/2018] unused
  Vec<Double_t> * /*YMvdStrip*/                                                                                                 //[R.K. 9/2018] unused

)
{
  //  fine cambio_in_perl ;

  Int_t i; // j, i1, ii,index, Kincl,,   nlow, nup, STATUS //[R.K. 01/2017] unused variable?

  Double_t xmin, xmax, ymin, ymax, xl, xu, yl, yu, gamma,
    // dx, dy, diff, d1, d2, //[R.K. 01/2017] unused variable?
    delta, deltax, deltay, // deltaz, deltaS, //[R.K. 01/2017] unused variable?
    ff,                    // factor, //[R.K. 01/2017] unused variable?
    // zmin, zmax, Smin, Smax, S1, S2, //[R.K. 01/2017] unused variable?
    y1, y2, x1, x2, // z1, z2,  //[R.K. 01/2017] unused variable?
    // vx1, vy1, vz1, C0x1, C0y1, C0z1, //[R.K. 01/2017] unused variable?
    aaa, bbb, rrr; // ccc, angle, minor, major, //[R.K. 01/2017] unused variable?
                   // distance, Rx, Ry, LL, //[R.K. 01/2017] unused variable?
  // Aellipsis1, Bellipsis1,fi1, //[R.K. 01/2017] unused variable?
  // fmin, fmax, offset, step, //[R.K. 01/2017] unused variable?
  // SkewInclWithRespectToS, zpos, zpos1, zpos2, //[R.K. 01/2017] unused variable?
  // Tiltdirection1[2], //[R.K. 01/2017] unused variable?
  // zl[200],zu[200], //[R.K. 01/2017] unused variable?
  // POINTS1[6]; //[R.K. 01/2017] unused variable?

  char nome[300], nome2[300];

  const double PI = 3.141592654;

  Double_t USciT[nSciTilHits];
  Vec<Double_t> USciTil(USciT, nSciTilHits, "USciTil");

  Double_t VSciT[nSciTilHits];
  Vec<Double_t> VSciTil(VSciT, nSciTilHits, "VSciTil");

  Double_t oX[Nhits], oY[Nhits], Radi[Nhits];
  Vec<Double_t> Ox(oX, Nhits, "Ox");
  Vec<Double_t> Oy(oY, Nhits, "Oy");
  Vec<Double_t> Radius(Radi, Nhits, "Radius");

  Double_t ALF[nTracksFoundSoFar], BET[nTracksFoundSoFar], GAMM[nTracksFoundSoFar];
  Vec<Double_t> ALFA(ALF, nTracksFoundSoFar, "ALFA");
  Vec<Double_t> BETA(BET, nTracksFoundSoFar, "BETA");
  Vec<Double_t> GAMMA(GAMM, nTracksFoundSoFar, "GAMMA");

  //---------- parallel straws Macro now con anche le tracce MC

  sprintf(nome, "MacroSttAllHitsConformalwithMCEvent%d", IVOLTE);
  sprintf(nome2, "%s.C", nome);
  FILE *MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);

  xmin = 1.e20;
  xmax = -1.e20;
  ymin = 1.e20;
  ymax = -1.e20;

  //--- SciTil  info
  for (i = 0; i < nSciTilHits; i++) {
    Double_t erre = posizSciTil->at(i * 3 + 0) * posizSciTil->at(i * 3 + 0) + posizSciTil->at(i * 3 + 1) * posizSciTil->at(i * 3 + 1);
    USciTil[i] = posizSciTil->at(i * 3 + 0) / erre;
    VSciTil[i] = posizSciTil->at(i * 3 + 1) / erre;
    if (USciTil[i] < xmin)
      xmin = USciTil[i];
    if (USciTil[i] > xmax)
      xmax = USciTil[i];
    if (VSciTil[i] < ymin)
      ymin = VSciTil[i];
    if (VSciTil[i] > ymax)
      ymax = VSciTil[i];
  }
  //------

  for (i = 0; i < Nhits; i++) {
    if (!In_Put.InclusionListStt[i])
      continue;
    if (info->at(i * 7 + 5) == 1) { // parallel straws
                                    //   centro sfera in sistema conforme
      gamma = info->at(i * 7 + 0) * info->at(i * 7 + 0) + info->at(i * 7 + 1) * info->at(i * 7 + 1) - info->at(i * 7 + 3) * info->at(i * 7 + 3);
      Ox[i] = info->at(i * 7 + 0) / gamma;
      Oy[i] = info->at(i * 7 + 1) / gamma;
      Radius[i] = info->at(i * 7 + 3) / gamma;
      if (Ox[i] - Radius[i] < xmin)
        xmin = Ox[i] - Radius[i];
      if (Ox[i] + Radius[i] > xmax)
        xmax = Ox[i] + Radius[i];
      if (Oy[i] - Radius[i] < ymin)
        ymin = Oy[i] - Radius[i];
      if (Oy[i] + Radius[i] > ymax)
        ymax = Oy[i] + Radius[i];
    }
  }

  if (xmin > 0.)
    xmin = 0.;
  if (xmax < 0.)
    xmax = 0.;
  if (ymin > 0.)
    ymin = 0.;
  if (ymax < 0.)
    ymax = 0.;

  deltax = xmax - xmin;
  deltay = ymax - ymin;

  if (deltax > deltay) {
    ymin -= 0.5 * (deltax - deltay);
    ymax = ymin + deltax;
    delta = deltax;
  } else {
    xmin -= 0.5 * (deltay - deltax);
    xmax = xmin + deltay;
    delta = deltay;
  }

  xmax = xmax + delta * 0.15;
  xmin = xmin - delta * 0.15;

  ymax = ymax + delta * 0.15;
  ymin = ymin - delta * 0.15;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  // ora la grigliatura  con i cerchi

  fprintf(MACRO, "TEllipse* Griglia%d = new TEllipse(0.,0.,%f,%f,0.,360.);\nGriglia%d->SetLineColor(4);\nGriglia%d->Draw();\n", In_Put.NRDIVCONFORMAL, 1. / RSTRAWDETECTORMIN,
          1. / RSTRAWDETECTORMIN, In_Put.NRDIVCONFORMAL, In_Put.NRDIVCONFORMAL);

  for (i = In_Put.NRDIVCONFORMAL - 1; i >= 0; i--) {
    fprintf(MACRO, "TEllipse* Griglia%d = new TEllipse(0.,0.,%f,%f,0.,360.);\nGriglia%d->SetLineColor(4);\nGriglia%d->Draw();\n", i, In_Put.radiaConf[i], In_Put.radiaConf[i], i,
            i);
  }
  //---------------------
  // ora la grigliatura  con i segmenti blu per delimitare la zona degli Stt.

  for (i = 0; i < In_Put.NFIDIVCONFORMAL; i++) {
    ff = i * 2. * PI / In_Put.NFIDIVCONFORMAL;
    x1 = cos(ff) / RSTRAWDETECTORMAX;
    y1 = sin(ff) / RSTRAWDETECTORMAX;
    x2 = cos(ff) / RSTRAWDETECTORMIN;
    y2 = sin(ff) / RSTRAWDETECTORMIN;
    fprintf(MACRO, "TLine* Seg%d = new TLine(%f,%f,%f,%f);\nSeg%d->SetLineColor(4);\nSeg%d->Draw();\n", i, x1, y1, x2, y2, i, i);
  }
  //---------------------
  // ora la grigliatura  con i segmenti magenta per comprendere la zona degli SciTil.

  double RMAXSCITIL = 50.; // cm
  for (i = 0; i < In_Put.NFIDIVCONFORMAL; i++) {
    ff = i * 2. * PI / In_Put.NFIDIVCONFORMAL;
    x1 = cos(ff) / RMAXSCITIL;
    y1 = sin(ff) / RMAXSCITIL;
    x2 = cos(ff) / RSTRAWDETECTORMAX;
    y2 = sin(ff) / RSTRAWDETECTORMAX;
    fprintf(MACRO, "TLine* Seg%d = new TLine(%f,%f,%f,%f);\nSeg%d->SetLineColor(6);\nSeg%d->Draw();\n", i, x1, y1, x2, y2, i, i);
  }
  //---------------------

  fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", xmin, 0., xmax, 0., xmin, xmax);
  fprintf(MACRO, "Assex->SetTitle(\"U    \");\n");
  fprintf(MACRO, "Assex->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assex->Draw();\n");
  fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0., ymin, 0., ymax, ymin, ymax);
  fprintf(MACRO, "Assey->SetTitle(\"V    \");\n");
  fprintf(MACRO, "Assey->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assey->Draw();\n");

  // plot degli Hits Stt.
  for (i = 0; i < Nhits; i++) {
    if (!In_Put.InclusionListStt[i])
      continue;
    if (info->at(i * 7 + 5) == 1) { // parallel straws
      fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n", i, Ox[i], Oy[i], Radius[i], Radius[i], i, i);
    }
  }

  // plot degli Hit SciTil
  for (i = 0; i < nSciTilHits; i++) {

    disegnaSciTilHit(1, // goes in the SetColor function of root;
                     In_Put.dimensionscitil, MACRO, posizSciTil->at(i * 3 + 0), posizSciTil->at(i * 3 + 1), i,
                     2 // 2--> disegno SciTil in conforme.
    );
  }

  //------------------   plotting all the tracks found

  for (i = 0; i < nTracksFoundSoFar; i++) {
    if (!keepit->at(i))
      continue;

    ALFA[i] = -2. * Oxxx->at(i);
    BETA[i] = -2. * Oyyy->at(i);
    GAMMA[i] = Oxxx->at(i) * Oxxx->at(i) + Oyyy->at(i) * Oyyy->at(i) - R->at(i) * R->at(i);
    if (fabs(GAMMA[i]) > 1.e-10) {
      aaa = -0.5 * ALFA[i] / GAMMA[i];
      bbb = -0.5 * BETA[i] / GAMMA[i];
      rrr = sqrt(aaa * aaa + bbb * bbb - 1. / GAMMA[i]);
      if (fabs(rrr / GAMMA[i]) < 30.) {
        fprintf(MACRO, "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n", i, aaa, bbb, rrr, rrr, i, i, i);
      } else {

        yl = -xmin * ALFA[i] / BETA[i] - 1. / BETA[i];
        yu = -xmax * ALFA[i] / BETA[i] - 1. / BETA[i];
        fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
        fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
        fprintf(MACRO, "ris%d->Draw();\n", i);
      }
    } else {

      if (fabs(BETA[i]) < 1.e-10) {
        if (fabs(ALFA[i]) < 1.e-10) {
          continue;
        } else {
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, -1. / ALFA[i], ymin, -1. / ALFA[i], ymax);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        }
      } else {
        yl = -xmin * ALFA[i] / BETA[i] - 1. / BETA[i];
        yu = -xmax * ALFA[i] / BETA[i] - 1. / BETA[i];
        fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
        fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
        fprintf(MACRO, "ris%d->Draw();\n", i);
      }
    }

  } // end of  for(i=0; i<nTracksFoundSoFar; i++)

  // ------------------------------

  //   plotting all the tracks MC generated
  if (doMcComparison) {
    Int_t icode;
    Double_t Rr, Oxx, Oyy, Cx, Cy, Px, Py, carica; // Dd, Fifi, //[R.K. 01/2017] unused variable?
    PndMCTrack *pMC;
    for (i = 0; i < nMCTracks; i++) {
      pMC = (PndMCTrack *)In_Put.fMCTrackArray->At(i);
      if (!pMC)
        continue;
      icode = pMC->GetPdgCode();       //   PDG code of track
      Oxx = pMC->GetStartVertex().X(); //   X of starting point track
      Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
      Px = pMC->GetMomentum().X();
      Py = pMC->GetMomentum().Y();
      aaa = sqrt(Px * Px + Py * Py);
      Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
      if (icode > 1000000000)
        carica = 1.;
      else
        carica = fParticle->Charge() / 3.; //   charge of track
      if (fabs(carica) < 0.1)
        continue;
      Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
      Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);
      gamma = -Rr * Rr + Cx * Cx + Cy * Cy;
      if (fabs(gamma) < 0.001) {
        if (Cy != 0.) {
          yl = xmin * (-Cx / Cy) + 0.5 / Cy;
          yu = xmax * (-Cx / Cy) + 0.5 / Cy;
          xl = xmin;
          xu = xmax;
        } else {
          yl = ymin;
          yu = ymax;
          xu = xl = 0.5 / Cx;
        }
        fprintf(MACRO, "TLine* MCris%d = new TLine(%f,%f,%f,%f);\n", i, xl, yl, xu, yu);
        fprintf(MACRO, "MCris%d->SetLineStyle(2);\n", i);
        fprintf(MACRO, "MCris%d->SetLineColor(3);\n", i);
        fprintf(MACRO, "MCris%d->SetLineWidth(1);\n", i);
        fprintf(MACRO, "MCris%d->Draw();\n", i);

      } else {
        if (fabs(Rr / gamma) > 1.) {
          if (fabs(Cy) > 0.001) {
            yl = -xmin * Cx / Cy + 0.5 / Cy;
            yu = -xmax * Cx / Cy + 0.5 / Cy;
            fprintf(MACRO, "TLine* MCline%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
            fprintf(MACRO, "MCline%d->SetLineColor(3);\n", i);
            fprintf(MACRO, "MCline%d->Draw();\n", i);
          } else {
            fprintf(MACRO, "TLine* MCline%d = new TLine(%f,%f,%f,%f);\n", i, 2. * Cx, ymin, 2. * Cx, ymax);
            fprintf(MACRO, "MCline%d->SetLineColor(2);\n", i);
            fprintf(MACRO, "MCline%d->Draw();\n", i);
          }
        } else {
          fprintf(MACRO, "TEllipse* MCcerchio%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nMCcerchio%d->SetLineColor(3);\n", i, Cx / gamma, Cy / gamma, Rr / fabs(gamma),
                  Rr / fabs(gamma), i);

          fprintf(MACRO, "MCcerchio%d->SetFillStyle(0);\nMCcerchio%d->SetLineStyle(2);\nMCcerchio%d->SetLineWidth(1);\nMCcerchio%d->Draw();\n", i, i, i, i);
        }
      }
    } // end of for(i=0;i<nMCTracks; i++)

  } // end of if(doMcComparison)

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  return;
}

//----------end of function PndTrkPlotMacros::WriteMacroParallelHitsGeneralConformalwithMC

//----------start of function PndTrkPlotMacros::WriteMacroParallel_MvdHitsGeneralConformalwithMC
//  inizio cambio_in_perl ;

void PndTrkPlotMacros::WriteMacroParallel_MvdHitsGeneralConformalwithMC(Double_t /*APOTEMAMAXINNERPARSTRAW*/,                         //[R.K. 9/2018] unused
                                                                        Double_t /*APOTEMAMAXSKEWSTRAW*/,                             //[R.K. 9/2018] unused
                                                                        Double_t /*APOTEMAMINOUTERPARSTRAW*/,                         //[R.K. 9/2018] unused
                                                                        Double_t /*APOTEMAMINSKEWSTRAW*/,                             //[R.K. 9/2018] unused
                                                                        Double_t BFIELD, Double_t CVEL, Double_t /*DIMENSIONSCITIL*/, //[R.K. 9/2018] unused
                                                                        bool doMcComparison, TClonesArray * /*fMCTrackArray*/,        //[R.K. 9/2018] unused
                                                                        Int_t Nhits, Vec<Double_t> *info, PndTrkPlotMacros_InputData In_Put, int IVOLTE, Short_t nMCTracks,
                                                                        Short_t nMvdPixelHit, Short_t nMvdStripHit, Short_t nSciTilHits, Short_t nTracksFoundSoFar,
                                                                        Vec<bool> *keepit, Vec<Double_t> * /*FI0*/, //[R.K. 9/2018] unused
                                                                        Vec<Double_t> *Oxxx, Vec<Double_t> *Oyyy, Vec<Double_t> *posizSciTil,
                                                                        Vec<Double_t> * /*primoangolo*/, //[R.K. 9/2018] unused
                                                                        Vec<Double_t> *R, Double_t RSTRAWDETECTORMAX, Double_t RSTRAWDETECTORMIN,
                                                                        Vec<Double_t> * /*sigmaXMvdPixel*/, //[R.K. 9/2018] unused
                                                                        Vec<Double_t> * /*sigmaXMvdStrip*/, //[R.K. 9/2018] unused
                                                                        Vec<Double_t> * /*sigmaYMvdPixel*/, //[R.K. 9/2018] unused
                                                                        Vec<Double_t> * /*sigmaYMvdStrip*/, //[R.K. 9/2018] unused
                                                                        Vec<Double_t> * /*ultimoangolo*/,   //[R.K. 9/2018] unused
                                                                        Double_t /*VERTICALGAP*/,           //[R.K. 9/2018] unused
                                                                        Vec<Double_t> *XMvdPixel, Vec<Double_t> *XMvdStrip, Vec<Double_t> *YMvdPixel, Vec<Double_t> *YMvdStrip

)
{
  //  fine cambio_in_perl ;

  Int_t i; //, j, i1, ii, index, Kincl, nlow, nup, STATUS; //[R.K. 01/2017] unused variable?

  Double_t xmin, xmax, ymin, ymax, xl, xu, yl, yu, gamma,
    // dx, dy, diff, d1, d2, //[R.K. 01/2017] unused variable?
    delta, deltax, deltay, // deltaz, deltaS, //[R.K. 01/2017] unused variable?
    ff,                    // factor, //[R.K. 01/2017] unused variable?
    // zmin, zmax, Smin, Smax, S1, S2, //[R.K. 01/2017] unused variable?
    y1, y2, x1, x2, // z1, z2,  //[R.K. 01/2017] unused variable?
    // vx1, vy1, vz1, C0x1, C0y1, C0z1, //[R.K. 01/2017] unused variable?
    aaa, bbb, rrr; // ccc, angle, minor, major, //[R.K. 01/2017] unused variable?
  // distance, Rx, Ry, LL, //[R.K. 01/2017] unused variable?
  // Aellipsis1, Bellipsis1,fi1, //[R.K. 01/2017] unused variable?
  // fmin, fmax, offset, step, //[R.K. 01/2017] unused variable?
  // SkewInclWithRespectToS, zpos, zpos1, zpos2, //[R.K. 01/2017] unused variable?
  // Tiltdirection1[2], //[R.K. 01/2017] unused variable?
  // zl[200],zu[200], //[R.K. 01/2017] unused variable?
  // POINTS1[6]; //[R.K. 01/2017] unused variable?

  char nome[300], nome2[300];

  const double PI = 3.141592654;

  Double_t USciT[nSciTilHits];
  Vec<Double_t> USciTil(USciT, nSciTilHits, "USciTil");

  Double_t VSciT[nSciTilHits];
  Vec<Double_t> VSciTil(VSciT, nSciTilHits, "VSciTil");

  Double_t oX[Nhits], oY[Nhits], Radi[Nhits];
  Vec<Double_t> Ox(oX, Nhits, "Ox");
  Vec<Double_t> Oy(oY, Nhits, "Oy");
  Vec<Double_t> Radius(Radi, Nhits, "Radius");

  Double_t UPixel[nMvdPixelHit], VPixel[nMvdPixelHit], PRadius[nMvdPixelHit], UStrip[nMvdStripHit], VStrip[nMvdStripHit], SRadius[nMvdStripHit];
  Vec<Double_t> PixelU(UPixel, nMvdPixelHit, "PixelU");
  Vec<Double_t> PixelV(VPixel, nMvdPixelHit, "PixelV");
  Vec<Double_t> RadiusP(PRadius, nMvdPixelHit, "RadiusP");
  Vec<Double_t> StripU(UStrip, nMvdStripHit, "StripU");
  Vec<Double_t> StripV(VStrip, nMvdStripHit, "StripV");
  Vec<Double_t> RadiusS(SRadius, nMvdStripHit, "RadiusS");

  Double_t ALF[nTracksFoundSoFar], BET[nTracksFoundSoFar], GAMM[nTracksFoundSoFar];
  Vec<Double_t> ALFA(ALF, nTracksFoundSoFar, "ALFA");
  Vec<Double_t> BETA(BET, nTracksFoundSoFar, "BETA");
  Vec<Double_t> GAMMA(GAMM, nTracksFoundSoFar, "GAMMA");

  //---------- parallel straws Macro now con anche le tracce MC

  sprintf(nome, "MacroSttMvdAllHitsConformalwithMCEvent%d", IVOLTE);
  sprintf(nome2, "%s.C", nome);
  FILE *MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);

  xmin = 1.e20;
  xmax = -1.e20;
  ymin = 1.e20;
  ymax = -1.e20;

  //--- SciTil  info
  for (i = 0; i < nSciTilHits; i++) {
    Double_t erre = posizSciTil->at(i * 3 + 0) * posizSciTil->at(i * 3 + 0) + posizSciTil->at(i * 3 + 1) * posizSciTil->at(i * 3 + 1);
    USciTil[i] = posizSciTil->at(i * 3 + 0) / erre;
    VSciTil[i] = posizSciTil->at(i * 3 + 1) / erre;
    if (USciTil[i] < xmin)
      xmin = USciTil[i];
    if (USciTil[i] > xmax)
      xmax = USciTil[i];
    if (VSciTil[i] < ymin)
      ymin = VSciTil[i];
    if (VSciTil[i] > ymax)
      ymax = VSciTil[i];
  }
  //------

  for (i = 0; i < Nhits; i++) {
    if (!In_Put.InclusionListStt[i])
      continue;
    if (info->at(i * 7 + 5) == 1) { // parallel straws
                                    //   centro sfera in sistema conforme
      gamma = info->at(i * 7 + 0) * info->at(i * 7 + 0) + info->at(i * 7 + 1) * info->at(i * 7 + 1) - info->at(i * 7 + 3) * info->at(i * 7 + 3);
      Ox[i] = info->at(i * 7 + 0) / gamma;
      Oy[i] = info->at(i * 7 + 1) / gamma;
      Radius[i] = info->at(i * 7 + 3) / gamma;
      if (Ox[i] - Radius[i] < xmin)
        xmin = Ox[i] - Radius[i];
      if (Ox[i] + Radius[i] > xmax)
        xmax = Ox[i] + Radius[i];
      if (Oy[i] - Radius[i] < ymin)
        ymin = Oy[i] - Radius[i];
      if (Oy[i] + Radius[i] > ymax)
        ymax = Oy[i] + Radius[i];
    }
  }

  // Mvd hits;

  double CC,
    pseudoRadius = 0.01; //  'raggio' di Pixel e Strip;
  // Pixel
  for (i = 0; i < nMvdPixelHit; i++) {
    // here, for the boundary calculation, it is enough to approximate the
    // rectangular sensor in a circle of radius 0.01 in XY space;

    CC = XMvdPixel->at(i) * XMvdPixel->at(i) + YMvdPixel->at(i) * YMvdPixel->at(i);
    // assumo un raggio del circolo che ingloba il sensore di 0.01 cm;
    gamma = CC - pseudoRadius * pseudoRadius;
    PixelU[i] = XMvdPixel->at(i) / gamma;
    PixelV[i] = YMvdPixel->at(i) / gamma;
    RadiusP[i] = pseudoRadius / gamma;

    if (PixelU[i] - RadiusP[i] < xmin)
      xmin = PixelU[i] - RadiusP[i];
    if (PixelU[i] + RadiusP[i] > xmax)
      xmax = PixelU[i] + RadiusP[i];
    if (PixelV[i] - RadiusP[i] < ymin)
      ymin = PixelV[i] - RadiusP[i];
    if (PixelV[i] + RadiusP[i] > ymax)
      ymax = PixelV[i] + RadiusP[i];

  } // end of  for( i=0; i< nMvdPixelHit; i++)

  // Strip
  for (i = 0; i < nMvdStripHit; i++) {
    // here, for the boundary calculation, it is enough to approximate the
    // rectangular sensor in a circle of radius 0.01 in XY space;

    CC = XMvdStrip->at(i) * XMvdStrip->at(i) + YMvdPixel->at(i) * YMvdStrip->at(i);
    // assumo un raggio del circolo che ingloba il sensore di 0.01 cm;
    gamma = CC - pseudoRadius * pseudoRadius;
    StripU[i] = XMvdStrip->at(i) / gamma;
    StripV[i] = YMvdStrip->at(i) / gamma;
    RadiusS[i] = pseudoRadius / gamma;

    if (StripU[i] - RadiusS[i] < xmin)
      xmin = StripU[i] - RadiusS[i];
    if (StripU[i] + RadiusS[i] > xmax)
      xmax = StripU[i] + RadiusS[i];
    if (StripV[i] - RadiusS[i] < ymin)
      ymin = StripV[i] - RadiusS[i];
    if (StripV[i] + RadiusS[i] > ymax)
      ymax = StripV[i] + RadiusS[i];

  } // end of  for( i=0; i< nMvdStripHit; i++)

  if (xmin > 0.)
    xmin = 0.;
  if (xmax < 0.)
    xmax = 0.;
  if (ymin > 0.)
    ymin = 0.;
  if (ymax < 0.)
    ymax = 0.;

  deltax = xmax - xmin;
  deltay = ymax - ymin;

  if (deltax > deltay) {
    ymin -= 0.5 * (deltax - deltay);
    ymax = ymin + deltax;
    delta = deltax;
  } else {
    xmin -= 0.5 * (deltay - deltax);
    xmax = xmin + deltay;
    delta = deltay;
  }

  xmax = xmax + delta * 0.15;
  xmin = xmin - delta * 0.15;

  ymax = ymax + delta * 0.15;
  ymin = ymin - delta * 0.15;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  // ora la grigliatura  con i cerchi

  fprintf(MACRO, "TEllipse* Griglia%d = new TEllipse(0.,0.,%f,%f,0.,360.);\nGriglia%d->SetLineColor(4);\nGriglia%d->Draw();\n", In_Put.NRDIVCONFORMAL, 1. / RSTRAWDETECTORMIN,
          1. / RSTRAWDETECTORMIN, In_Put.NRDIVCONFORMAL, In_Put.NRDIVCONFORMAL);

  for (i = In_Put.NRDIVCONFORMAL - 1; i >= 0; i--) {
    fprintf(MACRO, "TEllipse* Griglia%d = new TEllipse(0.,0.,%f,%f,0.,360.);\nGriglia%d->SetLineColor(4);\nGriglia%d->Draw();\n", i, In_Put.radiaConf[i], In_Put.radiaConf[i], i,
            i);
  }
  //---------------------
  // ora la grigliatura  con i segmenti blu per delimitare la zona degli Stt.

  for (i = 0; i < In_Put.NFIDIVCONFORMAL; i++) {
    ff = i * 2. * PI / In_Put.NFIDIVCONFORMAL;
    x1 = cos(ff) / RSTRAWDETECTORMAX;
    y1 = sin(ff) / RSTRAWDETECTORMAX;
    x2 = cos(ff) / RSTRAWDETECTORMIN;
    y2 = sin(ff) / RSTRAWDETECTORMIN;
    fprintf(MACRO, "TLine* Seg%d = new TLine(%f,%f,%f,%f);\nSeg%d->SetLineColor(4);\nSeg%d->Draw();\n", i, x1, y1, x2, y2, i, i);
  }
  //---------------------
  // ora la grigliatura  con i segmenti magenta per comprendere la zona degli SciTil.

  double RMAXSCITIL = 50.; // cm
  for (i = 0; i < In_Put.NFIDIVCONFORMAL; i++) {
    ff = i * 2. * PI / In_Put.NFIDIVCONFORMAL;
    x1 = cos(ff) / RMAXSCITIL;
    y1 = sin(ff) / RMAXSCITIL;
    x2 = cos(ff) / RSTRAWDETECTORMAX;
    y2 = sin(ff) / RSTRAWDETECTORMAX;
    fprintf(MACRO, "TLine* Seg%d = new TLine(%f,%f,%f,%f);\nSeg%d->SetLineColor(6);\nSeg%d->Draw();\n", i, x1, y1, x2, y2, i, i);
  }
  //---------------------

  fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", xmin, 0., xmax, 0., xmin, xmax);
  fprintf(MACRO, "Assex->SetTitle(\"U    \");\n");
  fprintf(MACRO, "Assex->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assex->Draw();\n");
  fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0., ymin, 0., ymax, ymin, ymax);
  fprintf(MACRO, "Assey->SetTitle(\"V    \");\n");
  fprintf(MACRO, "Assey->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assey->Draw();\n");

  // plot degli Hits Stt.
  for (i = 0; i < Nhits; i++) {
    if (!In_Put.InclusionListStt[i])
      continue;
    if (info->at(i * 7 + 5) == 1) { // parallel straws
      fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n", i, Ox[i], Oy[i], Radius[i], Radius[i], i, i);
    }
  }

  // plot degli Hits Pixel Mvd.
  for (i = 0; i < nMvdPixelHit; i++) {
    fprintf(MACRO, "TMarker* Pixel%d = new TMarker(%f,%f,%d);\nPixel%d->SetMarkerColor(1);\nPixel%d->Draw();\n", i, PixelU[i], PixelV[i], 26, i, i);
    //"TEllipse* Pixel%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nPixel%d->SetFillStyle(0);\nPixel%d->Draw();\n",
    //                  i,PixelU[i],PixelV[i],RadiusP[i],RadiusP[i],i,i);
  }
  //-----------

  // plot degli Hits Strip Mvd.
  for (i = 0; i < nMvdStripHit; i++) {
    fprintf(MACRO, "TMarker* Strip%d = new TMarker(%f,%f,%d);\nStrip%d->SetMarkerColor(1);\nStrip%d->Draw();\n", i, StripU[i], StripV[i], 25, i, i);
    // "TEllipse* Strip%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nStrip%d->SetFillStyle(0);\nStrip%d->Draw();\n",
    //                  i,StripU[i],StripV[i],RadiusS[i],RadiusS[i],i,i);
  }
  //-----------
  // plot degli Hit SciTil
  for (i = 0; i < nSciTilHits; i++) {

    disegnaSciTilHit(1, // goes in the SetColor function of root;
                     In_Put.dimensionscitil, MACRO, posizSciTil->at(i * 3 + 0), posizSciTil->at(i * 3 + 1), i,
                     2 // 2--> disegno SciTil in conforme.
    );
  }

  //------------------   plotting all the tracks found

  for (i = 0; i < nTracksFoundSoFar; i++) {
    if (!keepit->at(i))
      continue;

    ALFA[i] = -2. * Oxxx->at(i);
    BETA[i] = -2. * Oyyy->at(i);
    GAMMA[i] = Oxxx->at(i) * Oxxx->at(i) + Oyyy->at(i) * Oyyy->at(i) - R->at(i) * R->at(i);
    if (fabs(GAMMA[i]) > 1.e-10) {
      aaa = -0.5 * ALFA[i] / GAMMA[i];
      bbb = -0.5 * BETA[i] / GAMMA[i];
      rrr = sqrt(aaa * aaa + bbb * bbb - 1. / GAMMA[i]);
      if (fabs(rrr / GAMMA[i]) < 30.) {
        fprintf(MACRO, "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n", i, aaa, bbb, rrr, rrr, i, i, i);
      } else {

        yl = -xmin * ALFA[i] / BETA[i] - 1. / BETA[i];
        yu = -xmax * ALFA[i] / BETA[i] - 1. / BETA[i];
        fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
        fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
        fprintf(MACRO, "ris%d->Draw();\n", i);
      }
    } else {

      if (fabs(BETA[i]) < 1.e-10) {
        if (fabs(ALFA[i]) < 1.e-10) {
          continue;
        } else {
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, -1. / ALFA[i], ymin, -1. / ALFA[i], ymax);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        }
      } else {
        yl = -xmin * ALFA[i] / BETA[i] - 1. / BETA[i];
        yu = -xmax * ALFA[i] / BETA[i] - 1. / BETA[i];
        fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
        fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
        fprintf(MACRO, "ris%d->Draw();\n", i);
      }
    }

  } // end of  for(i=0; i<nTracksFoundSoFar; i++)

  // ------------------------------

  //   plotting all the tracks MC generated
  if (doMcComparison) {
    Int_t icode;
    Double_t Rr, Oxx, Oyy, Cx, Cy, Px, Py, carica; // Dd, Fifi, //[R.K. 01/2017] unused variable?
    PndMCTrack *pMC;
    for (i = 0; i < nMCTracks; i++) {
      pMC = (PndMCTrack *)In_Put.fMCTrackArray->At(i);
      if (!pMC)
        continue;
      icode = pMC->GetPdgCode();       //   PDG code of track
      Oxx = pMC->GetStartVertex().X(); //   X of starting point track
      Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
      Px = pMC->GetMomentum().X();
      Py = pMC->GetMomentum().Y();
      aaa = sqrt(Px * Px + Py * Py);
      Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
      if (icode > 1000000000)
        carica = 1.;
      else
        carica = fParticle->Charge() / 3.; //   charge of track
      if (fabs(carica) < 0.1)
        continue;
      Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
      Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);
      gamma = -Rr * Rr + Cx * Cx + Cy * Cy;
      if (fabs(gamma) < 0.001) {
        if (Cy != 0.) {
          yl = xmin * (-Cx / Cy) + 0.5 / Cy;
          yu = xmax * (-Cx / Cy) + 0.5 / Cy;
          xl = xmin;
          xu = xmax;
        } else {
          yl = ymin;
          yu = ymax;
          xu = xl = 0.5 / Cx;
        }
        fprintf(MACRO, "TLine* MCris%d = new TLine(%f,%f,%f,%f);\n", i, xl, yl, xu, yu);
        fprintf(MACRO, "MCris%d->SetLineStyle(2);\n", i);
        fprintf(MACRO, "MCris%d->SetLineColor(3);\n", i);
        fprintf(MACRO, "MCris%d->SetLineWidth(1);\n", i);
        fprintf(MACRO, "MCris%d->Draw();\n", i);

      } else {
        if (fabs(Rr / gamma) > 1.) {
          if (fabs(Cy) > 0.001) {
            yl = -xmin * Cx / Cy + 0.5 / Cy;
            yu = -xmax * Cx / Cy + 0.5 / Cy;
            fprintf(MACRO, "TLine* MCline%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
            fprintf(MACRO, "MCline%d->SetLineColor(3);\n", i);
            fprintf(MACRO, "MCline%d->Draw();\n", i);
          } else {
            fprintf(MACRO, "TLine* MCline%d = new TLine(%f,%f,%f,%f);\n", i, 2. * Cx, ymin, 2. * Cx, ymax);
            fprintf(MACRO, "MCline%d->SetLineColor(2);\n", i);
            fprintf(MACRO, "MCline%d->Draw();\n", i);
          }
        } else {
          fprintf(MACRO, "TEllipse* MCcerchio%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nMCcerchio%d->SetLineColor(3);\n", i, Cx / gamma, Cy / gamma, Rr / fabs(gamma),
                  Rr / fabs(gamma), i);

          fprintf(MACRO, "MCcerchio%d->SetFillStyle(0);\nMCcerchio%d->SetLineStyle(2);\nMCcerchio%d->SetLineWidth(1);\nMCcerchio%d->Draw();\n", i, i, i, i);
        }
      }
    } // end of for(i=0;i<nMCTracks; i++)

  } // end of if(doMcComparison)

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  return;
}

//----------end of function PndTrkPlotMacros::WriteMacroParallel_MvdHitsGeneralConformalwithMC

//----------start of function PndTrkPlotMacros::WriteMacroSkewAssociatedHitswithMC

// marker4 per cambioperl

void PndTrkPlotMacros::WriteMacroSkewAssociatedHitswithMC(Double_t *ESSE,      // S of all associated SciTil hits to the present track;
                                                          Double_t *ESSEalone, // S of 'Alone'  SciTil hits to the present track;
                                                          PndTrkPlotMacros_InputData In_Put,
                                                          int iNome, // questo e' per il nome delle Macro solamente.
                                                          int iTrack)
{

  int MAXMVDPIXELHITSINTRACK = In_Put.MAXMVDPIXELHITSINTRACK;
  int MAXSCITILHITSINTRACK = In_Put.MAXSCITILHITSINTRACK;
  int MAXMVDSTRIPHITSINTRACK = In_Put.MAXMVDSTRIPHITSINTRACK;
  int MAXSTTHITSINTRACK = In_Put.MAXSTTHITSINTRACK;

  Double_t BFIELD = In_Put.bfield;
  Double_t CVEL = In_Put.cvel;
  Short_t charge = In_Put.Charge[iTrack];
  Short_t daTrackFoundaTrackMC = In_Put.daTrackFoundaTrackMC[iTrack];
  Double_t DIMENSIONSCITIL = In_Put.dimensionscitil;
  Double_t FI0 = In_Put.FI0[iTrack];
  TClonesArray *fMCTrackArray = In_Put.fMCTrackArray;

  Double_t *info = In_Put.info;
  int IVOLTE = In_Put.IVOLTE;
  Double_t KAPPA = In_Put.KAPPA[iTrack];

  Short_t *ListPixelHitsinTrack = In_Put.ListMvdPixelHitsinTrack;
  Short_t *ListSciTilHitsinTrack = In_Put.ListSciTilHitsinTrack;
  Short_t *ListSkewHitsinTrack = In_Put.ListSttSkewHitsinTrack;
  Short_t *ListStripHitsinTrack = In_Put.ListMvdStripHitsinTrack; // output
  Short_t *MCMvdPixelAloneList = In_Put.MCMvdPixelAloneList;
  Short_t *MCMvdStripAloneList = In_Put.MCMvdStripAloneList;
  Short_t *MCSkewAloneList = In_Put.MCSkewAloneList;
  Short_t *MvdPixelCommonList = In_Put.MvdPixelCommonList;
  // Short_t *MvdPixelSpuriList = In_Put.MvdPixelSpuriList; //[R.K. 01/2017] unused variable?
  Short_t *MvdStripCommonList = In_Put.MvdStripCommonList;

  Short_t nMCMvdPixelAlone = In_Put.nMCMvdPixelAlone[iTrack];
  Short_t nMCMvdStripAlone = In_Put.nMCMvdStripAlone[iTrack];
  Short_t nMCSkewAlone = In_Put.nMCSkewAlone[iTrack];
  Short_t nMvdPixelCommon = In_Put.nMvdPixelCommon[iTrack];
  // Short_t nMvdPixelSpuriinTrack = In_Put.nMvdPixelSpuriinTrack[iTrack] ; //[R.K. 01/2017] unused variable?
  Short_t nMvdStripCommon = In_Put.nMvdStripCommon[iTrack];
  // Short_t nMvdStripSpuriinTrack = In_Put.nMvdStripSpuriinTrack[iTrack] ; //[R.K. 01/2017] unused variable?

  Short_t *nPixelHitsinTrack = In_Put.nMvdPixelHitsinTrack;
  Short_t *nSciTilHitsinTrack = In_Put.nSciTilHitsinTrack;
  Short_t nSkewCommon = In_Put.nSkewCommon[iTrack];
  Short_t *nSkewHitsinTrack = In_Put.nSttSkewHitsinTrack;
  Short_t *nStripHitsinTrack = In_Put.nMvdStripHitsinTrack; // output
  Double_t Oxx = In_Put.Ox[iTrack];
  Double_t Oyy = In_Put.Oy[iTrack];

  Double_t *posizSciTil = In_Put.posizSciTil;

  Double_t Rr = In_Put.R[iTrack];
  Short_t *SkewCommonList = In_Put.SkewCommonList;
  Double_t *XMvdPixel = In_Put.XMvdPixel;
  Double_t *XMvdStrip = In_Put.XMvdStrip;
  Double_t *YMvdPixel = In_Put.YMvdPixel;
  Double_t *YMvdStrip = In_Put.YMvdStrip;
  Double_t *WDX = In_Put.WDX;
  Double_t *WDY = In_Put.WDY;
  Double_t *WDZ = In_Put.WDZ;
  Double_t *ZMvdPixel = In_Put.ZMvdPixel;
  Double_t *ZMvdStrip = In_Put.ZMvdStrip;

  //-----------------------------------------

  TDatabasePDG *fdbPDG = TDatabasePDG::Instance();

  int icolore;

  Int_t i, j, i1, ii, iii, index, STATUS, imc, Nmin, Nmax; // Kincl, nlow, nup,

  Double_t // xmin , xmax, ymin, ymax, //[R.K. 01/2017] unused variable?
           // dx, dy, diff, d1, d2, //[R.K. 01/2017] unused variable?
    deltaz,
    deltaS,                                                        // delta, deltax, deltay,
    esse,                                                          // factor,
    zmin, zmax, zmin2, zmax2, Smin, Smax,                          // S1, S2, //[R.K. 01/2017] unused variable?
    z1, z2,                                                        // y1, y2, //[R.K. 01/2017] unused variable?
    vx1, vy1, vz1, C0x1, C0y1, C0z1, aaa, bbb,                     // ccc, angle, minor, major, //[R.K. 01/2017] unused variable?
    distance, Rx, Ry, LL, Aellipsis1, Bellipsis1, fi1, fmin, fmax, // offset, step, //[R.K. 01/2017] unused variable?
    SkewInclWithRespectToS,                                        // zpos, zpos1, zpos2, //[R.K. 01/2017] unused variable?
    dist[2], Tiltdirection1[2],
    // zl[200],zu[200], //[R.K. 01/2017] unused variable?
    POINTS1[6];

  const Double_t PI = 3.141592654;

  PndTrkCTGeometryCalculations GeoCalculator;

  //-------------------  skew straws hits Macro now

  char nome2[300], nome[300];
  FILE *MACRO;
  sprintf(nome, "MacroSttMvdSZwithMCEvent%dT%d", IVOLTE, iNome);
  sprintf(nome2, "%s.C", nome);
  MACRO = fopen(nome2, "w");
  fprintf(MACRO, "{\n");

  // KAPPA = 1./166.67 ;  FI0 = 1.5*PI;

  index = 0;

  //--------------- ricerca del minimo e massimo.

  if (nSciTilHitsinTrack[iTrack] + nSkewHitsinTrack[iTrack] + nPixelHitsinTrack[iTrack] + nStripHitsinTrack[iTrack] == 1) // solo 1 punto da disegnare, in questo caso aggiunge
  {                                                                                                                       // un punto finto in  (0, FI0 ).
    Smax = Smin = FI0;
    zmax = zmin = 0.;
  } else {
    Smin = zmin = 1.e10;
    Smax = zmax = -zmin;
  }

  // prima lo (gli) hits SciTil associati alla traccia;

  for (i = 0; i < nSciTilHitsinTrack[iTrack]; i++) {
    j = ListSciTilHitsinTrack[iTrack * MAXSCITILHITSINTRACK + i];
    if (ESSE[i] > Smax)
      Smax = ESSE[i];
    if (ESSE[i] < Smin)
      Smin = ESSE[i];
    if (posizSciTil[j * 3 + 2] > zmax)
      zmax = posizSciTil[j * 3 + 2];
    if (posizSciTil[j * 3 + 2] < zmin)
      zmin = posizSciTil[j * 3 + 2];
  }

  // poi lo (gli) hits SciTil 'Alone' della traccia;

  for (i = 0; i < In_Put.nMCSciTilAlone[iTrack]; i++) {
    j = In_Put.MCSciTilAloneList[iTrack * In_Put.nSciTilHits + i];
    if (ESSEalone[i] > Smax)
      Smax = ESSEalone[i];
    if (ESSEalone[i] < Smin)
      Smin = ESSEalone[i];
    if (posizSciTil[j * 3 + 2] > zmax)
      zmax = posizSciTil[j * 3 + 2];
    if (posizSciTil[j * 3 + 2] < zmin)
      zmin = posizSciTil[j * 3 + 2];
  }

  //-------------------------

  for (iii = 0; iii < nSkewHitsinTrack[iTrack]; iii++) {
    i = ListSkewHitsinTrack[iTrack * MAXSTTHITSINTRACK + iii];
    if (!In_Put.InclusionListStt[i])
      continue;
    aaa = sqrt(WDX[i] * WDX[i] + WDY[i] * WDY[i] + WDZ[i] * WDZ[i]);
    vx1 = WDX[i] / aaa;
    vy1 = WDY[i] / aaa;
    vz1 = WDZ[i] / aaa;
    C0x1 = info[i * 7 + 0];
    C0y1 = info[i * 7 + 1];
    C0z1 = info[i * 7 + 2];

    GeoCalculator.calculateintersections(Oxx, Oyy, Rr, C0x1, C0y1, C0z1, info[i * 7 + 3], vx1, vy1, vz1, &STATUS, POINTS1);
    if (STATUS < 0)
      continue;
    for (ii = 0; ii < 2; ii++) {
      j = 3 * ii;
      distance = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));
      Rx = POINTS1[j] - Oxx;     //  x component Radial vector of cylinder of trajectory
      Ry = POINTS1[1 + j] - Oyy; //  y direction Radial vector of cylinder of trajectory

      aaa = sqrt(Rx * Rx + Ry * Ry);
      SkewInclWithRespectToS = (-Ry * vx1 + Rx * vy1) / aaa;
      SkewInclWithRespectToS /= Rr;
      bbb = sqrt(SkewInclWithRespectToS * SkewInclWithRespectToS + vz1 * vz1);
      //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction

      LL = fabs(vx1 * Rx + vy1 * Ry);
      if (LL < 1.e-10)
        continue;
      Aellipsis1 = info[i * 7 + 3] * aaa / LL;

      Bellipsis1 = info[i * 7 + 3] / Rr;

      if (distance >= info[i * 7 + 4] + Aellipsis1)
        continue;

      //--------------------------

      fi1 = atan2(POINTS1[j + 1] - Oyy, POINTS1[j] - Oxx); // atan2 returns radians in (-pi and +pi]
      if (fi1 < 0.)
        fi1 += 2. * PI;

      if (zmin > POINTS1[j + 2] - Aellipsis1)
        zmin = POINTS1[j + 2] - Aellipsis1;
      if (zmax < POINTS1[j + 2] + Aellipsis1)
        zmax = POINTS1[j + 2] + Aellipsis1;

      if (Smin > fi1 - Bellipsis1)
        Smin = fi1 - Bellipsis1;
      if (Smax < fi1 + Bellipsis1)
        Smax = fi1 + Bellipsis1;

    } //  end of    for( ii=0; ii<2; ii++)

  } //   end of  for( iii=0; iii< nSkewHitsinTrack[iTrack]; iii++)

  //------ aggiungo in blu eventuali punti della traccia MC che sono non mecciati

  for (iii = 0; iii < nMCSkewAlone; iii++) {
    i = MCSkewAloneList[iTrack * In_Put.nSttHit + iii];
    aaa = sqrt(WDX[i] * WDX[i] + WDY[i] * WDY[i] + WDZ[i] * WDZ[i]);
    vx1 = WDX[i] / aaa;
    vy1 = WDY[i] / aaa;
    vz1 = WDZ[i] / aaa;
    C0x1 = info[i * 7 + 0];
    C0y1 = info[i * 7 + 1];
    C0z1 = info[i * 7 + 2];
    GeoCalculator.calculateintersections(Oxx, Oyy, Rr, C0x1, C0y1, C0z1, info[i * 7 + 3], vx1, vy1, vz1, &STATUS, POINTS1);

    if (STATUS < 0)
      continue;

    for (ii = 0; ii < 2; ii++) {
      j = 3 * ii;
      dist[ii] = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));
    }
    if (dist[0] > dist[1])
      ii = 1;
    else
      ii = 0;

    distance = dist[ii];

    j = 3 * ii;
    distance = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));

    Rx = POINTS1[j] - Oxx;     //  x component Radial vector of cylinder of trajectory
    Ry = POINTS1[1 + j] - Oyy; //  y direction Radial vector of cylinder of trajectory

    aaa = sqrt(Rx * Rx + Ry * Ry);
    SkewInclWithRespectToS = (-Ry * vx1 + Rx * vy1) / aaa;
    SkewInclWithRespectToS /= Rr;
    bbb = sqrt(SkewInclWithRespectToS * SkewInclWithRespectToS + vz1 * vz1);
    //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction

    LL = fabs(vx1 * Rx + vy1 * Ry);
    if (LL < 1.e-10)
      continue;
    Aellipsis1 = info[i * 7 + 3] * aaa / LL;

    Bellipsis1 = info[i * 7 + 3] / Rr;
    //--------------------------
    fi1 = atan2(POINTS1[j + 1] - Oyy, POINTS1[j] - Oxx); // atan2 returns radians in (-pi and +pi]
    if (fi1 < 0.)
      fi1 += 2. * PI;

    if (zmin > POINTS1[j + 2] - Aellipsis1)
      zmin = POINTS1[j + 2] - Aellipsis1;
    if (zmax < POINTS1[j + 2] + Aellipsis1)
      zmax = POINTS1[j + 2] + Aellipsis1;

    if (Smin > fi1 - Bellipsis1)
      Smin = fi1 - Bellipsis1;
    if (Smax < fi1 + Bellipsis1)
      Smax = fi1 + Bellipsis1;

    //   }    //  end of    for( ii=0; ii<2; ii++)

  } //   end of  for( iii=0; iii< nMCSkewAlone[imaxima]; iii++)

  //-------------------------------
  //------ fine aggiunta in blu eventuali punti della traccia MC che sono non mecciati

  //   ora la parte delle Mvd

  //   prima i pixel

  for (i = 0; i < nPixelHitsinTrack[iTrack]; i++) {
    ii = ListPixelHitsinTrack[iTrack * MAXMVDPIXELHITSINTRACK + i];

    if (zmin > ZMvdPixel[ii])
      zmin = ZMvdPixel[ii];
    if (zmax < ZMvdPixel[ii])
      zmax = ZMvdPixel[ii];

    esse = atan2(YMvdPixel[ii] - Oyy, XMvdPixel[ii] - Oxx);
    if (esse < 0.)
      esse += 2. * PI;

    if (Smin > esse)
      Smin = esse;
    if (Smax < esse)
      Smax = esse;
  }

  //   poi le strip

  for (i = 0; i < nStripHitsinTrack[iTrack]; i++) {
    ii = ListStripHitsinTrack[iTrack * MAXMVDSTRIPHITSINTRACK + i];
    if (zmin > ZMvdStrip[ii])
      zmin = ZMvdStrip[ii];
    if (zmax < ZMvdStrip[ii])
      zmax = ZMvdStrip[ii];

    esse = atan2(YMvdStrip[ii] - Oyy, XMvdStrip[ii] - Oxx);
    if (esse < 0.)
      esse += 2. * PI;

    if (Smin > esse)
      Smin = esse;
    if (Smax < esse)
      Smax = esse;
  }

  //   ora i pixel 'Alone'
  for (i = 0; i < nMCMvdPixelAlone; i++) {

    ii = MCMvdPixelAloneList[iTrack * In_Put.nMvdPixelHit + i];
    if (zmin > ZMvdPixel[ii])
      zmin = ZMvdPixel[ii];
    if (zmax < ZMvdPixel[ii])
      zmax = ZMvdPixel[ii];

    esse = atan2(YMvdPixel[ii] - Oyy, XMvdPixel[ii] - Oxx);
    if (esse < 0.)
      esse += 2. * PI;
    if (Smin > esse)
      Smin = esse;
    if (Smax < esse)
      Smax = esse;
  }

  //   ora le strip 'Alone'

  for (i = 0; i < nMCMvdStripAlone; i++) {
    ii = MCMvdStripAloneList[iTrack * In_Put.nMvdStripHit + i];
    if (zmin > ZMvdStrip[ii])
      zmin = ZMvdStrip[ii];
    if (zmax < ZMvdStrip[ii])
      zmax = ZMvdStrip[ii];

    esse = atan2(YMvdStrip[ii] - Oyy, XMvdStrip[ii] - Oxx);
    if (esse < 0.)
      esse += 2. * PI;
    if (Smin > esse)
      Smin = esse;
    if (Smax < esse)
      Smax = esse;
  }

  if (zmax >= zmin && Smax >= Smin) {

    aaa = Smax - Smin;
    Smin -= aaa * 0.2;
    Smax += aaa * 0.2;

    aaa = zmax - zmin;
    zmin -= aaa * 0.05;
    zmax += aaa * 0.05;

    if (Smax > 2. * PI)
      Smax = 2. * PI;
    if (Smin < 0.)
      Smin = 0.;

    deltaz = zmax - zmin;
    deltaS = Smax - Smin;

    fprintf(MACRO, "TCanvas* my= new  TCanvas();\nmy->Range(%f,%f,%f,%f);\n", zmin - 0.1 * deltaz, Rr * (Smin - .1 * deltaS), zmax + 0.1 * deltaz, Rr * (Smax + 0.1 * deltaS));
    for (ii = 0; ii < index; ii++) {
      fprintf(MACRO, "E%d->Draw();\n", ii);
    }
    fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", zmin - 0.01 * deltaz, Rr * (Smin + 0.05 * deltaS), zmax + 0.01 * deltaz, Rr * (Smin + 0.05 * deltaS),
            zmin - 0.01 * deltaz, zmax + 0.01 * deltaz);
    fprintf(MACRO, "Assex->SetTitle(\"Z (cm)\");\n");
    fprintf(MACRO, "Assex->Draw();\n");
    fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", zmin + 0.05 * deltaz, Rr * (Smin - 0.01 * deltaS), zmin + 0.05 * deltaz, Rr * (Smax + 0.01 * deltaS),
            Rr * (Smin - 0.01 * deltaS), Rr * (Smax + 0.01 * deltaS));
    fprintf(MACRO, "Assey->SetTitle(\"Helix crf (cm)\");\n");
    fprintf(MACRO, "Assey->Draw();\n");

    //------------
    //  plot di eventuali hits  SciTil;
    // prima la lista di SciTil 'common' e spuri :
    for (i = 0; i < nSciTilHitsinTrack[iTrack]; i++) {
      j = ListSciTilHitsinTrack[iTrack * MAXSCITILHITSINTRACK + i];
      // controlla se sono hit 'common' o spuri;
      icolore = 2; // colore rosso, assegnato agli spuri;
      for (int h = 0; h < In_Put.nSciTilCommon[iTrack]; h++) {
        if (j == In_Put.SciTilCommonList[iTrack * MAXSCITILHITSINTRACK + h]) {
          icolore = 1; // colore nero, usato per gli hits 'common';
          break;
        }
      }

      disegnaSciTilHit(icolore, // color code; the same as in SetColor of root;
                       DIMENSIONSCITIL, MACRO, posizSciTil[j * 3 + 2], ESSE[i] * Rr, j,
                       1 // if 0 then SciTil draw in XY; if 1 then SciTil draw in SZ; else
                         // SciTil draw in UV.
      );
    }
    // poi la lista di SciTil 'alone' :
    for (i = 0; i < In_Put.nMCSciTilAlone[iTrack]; i++) {
      j = In_Put.MCSciTilAloneList[iTrack * In_Put.nSciTilHits + i];
      disegnaSciTilHit(4, // color code; the same as in SetColor of root;
                       DIMENSIONSCITIL, MACRO, posizSciTil[j * 3 + 2], ESSEalone[i] * Rr, j,
                       1 // if 0 then SciTil draw in XY; if 1 then SciTil draw in SZ; else
                         // SciTil draw in UV.
      );
    }

    // --------------------------------

    for (iii = 0; iii < nSkewHitsinTrack[iTrack]; iii++) {
      i = ListSkewHitsinTrack[iTrack * MAXSTTHITSINTRACK + iii];
      if (!In_Put.InclusionListStt[i])
        continue;

      aaa = sqrt(WDX[i] * WDX[i] + WDY[i] * WDY[i] + WDZ[i] * WDZ[i]);
      vx1 = WDX[i] / aaa;
      vy1 = WDY[i] / aaa;
      vz1 = WDZ[i] / aaa;
      C0x1 = info[i * 7 + 0];
      C0y1 = info[i * 7 + 1];
      C0z1 = info[i * 7 + 2];

      GeoCalculator.calculateintersections(Oxx, Oyy, Rr, C0x1, C0y1, C0z1, info[i * 7 + 3], vx1, vy1, vz1, &STATUS, POINTS1);
      if (STATUS < 0)
        continue;
      for (ii = 0; ii < 2; ii++) {
        j = 3 * ii;
        distance = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));
        Rx = POINTS1[j] - Oxx;     //  x component Radial vector of cylinder of trajectory
        Ry = POINTS1[1 + j] - Oyy; //  y direction Radial vector of cylinder of trajectory

        aaa = sqrt(Rx * Rx + Ry * Ry);
        SkewInclWithRespectToS = (-Ry * vx1 + Rx * vy1) / aaa;
        SkewInclWithRespectToS /= Rr;
        bbb = sqrt(SkewInclWithRespectToS * SkewInclWithRespectToS + vz1 * vz1);
        //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
        if (bbb > 1.e-10) {
          Tiltdirection1[0] = vz1 / bbb;
          Tiltdirection1[1] = SkewInclWithRespectToS / bbb;
        } else {
          Tiltdirection1[0] = 1.;
          Tiltdirection1[1] = 0.;
        }

        LL = fabs(vx1 * Rx + vy1 * Ry);
        if (LL < 1.e-10)
          continue;
        Aellipsis1 = info[i * 7 + 3] * aaa / LL;

        Bellipsis1 = info[i * 7 + 3] / Rr;

        if (distance >= info[i * 7 + 4] + Aellipsis1)
          continue;

        //--------------------------

        fi1 = atan2(POINTS1[j + 1] - Oyy, POINTS1[j] - Oxx); // atan2 returns radians in (-pi and +pi]
        if (fi1 < 0.)
          fi1 += 2. * PI;

        if (zmin > POINTS1[j + 2] - Aellipsis1)
          zmin = POINTS1[j + 2] - Aellipsis1;
        if (zmax < POINTS1[j + 2] + Aellipsis1)
          zmax = POINTS1[j + 2] + Aellipsis1;

        if (Smin > fi1 - Bellipsis1)
          Smin = fi1 - Bellipsis1;
        if (Smax < fi1 + Bellipsis1)
          Smax = fi1 + Bellipsis1;

        Double_t rotation1 = 180. * atan2(Tiltdirection1[1], Tiltdirection1[0]) / PI;
        //        fprintf(MACRO,"TEllipse* Skew%d_%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nSkew%d_%d->SetFillStyle(0);\n",
        //                    i,ii,POINTS1[j+2],Rr*fi1,Aellipsis1,Rr*Bellipsis1,rotation1,i,ii);

        // ------ se lo hit e' spurio marcalo in rosso
        bool flaggo = true;
        for (i1 = 0; i1 < nSkewCommon; i1++) {
          if (SkewCommonList[iTrack * MAXSTTHITSINTRACK + i1] == i) {
            flaggo = false;
            break;
          }
        }
        if (flaggo) {
          fprintf(MACRO, "TEllipse* spurioSkew%d_%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nspurioSkew%d_%d->SetFillStyle(0);\n", i, ii, POINTS1[j + 2], Rr * fi1, Aellipsis1,
                  Rr * Bellipsis1, rotation1, i, ii);
          fprintf(MACRO, "spurioSkew%d_%d->SetLineColor(2);\n", i, ii);
          fprintf(MACRO, "spurioSkew%d_%d->Draw();\n", i, ii);
        } else {
          fprintf(MACRO, "TEllipse* Skew%d_%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nSkew%d_%d->SetFillStyle(0);\n", i, ii, POINTS1[j + 2], Rr * fi1, Aellipsis1, Rr * Bellipsis1,
                  rotation1, i, ii);
          fprintf(MACRO, "Skew%d_%d->Draw();\n", i, ii);
        }

        index++;

      } //  end of    for( ii=0; ii<2; ii++)

    } //   end of  for( iii=0; iii< nSkewHitsinTrack[iTrack]; iii++)

    //------ aggiungo in blu eventuali punti della traccia MC che sono non mecciati

    for (iii = 0; iii < nMCSkewAlone; iii++) {
      i = MCSkewAloneList[iTrack * In_Put.nSttHit + iii];
      aaa = sqrt(WDX[i] * WDX[i] + WDY[i] * WDY[i] + WDZ[i] * WDZ[i]);
      vx1 = WDX[i] / aaa;
      vy1 = WDY[i] / aaa;
      vz1 = WDZ[i] / aaa;

      C0x1 = info[i * 7 + 0];
      C0y1 = info[i * 7 + 1];
      C0z1 = info[i * 7 + 2];
      GeoCalculator.calculateintersections(Oxx, Oyy, Rr, C0x1, C0y1, C0z1, info[i * 7 + 3], vx1, vy1, vz1, &STATUS, POINTS1);

      if (STATUS < 0)
        continue;

      for (ii = 0; ii < 2; ii++) {
        j = 3 * ii;
        dist[ii] = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));
      }
      if (dist[0] > dist[1])
        ii = 1;
      else
        ii = 0;

      distance = dist[ii];

      j = 3 * ii;

      Rx = POINTS1[j] - Oxx;     //  x component Radial vector of cylinder of trajectory
      Ry = POINTS1[1 + j] - Oyy; //  y direction Radial vector of cylinder of trajectory

      aaa = sqrt(Rx * Rx + Ry * Ry);
      SkewInclWithRespectToS = (-Ry * vx1 + Rx * vy1) / aaa;
      SkewInclWithRespectToS /= Rr;
      bbb = sqrt(SkewInclWithRespectToS * SkewInclWithRespectToS + vz1 * vz1);
      //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
      if (bbb > 1.e-10) {
        Tiltdirection1[0] = vz1 / bbb;
        Tiltdirection1[1] = SkewInclWithRespectToS / bbb;
      } else {
        Tiltdirection1[0] = 1.;
        Tiltdirection1[1] = 0.;
      }

      LL = fabs(vx1 * Rx + vy1 * Ry);
      if (LL < 1.e-10)
        continue;
      Aellipsis1 = info[i * 7 + 3] * aaa / LL;

      Bellipsis1 = info[i * 7 + 3] / Rr;
      //--------------------------
      fi1 = atan2(POINTS1[j + 1] - Oyy, POINTS1[j] - Oxx); // atan2 returns radians in (-pi and +pi]
      if (fi1 < 0.)
        fi1 += 2. * PI;

      if (zmin > POINTS1[j + 2] - Aellipsis1)
        zmin = POINTS1[j + 2] - Aellipsis1;
      if (zmax < POINTS1[j + 2] + Aellipsis1)
        zmax = POINTS1[j + 2] + Aellipsis1;

      if (Smin > fi1 - Bellipsis1)
        Smin = fi1 - Bellipsis1;
      if (Smax < fi1 + Bellipsis1)
        Smax = fi1 + Bellipsis1;

      Double_t rotation1 = 180. * atan2(Tiltdirection1[1], Tiltdirection1[0]) / PI;
      fprintf(MACRO, "TEllipse* AloneSkew%d_%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nAloneSkew%d_%d->SetFillStyle(0);\n",
              //                     i,ii,POINTS1[j+2],fi1,Aellipsis1,Bellipsis1,rotation1,i,ii);
              i, ii, POINTS1[j + 2], Rr * fi1, Aellipsis1, Rr * Bellipsis1, rotation1, i, ii);

      // ------  marca lo hit in blu
      fprintf(MACRO, "AloneSkew%d_%d->SetLineColor(4);\n", i, ii);
      fprintf(MACRO, "AloneSkew%d_%d->Draw();\n", i, ii);

      index++;

    } //   end of  for( iii=0; iii< nMCSkewAlone; iii++)

    //-------------------------------
    //------ fine aggiunta in blu eventuali punti della traccia MC che sono non mecciati

    //   ora la parte delle Mvd

    //   prima i pixel

    for (i = 0; i < nPixelHitsinTrack[iTrack]; i++) {
      ii = ListPixelHitsinTrack[iTrack * MAXMVDPIXELHITSINTRACK + i];
      if (zmin > ZMvdPixel[ii])
        zmin = ZMvdPixel[ii];
      if (zmax < ZMvdPixel[ii])
        zmax = ZMvdPixel[ii];

      esse = atan2(YMvdPixel[ii] - Oyy, XMvdPixel[ii] - Oxx);
      if (esse < 0.)
        esse += 2. * PI;

      if (Smin > esse)
        Smin = esse;
      if (Smax < esse)
        Smax = esse;

      bool flaggo = true;
      for (int k = 0; k < nMvdPixelCommon; k++) {
        if (MvdPixelCommonList[iTrack * MAXMVDPIXELHITSINTRACK + k] == ii) {
          fprintf(MACRO, "TMarker* CommonPixel%d = new TMarker(%f,%f,%d);\nCommonPixel%d->SetMarkerColor(1);\n", ii, ZMvdPixel[ii], Rr * esse, 26, ii);
          fprintf(MACRO, "CommonPixel%d->Draw();\n", ii);
          flaggo = false;
          break;
        }
      }
      if (flaggo) {
        fprintf(MACRO, "TMarker* SpuriousPixel%d = new TMarker(%f,%f,%d);\nSpuriousPixel%d->SetMarkerColor(2);\n", ii, ZMvdPixel[ii], Rr * esse, 26, ii);
        fprintf(MACRO, "SpuriousPixel%d->Draw();\n", ii);
      }
    }
    //   poi le strip

    for (i = 0; i < nStripHitsinTrack[iTrack]; i++) {
      ii = ListStripHitsinTrack[iTrack * MAXMVDSTRIPHITSINTRACK + i];
      if (zmin > ZMvdStrip[ii])
        zmin = ZMvdStrip[ii];
      if (zmax < ZMvdStrip[ii])
        zmax = ZMvdStrip[ii];

      esse = atan2(YMvdStrip[ii] - Oyy, XMvdStrip[ii] - Oxx);
      if (esse < 0.)
        esse += 2. * PI;

      if (Smin > esse)
        Smin = esse;
      if (Smax < esse)
        Smax = esse;

      bool flaggo = true;
      for (int k = 0; k < nMvdStripCommon; k++) {
        if (MvdStripCommonList[iTrack * MAXMVDSTRIPHITSINTRACK + k] == ii) {
          fprintf(MACRO, "TMarker* CommonStrip%d = new TMarker(%f,%f,%d);\nCommonStrip%d->SetMarkerColor(1);\n", ii, ZMvdStrip[ii], Rr * esse, 25, ii);
          fprintf(MACRO, "CommonStrip%d->Draw();\n", ii);
          flaggo = false;
          break;
        }
      }
      if (flaggo) {
        fprintf(MACRO, "TMarker* SpuriousStrip%d = new TMarker(%f,%f,%d);\nSpuriousStrip%d->SetMarkerColor(2);\n", ii, ZMvdStrip[ii], Rr * esse, 25, ii);
        fprintf(MACRO, "SpuriousStrip%d->Draw();\n", ii);
      }
    }

    //   ora i pixel 'Alone'
    for (i = 0; i < nMCMvdPixelAlone; i++) {

      ii = MCMvdPixelAloneList[iTrack * In_Put.nMvdPixelHit + i];
      if (zmin > ZMvdPixel[ii])
        zmin = ZMvdPixel[ii];
      if (zmax < ZMvdPixel[ii])
        zmax = ZMvdPixel[ii];

      esse = atan2(YMvdPixel[ii] - Oyy, XMvdPixel[ii] - Oxx);
      if (esse < 0.)
        esse += 2. * PI;
      if (Smin > esse)
        Smin = esse;
      if (Smax < esse)
        Smax = esse;
      fprintf(MACRO, "TMarker* AlonePixel%d = new TMarker(%f,%f,%d);\nAlonePixel%d->SetMarkerColor(4);\n", ii, ZMvdPixel[ii], Rr * esse, 26, ii);
      fprintf(MACRO, "AlonePixel%d->Draw();\n", ii);
    }

    //   ora le strip 'Alone'

    for (i = 0; i < nMCMvdStripAlone; i++) {
      ii = MCMvdStripAloneList[iTrack * In_Put.nMvdStripHit + i];
      if (zmin > ZMvdStrip[ii])
        zmin = ZMvdStrip[ii];
      if (zmax < ZMvdStrip[ii])
        zmax = ZMvdStrip[ii];

      esse = atan2(YMvdStrip[ii] - Oyy, XMvdStrip[ii] - Oxx);
      if (esse < 0.)
        esse += 2. * PI;
      if (Smin > esse)
        Smin = esse;
      if (Smax < esse)
        Smax = esse;

      fprintf(MACRO, "TMarker* AloneStrip%d = new TMarker(%f,%f,%d);\nAloneStrip%d->SetMarkerColor(4);\n", ii, ZMvdStrip[ii], Rr * esse, 25, ii);
      fprintf(MACRO, "AloneStrip%d->Draw();\n", ii);
    }

    //  plot della traccia trovata dal finder

    zmin2 = zmin;
    zmax2 = zmax;

    bool flaggo = true;
    if (-KAPPA * charge > 0.) { // Pz>0.
      if (zmax < 0.) {
        cout << "da WriteMacroSkewAssociatedHitswithMC, questa traccia"
             << " e' inconsistente col proprio Pz, non plottata!\n";
        flaggo = false;
      } else {
        zmin = 0.;
      }
    } else { // Pz<0.
      if (zmin > 0.) {
        cout << "da WriteMacroSkewAssociatedHitswithMC, questa traccia"
             << " e' inconsistente col proprio Pz, non plottata!\n";
        flaggo = false;
      } else {
        zmax = 0.;
      }
    }

    if (flaggo) {

      if (KAPPA >= 0.) {
        fmin = KAPPA * zmin + FI0;
        fmax = KAPPA * zmax + FI0;
      } else {
        fmax = KAPPA * zmin + FI0;
        fmin = KAPPA * zmax + FI0;
      }
      if (fmax >= 0.) {
        Nmax = (int)(0.5 * fmax / PI);
      } else {
        Nmax = ((int)(0.5 * fmax / PI)) - 1;
      }
      if (fmin >= 0.) {
        Nmin = (int)(0.5 * fmin / PI);
      } else {
        Nmin = ((int)(0.5 * fmin / PI)) - 1;
      }
      if (fabs(KAPPA) < 1.e-10) {
        cout << "da WriteMacroSkewAssociatedHitswithMC, questa traccia Found da PR non plottata"
             << " perche' ha fabs(KAPPA)<1.e-10.\n";
      } else {
        for (i = Nmin; i <= Nmax; i++) {
          // offset = 2.*PI*i; // [R.K. 02/2017] unused variable?
          z1 = (i * 2. * PI - FI0) / KAPPA;
          z2 = ((i + 1) * 2. * PI - FI0) / KAPPA;
          fprintf(MACRO, "TLine* FOUND%d = new TLine(%f,%f,%f,%f);\nFOUND%d->SetLineColor(2);\nFOUND%d->Draw();\n", i - Nmin, z1, 0., z2, Rr * 2. * PI, i - Nmin, i - Nmin);

        } //  end of  for(i=Nmin; i<= Nmax;++)
      }   // end of if(fabs(KAPPA)<1.e-10)

    } // end of if(flaggo)

    zmin = zmin2;
    zmax = zmax2;

    //----------------- ora la traccia MC corrispondente a questa traccia Stt

    // for(imc=0; imc<nMCTracks ; imc++){
    //            if(imc !=    daTrackFoundaTrackMC[ iTrack ]) continue;
    imc = daTrackFoundaTrackMC;

    if (imc > -1) {
      Int_t icode;
      Double_t Fifi, Kakka, o_x, o_y, Cx, Cy, Px, Py, carica; // Dd,r_r,  //[R.K. 01/2017] unused variable?
      PndMCTrack *pMC;
      pMC = (PndMCTrack *)fMCTrackArray->At(imc);
      if (pMC) {
        icode = pMC->GetPdgCode();       //   PDG code of track
        o_x = pMC->GetStartVertex().X(); //   X of starting point track
        o_y = pMC->GetStartVertex().Y(); //   Y of starting point track
        Px = pMC->GetMomentum().X();
        Py = pMC->GetMomentum().Y();
        aaa = sqrt(Px * Px + Py * Py);
        // r_r =   aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla //[R.K. 01/2017] unused variable?
        TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
        if (icode > 1000000000)
          carica = 1.;
        else
          carica = fParticle->Charge() / 3.; //   charge of track
        if (fabs(carica) >= 0.1) {
          Cx = o_x + Py * 1000. / (BFIELD * CVEL * carica);
          Cy = o_y - Px * 1000. / (BFIELD * CVEL * carica);
          Fifi = atan2(Cy, Cx); // MC truth Fifi angle of circle of Helix trajectory
          if (Fifi < 0.)
            Fifi += 2. * PI;
          if (fabs(pMC->GetMomentum().Z()) < 1.e-20)
            Kakka = 99999999.;
          else
            Kakka = -carica * 0.001 * BFIELD * CVEL / pMC->GetMomentum().Z();
          KAPPA = Kakka;
          FI0 = fmod(Fifi + PI, 2. * PI);
          if (KAPPA >= 0.) {
            fmin = KAPPA * zmin + FI0;
            fmax = KAPPA * zmax + FI0;
          } else {
            fmax = KAPPA * zmin + FI0;
            fmin = KAPPA * zmax + FI0;
          }
          if (fmax >= 0.) {
            Nmax = (int)(0.5 * fmax / PI);
          } else {
            Nmax = ((int)(0.5 * fmax / PI)) - 1;
          }
          if (fmin >= 0.) {
            Nmin = (int)(0.5 * fmin / PI);
          } else {
            Nmin = ((int)(0.5 * fmin / PI)) - 1;
          }

          for (i = Nmin; i <= Nmax; i++) {
            // offset = 2.*PI*i; // [R.K. 02/2017] unused variable?
            z1 = (i * 2. * PI - FI0) / KAPPA;
            z2 = ((i + 1) * 2. * PI - FI0) / KAPPA;
            fprintf(MACRO, "TLine* MC%d_%d = new TLine(%f,%f,%f,%f);\nMC%d_%d->SetLineColor(3);\nMC%d_%d->Draw();\n", imc, i - Nmin, z1, 0., z2, Rr * 2. * PI, imc, i - Nmin, imc,
                    i - Nmin);
          } //  end of  for(i=Nmin; i<= Nmax;++)

        } // end of if (fabs(carica)>=0.1 )
      }   // end of if ( pMC )
    }     // end of if( imc>-1 )

  } // end of  if( zmax >= zmin  &&  Smax >= Smin )

  fprintf(MACRO, "}\n");
  fclose(MACRO);
}

//------------------end of function PndTrkPlotMacros::WriteMacroSkewAssociatedHitswithMC

//--start of function PndTrkPlotMacros::WriteMacroSttParallelAssociatedHitsandMvdwithMC

// marker5 per cambioperl ;
//  inizio cambio_in_perl;

void PndTrkPlotMacros::WriteMacroSttParallelAssociatedHitsandMvdwithMC(PndTrkPlotMacros_InputData In_Put, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t primoangolo,
                                                                       Double_t ultimoangolo, Short_t /*Nhits*/, int iTrack, int iNome, Short_t daSttTrackaMCTrack,
                                                                       Short_t nMvdPixelHitsAssociatedToSttTra, Short_t nMvdStripHitsAssociatedToSttTra, Short_t nSkewHitsinTrack)
{
  //  fine cambio_in_perl;

  Int_t i, j, ii; //, i1, index, Kincl, nlow, nup, STATUS; //[R.K. 01/2017] unused variable?

  Double_t xmin, xmax, ymin, ymax,
    // dx, dy, diff, d1, d2, //[R.K. 01/2017] unused variable?
    // delta, deltax, deltay, deltaz, deltaS, //[R.K. 01/2017] unused variable?
    // factor, //[R.K. 01/2017] unused variable?
    // zmin, zmax, Smin, Smax, S1, S2, //[R.K. 01/2017] unused variable?
    // y1, y2,x1,x2, //z1, z2,  //[R.K. 01/2017] unused variable?
    // vx1, vy1, vz1, C0x1, C0y1, C0z1, //[R.K. 01/2017] unused variable?
    aaa, bbb; // ccc, angle, minor, major, //[R.K. 01/2017] unused variable?
              // distance, Rx, Ry, LL, //[R.K. 01/2017] unused variable?
  // Aellipsis1, Bellipsis1,fi1, //[R.K. 01/2017] unused variable?
  // fmin, fmax, offset, step, //[R.K. 01/2017] unused variable?
  // SkewInclWithRespectToS, zpos, zpos1, zpos2, //[R.K. 01/2017] unused variable?
  // Tiltdirection1[2], //[R.K. 01/2017] unused variable?
  // zl[200],zu[200], //[R.K. 01/2017] unused variable?
  // POINTS1[6]; //[R.K. 01/2017] unused variable?

  //  istruzione per perl : da qui non usare cambia_cxx ;

  //  conversioni  in variabili locali;

  // int MAXMCTRACKS = In_Put.MAXMCTRACKS; //[R.K. 01/2017] unused variable?
  // int MAXMVDPIXELHITS = In_Put.MAXMVDPIXELHITS; //[R.K. 01/2017] unused variable?
  int MAXMVDPIXELHITSINTRACK = In_Put.MAXMVDPIXELHITSINTRACK;
  // int MAXMVDSTRIPHITS = In_Put.MAXMVDSTRIPHITS; //[R.K. 01/2017] unused variable?
  // int MAXMVDSTRIPHITSINTRACK = In_Put.MAXMVDSTRIPHITSINTRACK; //[R.K. 01/2017] unused variable?
  // int MAXSCITILHITS = In_Put.MAXSCITILHITS; //[R.K. 01/2017] unused variable?
  int MAXSCITILHITSINTRACK = In_Put.MAXSCITILHITSINTRACK;
  int MAXSTTHITS = In_Put.MAXSTTHITS;
  int MAXSTTHITSINTRACK = In_Put.MAXSTTHITSINTRACK;
  // int MAXTRACKSPEREVENT = In_Put.MAXTRACKSPEREVENT; //[R.K. 01/2017] unused variable?
  // int dime = MAXSTTHITSINTRACK+MAXMVDPIXELHITSINTRACK+
  // MAXMVDSTRIPHITSINTRACK+MAXSCITILHITSINTRACK; //[R.K. 01/2017] unused variable?

  Double_t APOTEMAMAXINNERPARSTRAW = In_Put.apotemamaxinnerparstraw;
  Double_t APOTEMAMAXSKEWSTRAW = In_Put.apotemamaxskewstraw;
  Double_t APOTEMAMINOUTERPARSTRAW = In_Put.apotemaminouterparstraw;
  Double_t APOTEMAMINSKEWSTRAW = In_Put.apotemaminskewstraw;
  Double_t BFIELD = In_Put.bfield;

  Vec<Short_t> Charge(In_Put.Charge, In_Put.MAXTRACKSPEREVENT, "Charge");

  Double_t CVEL = In_Put.cvel;
  Vec<Short_t> daTrackFoundaTrackMC(In_Put.daTrackFoundaTrackMC, In_Put.MAXTRACKSPEREVENT, "daTrackFoundaTrackMC");
  Double_t DIMENSIONSCITIL = In_Put.dimensionscitil;
  // bool doMcComparison = In_Put.doMcComparison; //[R.K. 01/2017] unused variable?
  Vec<Double_t> FI0(In_Put.FI0, In_Put.MAXTRACKSPEREVENT, "FI0");
  TClonesArray *fMCTrackArray = In_Put.fMCTrackArray;
  // TClonesArray *fSttPointArray = In_Put.fSttPointArray; //[R.K. 01/2017] unused variable?

  Vec<Double_t> info(In_Put.info, In_Put.MAXSTTHITS * 7, "info"); // dimensione originale : [MAXSTTHITS][7];

  Vec<bool> InclusionListSciTil(In_Put.InclusionListSciTil, In_Put.MAXSCITILHITS, "InclusionListSciTil");
  Vec<bool> InclusionListStt(In_Put.InclusionListStt, In_Put.MAXSTTHITS, "InclusionListStt");
  int IVOLTE = In_Put.IVOLTE;
  Vec<Double_t> KAPPA(In_Put.KAPPA, In_Put.MAXTRACKSPEREVENT, "KAPPA");
  Vec<bool> keepit(In_Put.keepit, In_Put.MAXTRACKSPEREVENT, "keepit");
  // int istampa = In_Put.istampa; //[R.K. 01/2017] unused variable?

  Vec<Short_t> ListMvdPixelHitsinTrack(In_Put.ListMvdPixelHitsinTrack, In_Put.MAXTRACKSPEREVENT * In_Put.MAXMVDPIXELHITSINTRACK, "ListMvdPixelHitsinTrack");

  Vec<Short_t> ListMvdStripHitsinTrack(In_Put.ListMvdStripHitsinTrack, In_Put.MAXTRACKSPEREVENT * In_Put.MAXMVDSTRIPHITSINTRACK, "ListMvdStripHitsinTrack");

  Vec<Short_t> ListSciTilHitsinTrack(In_Put.ListSciTilHitsinTrack, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSCITILHITSINTRACK, "ListSciTilHitsinTrack");

  Vec<Short_t> ListSttParHitsinTrack(In_Put.ListSttParHitsinTrack, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITSINTRACK, "ListSttParHitsinTrack");

  Vec<Short_t> ListSttSkewHitsinTrack(In_Put.ListSttSkewHitsinTrack, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITSINTRACK, "ListSttSkewHitsinTrack");

  Vec<Short_t> ListTrackCandHit(In_Put.ListTrackCandHit,
                                In_Put.MAXTRACKSPEREVENT + In_Put.MAXSTTHITSINTRACK + In_Put.MAXMVDPIXELHITSINTRACK + In_Put.MAXMVDSTRIPHITSINTRACK + In_Put.MAXSCITILHITSINTRACK,
                                "ListTrackCandHit");

  Vec<Short_t> ListTrackCandHitType(
    In_Put.ListTrackCandHitType, In_Put.MAXTRACKSPEREVENT + In_Put.MAXSTTHITSINTRACK + In_Put.MAXMVDPIXELHITSINTRACK + In_Put.MAXMVDSTRIPHITSINTRACK + In_Put.MAXSCITILHITSINTRACK,
    "ListTrackCandHitType");

  Vec<Short_t> MCMvdPixelAloneList(In_Put.MCMvdPixelAloneList, In_Put.nTotalCandidates * In_Put.nMvdPixelHit, "MCMvdPixelAloneList");

  Vec<Short_t> MCMvdStripAloneList(In_Put.MCMvdStripAloneList, In_Put.nTotalCandidates * In_Put.nMvdStripHit, "MCMvdStripAloneList");

  Vec<Short_t> MCParalAloneList(In_Put.MCParalAloneList, In_Put.MAXTRACKSPEREVENT * In_Put.nSttHit, "MCParalAloneList");

  Vec<Short_t> MCSkewAloneList(In_Put.MCSkewAloneList, In_Put.MAXTRACKSPEREVENT * In_Put.nSttHit, "MCSkewAloneList");

  Vec<Double_t> MCSkewAloneX(In_Put.MCSkewAloneX, In_Put.MAXSTTHITS, "MCSkewAloneX");
  Vec<Double_t> MCSkewAloneY(In_Put.MCSkewAloneY, In_Put.MAXSTTHITS, "MCSkewAloneY");

  Vec<Short_t> MvdPixelCommonList(In_Put.MvdPixelCommonList, In_Put.nTotalCandidates * In_Put.MAXMVDPIXELHITSINTRACK, "MvdPixelCommonList");

  Vec<Short_t> MvdPixelSpuriList(In_Put.MvdPixelSpuriList, In_Put.nTotalCandidates * In_Put.MAXMVDPIXELHITSINTRACK, "MvdPixelSpuriList");

  Vec<Short_t> MvdStripCommonList(In_Put.MvdStripCommonList, In_Put.nTotalCandidates * In_Put.MAXMVDSTRIPHITSINTRACK, "MvdStripCommonList");

  Vec<Short_t> MvdStripSpuriList(In_Put.MvdStripSpuriList, In_Put.nTotalCandidates * In_Put.MAXMVDSTRIPHITSINTRACK, "MvdStripSpuriList");

  Vec<Short_t> nMCMvdPixelAlone(In_Put.nMCMvdPixelAlone, In_Put.nTotalCandidates, "nMCMvdPixelAlone");
  Vec<Short_t> nMCMvdStripAlone(In_Put.nMCMvdStripAlone, In_Put.nTotalCandidates, "nMCMvdStripAlone");

  Vec<Short_t> nMCParalAlone(In_Put.nMCParalAlone, In_Put.MAXTRACKSPEREVENT, "nMCParalAlone");
  Vec<Short_t> nMCSkewAlone(In_Put.nMCSkewAlone, In_Put.MAXTRACKSPEREVENT, "nMCSkewAlone");

  // Short_t nMCTracks = In_Put.nMCTracks; //[R.K. 01/2017] unused variable?

  Vec<Short_t> nMvdPixelCommon(In_Put.nMvdPixelCommon, In_Put.nTotalCandidates, "nMvdPixelCommon");
  // Short_t nMvdPixelHit = In_Put.nMvdPixelHit; //[R.K. 01/2017] unused variable?
  Vec<Short_t> nMvdPixelHitsinTrack(In_Put.nMvdPixelHitsinTrack, In_Put.MAXTRACKSPEREVENT, "nMvdPixelHitsinTrack");
  Vec<Short_t> nMvdPixelSpuriinTrack(In_Put.nMvdPixelSpuriinTrack, In_Put.MAXTRACKSPEREVENT, "nMvdPixelSpuriinTrack");

  Vec<Short_t> nMvdStripCommon(In_Put.nMvdStripCommon, In_Put.nTotalCandidates, "nMvdStripCommon");
  // Short_t nMvdStripHit = In_Put.nMvdStripHit; //[R.K. 01/2017] unused variable?
  Vec<Short_t> nMvdStripHitsinTrack(In_Put.nMvdStripHitsinTrack, In_Put.MAXTRACKSPEREVENT, "nMvdStripHitsinTrack");
  Vec<Short_t> nMvdStripSpuriinTrack(In_Put.nMvdStripSpuriinTrack, In_Put.nTotalCandidates, "nMvdStripSpuriinTrack");

  Vec<Short_t> nParalCommon(In_Put.nParalCommon, In_Put.MAXTRACKSPEREVENT, "nParalCommon");

  // Short_t nSciTilHits = In_Put.nSciTilHits; //[R.K. 01/2017] unused variable?

  Vec<Short_t> nSciTilHitsinTrack(In_Put.nSciTilHitsinTrack, In_Put.MAXTRACKSPEREVENT, "nSciTilHitsinTrack");

  Vec<Short_t> nSkewCommon(In_Put.nSkewCommon, In_Put.MAXTRACKSPEREVENT, "nSkewCommon");

  Vec<Short_t> nSpuriParinTrack(In_Put.nSpuriParinTrack, In_Put.MAXTRACKSPEREVENT, "nSpuriParinTrack");

  // Int_t    nSttHit = In_Put.nSttHit; //[R.K. 01/2017] unused variable?
  // Int_t    nSttParHit = In_Put.nSttParHit; //[R.K. 01/2017] unused variable?
  Vec<Short_t> nSttParHitsinTrack(In_Put.nSttParHitsinTrack, In_Put.MAXTRACKSPEREVENT, "nSttParHitsinTrack");
  // Int_t    nSttSkewHit = In_Put.nSttSkewHit; //[R.K. 01/2017] unused variable?
  Vec<Short_t> nSttSkewHitsinTrack(In_Put.nSttSkewHitsinTrack, In_Put.MAXTRACKSPEREVENT, "nSttSkewHitsinTrack");

  // Short_t  nTotalCandidates = In_Put.nTotalCandidates; //[R.K. 01/2017] unused variable?
  Vec<Short_t> nTrackCandHit(In_Put.nTrackCandHit, In_Put.MAXTRACKSPEREVENT, "nTrackCandHit");

  Vec<Double_t> Ox(In_Put.Ox, In_Put.MAXTRACKSPEREVENT, "Ox");
  Vec<Double_t> Oy(In_Put.Oy, In_Put.MAXTRACKSPEREVENT, "Oy");

  Vec<Short_t> ParalCommonList(In_Put.ParalCommonList, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITSINTRACK, "ParalCommonList");

  Vec<Short_t> ParSpuriList(In_Put.ParSpuriList, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITSINTRACK, "ParSpuriList");

  // dimensione originale : [MAXSCITILHITS][3];
  Vec<Double_t> posizSciTil(In_Put.posizSciTil, In_Put.MAXSCITILHITS * 3, "posizSciTil");

  Vec<Double_t> R(In_Put.R, In_Put.MAXTRACKSPEREVENT, "R");

  Double_t RSTRAWDETECTORMAX = In_Put.rstrawdetectormax;
  Double_t RSTRAWDETECTORMIN = In_Put.rstrawdetectormin;

  Vec<Double_t> sigmaXMvdPixel(In_Put.sigmaXMvdPixel, In_Put.MAXMVDPIXELHITS, "sigmaXMvdPixel");
  Vec<Double_t> sigmaXMvdStrip(In_Put.sigmaXMvdStrip, In_Put.MAXMVDSTRIPHITS, "sigmaXMvdStrip");
  Vec<Double_t> sigmaYMvdPixel(In_Put.sigmaYMvdPixel, In_Put.MAXMVDPIXELHITS, "sigmaYMvdPixel");
  Vec<Double_t> sigmaYMvdStrip(In_Put.sigmaYMvdStrip, In_Put.MAXMVDSTRIPHITS, "sigmaYMvdStrip");

  Vec<Double_t> SchosenSkew(In_Put.SchosenSkew, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITS, "SchosenSkew");

  Vec<Short_t> SkewCommonList(In_Put.SkewCommonList, In_Put.MAXTRACKSPEREVENT * In_Put.MAXSTTHITSINTRACK, "SkewCommonList");

  Double_t VERTICALGAP = In_Put.verticalgap;
  Vec<Double_t> XMvdPixel(In_Put.XMvdPixel, In_Put.MAXMVDPIXELHITS, "XMvdPixel");
  Vec<Double_t> XMvdStrip(In_Put.XMvdStrip, In_Put.MAXMVDSTRIPHITS, "XMvdStrip");
  Vec<Double_t> YMvdPixel(In_Put.YMvdPixel, In_Put.MAXMVDPIXELHITS, "YMvdPixel");
  Vec<Double_t> YMvdStrip(In_Put.YMvdStrip, In_Put.MAXMVDSTRIPHITS, "YMvdStrip");
  Vec<Double_t> WDX(In_Put.WDX, In_Put.MAXSTTHITS, "WDX");
  Vec<Double_t> WDY(In_Put.WDY, In_Put.MAXSTTHITS, "WDY");
  Vec<Double_t> WDZ(In_Put.WDZ, In_Put.MAXSTTHITS, "WDZ");
  Vec<Double_t> ZMvdPixel(In_Put.ZMvdPixel, In_Put.MAXMVDPIXELHITS, "ZMvdPixel");
  Vec<Double_t> ZMvdStrip(In_Put.ZMvdStrip, In_Put.MAXMVDSTRIPHITS, "ZMvdStrip");

  //  istruzione per perl : da qui usa cambia_cxx ;

  //---------- parallel straws Macro now
  char nome[300], nome2[300];
  sprintf(nome, "MacroSttMvdXYwithMCEvent%dT%d", IVOLTE, iNome);
  sprintf(nome2, "%s.C", nome);
  FILE *MACRO = fopen(nome2, "w");
  fprintf(MACRO, "{\n");

  xmin = -1.3 * RSTRAWDETECTORMAX;
  xmax = 1.3 * RSTRAWDETECTORMAX;
  ymin = -1.3 * RSTRAWDETECTORMAX;
  ymax = 1.3 * RSTRAWDETECTORMAX;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  //	disegna il BiHexagon destro e sinistro delle inner parallel straws.

  char myname[100];

  sprintf(myname, "InnerPar");
  DrawBiHexagonInMacro(VERTICALGAP, MACRO, RSTRAWDETECTORMIN, APOTEMAMAXINNERPARSTRAW,
                       4, // color code, 4= blue.
                       myname);
  //--------------
  //	disegna il BiHexagon destro e sinistro delle skew straws.
  sprintf(myname, "Skew");
  DrawBiHexagonInMacro(VERTICALGAP, MACRO, APOTEMAMINSKEWSTRAW, APOTEMAMAXSKEWSTRAW,
                       2, // color code.
                       myname);
  //--------------
  //	disegna il BiHexagon destro e sinistro delle skew straws.
  sprintf(myname, "OuterPar");
  DrawHexagonCircleInMacro(VERTICALGAP, MACRO, APOTEMAMINOUTERPARSTRAW, RSTRAWDETECTORMAX,
                           4, // color code.
                           myname);
  //--------------

  //---- disegna gli Scitil.

  // prima la lista di SciTil 'common' :
  for (i = 0; i < In_Put.nSciTilCommon[iTrack]; i++) {
    j = In_Put.SciTilCommonList[iTrack * MAXSCITILHITSINTRACK + i];
    disegnaSciTilHit(1, // color code; the same as in SetColor of root;
                     DIMENSIONSCITIL, MACRO, posizSciTil.at(j * 3 + 0), posizSciTil.at(j * 3 + 1), j,
                     0 // if 0 then SciTil draw in XY; if 1 then SciTil draw in SZ; else
                       // SciTil draw in UV.
    );
  }
  // poi la lista di SciTil 'spuri' :
  for (i = 0; i < In_Put.nSciTilSpuriinTrack[iTrack]; i++) {
    j = In_Put.SciTilSpuriList[iTrack * MAXSCITILHITSINTRACK + i];
    disegnaSciTilHit(2, // color code; the same as in SetColor of root;
                     DIMENSIONSCITIL, MACRO, posizSciTil.at(j * 3 + 0), posizSciTil.at(j * 3 + 1), j,
                     0 // if 0 then SciTil draw in XY; if 1 then SciTil draw in SZ; else
                       // SciTil draw in UV.
    );
  }
  // poi la lista di SciTil 'alone' :
  for (i = 0; i < In_Put.nMCSciTilAlone[iTrack]; i++) {
    j = In_Put.MCSciTilAloneList[iTrack * In_Put.nSciTilHits + i];
    disegnaSciTilHit(4, // color code; the same as in SetColor of root;
                     DIMENSIONSCITIL, MACRO, posizSciTil.at(j * 3 + 0), posizSciTil.at(j * 3 + 1), j,
                     0 // if 0 then SciTil draw in XY; if 1 then SciTil draw in SZ; else
                       // SciTil draw in UV.
    );
  }

  // --------------------------------

  fprintf(MACRO, "TEllipse* FoundTrack = new TEllipse(%f,%f,%f,%f,%f,%f);\n", Oxx, Oyy, Rr, Rr, primoangolo, ultimoangolo);

  fprintf(MACRO, "FoundTrack->SetLineColor(2);\nFoundTrack->SetFillStyle(0);\nFoundTrack->Draw(\"only\");\n");

  disegnaAssiXY(MACRO, xmin, xmax, ymin, ymax);

  //------------- hits paralleli in comune con traccia MC
  for (ii = 0; ii < nParalCommon.at(iTrack); ii++) {
    i = ParalCommonList.at(iTrack * MAXSTTHITSINTRACK + ii);
    fprintf(MACRO, "TEllipse* CommonParalHit%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nCommonParalHit%d->SetFillStyle(0);\nCommonParalHit%d->Draw();\n", i, info[i * 7 + 0],
            info.at(i * 7 + 1), info.at(i * 7 + 3), info.at(i * 7 + 3), i, i);
  }
  //------------- hits paralleli spuri
  for (ii = 0; ii < nSpuriParinTrack.at(iTrack); ii++) {
    i = ParSpuriList.at(iTrack * MAXSTTHITSINTRACK + ii);
    fprintf(MACRO, "TEllipse* SpurParalHit%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nSpurParalHit%d->SetFillStyle(0);\nSpurParalHit%d->SetLineColor(2);\nSpurParalHit%d->Draw();\n",
            i, info[i * 7 + 0], info.at(i * 7 + 1), info.at(i * 7 + 3), info.at(i * 7 + 3), i, i, i);
  }
  //------------- hits paralleli MC 'alone'
  for (ii = 0; ii < nMCParalAlone.at(iTrack); ii++) {
    i = MCParalAloneList.at(iTrack * In_Put.nSttHit + ii);
    fprintf(MACRO,
            "TEllipse* AloneParalHit%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nAloneParalHit%d->SetFillStyle(0);\nAloneParalHit%d->SetLineColor(4);\nAloneParalHit%d->Draw();\n", i,
            info[i * 7 + 0], info.at(i * 7 + 1), info.at(i * 7 + 3), info.at(i * 7 + 3), i, i, i);
  }

  //-------------
  //------------- hits skew in comune e spuri di traccia MC

  for (i = 0; i < nSkewHitsinTrack; i++) {
    ii = ListSttSkewHitsinTrack.at(iTrack * MAXSTTHITSINTRACK + i);
    aaa = Oxx + Rr * cos(SchosenSkew.at(iTrack * MAXSTTHITS + ii));
    bbb = Oyy + Rr * sin(SchosenSkew.at(iTrack * MAXSTTHITS + ii));
    bool flaggo = true;
    for (int k = 0; k < nSkewCommon.at(iTrack); k++) {
      if (SkewCommonList.at(iTrack * MAXSTTHITSINTRACK + k) == ii) {
        fprintf(MACRO, "TMarker* CommonSkewHit%d = new TMarker(%f,%f,%d);\n", ii, aaa, bbb, 28);
        fprintf(MACRO, "CommonSkewHit%d->SetMarkerColor(1);\nCommonSkewHit%d->Draw();\n", ii, ii);
        flaggo = false;
        break;
      }
    }
    if (flaggo) {
      fprintf(MACRO, "TMarker* SpurSkewHit%d = new TMarker(%f,%f,%d);\n", ii, aaa, bbb, 28);
      fprintf(MACRO, "SpurSkewHit%d->SetMarkerColor(2);\nSpurSkewHit%d->Draw();\n", ii, ii);
    }
  }
  //------------- hits paralleli MC 'alone'
  for (ii = 0; ii < nMCSkewAlone.at(iTrack); ii++) {
    i = MCSkewAloneList.at(iTrack * In_Put.nSttHit + ii);
    fprintf(MACRO, "TMarker* AloneSkewHit%d = new TMarker(%f,%f,%d);\nAloneSkewHit%d->SetMarkerColor(4);\nAloneSkewHit%d->Draw();\n", i, MCSkewAloneX.at(i), MCSkewAloneY.at(i), 28,
            i, i);
  }

  //------------- now the Strips

  for (i = 0; i < nMvdStripHitsAssociatedToSttTra; i++) {
    ii = ListMvdStripHitsinTrack.at(iTrack * In_Put.MAXMVDSTRIPHITSINTRACK + i);
    // x1= XMvdStrip.at(ii)-sigmaXMvdStrip.at(ii); //[R.K. 9/2018] unused
    // x2= XMvdStrip.at(ii)+sigmaXMvdStrip.at(ii); //[R.K. 9/2018] unused
    // y1= YMvdStrip.at(ii)-sigmaYMvdStrip.at(ii); //[R.K. 9/2018] unused
    // y2= YMvdStrip.at(ii)+sigmaYMvdStrip.at(ii); //[R.K. 9/2018] unused

    bool flaggo = true;
    for (int k = 0; k < nMvdStripCommon.at(iTrack); k++) {
      if (MvdStripCommonList.at(iTrack * In_Put.MAXMVDSTRIPHITSINTRACK + k) == ii) {
        fprintf(MACRO, "TMarker* CommonMvdStrip%d = new TMarker(%f,%f,%d);\n", ii, XMvdStrip.at(ii), YMvdStrip.at(ii), 25);
        fprintf(MACRO, "CommonMvdStrip%d->SetMarkerColor(1);\nCommonMvdStrip%d->Draw();\n", ii, ii);
        flaggo = false;
        break;
      }
    }
    if (flaggo) {
      fprintf(MACRO, "TMarker* SpurMvdStrip%d = new TMarker(%f,%f,%d);\n", ii, XMvdStrip.at(ii), YMvdStrip.at(ii), 25);
      fprintf(MACRO, "SpurMvdStrip%d->SetMarkerColor(2);\nSpurMvdStrip%d->Draw();\n", ii, ii);
    }
  }

  //-------------- hit Mvd Strips 'Alone'
  for (ii = 0; ii < nMCMvdStripAlone.at(iTrack); ii++) {
    i = MCMvdStripAloneList.at(iTrack * In_Put.nMvdStripHit + ii);
    fprintf(MACRO, "TMarker* AloneMvdStrip%d = new TMarker(%f,%f,%d);\nAloneMvdStrip%d->SetMarkerColor(4);\nAloneMvdStrip%d->Draw();\n", i, XMvdStrip.at(i), YMvdStrip.at(i), 25, i,
            i);
  }
  //-------------

  for (i = 0; i < nMvdPixelHitsAssociatedToSttTra; i++) {
    ii = ListMvdPixelHitsinTrack.at(iTrack * MAXMVDPIXELHITSINTRACK + i);
    // x1= XMvdPixel.at(ii)-sigmaXMvdPixel.at(ii); //[R.K. 9/2018] unused
    // x2= XMvdPixel.at(ii)+sigmaXMvdPixel.at(ii); //[R.K. 9/2018] unused
    // y1= YMvdPixel.at(ii)-sigmaYMvdPixel.at(ii); //[R.K. 9/2018] unused
    // y2= YMvdPixel.at(ii)+sigmaYMvdPixel.at(ii); //[R.K. 9/2018] unused

    bool flaggo = true;
    for (int k = 0; k < nMvdPixelCommon.at(iTrack); k++) {
      if (MvdPixelCommonList.at(iTrack * In_Put.MAXMVDPIXELHITSINTRACK + k) == ii) {
        fprintf(MACRO, "TMarker* CommonMvdPixel%d = new TMarker(%f,%f,%d);\n", ii, XMvdPixel.at(ii), YMvdPixel.at(ii), 26);
        fprintf(MACRO, "CommonMvdPixel%d->SetMarkerColor(1);\nCommonMvdPixel%d->Draw();\n", ii, ii);
        flaggo = false;
        break;
      }
    }
    if (flaggo) {
      fprintf(MACRO, "TMarker* SpurMvdPixel%d = new TMarker(%f,%f,%d);\n", ii, XMvdPixel.at(ii), YMvdPixel.at(ii), 26);
      fprintf(MACRO, "SpurMvdPixel%d->SetMarkerColor(2);\nSpurMvdPixel%d->Draw();\n", ii, ii);
    }
  }

  //-------------- hit Mvd Pixel 'Alone'
  for (ii = 0; ii < nMCMvdPixelAlone.at(iTrack); ii++) {
    i = MCMvdPixelAloneList.at(iTrack * In_Put.nMvdPixelHit + ii);
    fprintf(MACRO, "TMarker* AloneMvdPixel%d = new TMarker(%f,%f,%d);\nAloneMvdPixel%d->SetMarkerColor(4);\nAloneMvdPixel%d->Draw();\n", i, XMvdPixel.at(i), YMvdPixel.at(i), 26, i,
            i);
  }
  //-------------
  //----------------- ora la traccia MC corrispondente a questa traccia Stt
  if (daSttTrackaMCTrack > -1) {
    Int_t icode, im;
    Double_t r_r, o_x, o_y, Cx, Cy, Px, Py, carica; // Dd, Fifi, //[R.K. 01/2017] unused variable?
    PndMCTrack *pMC;
    im = daSttTrackaMCTrack;
    pMC = (PndMCTrack *)fMCTrackArray->At(im);
    if (pMC) {
      icode = pMC->GetPdgCode();       //   PDG code of track
      o_x = pMC->GetStartVertex().X(); //   X of starting point track
      o_y = pMC->GetStartVertex().Y(); //   Y of starting point track
      Px = pMC->GetMomentum().X();
      Py = pMC->GetMomentum().Y();
      aaa = sqrt(Px * Px + Py * Py);
      r_r = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
      if (icode > 1000000000)
        carica = 1.;
      else
        carica = fParticle->Charge() / 3.; //   charge of track
      if (fabs(carica) >= 0.1) {
        Cx = o_x + Py * 1000. / (BFIELD * CVEL * carica);
        Cy = o_y - Px * 1000. / (BFIELD * CVEL * carica);
        fprintf(MACRO, "TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,%f,%f);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw(\"only\");\n", im, Cx, Cy, r_r, r_r, 0., 360.,
                im, im, im);
      } // end of  if (fabs(carica)>=0.1 )
    }   // end if ( pMC )
  };    //  end of  if( daSttTrackaMCTrack>-1
        //----------- fine parte del MC
  fprintf(MACRO, "}\n");
  fclose(MACRO);

  return;
}

//--end of function PndTrkPlotMacros::WriteMacroSttParallelAssociatedHitsandMvdwithMC

ClassImp(PndTrkPlotMacros);
