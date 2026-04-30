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

#include "PndTrkSttAdjacencies.h"

#include "PndSttTube.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include <stdlib.h>
#include <iostream>
#include <cmath>

using namespace std;

/** Default constructor **/
PndTrkSttAdjacencies::PndTrkSttAdjacencies(){};

/** Destructor **/
PndTrkSttAdjacencies::~PndTrkSttAdjacencies(){};

//-----------------------------------------------------------

void PndTrkSttAdjacencies::CalculateAdjacentStt(const Short_t NUMBER_STRAWS,   // number of Stt Straws in total;
                                                TClonesArray *SttTubeArray,    // input; array of the Stt tubes;
                                                Short_t *nParContiguous,       // output; number of contiguous straws (axial Stt);
                                                                               // NUMBER_STRAWS even if the numbering scheme for the Stt straws goes
                                                                               //  from 1 to NUMBER_STRAWS included;
                                                Short_t ListParContiguous[][6] // output list (axial Stt); first dimension is NUMBER_STRAWS;
)
{
  int i, j; // n, ncontigue, //[R.K. 01/2017] unused variable?

  //	FILE *HAND = fopen("ListaSttStraw.lis","w");

  double dis2, x[NUMBER_STRAWS],
    // Xwiredirection[NUMBER_STRAWS], //[R.K. 9/2018] unused
    y[NUMBER_STRAWS],
    // Ywiredirection[NUMBER_STRAWS], //[R.K. 9/2018] unused
    // z[NUMBER_STRAWS], //[R.K. 9/2018] unused
    Zwiredirection[NUMBER_STRAWS];

  PndSttTube *pSttTube;

  // the total n. of STT straws is NUMBER_STRAWS;
  for (i = 1; i <= NUMBER_STRAWS; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    x[i - 1] = pSttTube->GetPosition().X();
    y[i - 1] = pSttTube->GetPosition().Y();
    // z[i-1] = pSttTube->GetPosition().Z(); //[R.K. 9/2018] unused
    // Xwiredirection[i-1] = pSttTube->GetWireDirection().X(); //[R.K. 9/2018] unused
    // Ywiredirection[i-1] = pSttTube->GetWireDirection().Y(); //[R.K. 9/2018] unused
    Zwiredirection[i - 1] = pSttTube->GetWireDirection().Z();
  }

  for (i = 0; i < NUMBER_STRAWS; i++) {
    if (fabs(Zwiredirection[i] - 1.) > 1.e-5)
      continue; // for now I don't consider the skews;

    nParContiguous[i + 1] = 0;
    for (j = 0; j < NUMBER_STRAWS; j++) {
      if (j == i)
        continue;
      if (fabs(Zwiredirection[j] - 1.) > 1.e-5)
        continue; // for now I don't consider the skews;
      dis2 = (x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j]);
      if (dis2 > 1.1)
        continue;
      ListParContiguous[i][nParContiguous[i]] = j + 1;
      nParContiguous[i]++;
      if (nParContiguous[i] > 6) {
        cout << "Error ! Contiguous Axial Straws > 6!!! Exit.\n";
        exit(-1);
      }
    }

  } // end of   for (i=0;i< NUMBER_STRAWS; i++)

  //	fclose (HAND);
}
//------------------ end function  PndTrkSttAdjacencies::CalculateAdjacentStt

void PndTrkSttAdjacencies::CalculateAdjacentStt2(const Short_t NUMBER_STRAWS,    // number of Stt Straws in total;
                                                 TClonesArray *SttTubeArray,     // input; array of the Stt tubes;
                                                 Short_t *nParContiguous,        // output; number of contiguous straws (axial Stt);
                                                                                 // NUMBER_STRAWS even if the numbering scheme for the Stt straws goes
                                                                                 //  from 1 to NUMBER_STRAWS included;
                                                 Short_t ListParContiguous[][6], // output list (axial Stt); first dimension is NUMBER_STRAWS;
                                                 Double_t *x,                    // X position center of tube;
                                                 Double_t *y,                    // Y position center of tube;
                                                 Double_t *z,                    // Z position center of tube;
                                                 Double_t *rxy2                  // X*X+Y*Y position center of tube;
)
{
  int i, j; //,n, ncontigue; //[R.K. 01/2017] unused variable?

  //	FILE *HAND = fopen("ListaSttStraw.lis","w");

  double dis2,
    // Xwiredirection[NUMBER_STRAWS], //[R.K. 9/2018] unused
    // Ywiredirection[NUMBER_STRAWS], //[R.K. 9/2018] unused
    Zwiredirection[NUMBER_STRAWS];

  PndSttTube *pSttTube;

  // the total n. of STT straws is NUMBER_STRAWS;
  for (i = 1; i <= NUMBER_STRAWS; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    x[i - 1] = pSttTube->GetPosition().X();
    y[i - 1] = pSttTube->GetPosition().Y();
    z[i - 1] = pSttTube->GetPosition().Z();
    rxy2[i - 1] = x[i - 1] * x[i - 1] + y[i - 1] * y[i - 1];
    // Xwiredirection[i-1] = pSttTube->GetWireDirection().X(); //[R.K. 9/2018] unused
    // Ywiredirection[i-1] = pSttTube->GetWireDirection().Y(); //[R.K. 9/2018] unused
    Zwiredirection[i - 1] = pSttTube->GetWireDirection().Z();
  }

  for (i = 0; i < NUMBER_STRAWS; i++) {
    if (fabs(Zwiredirection[i] - 1.) > 1.e-5)
      continue; // for now I don't consider the skews;

    nParContiguous[i + 1] = 0;
    for (j = 0; j < NUMBER_STRAWS; j++) {
      if (j == i)
        continue;
      if (fabs(Zwiredirection[j] - 1.) > 1.e-5)
        continue; // for now I don't consider the skews;
      dis2 = (x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j]);
      if (dis2 > 1.1)
        continue;
      ListParContiguous[i][nParContiguous[i]] = j + 1;
      nParContiguous[i]++;
      if (nParContiguous[i] > 6) {
        cout << "Errore ! N contigue Par > 6!!! Exit.\n";
        exit(-1);
      }
    }

    /*
        fprintf(HAND,"Stt straw n. %d ",i+1);
        fprintf(HAND,", X %g, Y %g, Z %g ",
        x[i],y[i],z[i]);
        if(Zwiredirection[i] <0.) {
          fprintf(HAND," dirX %g , dirY %g , dirZ %g ; straw contigue = %d ;",
          -Xwiredirection[i],-Ywiredirection[i],Zwiredirection[i],
          nParContiguous[i]);
        }else{
          fprintf(HAND," dirX %g , dirY %g , dirZ %g ; straw contigue = %d ;",
          Xwiredirection[i],Ywiredirection[i],Zwiredirection[i],
          nParContiguous[i]);
        }
        if( nParContiguous[i]>0 ) {
          fprintf(HAND," lista :");
          for(n=0;n<nParContiguous[i];n++){
            fprintf(HAND," %d",ListParContiguous[i][n]);
          }
          fprintf(HAND,";\n");
        } else {
          fprintf(HAND,"\n");
        }
    */

  } // end of   for (i=0;i< NUMBER_STRAWS; i++)

  //	fclose (HAND);
}
//------------------ end function  PndTrkSttAdjacencies::CalculateAdjacentStt2

ClassImp(PndTrkSttAdjacencies)
