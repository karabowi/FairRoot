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

#include "PndTrkCategorizeStt.h"

#include "PndSttTube.h"
#include "PndTrkConstants.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include <stdlib.h>
#include <iostream>
#include <cmath>

using namespace std;

/** Default constructor **/
PndTrkCategorizeStt::PndTrkCategorizeStt(){};

/** Destructor **/
PndTrkCategorizeStt::~PndTrkCategorizeStt(){};

//-----------------------------------------------------------

void PndTrkCategorizeStt::CategorizeStt(
  // in this geametry there are 4542 Stt Straws (axial and skew);
  const Short_t NUMBER_STRAWS, // number of Stt Straws in total;
  TClonesArray *SttTubeArray,  // input; array of the Stt tubes;
  Short_t &nAxialOuterRight,   // output; number of axial Stt, outer, on the right (looking into the beam);
  Short_t &nAxialInnerRight,   // output; number of axial Stt, inner, on the right (looking into the beam);
  Short_t &nAxialOuterLeft,    // output; number of axial Stt, outer, on the left (looking into the beam);
  Short_t &nAxialInnerLeft,    // output; number of axial Stt, inner, on the left (looking into the beam);

  Short_t *ListAxialOuterRight, // output; list of axial Stt, outer, on the right (looking into the beam);
  Short_t *ListAxialInnerRight, // output; list of axial Stt, inner, on the lright (looking into the beam);
  Short_t *ListAxialOuterLeft,  // output; list of axial Stt, outer, on the left (looking into the beam);
  Short_t *ListAxialInnerLeft,  // output; list of axial Stt, inner, on the left (looking into the beam);

  Short_t &nSkewRight,    // output; number of skew Stt, on the right (looking into the beam);
  Short_t &nSkewLeft,     // output; number of skew Stt, on the right (looking into the beam);
  Short_t *ListSkewRight, // output; list of axial Stt, inner, on the lright (looking into the beam);
  Short_t *ListSkewLeft   // output; list of axial Stt, outer, on the left (looking into the beam);
)
{
  int i;

  double r2, x[NUMBER_STRAWS],
    // Xwiredirection[NUMBER_STRAWS], //[R.K.02/2017] Unused variable?
    y[NUMBER_STRAWS],
    // Ywiredirection[NUMBER_STRAWS], //[R.K.02/2017] Unused variable?
    // z[NUMBER_STRAWS], //[R.K.02/2017] Unused variable?
    Zwiredirection[NUMBER_STRAWS];

  PndSttTube *pSttTube;

  nAxialOuterRight = 0;
  nAxialOuterLeft = 0;
  nAxialInnerRight = 0;
  nAxialInnerLeft = 0;
  nSkewRight = 0;
  nSkewLeft = 0;

  // the total n. of STT straws is NUMBER_STRAWS;
  for (i = 1; i <= NUMBER_STRAWS; i++) {
    pSttTube = (PndSttTube *)SttTubeArray->At(i);
    x[i - 1] = pSttTube->GetPosition().X();
    y[i - 1] = pSttTube->GetPosition().Y();
    // z[i-1] = pSttTube->GetPosition().Z(); //[R.K.02/2017] Unused variable?
    // Xwiredirection[i-1] = pSttTube->GetWireDirection().X(); //[R.K.02/2017] Unused variable?
    // Ywiredirection[i-1] = pSttTube->GetWireDirection().Y(); //[R.K.02/2017] Unused variable?
    Zwiredirection[i - 1] = pSttTube->GetWireDirection().Z();
  }

  for (i = 0; i < NUMBER_STRAWS; i++) {
    if (fabs(Zwiredirection[i] - 1.) > 1.e-5) {
      // skew straws;
      if (x[i] < 0.) {
        //  left skew straws;
        ListSkewLeft[nSkewLeft] = i + 1;
        nSkewLeft++;
      } else {
        // right skew straws;
        ListSkewRight[nSkewRight] = i + 1;
        nSkewRight++;
      }
    } else {
      r2 = x[i] * x[i] + y[i] * y[i];
      // axial straws;
      if (x[i] < 0.) {
        //  left axial straws;

        if (r2 < APOTEMAMINOUTERPARSTRAW * APOTEMAMINOUTERPARSTRAW) {
          // inner;
          ListAxialInnerLeft[nAxialInnerLeft] = i + 1;
          nAxialInnerLeft++;
        } else {
          // outer;
          ListAxialOuterLeft[nAxialOuterLeft] = i + 1;
          nAxialOuterLeft++;
        } // end of   if( r2 < APOTEMAMINOUTERPARSTRAW * APOTEMAMINOUTERPARSTRAW )

      } else {
        // right axial straws;

        if (r2 < APOTEMAMINOUTERPARSTRAW * APOTEMAMINOUTERPARSTRAW) {
          // inner;
          ListAxialInnerRight[nAxialInnerRight] = i + 1;
          nAxialInnerRight++;
        } else {
          // outer;
          ListAxialOuterRight[nAxialOuterRight] = i + 1;
          nAxialOuterRight++;
        } // end of   if( r2 < APOTEMAMINOUTERPARSTRAW * APOTEMAMINOUTERPARSTRAW )
      }   // end of if( x[i] < 0. )

    } // end of if( fabs(Zwiredirection[i] -1.) > 1.e-5)

  } // end of  for (i=0;i< NUMBER_STRAWS; i++)
}
//------------------ end function  PndTrkCategorizeStt::CategorizeStt

ClassImp(PndTrkCategorizeStt)
