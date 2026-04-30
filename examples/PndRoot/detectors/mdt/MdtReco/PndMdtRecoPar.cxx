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

/////////////////////////////////////////////////////////////
// PndMdtRecoPar
//
// Container class for Reconstruction parameters
//
/////////////////////////////////////////////////////////////

#include "PndMdtRecoPar.h"

ClassImp(PndMdtRecoPar)

  PndMdtRecoPar::PndMdtRecoPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context)
{
  Clear();

  fLayerPos[0][0] = 149.3;
  fLayerPos[0][1] = 155.8;
  fLayerPos[0][2] = 162.3;
  fLayerPos[0][3] = 168.8;
  fLayerPos[0][4] = 175.3;
  fLayerPos[0][5] = 181.8;
  fLayerPos[0][6] = 188.3;
  fLayerPos[0][7] = 194.8;
  fLayerPos[0][8] = 201.3;
  fLayerPos[0][9] = 207.8;
  fLayerPos[1][0] = 244.5;
  fLayerPos[1][1] = 250.5;
  fLayerPos[1][2] = 258.5;
  fLayerPos[1][3] = 266.5;
  fLayerPos[1][4] = 274.5;
  fLayerPos[1][5] = 282.5;
  fLayerPos[1][6] = 290.5;

  fAngleCut[0] = 2.5;
  fAngleCut[1] = 3.;
}

void PndMdtRecoPar::Clear()
{
  // Function to clear the Position array
  for (Int_t mm = 0; mm < 2; mm++) {
    fAngleCut[mm] = -1;
    for (Int_t ll = 0; ll < 10; ll++) {
      fLayerPos[mm][ll] = -1;
    }
  }
}
void PndMdtRecoPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  // list->add("LayerPosition",fLayerPos);
}

Bool_t PndMdtRecoPar::getParams(FairParamList *list)
{
  if (!list)
    return kFALSE;
  // if (!list->fill("LayerPosition",&fLayerPos)) return kFALSE;

  return kTRUE;
}
