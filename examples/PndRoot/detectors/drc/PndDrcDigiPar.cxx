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

//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndDrcDigiPar source file                 -----
// -----                  Created 09/11/2010 by M. Patsyuk            -----
// -------------------------------------------------------------------------

/** PndDrcDigiPar
 *@author Maria Patsyuk <m.patsyuk@gsi.de>
 *@since 09.11.2010
 *@version 1.0
 **
 ** PANDA task class for storing digitization parameters for DIRC barrel
 ** Task level RECO
 **/

#include <iostream>
#include "PndDrcDigiPar.h"
#include "PndDetectorList.h"

using std::cout;
using std::endl;
using std::flush;
// using std::map;
using std::pair;

PndDrcDigiPar::PndDrcDigiPar(const char *name, const char *title, const char *context) : FairParGenericSet(name, title, context)
{
  clear();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndDrcDigiPar::~PndDrcDigiPar(void) {}
// -------------------------------------------------------------------------

void PndDrcDigiPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->add("parameters", fDrcParameters);
  list->add("NumberOfHitPixels", fNOfHitPixels);
  list->add("NumberOfAmbiguities", fNOfAmbiguities);
  list->add("NumberOfParametersPerPixel", fNOfParametersPerPixel);
}

Bool_t PndDrcDigiPar::getParams(FairParamList *list)
{
  if (!list) {
    std::cout << "list not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("parameters", &fDrcParameters)) {
    std::cout << "par: fPndParameters not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("NumberOfHitPixels", &fNOfHitPixels)) {
    std::cout << "par: NumberOfHitPixels not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("NumberOfAmbiguities", &fNOfAmbiguities)) {
    std::cout << "par: NumberOfAmbiguities not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("NumberOfParametersPerPixel", &fNOfParametersPerPixel)) {
    std::cout << "par: NumberOfParametersPerPixel not found" << std::endl;
    return kFALSE;
  }

  return kTRUE;
}
/*
Bool_t PndDrcDigiPar::GetParamsForPixel(Double_t xp, Double_t yp, Double_t* parArray)
{
  Int_t a = 0;
  for(Int_t i=0; i<fNOfHitPixels; i++){
    if(fabs(fDrcParameters.At(i*(fNOfParametersPerPixel+2)) - xp) < 0.0005 &&
       fabs(fDrcParameters.At(i*(fNOfParametersPerPixel+2)+1) - yp) < 0.0005){
         a = 1;
         for(Int_t j=0; j<fNOfParametersPerPixel; j++){
           parArray[j] = fDrcParameters.At(i*(fNOfParametersPerPixel+2)+j+2);
     //cout<<"PPar "<<j<<" = "<<fDrcParameters.At(i*(fNOfParametersPerPixel+2)+j+2)<<endl;
         }
    }
  }
  if (a == 1 && !TMath::IsNaN(parArray[1])) return true;
  else return false;
}
*/
Bool_t PndDrcDigiPar::GetParamsForPixel(Int_t pixid, Double_t *parArray)
{
  Int_t a = 0;
  for (Int_t i = 0; i < fNOfHitPixels; i++) {
    if (fDrcParameters.At(i * (fNOfParametersPerPixel + 1)) == pixid) {
      a = 1;
      for (Int_t j = 0; j < fNOfParametersPerPixel; j++) {
        parArray[j] = fDrcParameters.At(i * (fNOfParametersPerPixel + 1) + j + 1);
        // cout<<"PPar "<<j<<" = "<<fDrcParameters.At(i*(fNOfParametersPerPixel+1)+j+1)<<endl;
      }
    }
  }
  if (a == 1 && !TMath::IsNaN(parArray[1]))
    return true;
  else
    return false;
}

ClassImp(PndDrcDigiPar);

void PndDrcDigiPar::Print()
{
  Int_t arrayIndex = 0;
  cout << "-------------------------------------------------" << endl;
  cout << "DIRC barrel PD Parameters:" << endl;
  while (arrayIndex < fDrcParameters.GetSize()) {
    cout << fDrcParameters[arrayIndex] << " " << endl;
  }
  cout << "-------------------------------------------------" << endl;
}
