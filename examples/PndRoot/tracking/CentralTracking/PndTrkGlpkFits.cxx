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

#include "PndTrkGlpkFits.h"
#include "glpk.h"
#include <cmath>
#include <iostream>
// Root includes
#include "TROOT.h"

using namespace std;

#define PI 3.141592654

//----------begin of function PndTrkGlpkFits::FitHelixCylinder

Short_t PndTrkGlpkFits::FitHelixCylinder(Short_t nHitsinTrack, Double_t *Xconformal, Double_t *Yconformal, Double_t *DriftRadiusconformal, Double_t *ErrorDriftRadiusconformal,
                                         Double_t rotationangle, Double_t trajectory_vertex[2], Short_t NMAX, Double_t *emme, Double_t *qu, Double_t *pAlfa, Double_t *pBeta,
                                         Double_t *pGamma, bool *Type, int istampa, int IVOLTE)
{

  //   definition of variables for the glpsol  solver
  //    ROWS (for read_rows  function)
  //

  Short_t NpointsInFit = nHitsinTrack - NMAX < 0 ? nHitsinTrack : NMAX;
  bool mvdhit[NpointsInFit];

  Double_t A, alfetta, angle, Delta[NpointsInFit], M = 1., m_result, offsety, Oxx[NpointsInFit], Oyy[NpointsInFit], q_result;

  Short_t i, j, ii, iii, nSttHits, nMvdHits;
  Short_t Status;

  float m1_result, m2_result, q1_result, q2_result, A1_result, A2_result;

  // --

  //-------------- stampaggi
  if (istampa >= 3) {
    cout << "from FitHelixCylinder,prima di rotazione,  Evento " << IVOLTE << ", nHitsinTrack = " << nHitsinTrack << "\nfrom FitHelixCylinder, nPointsinFit = " << NpointsInFit
         << endl;
    for (i = 0; i < NpointsInFit; i++) {
      cout << "  Xconformal[" << i << "] = " << Xconformal[i] << ";   Yconformal[" << i << "] = " << Yconformal[i] << ",  drift radius conformal " << DriftRadiusconformal[i]
           << endl
           << "\tErrordiriftradiusconformal = " << ErrorDriftRadiusconformal[i] << endl;
    }
  }
  //------------ end stampaggi

  if (nHitsinTrack < 2) {
    *Type = false;
    return -1;
  }
  //  use the trick of increasing the rotation angle by 10 degrees in order to obtain always a positive m
  rotationangle -= PI / 18.;

  Double_t cose = cos(rotationangle), sine = sin(rotationangle);

  nSttHits = nMvdHits = 0;
  for (i = 0; i < NpointsInFit; i++) {
    Oxx[i] = Xconformal[i] * cose + Yconformal[i] * sine;
    Oyy[i] = -Xconformal[i] * sine + Yconformal[i] * cose;
    Delta[i] = 3. * ErrorDriftRadiusconformal[i]; //   3 times the Drift Radius

    if (DriftRadiusconformal[i] < 0.) {
      mvdhit[i] = true;
      nMvdHits++;
    } else {
      mvdhit[i] = false;
      nSttHits++;
    }
  }

  //-------------- stampaggi
  if (istampa >= 3) {
    cout << "from FitHelixCylinder, dopo rotazione, Evento " << IVOLTE << ", nHitsinTrack = " << nHitsinTrack << "\nfrom FitHelixCylinder, nPointsinFit = " << NpointsInFit << endl;
    for (i = 0; i < NpointsInFit; i++) {
      cout << "  Ox[" << i << "] = " << Oxx[i] << ";   Oy[" << i << "] = " << Oyy[i] << ",  Delta " << Delta[i] << endl;
    }
  }
  //------------ end stampaggi
  //--------- calculation of # structural variables (see Gianluigi's logbook pag. 236) etc.etc.

  int NStructVar = 4 + 1 + nMvdHits * 2 + nSttHits * 4;
  //               m1,m2,q1,q2     DUM     lam & sigma      lamp & lamm & sigmap & sigmam

  int nRows = 1 + nMvdHits * 4 + nSttHits * 9;
  //	         OBJECT     A,B,C,D        Ap,Bp,Cp,Dp,Am,Bm,Cm,Dm,LM
  int NStructRowsMax = 8 * NpointsInFit; //  maximum number of ROWS in which a
                                         //  structural variable (for instance M ) can be found
  int NRowsInWhichStructVarArePresent[NStructVar];
  int nRanges = nSttHits;
  int nBounds = NpointsInFit + nSttHits + 1;

  //----  creating the various service arrays

  char OBJECTname[8];
  sprintf(OBJECTname, "OBJECT");

  int typeRows[nRows];
  char *nameRows[nRows];
  char auxnameRows[nRows][5];

  double final_values[NStructVar];
  char *StructVarName[NStructVar];
  char auxStructVarName[NStructVar][8];
  char *NameRowsInWhichStructVarArePresent[NStructVar * NStructRowsMax];
  char aux[NStructVar * NStructRowsMax][5];
  double Coefficients[NStructVar * NStructRowsMax];
  //--------for RHS information
  double ValueB[nRows - 1]; // -1 because OBJECT dowsn't have a RHS boundary.
  //--------for RANGES information
  double ValueRanges[nRanges];
  char *NameRanges[nRanges];
  char auxNameRanges[nRanges][20];

  //--------for BOUNDS information
  double BoundValue[nBounds];
  char *BoundStructVarName[nBounds];
  char auxBoundStructVarName[nBounds][8];
  char *TypeofBound[nBounds];
  char auxTypeofBound[nBounds][20];
  //--------end BOUNDS information

  //---------------------------------------------------

  //--- calculate array      NRowsInWhichStructVarArePresent
  NRowsInWhichStructVarArePresent[0] =                                //  this is for m1
    NRowsInWhichStructVarArePresent[1] =                              //  this is for m2
    NRowsInWhichStructVarArePresent[2] =                              //  this is for q1
    NRowsInWhichStructVarArePresent[3] = nMvdHits * 2 + nSttHits * 4; //  this is for q2
  //--- the following is for the  lam* (Mvd hits) or lamp* (Stt hits) structural variables
  ii = 0;
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      NRowsInWhichStructVarArePresent[4 + ii] = 4;
    } else {
      NRowsInWhichStructVarArePresent[4 + ii] = 5;
    }
    ii++;
  }
  //--- the following is for the  lamm* (Mvd Hits, if any)  structural variables
  for (i = 0; i < NpointsInFit; i++) {
    if (!mvdhit[i]) {
      NRowsInWhichStructVarArePresent[4 + ii] = 5;
      ii++;
    }
  }

  //--- the following is for the  sigma   structural variables
  for (i = 0; i < nMvdHits + 2 * nSttHits; i++) {
    NRowsInWhichStructVarArePresent[4 + ii + i] = 5;
  }
  //--- the following is for the    DUM    structural variable

  NRowsInWhichStructVarArePresent[4 + ii + nMvdHits + 2 * nSttHits] = nMvdHits * 4 + nSttHits * 8;

  //-----------------  write the ROWS  section
  // sprintf(&(auxnameRows[0][0]),"OBJECT");
  // nameRows[0]=&auxnameRows[0][0];
  nameRows[0] = OBJECTname;
  typeRows[0] = GLP_FR;
  for (i = 0, ii = 0; i < NpointsInFit; i++) {

    if (mvdhit[i]) {
      typeRows[1 + ii] = GLP_UP;
      typeRows[2 + ii] = GLP_UP;
      typeRows[3 + ii] = GLP_UP;
      typeRows[4 + ii] = GLP_UP;
      typeRows[5 + ii] = GLP_LO;

      sprintf(&(auxnameRows[1 + ii][0]), "A%d", i);
      nameRows[1 + ii] = &auxnameRows[1 + ii][0];
      sprintf(&(auxnameRows[2 + ii][0]), "B%d", i);
      nameRows[2 + ii] = &auxnameRows[2 + ii][0];
      sprintf(&(auxnameRows[3 + ii][0]), "C%d", i);
      nameRows[3 + ii] = &auxnameRows[3 + ii][0];
      sprintf(&(auxnameRows[4 + ii][0]), "D%d", i);
      nameRows[4 + ii] = &auxnameRows[4 + ii][0];
      ii += 4;

    } else {
      typeRows[1 + ii] = GLP_UP;
      typeRows[2 + ii] = GLP_UP;
      typeRows[3 + ii] = GLP_UP;
      typeRows[4 + ii] = GLP_UP;
      typeRows[5 + ii] = GLP_UP;
      typeRows[6 + ii] = GLP_UP;
      typeRows[7 + ii] = GLP_UP;
      typeRows[8 + ii] = GLP_UP;
      typeRows[9 + ii] = GLP_LO;

      sprintf(&(auxnameRows[1 + ii][0]), "Ap%d", i);
      nameRows[1 + ii] = &auxnameRows[1 + ii][0];
      sprintf(&(auxnameRows[2 + ii][0]), "Bp%d", i);
      nameRows[2 + ii] = &auxnameRows[2 + ii][0];
      sprintf(&(auxnameRows[3 + ii][0]), "Cp%d", i);
      nameRows[3 + ii] = &auxnameRows[3 + ii][0];
      sprintf(&(auxnameRows[4 + ii][0]), "Dp%d", i);
      nameRows[4 + ii] = &auxnameRows[4 + ii][0];
      sprintf(&(auxnameRows[5 + ii][0]), "Am%d", i);
      nameRows[5 + ii] = &auxnameRows[5 + ii][0];
      sprintf(&(auxnameRows[6 + ii][0]), "Bm%d", i);
      nameRows[6 + ii] = &auxnameRows[6 + ii][0];
      sprintf(&(auxnameRows[7 + ii][0]), "Cm%d", i);
      nameRows[7 + ii] = &auxnameRows[7 + ii][0];
      sprintf(&(auxnameRows[8 + ii][0]), "Dm%d", i);
      nameRows[8 + ii] = &auxnameRows[8 + ii][0];
      sprintf(&(auxnameRows[9 + ii][0]), "LM%d", i);
      nameRows[9 + ii] = &auxnameRows[9 + ii][0];
      ii += 9;
    }
  }

  //-----------------  write the COLUMNS  section

  //      fprintf(FMCS,"COLUMNS\n");  //--------stampaggi

  //  Column variable  m1

  ii = 0;
  for (i = 0; i < NpointsInFit; i++) {

    if (mvdhit[i]) {
      //---stampaggi
      //          fprintf(FMCS,"  m1 A%d   %g\n  m1 B%d  %g\n",i,Oxx[i],i,-Oxx[i]);
      //-----stampaggi, fine

      Coefficients[ii] = Oxx[i];
      Coefficients[ii + 1] = -Oxx[i];
      ii += 2;
    } else {
      //---stampaggi
      //          fprintf(FMCS,"  m1 Ap%d  %g  Am%d  %g\n  m1 Bp%d  %g   Bm%d  %g\n",
      //                                  i,Oxx[i],i,Oxx[i],i,-Oxx[i],i,-Oxx[i]);
      //-----stampaggi, fine

      Coefficients[ii] = Oxx[i];
      Coefficients[ii + 1] = Oxx[i];
      Coefficients[ii + 2] = -Oxx[i];
      Coefficients[ii + 3] = -Oxx[i];
      ii += 4;
    }
  }

  //  Column variable  m2
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  m2 A%d  %g\n  m2 B%d  %g\n",//---stampaggi
      //                                  i,-Oxx[i],i,Oxx[i]);//---stampaggi
      Coefficients[NStructRowsMax + ii] = -Oxx[i];
      Coefficients[NStructRowsMax + ii + 1] = Oxx[i];
      ii += 2;
    } else {
      //          fprintf(FMCS,"  m2 Ap%d  %g  Am%d  %g\n  m2 Bp%d  %g   Bm%d  %g\n",//---stampaggi
      //                                  i,-Oxx[i],i,-Oxx[i],i,Oxx[i],i,Oxx[i]);//---stampaggi
      Coefficients[NStructRowsMax + ii] = -Oxx[i];
      Coefficients[NStructRowsMax + ii + 1] = -Oxx[i];
      Coefficients[NStructRowsMax + ii + 2] = Oxx[i];
      Coefficients[NStructRowsMax + ii + 3] = Oxx[i];
      ii += 4;
    }
  }

  //  Column variable  q1
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  q1 A%d   1.\n  q1 B%d  -1.\n",//---stampaggi
      //                                  i,i);//---stampaggi
      Coefficients[2 * NStructRowsMax + ii] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 1] = -1.;
      ii += 2;
    } else {
      //          fprintf(FMCS,"  q1 Ap%d   1.  Am%d   1.\n  q1 Bp%d  -1.  Bm%d  -1.\n",//---stampaggi
      //                                  i,i,i,i);//---stampaggi
      Coefficients[2 * NStructRowsMax + ii] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 1] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 2] = -1.;
      Coefficients[2 * NStructRowsMax + ii + 3] = -1.;
      ii += 4;
    }
  }

  //  Column variable  q2
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  q2 A%d   -1.\n  q2 B%d   1.\n",//---stampaggi
      //                                  i,i);//---stampaggi
      Coefficients[3 * NStructRowsMax + ii] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 1] = 1.;
      ii += 2;
    } else {
      //          fprintf(FMCS,"  q2 Ap%d   -1.  Am%d   -1.\n  q2 Bp%d   1.   Bm%d   1.\n",//---stampaggi
      //                                  i,i,i,i);//---stampaggi
      Coefficients[3 * NStructRowsMax + ii] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 1] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 2] = 1.;
      Coefficients[3 * NStructRowsMax + ii + 3] = 1.;
      ii += 4;
    }
  }

  //  Column variable  lambdap(i)
  for (i = 0; i < NpointsInFit; i++) {
    ii = (4 + i) * NStructRowsMax;
    Coefficients[ii] = -M;
    Coefficients[ii + 1] = -M;
    Coefficients[ii + 2] = -M;
    Coefficients[ii + 3] = M;
    //	if( mvdhit[i]){
    //  fprintf(FMCS,"  lam%d  A%d  %g  B%d  %g\n  lam%d  C%d  %g  D%d   %g\n",//---stampaggi
    //                      i,i,-M,i,-M, i , i,-M, i, M);//---stampaggi
    //	} else {
    //  fprintf(FMCS,"  lamp%d  Ap%d  %g  Bp%d  %g\n  lamp%d  Cp%d  %g  Dp%d   %g\n  lamp%d  LM%d  1.\n",//---stampaggi
    //                      i,i,-M,i,-M, i , i,-M, i, M, i,i);//---stampaggi
    //	}

    if (!mvdhit[i])
      Coefficients[ii + 4] = 1.;
  }

  //  Column variable  lambdam(i)
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    ii += NStructRowsMax;
    //         fprintf(FMCS,"  lamm%d  Am%d  %g  Bm%d  %g\n  lamm%d  Cm%d  %g  Dm%d %g\n  lamm%d  LM%d  1.\n",//---stampaggi
    //                                                i,i,-M,i,-M, i,i , -M, i, M, i,i);//---stampaggi
    Coefficients[ii] = -M;
    Coefficients[ii + 1] = -M;
    Coefficients[ii + 2] = -M;
    Coefficients[ii + 3] = M;
    Coefficients[ii + 4] = 1.;
  }
  //  Column variable  SIGp(i)
  for (i = 0; i < NpointsInFit; i++) {
    ii += NStructRowsMax;

    /*
      if( mvdhit[i]){
          fprintf(FMCS,"  SIG%d  OBJECT  %g  A%d  -1.\n  SIG%d  B%d    -1. C%d  1.\n  SIG%d  D%d -1.\n",//---stampaggi
                                                    i,1./Delta[i],i,i,i,i,i,i);//---stampaggi

      } else {
          fprintf(FMCS,"  SIGp%d  OBJECT  %g  Ap%d  -1.\n  SIGp%d  Bp%d    -1. Cp%d  1.\n  SIGp%d  Dp%d -1.\n",//---stampaggi
                                                    i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
      }
    */

    Coefficients[ii] = 1. / Delta[i];
    Coefficients[ii + 1] = -1.;
    Coefficients[ii + 2] = -1.;
    Coefficients[ii + 3] = 1.;
    Coefficients[ii + 4] = -1.;
  }
  //  Column variable  SIGm(i)
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    ii += NStructRowsMax;
    //         fprintf(FMCS,"  SIGm%d  OBJECT %g  Am%d  -1.\n  SIGm%d  Bm%d   -1. Cm%d   1.\n  SIGm%d  Dm%d  -1.\n",//---stampaggi
    //                                                i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
    Coefficients[ii] = 1. / Delta[i];
    Coefficients[ii + 1] = -1.;
    Coefficients[ii + 2] = -1.;
    Coefficients[ii + 3] = 1.;
    Coefficients[ii + 4] = -1.;
  }

  //  Column variable  DUM
  //-----------stampaggi
  /*
        for(i=0 ; i< NpointsInFit ; i++) {
    if( mvdhit[i]){
           fprintf(FMCS,"  DUM     A%d  1.\n  DUM   B%d   1.\n  DUM    C%d   1.\n",i,i,i);
           fprintf(FMCS,"  DUM     D%d  1.\n",i);
    } else {
           fprintf(FMCS,"  DUM     Ap%d  1.      Am%d       1.\n  DUM   Bp%d   1.    Bm%d   1.\n  DUM    Cp%d   1.   Cm%d   1\n",
                                                  i,i,i,i,i,i);
           fprintf(FMCS,"  DUM     Dp%d  1.      Dm%d       1.\n",i,i);
    }
        }
  */
  //---fine stampaggi
  ii += NStructRowsMax;
  for (i = 0, iii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      Coefficients[ii + iii] = 1.;
      Coefficients[ii + iii + 1] = 1.;
      Coefficients[ii + iii + 2] = 1.;
      Coefficients[ii + iii + 3] = 1.;
      iii += 4;
    } else {
      Coefficients[ii + iii] = 1.;
      Coefficients[ii + iii + 1] = 1.;
      Coefficients[ii + iii + 2] = 1.;
      Coefficients[ii + iii + 3] = 1.;
      Coefficients[ii + iii + 4] = 1.;
      Coefficients[ii + iii + 5] = 1.;
      Coefficients[ii + iii + 6] = 1.;
      Coefficients[ii + iii + 7] = 1.;
      iii += 8;
    }
  }

  //-----------

  //--- give the names to the Structural Variables

  sprintf(&auxStructVarName[0][0], "m1", i);
  StructVarName[0] = &auxStructVarName[0][0];
  sprintf(&auxStructVarName[1][0], "m2", i);
  StructVarName[1] = &auxStructVarName[1][0];

  sprintf(&auxStructVarName[2][0], "q1", i);
  StructVarName[2] = &auxStructVarName[2][0];

  sprintf(&auxStructVarName[3][0], "q2", i);
  StructVarName[3] = &auxStructVarName[3][0];
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&auxStructVarName[4 + i][0], "lam%d", i);
      StructVarName[4 + i] = &auxStructVarName[4 + i][0];

      sprintf(&auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0], "SIG%d", i);
      StructVarName[4 + nMvdHits + 2 * nSttHits + i] = &auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0];
    } else {
      sprintf(&auxStructVarName[4 + i][0], "lamp%d", i);
      StructVarName[4 + i] = &auxStructVarName[4 + i][0];

      sprintf(&auxStructVarName[4 + NpointsInFit + ii][0], "lamm%d", i);
      StructVarName[4 + NpointsInFit + ii] = &auxStructVarName[4 + NpointsInFit + ii][0];

      sprintf(&auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0], "SIGp%d", i);
      StructVarName[4 + nMvdHits + 2 * nSttHits + i] = &auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0];

      sprintf(&auxStructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii][0], "SIGm%d", i);
      StructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii] = &auxStructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii][0];
      ii++;
    }
  }

  sprintf(&auxStructVarName[NStructVar - 1][0], "DUM", i);
  StructVarName[NStructVar - 1] = &auxStructVarName[NStructVar - 1][0];

  //--- give the names of those Rows in which the Structural Variables are present

  //  for m1, m2, q1, q2
  for (i = 0; i < 4; i++) {
    for (j = 0, ii = 0; j < NpointsInFit; j++) {
      if (mvdhit[j]) {
        sprintf(&aux[i * NStructRowsMax + ii][0], "A%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii] = &aux[i * NStructRowsMax + ii][0];
        sprintf(&aux[i * NStructRowsMax + ii + 1][0], "B%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 1] = &aux[i * NStructRowsMax + ii + 1][0];
        ii += 2;
      } else {
        sprintf(&aux[i * NStructRowsMax + ii][0], "Ap%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii] = &aux[i * NStructRowsMax + ii][0];
        sprintf(&aux[i * NStructRowsMax + ii + 1][0], "Am%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 1] = &aux[i * NStructRowsMax + ii + 1][0];
        sprintf(&aux[i * NStructRowsMax + ii + 2][0], "Bp%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 2] = &aux[i * NStructRowsMax + ii + 2][0];
        sprintf(&aux[i * NStructRowsMax + ii + 3][0], "Bm%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 3] = &aux[i * NStructRowsMax + ii + 3][0];
        ii += 4;
      }
    }
  }

  //  now for the    lamp*   variables
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&aux[(i + 4) * NStructRowsMax + 0][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 0] = &aux[(i + 4) * NStructRowsMax + 0][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 1][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 1] = &aux[(i + 4) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 2][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 2] = &aux[(i + 4) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 3][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 3] = &aux[(i + 4) * NStructRowsMax + 3][0];
    } else {
      sprintf(&aux[(i + 4) * NStructRowsMax + 0][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 0] = &aux[(i + 4) * NStructRowsMax + 0][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 1][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 1] = &aux[(i + 4) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 2][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 2] = &aux[(i + 4) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 3][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 3] = &aux[(i + 4) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 4][0], "LM%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 4] = &aux[(i + 4) * NStructRowsMax + 4][0];
    }
  }

  //  now for the    lamm*   variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 1][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 1] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 1][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 2][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 2] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 2][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 3][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 3] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 3][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 4][0], "LM%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 4] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 4][0];
    ii++;
  }

  //  now for the    SIGp*   variables
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT");
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax] = OBJECTname;
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    } else {
      //         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT");
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax] = OBJECTname;
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    }
  }

  //  now for the    SIGm*   variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT");
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax] = OBJECTname;

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    ii++;
  }

  //  now for the    DUM   variable
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii] = &aux[(NStructVar - 1) * NStructRowsMax + ii][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 1] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 2] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 3] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0];
      ii += 4;
    } else {
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii] = &aux[(NStructVar - 1) * NStructRowsMax + ii][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0], "Am%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 1] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 2] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0], "Bm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 3] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0];
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 4][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 4] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 4][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 5][0], "Cm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 5] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 5][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 6][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 6] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 6][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 7][0], "Dm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 7] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 7][0];
      ii += 8;
    }
  }

  //-----------------  write the RHS  section

  //      fprintf(FMCS,"RHS\n");//---stampaggi
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  A%d  %g  B%d  %g\n  BOUND  C%d  %g  D%d  %g\n",
                    i, Oyy[i]+2.*M,i,
                      -Oyy[i]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);
      */
      //---fine stampaggi
      ValueB[ii] = Oyy[i] + 2. * M;
      ValueB[ii + 1] = -Oyy[i] + 2. * M;
      ValueB[ii + 2] = Delta[i] + 2. * M;
      ValueB[ii + 3] = M - Delta[i] + 2. * M;
      ii += 4;
    } else {
      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  Ap%d  %g  Bp%d  %g\n  BOUND  Cp%d  %g  Dp%d  %g\n",
                    i, Oyy[i]+DriftRadiusconformal[ i ]+2.*M,i,
                      -Oyy[i]-DriftRadiusconformal[ i ]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);
      */
      //---fine stampaggi
      ValueB[ii] = Oyy[i] + DriftRadiusconformal[i] + 2. * M;
      ValueB[ii + 1] = -Oyy[i] - DriftRadiusconformal[i] + 2. * M;
      ValueB[ii + 2] = Delta[i] + 2. * M;
      ValueB[ii + 3] = M - Delta[i] + 2. * M;

      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  Am%d  %g  Bm%d  %g\n  BOUND  Cm%d  %g  Dm%d %g\n",
                    i, Oyy[i]-DriftRadiusconformal[ i ]+2.*M,i,
                      -Oyy[i]+DriftRadiusconformal[ i ]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);
                fprintf(FMCS,"  BOUND  LM%d   1.\n",i);
      */
      //---fine stampaggi
      ValueB[ii + 4] = Oyy[i] - DriftRadiusconformal[i] + 2. * M;
      ValueB[ii + 5] = -Oyy[i] + DriftRadiusconformal[i] + 2. * M;
      ValueB[ii + 6] = Delta[i] + 2. * M;
      ValueB[ii + 7] = M - Delta[i] + 2. * M;
      ValueB[ii + 8] = 1.;
      ii += 9;
    }
  }

  //-----------------  write the RANGES  section

  //      fprintf(FMCS,"RANGES\n");//---stampaggi
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //          fprintf(FMCS,"  RANGE  LM%d  1.\n",i);//---stampaggi
    //---
    ValueRanges[ii] = 1.;
    sprintf(&auxNameRanges[ii][0], "LM%d", i);
    NameRanges[ii] = &auxNameRanges[ii][0];
    ii++;
  }

  //-----------------  write the BOUNDS  section
  //      fprintf(FMCS,"BOUNDS\n");//---stampaggi

  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS," BV  Bounds  lam%d\n",  i);//---stampaggi
      sprintf(&auxTypeofBound[i][0], "BV");
      TypeofBound[i] = &auxTypeofBound[i][0];
      sprintf(&auxBoundStructVarName[i][0], "lam%d", i);
    } else {
      //          fprintf(FMCS," BV  Bounds  lamp%d\n",  i);//---stampaggi
      sprintf(&auxTypeofBound[i][0], "BV");
      TypeofBound[i] = &auxTypeofBound[i][0];
      sprintf(&auxBoundStructVarName[i][0], "lamp%d", i);
    }

    BoundStructVarName[i] = &auxBoundStructVarName[i][0];
    BoundValue[i] = 0.;
  }

  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //          fprintf(FMCS," BV  Bounds  lamm%d\n", i);//---stampaggi
    sprintf(&auxTypeofBound[ii + NpointsInFit][0], "BV");
    TypeofBound[ii + NpointsInFit] = &auxTypeofBound[ii + NpointsInFit][0];
    sprintf(&auxBoundStructVarName[ii + NpointsInFit][0], "lamm%d", i);
    BoundStructVarName[ii + NpointsInFit] = &auxBoundStructVarName[ii + NpointsInFit][0];
    BoundValue[ii + NpointsInFit] = 0.;
    ii++;
  }

  //          fprintf(FMCS," FX  Bounds  DUM  %g\n",2.*M);//--stampaggi
  sprintf(&auxTypeofBound[NpointsInFit + nSttHits][0], "FX");
  TypeofBound[NpointsInFit + nSttHits] = &auxTypeofBound[NpointsInFit + nSttHits][0];

  sprintf(&auxTypeofBound[NpointsInFit + nSttHits][0], "FX");
  TypeofBound[NpointsInFit + nSttHits] = &auxTypeofBound[NpointsInFit + nSttHits][0];

  sprintf(&auxBoundStructVarName[NpointsInFit + nSttHits][0], "DUM");
  BoundStructVarName[NpointsInFit + nSttHits] = &auxBoundStructVarName[NpointsInFit + nSttHits][0];
  BoundValue[NpointsInFit + nSttHits] = 2. * M;
  //-----

  //------------------------------------------------------------stampaggi
  /*
        fprintf(FMCS,"ENDATA\n");
        fclose(FMCS);
  */

  if (istampa >= 4) {

    cout << "n.  punti nel fit " << NpointsInFit << endl;

    cout << "nRows " << nRows << endl;
    for (int ic = 0; ic < nRows; ic++) {
      cout << "n.  Row  " << ic << ", nameRows " << nameRows[ic] << ",  typeRows " << typeRows[ic] << endl;
    }

    cout << "NStructRowsMax, NStructVar " << NStructRowsMax << ", " << NStructVar << endl;
    for (int ic = 0; ic < NStructVar; ic++) {
      cout << "NRowsInWhichStructVarArePresent  " << NRowsInWhichStructVarArePresent[ic] << ", nome var. strut. n." << ic << "  = " << StructVarName[ic] << endl;

      for (int jc = 0; jc < NRowsInWhichStructVarArePresent[ic]; jc++) {
        cout << "n. " << jc << "  NameRowsInWhichStructVarArePresent  " << NameRowsInWhichStructVarArePresent[ic * NStructRowsMax + jc] << endl;
      }
    }

    cout << "n Coefficient " << 21 * nMvdHits + 24 * nSttHits << endl;
    iii = 0;
    for (int ic = 0; ic < NStructVar; ic++) {
      cout << "Struct. Var." << StructVarName[ic] << " e' presente in " << NRowsInWhichStructVarArePresent[ic] << "  Rows;" << endl;
      for (ii = 0; ii < NRowsInWhichStructVarArePresent[ic]; ii++) {

        cout << "\tin Row " << NameRowsInWhichStructVarArePresent[ic * NStructRowsMax + ii] << ", ha  Coefficient   " << Coefficients[ic * NStructRowsMax + ii]
             << " (n. sequenziale = " << iii << ")" << endl;
        iii++;
      }
    }

    cout << "n valuesB " << nRows - 1 << endl;
    for (int ic = 0; ic < nRows - 1; ic++) {
      cout << "n. " << ic << ",  valuesB   " << ValueB[ic] << endl;
    }
    cout << "n ranges " << nRanges << endl;
    for (int ic = 0; ic < nRanges; ic++) {
      cout << "n. " << ic << ",  RANGES   " << ValueRanges[ic] << endl;
    }
    cout << "n Bounds " << nBounds << endl;
    for (int ic = 0; ic < nBounds; ic++) {
      cout << "n. " << ic << ",  Bounds   " << BoundValue[ic] << endl;
      cout << "n. " << ic << ",  Bound Type   " << TypeofBound[ic] << endl;
      cout << "n. " << ic << ",  Bound Name   " << BoundStructVarName[ic] << endl;
    }
  } // end of if(istampa

  //-------fine stampaggi

  //-----------------------  funzioni chiamate direttamente
  /*
  cout<<"cavolo, da sttmvdtracking : nRows = "<<nRows<<", NStructVar = "<<
    NStructVar<<", NStructRowsMax = "<<NStructRowsMax<<
    ", NRowsInWhichStructVarArePresent = "<<
    NRowsInWhichStructVarArePresent<<", nRanges = "<<nRanges
    <<", nBounds = "<<nBounds<<endl;
  */
  int status = glp_main(nRows, nameRows,
                        typeRows, //  ROWS info
                        NStructVar, NStructRowsMax,
                        NRowsInWhichStructVarArePresent, //  COLUMNS info
                        StructVarName,
                        NameRowsInWhichStructVarArePresent, //  COLUMNS info
                        Coefficients,                       //  COLUMNS info
                        ValueB,                             // RHS  info
                        nRanges, ValueRanges,
                        NameRanges, //  RANGES  info
                        nBounds, BoundValue, BoundStructVarName,
                        TypeofBound //  BOUNDS info
                                    //      ,final_values,TIMEOUT
                        ,
                        final_values);

  if (status != 0) {
    *Type = false;
    return -5;
  }

  //--------stampaggi
  if (istampa >= 3) {
    printf("from FitHelixCylinder  printout dopo glp_main -------------------------------\n");
    printf("      number of structural variables %d\n", NStructVar);
    int ica;
    for (ica = 0; ica < NStructVar; ica++) {
      printf("name of structural variable %s and its final value %g\n", StructVarName[ica], final_values[ica]);
    }
    printf("from FitHelixCylinder  printout dopo glp_main  -------------------------------\n");
  } // end of if(istampa
    //--------fine stampaggi

  //-----------------------  fine funzioni chiamate direttamente

  /*
     if(istampa>=3 && IVOLTE <= 20){
       sprintf(stringa,
  "/home/boca/panda/glpk/glpk-4.39/examples/glpsol --min -o soluztrack%dEvent%dstep%d    GeneralParallelHitsConformeTraccia%dEvent%d.mcs",
                                  0,IVOLTE,1,0,IVOLTE);
     }  else {
       sprintf(stringa,
  "/home/boca/panda/glpk/glpk-4.39/examples/glpsol --min -o soluztrack%dEvent%dstep%d    GeneralParallelHitsConformeTraccia%dEvent%d.mcs >& /dev/null",
                                  0, IVOLTE,1,0,IVOLTE,1);
     }
  */

  m1_result = final_values[0];
  m2_result = final_values[1];
  q1_result = final_values[2];
  q2_result = final_values[3];

  if (istampa > 2)
    cout << "Results : m1 = " << m1_result << ", m2= " << m2_result << ", q1 = " << q1_result << ", q2 = " << q2_result << endl;

  //---------  case in which the fit failed
  if (final_values[0] == 0. && final_values[1] == 0. && final_values[2] == 0. && final_values[3] == 0.) {
    *Type = false;
    return -10;
  }

  //------------------------  transformation of the result in terms of ALFA, BETA, GAMMA

  *qu = q1_result - q2_result;
  *emme = m1_result - m2_result;

  *pGamma = 0.;
  if (fabs(*qu) > 1.e-10) { //  trajectory is a circle in XY space
    *pAlfa = *emme / (*qu);
    *pBeta = -1. / (*qu);
    *Type = true;
    //  now take into account the rotation and correct; the only affected quantities are ALFA and BETA
    alfetta = *pAlfa;
    *pAlfa = *pAlfa * cose - *pBeta * sine;
    *pBeta = alfetta * sine + *pBeta * cose;

  } else if (fabs(*emme) > 1.e-10) { //  trajectory is a straight line in XY space of equation y= m*x
    //  the rotation first
    angle = atan(*emme) + rotationangle;
    if (fabs(cos(angle)) > 1.e-10) {
      *pAlfa = 999999.;
      *pBeta = -(*pAlfa) / tan(angle);
      *Type = false;

    } else { //  in this case the equation in XY plane is y = 0.
      *pAlfa = 0.;
      *pBeta = 999999.;
      *Type = false;
    }
  } else { //  in this case also the equation in XY plane is  y = 0.
    *pAlfa = 0.;
    *pBeta = 999999.;
    *Type = false;
  } // end of 	if( fabs( *qu ) > 1.e-10)

  //------------------

  // now take into account the displacement and correct
  *pGamma += (trajectory_vertex[0] * trajectory_vertex[0] + trajectory_vertex[1] * trajectory_vertex[1] - *pAlfa * trajectory_vertex[0] - *pBeta * trajectory_vertex[1]);
  *pAlfa -= 2. * trajectory_vertex[0];
  *pBeta -= 2. * trajectory_vertex[1];

  if (fabs(cose - *emme * sine) > 1.e-10) {
    *qu = *qu / (cose - *emme * sine);
    *emme = (*emme * cose + sine) / (cose - *emme * sine);
    return 1;
  } else { //  in this case the equation is   0 = x+*qu .
    if (fabs(sine + *emme * cose) < 1.e-10) {
      cout << " From FitHelixCylinder, equation of XY circle : X**2 + Y**2 =0,"
           << " situation impossible in principle! Returning -1" << endl;
      return -1;
    }

    *emme = 1.;
    *qu = *qu / (sine + *emme * cose);
    return 99; //  in this case the equation is   0 = x+*qu .
  }
}

//----------end of function PndTrkGlpkFits::FitHelixCylinder

//----------begin of function PndTrkGlpkFits::FitSZspace

Short_t PndTrkGlpkFits::FitSZspace(Short_t nSkewHitsinTrack, Double_t *S,
                                   Double_t *Z, //
                                   Double_t *DriftRadius,
                                   Double_t *ErrorDriftRadius, //
                                   Double_t FInot, Short_t NMAX, Double_t *emme, int IVOLTE)
{

  //   definition of variables for the glpsol  solver
  //    ROWS (for read_rows  function)
  //
  Short_t NpointsInFit = nSkewHitsinTrack - NMAX < 0 ? nSkewHitsinTrack : NMAX;

  bool mvdhit[NpointsInFit];

  Double_t ave, avex, avey, cose, sine, M = 50., m_result, q_result, A, alfetta, angle, offsety, rotationangle, Oxx[NpointsInFit], Oyy[NpointsInFit], Delta[NpointsInFit];

  Short_t i, j, ii, iii, n, nSttHits, nMvdHits;
  Short_t Status;

  float m1_result, m2_result, q1_result, q2_result, A1_result, A2_result;

  // --

  if (nSkewHitsinTrack == 0) {
    cout << "from FitSZspace, Evento " << IVOLTE << endl;
    cout << "from PndTrkGlpkFits::FitSZspace  :  no points in fit, return!\n";
    return -10;
  }

  /*
    ave=0.;
    avex=0.;
    avey=0.;
    n=0;
    for(i=0;i<nSkewHitsinTrack;i++){
      if( fabs(Z[ i ]) > 1.e-10&& DriftRadius[ i ]>0.){
        n++;
        ave += (S[ i ] - FInot)/Z[ i ];
        avex += Z[ i ];
        avey += (S[ i ] - FInot);
      }
    }

    if( n>0) {
      ave /=n;
      avex /=n;
      avey /=n;
      rotationangle = atan2(avey,avex);
    } else {
      rotationangle=PI/2.;
    }

  */

  rotationangle = PI / 2.;

  //      cose = cos(rotationangle);
  //     sine = sin(rotationangle);
  cose = 0.;
  sine = 1.;

  // Delta[i] is actually used in the fit, and also Drift Radius.

  nSttHits = nMvdHits = 0;
  for (i = 0; i < NpointsInFit; i++) {
    Oxx[i] = Z[i] * cose + (S[i] - FInot) * sine;
    Oyy[i] = -Z[i] * sine + (S[i] - FInot) * cose;
    Delta[i] = ErrorDriftRadius[i];
    //          Delta[i] = 2.*DriftRadius[i];

    if (DriftRadius[i] < 0.) {
      mvdhit[i] = true;
      nMvdHits++;
    } else {
      mvdhit[i] = false;
      nSttHits++;
    }
  }

  //--------- calculation of # structural variables (see Gianluigi's logbook pag. 236) etc.etc.

  int NStructVar = 4 + 1 + nMvdHits * 2 + nSttHits * 4;
  //                  m1,m2,q1,q2     DUM      lam & SIG            lamp & lamm & SIGp & SIGm

  int nRows = 1 + nMvdHits * 4 + nSttHits * 9;
  //	         OBJECT     A,B,C,D        Ap,Bp,Cp,Dp,Am,Bm,Cm,Dm,LM

  //----  creating the various service arrays

  char OBJECTname[8];
  sprintf(OBJECTname, "OBJECT");

  int typeRows[nRows];
  char *nameRows[nRows];
  char auxnameRows[nRows][5];

  int NStructRowsMax = 8 * NpointsInFit; //  maximum number of ROWS in which a
                                         //  structural variable (for instance M ) can be found
  double final_values[NStructVar];
  int NRowsInWhichStructVarArePresent[NStructVar];
  char *StructVarName[NStructVar];
  char auxStructVarName[NStructVar][8];
  char *NameRowsInWhichStructVarArePresent[NStructVar * NStructRowsMax];
  char aux[NStructVar * NStructRowsMax][5];
  double Coefficients[NStructVar * NStructRowsMax];

  //--------for RHS information
  double ValueB[nRows - 1]; // -1 because OBJECT dowsn't have a RHS boundary.
  //--------for RANGES information
  int nRanges = nSttHits;
  double ValueRanges[nRanges];
  char *NameRanges[nRanges];
  char auxNameRanges[nRanges][20];

  //--------for BOUNDS information
  int nBounds = NpointsInFit + nSttHits + 1 + 2; // +2  because q1 =  q2 = 0 fixed.
  double BoundValue[nBounds];
  char *BoundStructVarName[nBounds];
  char auxBoundStructVarName[nBounds][8];
  char *TypeofBound[nBounds];
  char auxTypeofBound[nBounds][20];
  //--------end BOUNDS information

  //---------------------------------------------------

  //--- calculate array      NRowsInWhichStructVarArePresent
  NRowsInWhichStructVarArePresent[0] =                                //  this is for m1
    NRowsInWhichStructVarArePresent[1] =                              //  this is for m2
    NRowsInWhichStructVarArePresent[2] =                              //  this is for q1
    NRowsInWhichStructVarArePresent[3] = nMvdHits * 2 + nSttHits * 4; //  this is for q2
                                                                      //--- the following is for the  lam* (Mvd Hits) and lamp* (Stt hits) structural variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      NRowsInWhichStructVarArePresent[4 + ii] = 4;
    } else {
      NRowsInWhichStructVarArePresent[4 + ii] = 5;
    }
    ii++;
  }

  //  the lamm* (Stt hits) if any.
  for (i = 0; i < NpointsInFit; i++) {
    if (!mvdhit[i]) {
      NRowsInWhichStructVarArePresent[4 + ii] = 5;
      ii++;
    }
  }

  //--- the following is for the  SIG   structural variables
  for (i = 0; i < nMvdHits + 2 * nSttHits; i++) {
    NRowsInWhichStructVarArePresent[4 + ii + i] = 5;
  }
  //--- the following is for the    DUM    structural variable
  NRowsInWhichStructVarArePresent[4 + ii + nMvdHits + 2 * nSttHits] = nMvdHits * 4 + nSttHits * 8;

  //-----------------  write the ROWS  section

  //-------stampaggi
  /*
        sprintf(nome,"GeneralSkewEvent%d.mcs", IVOLTE);
        FILE * FMCS = fopen(nome,"w");
        fprintf(FMCS,"NAME    FIT\n");


        fprintf(FMCS,"ROWS\n");
        fprintf(FMCS," N OBJECT\n");
        for(i=0 ; i< NpointsInFit ; i++) {
    if( mvdhit[i]){
             fprintf(FMCS," L A%d\n L B%d\n L C%d\n L D%d\n",i,i,i,i);
    }else{
             fprintf(FMCS," L Ap%d\n L Bp%d\n L Cp%d\n L Dp%d\n",i,i,i,i);
             fprintf(FMCS," L Am%d\n L Bm%d\n L Cm%d\n L Dm%d\n G LM%d\n",i,i,i,i,i);
    }
        }
  */
  //--------------------------fine stampaggi

  //      sprintf(&(auxnameRows[0][0]),"OBJECT");
  //      nameRows[0]=&auxnameRows[0][0];
  nameRows[0] = OBJECTname;
  typeRows[0] = GLP_FR;
  for (i = 0, ii = 0; i < NpointsInFit; i++) {

    if (mvdhit[i]) {
      typeRows[1 + ii] = GLP_UP;
      typeRows[2 + ii] = GLP_UP;
      typeRows[3 + ii] = GLP_UP;
      typeRows[4 + ii] = GLP_UP;
      typeRows[5 + ii] = GLP_LO;

      sprintf(&(auxnameRows[1 + ii][0]), "A%d", i);
      nameRows[1 + ii] = &auxnameRows[1 + ii][0];
      sprintf(&(auxnameRows[2 + ii][0]), "B%d", i);
      nameRows[2 + ii] = &auxnameRows[2 + ii][0];
      sprintf(&(auxnameRows[3 + ii][0]), "C%d", i);
      nameRows[3 + ii] = &auxnameRows[3 + ii][0];
      sprintf(&(auxnameRows[4 + ii][0]), "D%d", i);
      nameRows[4 + ii] = &auxnameRows[4 + ii][0];
      ii += 4;

    } else {
      typeRows[1 + ii] = GLP_UP;
      typeRows[2 + ii] = GLP_UP;
      typeRows[3 + ii] = GLP_UP;
      typeRows[4 + ii] = GLP_UP;
      typeRows[5 + ii] = GLP_UP;
      typeRows[6 + ii] = GLP_UP;
      typeRows[7 + ii] = GLP_UP;
      typeRows[8 + ii] = GLP_UP;
      typeRows[9 + ii] = GLP_LO;

      sprintf(&(auxnameRows[1 + ii][0]), "Ap%d", i);
      nameRows[1 + ii] = &auxnameRows[1 + ii][0];
      sprintf(&(auxnameRows[2 + ii][0]), "Bp%d", i);
      nameRows[2 + ii] = &auxnameRows[2 + ii][0];
      sprintf(&(auxnameRows[3 + ii][0]), "Cp%d", i);
      nameRows[3 + ii] = &auxnameRows[3 + ii][0];
      sprintf(&(auxnameRows[4 + ii][0]), "Dp%d", i);
      nameRows[4 + ii] = &auxnameRows[4 + ii][0];
      sprintf(&(auxnameRows[5 + ii][0]), "Am%d", i);
      nameRows[5 + ii] = &auxnameRows[5 + ii][0];
      sprintf(&(auxnameRows[6 + ii][0]), "Bm%d", i);
      nameRows[6 + ii] = &auxnameRows[6 + ii][0];
      sprintf(&(auxnameRows[7 + ii][0]), "Cm%d", i);
      nameRows[7 + ii] = &auxnameRows[7 + ii][0];
      sprintf(&(auxnameRows[8 + ii][0]), "Dm%d", i);
      nameRows[8 + ii] = &auxnameRows[8 + ii][0];
      sprintf(&(auxnameRows[9 + ii][0]), "LM%d", i);
      nameRows[9 + ii] = &auxnameRows[9 + ii][0];
      ii += 9;
    }
  }

  //-----------------  write the COLUMNS  section

  //      fprintf(FMCS,"COLUMNS\n");  //--------stampaggi

  //  Column variable  m1

  ii = 0;
  for (i = 0; i < NpointsInFit; i++) {

    if (mvdhit[i]) {
      //---stampaggi
      //          fprintf(FMCS,"  m1 A%d   %g\n  m1 B%d  %g\n",i,Oxx[i],i,-Oxx[i]);
      //-----stampaggi, fine

      Coefficients[ii] = Oxx[i];
      Coefficients[ii + 1] = -Oxx[i];
      ii += 2;
    } else {
      //---stampaggi
      //          fprintf(FMCS,"  m1 Ap%d  %g  Am%d  %g\n  m1 Bp%d  %g   Bm%d  %g\n",
      //                                  i,Oxx[i],i,Oxx[i],i,-Oxx[i],i,-Oxx[i]);
      //-----stampaggi, fine

      Coefficients[ii] = Oxx[i];
      Coefficients[ii + 1] = Oxx[i];
      Coefficients[ii + 2] = -Oxx[i];
      Coefficients[ii + 3] = -Oxx[i];
      ii += 4;
    }
  }

  //  Column variable  m2
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  m2 A%d  %g\n  m2 B%d  %g\n",//---stampaggi
      //                                  i,-Oxx[i],i,Oxx[i]);//---stampaggi
      Coefficients[NStructRowsMax + ii] = -Oxx[i];
      Coefficients[NStructRowsMax + ii + 1] = Oxx[i];
      ii += 2;
    } else {
      //          fprintf(FMCS,"  m2 Ap%d  %g  Am%d  %g\n  m2 Bp%d  %g   Bm%d  %g\n",//---stampaggi
      //                                  i,-Oxx[i],i,-Oxx[i],i,Oxx[i],i,Oxx[i]);//---stampaggi
      Coefficients[NStructRowsMax + ii] = -Oxx[i];
      Coefficients[NStructRowsMax + ii + 1] = -Oxx[i];
      Coefficients[NStructRowsMax + ii + 2] = Oxx[i];
      Coefficients[NStructRowsMax + ii + 3] = Oxx[i];
      ii += 4;
    }
  }

  //  Column variable  q1
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  q1 A%d   1.\n  q1 B%d  -1.\n",//---stampaggi
      //                                  i,i);//---stampaggi
      Coefficients[2 * NStructRowsMax + ii] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 1] = -1.;
      ii += 2;
    } else {
      //          fprintf(FMCS,"  q1 Ap%d   1.  Am%d   1.\n  q1 Bp%d  -1.  Bm%d  -1.\n",//---stampaggi
      //                                  i,i,i,i);//---stampaggi
      Coefficients[2 * NStructRowsMax + ii] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 1] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 2] = -1.;
      Coefficients[2 * NStructRowsMax + ii + 3] = -1.;
      ii += 4;
    }
  }

  //  Column variable  q2
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  q2 A%d   -1.\n  q2 B%d   1.\n",//---stampaggi
      //                                  i,i);//---stampaggi
      Coefficients[3 * NStructRowsMax + ii] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 1] = 1.;
      ii += 2;
    } else {
      //          fprintf(FMCS,"  q2 Ap%d   -1.  Am%d   -1.\n  q2 Bp%d   1.   Bm%d   1.\n",//---stampaggi
      //                                  i,i,i,i);//---stampaggi
      Coefficients[3 * NStructRowsMax + ii] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 1] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 2] = 1.;
      Coefficients[3 * NStructRowsMax + ii + 3] = 1.;
      ii += 4;
    }
  }

  //  Column variable  lambdap(i)
  for (i = 0; i < NpointsInFit; i++) {
    ii = (4 + i) * NStructRowsMax;
    Coefficients[ii] = -M;
    Coefficients[ii + 1] = -M;
    Coefficients[ii + 2] = -M;
    Coefficients[ii + 3] = M;
    /*
      if( mvdhit[i]){
      fprintf(FMCS,"  lam%d  A%d  %g  B%d  %g\n  lam%d  C%d  %g  D%d   %g\n",//---stampaggi
                          i,i,-M,i,-M, i , i,-M, i, M);//---stampaggi
      } else {
      fprintf(FMCS,"  lamp%d  Ap%d  %g  Bp%d  %g\n  lamp%d  Cp%d  %g  Dp%d   %g\n  lamp%d  LM%d  1.\n",//---stampaggi
                          i,i,-M,i,-M, i , i,-M, i, M, i,i);//---stampaggi
      }
    */
    if (!mvdhit[i])
      Coefficients[ii + 4] = 1.;
  }

  //  Column variable  lambdam(i)
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    ii += NStructRowsMax;
    //         fprintf(FMCS,"  lamm%d  Am%d  %g  Bm%d  %g\n  lamm%d  Cm%d  %g  Dm%d %g\n  lamm%d  LM%d  1.\n",//---stampaggi
    //                     i,i,-M,i,-M, i,i , -M, i, M, i,i);//---stampaggi
    Coefficients[ii] = -M;
    Coefficients[ii + 1] = -M;
    Coefficients[ii + 2] = -M;
    Coefficients[ii + 3] = M;
    Coefficients[ii + 4] = 1.;
  }
  //  Column variable  SIGp(i)
  for (i = 0; i < NpointsInFit; i++) {
    ii += NStructRowsMax;

    /*
      if( mvdhit[i]){
          fprintf(FMCS,"  SIG%d  OBJECT  %g  A%d  -1.\n  SIG%d  B%d    -1. C%d  1.\n  SIG%d  D%d -1.\n",//---stampaggi
                                                    i,1./Delta[i],i,i,i,i,i,i);//---stampaggi

      } else {
          fprintf(FMCS,"  SIGp%d  OBJECT  %g  Ap%d  -1.\n  SIGp%d  Bp%d    -1. Cp%d  1.\n  SIGp%d  Dp%d -1.\n",//---stampaggi
                                                    i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
      }
    */

    Coefficients[ii] = 1. / Delta[i];
    Coefficients[ii + 1] = -1.;
    Coefficients[ii + 2] = -1.;
    Coefficients[ii + 3] = 1.;
    Coefficients[ii + 4] = -1.;
  }
  //  Column variable  SIGm(i)
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    ii += NStructRowsMax;
    /*
      fprintf(FMCS,"  SIGm%d  OBJECT %g  Am%d  -1.\n  SIGm%d  Bm%d   -1. Cm%d   1.\n  SIGm%d  Dm%d  -1.\n",//---stampaggi
                                                    i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
    */
    Coefficients[ii] = 1. / Delta[i];
    Coefficients[ii + 1] = -1.;
    Coefficients[ii + 2] = -1.;
    Coefficients[ii + 3] = 1.;
    Coefficients[ii + 4] = -1.;
  }

  //  Column variable  DUM
  //-----------stampaggi
  /*
        for(i=0 ; i< NpointsInFit ; i++) {
    if( mvdhit[i]){
           fprintf(FMCS,"  DUM     A%d  1.\n  DUM   B%d   1.\n  DUM    C%d   1.\n",i,i,i);
           fprintf(FMCS,"  DUM     D%d  1.\n",i);
    } else {
           fprintf(FMCS,"  DUM     Ap%d  1.      Am%d       1.\n  DUM   Bp%d   1.    Bm%d   1.\n  DUM    Cp%d   1.   Cm%d   1\n",
                                                  i,i,i,i,i,i);
           fprintf(FMCS,"  DUM     Dp%d  1.      Dm%d       1.\n",i,i);
    }
        }
  */
  //---fine stampaggi
  ii += NStructRowsMax;
  for (i = 0, iii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      Coefficients[ii + iii] = 1.;
      Coefficients[ii + iii + 1] = 1.;
      Coefficients[ii + iii + 2] = 1.;
      Coefficients[ii + iii + 3] = 1.;
      iii += 4;
    } else {
      Coefficients[ii + iii] = 1.;
      Coefficients[ii + iii + 1] = 1.;
      Coefficients[ii + iii + 2] = 1.;
      Coefficients[ii + iii + 3] = 1.;
      Coefficients[ii + iii + 4] = 1.;
      Coefficients[ii + iii + 5] = 1.;
      Coefficients[ii + iii + 6] = 1.;
      Coefficients[ii + iii + 7] = 1.;
      iii += 8;
    }
  }

  //-----------

  //--- give the names to the Structural Variables

  sprintf(&auxStructVarName[0][0], "m1", i);
  StructVarName[0] = &auxStructVarName[0][0];
  sprintf(&auxStructVarName[1][0], "m2", i);
  StructVarName[1] = &auxStructVarName[1][0];

  sprintf(&auxStructVarName[2][0], "q1", i);
  StructVarName[2] = &auxStructVarName[2][0];

  sprintf(&auxStructVarName[3][0], "q2", i);
  StructVarName[3] = &auxStructVarName[3][0];
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&auxStructVarName[4 + i][0], "lam%d", i);
      StructVarName[4 + i] = &auxStructVarName[4 + i][0];

      sprintf(&auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0], "SIG%d", i);
      StructVarName[4 + nMvdHits + 2 * nSttHits + i] = &auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0];
    } else {
      sprintf(&auxStructVarName[4 + i][0], "lamp%d", i);
      StructVarName[4 + i] = &auxStructVarName[4 + i][0];

      sprintf(&auxStructVarName[4 + NpointsInFit + ii][0], "lamm%d", i);
      StructVarName[4 + NpointsInFit + ii] = &auxStructVarName[4 + NpointsInFit + ii][0];

      sprintf(&auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0], "SIGp%d", i);
      StructVarName[4 + nMvdHits + 2 * nSttHits + i] = &auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0];

      sprintf(&auxStructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii][0], "SIGm%d", i);
      StructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii] = &auxStructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii][0];
      ii++;
    }
  }

  sprintf(&auxStructVarName[NStructVar - 1][0], "DUM", i);
  StructVarName[NStructVar - 1] = &auxStructVarName[NStructVar - 1][0];

  //--- give the names of those Rows in which the Structural Variables are present

  //  for m1, m2, q1, q2
  for (i = 0; i < 4; i++) {
    for (j = 0, ii = 0; j < NpointsInFit; j++) {
      if (mvdhit[j]) {
        sprintf(&aux[i * NStructRowsMax + ii][0], "A%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii] = &aux[i * NStructRowsMax + ii][0];
        sprintf(&aux[i * NStructRowsMax + ii + 1][0], "B%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 1] = &aux[i * NStructRowsMax + ii + 1][0];
        ii += 2;
      } else {
        sprintf(&aux[i * NStructRowsMax + ii][0], "Ap%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii] = &aux[i * NStructRowsMax + ii][0];
        sprintf(&aux[i * NStructRowsMax + ii + 1][0], "Am%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 1] = &aux[i * NStructRowsMax + ii + 1][0];
        sprintf(&aux[i * NStructRowsMax + ii + 2][0], "Bp%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 2] = &aux[i * NStructRowsMax + ii + 2][0];
        sprintf(&aux[i * NStructRowsMax + ii + 3][0], "Bm%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 3] = &aux[i * NStructRowsMax + ii + 3][0];
        ii += 4;
      }
    }
  }

  //  now for the    lamp*   variables
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&aux[(i + 4) * NStructRowsMax + 0][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 0] = &aux[(i + 4) * NStructRowsMax + 0][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 1][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 1] = &aux[(i + 4) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 2][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 2] = &aux[(i + 4) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 3][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 3] = &aux[(i + 4) * NStructRowsMax + 3][0];
    } else {
      sprintf(&aux[(i + 4) * NStructRowsMax + 0][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 0] = &aux[(i + 4) * NStructRowsMax + 0][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 1][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 1] = &aux[(i + 4) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 2][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 2] = &aux[(i + 4) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 3][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 3] = &aux[(i + 4) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 4][0], "LM%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 4] = &aux[(i + 4) * NStructRowsMax + 4][0];
    }
  }

  //  now for the    lamm*   variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 1][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 1] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 1][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 2][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 2] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 2][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 3][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 3] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 3][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 4][0], "LM%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 4] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 4][0];
    ii++;
  }

  //  now for the    SIGp*   variables
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT");
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax] = OBJECTname;
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    } else {
      //         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT");
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax] = OBJECTname;
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    }
  }

  //  now for the    SIGm*   variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT");
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax] = OBJECTname;

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    ii++;
  }

  //  now for the    DUM   variable
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii] = &aux[(NStructVar - 1) * NStructRowsMax + ii][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 1] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 2] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 3] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0];
      ii += 4;
    } else {
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii] = &aux[(NStructVar - 1) * NStructRowsMax + ii][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0], "Am%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 1] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 2] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0], "Bm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 3] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0];
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 4][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 4] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 4][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 5][0], "Cm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 5] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 5][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 6][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 6] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 6][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 7][0], "Dm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 7] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 7][0];
      ii += 8;
    }
  }

  //-----------------  write the RHS  section

  //      fprintf(FMCS,"RHS\n");//---stampaggi
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  A%d  %g  B%d  %g\n  BOUND  C%d  %g  D%d  %g\n",
                    i, Oyy[i]+2.*M,i,
                      -Oyy[i]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);
      */
      //---fine stampaggi
      ValueB[ii] = Oyy[i] + 2. * M;
      ValueB[ii + 1] = -Oyy[i] + 2. * M;
      ValueB[ii + 2] = Delta[i] + 2. * M;
      ValueB[ii + 3] = M - Delta[i] + 2. * M;
      ii += 4;
    } else {
      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  Ap%d  %g  Bp%d  %g\n  BOUND  Cp%d  %g  Dp%d  %g\n",
                    i, Oyy[i]+DriftRadius[ i ]+2.*M,i,
                      -Oyy[i]-DriftRadius[ i ]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);
      */
      //---fine stampaggi
      ValueB[ii] = Oyy[i] + DriftRadius[i] + 2. * M;
      ValueB[ii + 1] = -Oyy[i] - DriftRadius[i] + 2. * M;
      ValueB[ii + 2] = Delta[i] + 2. * M;
      ValueB[ii + 3] = M - Delta[i] + 2. * M;

      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  Am%d  %g  Bm%d  %g\n  BOUND  Cm%d  %g  Dm%d %g\n",
                    i, Oyy[i]-DriftRadius[ i ]+2.*M,i,
                      -Oyy[i]+DriftRadius[ i ]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);f
                fprintf(FMCS,"  BOUND  LM%d   1.\n",i);
      */
      //---fine stampaggi
      ValueB[ii + 4] = Oyy[i] - DriftRadius[i] + 2. * M;
      ValueB[ii + 5] = -Oyy[i] + DriftRadius[i] + 2. * M;
      ValueB[ii + 6] = Delta[i] + 2. * M;
      ValueB[ii + 7] = M - Delta[i] + 2. * M;
      ValueB[ii + 8] = 1.;
      ii += 9;
    }
  }

  //-----------------  write the RANGES  section

  //      fprintf(FMCS,"RANGES\n");//---stampaggi
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //          fprintf(FMCS,"  RANGE  LM%d  1.\n",i);//---stampaggi
    //---
    ValueRanges[ii] = 1.;
    sprintf(&auxNameRanges[ii][0], "LM%d", i);
    NameRanges[ii] = &auxNameRanges[ii][0];
    ii++;
  }

  //-----------------  write the BOUNDS  section
  //      fprintf(FMCS,"BOUNDS\n");//---stampaggi

  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS," BV  Bounds  lam%d\n",  i);//---stampaggi
      sprintf(&auxTypeofBound[i][0], "BV");
      TypeofBound[i] = &auxTypeofBound[i][0];
      sprintf(&auxBoundStructVarName[i][0], "lam%d", i);
    } else {
      //          fprintf(FMCS," BV  Bounds  lamp%d\n",  i);//---stampaggi
      sprintf(&auxTypeofBound[i][0], "BV");
      TypeofBound[i] = &auxTypeofBound[i][0];
      sprintf(&auxBoundStructVarName[i][0], "lamp%d", i);
    }

    BoundStructVarName[i] = &auxBoundStructVarName[i][0];
    BoundValue[i] = 0.;
  }

  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //          fprintf(FMCS," BV  Bounds  lamm%d\n", i);//---stampaggi
    sprintf(&auxTypeofBound[ii + NpointsInFit][0], "BV");
    TypeofBound[ii + NpointsInFit] = &auxTypeofBound[ii + NpointsInFit][0];
    sprintf(&auxBoundStructVarName[ii + NpointsInFit][0], "lamm%d", i);
    BoundStructVarName[ii + NpointsInFit] = &auxBoundStructVarName[ii + NpointsInFit][0];
    BoundValue[ii + NpointsInFit] = 0.;
    ii++;
  }

  //          fprintf(FMCS," FX  Bounds  DUM  %g\n",2.*M);//--stampaggi
  sprintf(&auxTypeofBound[NpointsInFit + nSttHits][0], "FX");
  TypeofBound[NpointsInFit + nSttHits] = &auxTypeofBound[NpointsInFit + nSttHits][0];

  sprintf(&auxTypeofBound[NpointsInFit + nSttHits][0], "FX");
  TypeofBound[NpointsInFit + nSttHits] = &auxTypeofBound[NpointsInFit + nSttHits][0];

  sprintf(&auxBoundStructVarName[NpointsInFit + nSttHits][0], "DUM");
  BoundStructVarName[NpointsInFit + nSttHits] = &auxBoundStructVarName[NpointsInFit + nSttHits][0];
  BoundValue[NpointsInFit + nSttHits] = 2. * M;

  //   fixing q1
  //          fprintf(FMCS," FX  Bounds  q1  %g\n",0.);//--stampaggi
  sprintf(&auxTypeofBound[NpointsInFit + nSttHits + 1][0], "FX");
  TypeofBound[NpointsInFit + nSttHits + 1] = &auxTypeofBound[NpointsInFit + nSttHits + 1][0];
  sprintf(&auxBoundStructVarName[NpointsInFit + nSttHits + 1][0], "q1");
  BoundStructVarName[NpointsInFit + nSttHits + 1] = &auxBoundStructVarName[NpointsInFit + nSttHits + 1][0];
  BoundValue[NpointsInFit + nSttHits + 1] = 0.;
  //   fixing q2
  //          fprintf(FMCS," FX  Bounds  q2  %g\n",0.);//--stampaggi
  sprintf(&auxTypeofBound[NpointsInFit + nSttHits + 2][0], "FX");
  TypeofBound[NpointsInFit + nSttHits + 2] = &auxTypeofBound[NpointsInFit + nSttHits + 2][0];
  sprintf(&auxBoundStructVarName[NpointsInFit + nSttHits + 2][0], "q2");
  BoundStructVarName[NpointsInFit + nSttHits + 2] = &auxBoundStructVarName[NpointsInFit + nSttHits + 2][0];
  BoundValue[NpointsInFit + nSttHits + 2] = 0.;

  //-----

  //------------------------------------------------------------stampaggi
  /*
        fprintf(FMCS,"ENDATA\n");
        fclose(FMCS);
  */

  /*
  cout<<"n.  punti nel fit "<<NpointsInFit<<endl;


  cout<<"nRows "<<nRows<<endl;
  for(int ic =0;ic<nRows; ic++){
     cout<<"n.  Row  "<<ic<<", nameRows "<<nameRows[ic]<<",  typeRows "<<typeRows[ic]<<endl;
  }

  cout<<"NStructRowsMax = "<<NStructRowsMax<<endl;
  cout<<"NStructVar "<<NStructVar<<" e loro elenco "<<endl;
  for(int ic =0;ic<NStructVar; ic++){
    cout<<"\tvar. n. "<<ic<<", nome = "<<StructVarName[ic]<<endl;
  }



  for(int ic =0;ic<NStructVar; ic++){
     cout<<"NRowsInWhichStructVarArePresent  "<<NRowsInWhichStructVarArePresent[ic]
       <<", nome var. strut. n."<<ic<<"  = "
       <<StructVarName[ic]<<endl;

    for(int jc=0; jc<NRowsInWhichStructVarArePresent[ic];jc++){
     cout<<"n. "<<jc<<"  NameRowsInWhichStructVarArePresent  "<<NameRowsInWhichStructVarArePresent[ic*NStructRowsMax+jc]<<endl;
    }
  }


  cout<<"n Coefficient "<<21*nMvdHits+24*nSttHits<<endl;
  iii=0;
  for(int ic =0;ic<NStructVar; ic++){
     cout<<"Struct. Var."<< StructVarName[ic] <<" e' presente in "<< NRowsInWhichStructVarArePresent[ic]
    <<"  Rows;"<<endl;
    for(ii=0;ii<NRowsInWhichStructVarArePresent[ic];ii++){

     cout<<"\tin Row "<<NameRowsInWhichStructVarArePresent[ic*NStructRowsMax+ii]
           <<", ha  Coefficient   "<<Coefficients[ic*NStructRowsMax+ii]<<
    " (n. sequenziale = "<<iii<<")"<<endl;
    iii++;
    }
  }

  cout<<"n valuesB "<<nRows-1<<endl;
  for(int ic =0;ic<nRows-1; ic++){
     cout<<"n. "<<ic<<",  valuesB   "<<ValueB[ic]<<endl;
  }
  cout<<"n ranges "<<nRanges<<endl;
  for(int ic =0;ic<nRanges; ic++){
     cout<<"n. "<<ic<<",  RANGES   "<<ValueRanges[ic]<<endl;
  }
  cout<<"n Bounds "<<nBounds<<endl;
  for(int ic =0;ic<nBounds; ic++){
     cout<<"n. "<<ic<<",  Bounds   "<<BoundValue[ic]<<endl;
     cout<<"n. "<<ic<<",  Bound Type   "<<TypeofBound[ic]<<endl;
     cout<<"n. "<<ic<<",  Bound Name   "<<BoundStructVarName[ic]<<endl;
  }
  */

  //-------fine stampaggi

  //-----------------------  funzioni chiamate direttamente
  /*
  cout<<"cavolo2, da sttmvdtracking : nRows = "<<nRows<<", NStructVar = "<<
    NStructVar<<", NStructRowsMax = "<<NStructRowsMax<<
    ", NRowsInWhichStructVarArePresent = "<<
    NRowsInWhichStructVarArePresent<<", nRanges = "<<nRanges
    <<", nBounds = "<<nBounds<<endl;
  */
  int status = glp_main(nRows, nameRows, typeRows,                                   //  ROWS info
                        NStructVar, NStructRowsMax, NRowsInWhichStructVarArePresent, //  COLUMNS info
                        StructVarName, NameRowsInWhichStructVarArePresent,           //  COLUMNS info
                        Coefficients,                                                //  COLUMNS info
                        ValueB,                                                      // RHS  info
                        nRanges, ValueRanges, NameRanges,                            //  RANGES  info
                        nBounds, BoundValue, BoundStructVarName,
                        TypeofBound //  BOUNDS info
                                    //      ,final_values, TIMEOUT
                        ,
                        final_values);

  if (status != 0)
    return -5; // fit failed

  //--------stampaggi
  /*
  printf("from main, final printout con routines chiamate direttamente -------------------------------\n");
  printf("      number of structural variables %d\n",NStructVar);
  int ica;
  for(ica=0;ica<NStructVar;ica++){
      printf("name of structural variable %s and its final value %g\n",
             StructVarName[ica], final_values[ica]);
  }
  printf("from main, end of final printout  con routines chiamate direttamente -------------------------------\n");
  */
  //--------fine stampaggi

  //-----------------------  fine funzioni chiamate direttamente

  m1_result = final_values[0];
  m2_result = final_values[1];
  //     q1_result=final_values[2];
  //     q2_result=final_values[3];

  *emme = m1_result - m2_result;
  // taking into account the rotation + traslation that was performed and calculate emme and qu

  if (fabs(cose - *emme * sine) > 1.e-10) {
    *emme = ((*emme) * cose + sine) / (cose - (*emme) * sine);
    return 1;
  } else { //  in this case the equation is   0 = U in the Conformal plane --> x=0 in the XY plane.
    return -99;
  }
}

//----------end of function PndTrkGlpkFits::FitSZspace

ClassImp(PndTrkGlpkFits)
