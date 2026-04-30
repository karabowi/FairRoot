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
 
#include "PndStt2DigiPar.h"
#include "FairParamList.h"

#include <TObjArray.h>

#include <iostream>

ClassImp(PndStt2DigiPar);

PndStt2DigiPar::PndStt2DigiPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fFcnTime2RadMean(""), fFcnTime2RadSigL(""), fFcnTime2RadSigH(""), fFcnRad2TimeMean(""), fFcnRad2TimeSigL(""), fFcnRad2TimeSigH(""),
    fResFactTime(1.0), fResFactRad(1.0), fResIsoIdeal(0.0), fSignalSpeed(20.0), fReadoutZ(35.0)
{
}

void PndStt2DigiPar::putParams(FairParamList *l)
{
  if (!l) 
    return;

  l->add("FcnTime2RadMean",  fFcnTime2RadMean);
  l->add("FcnTime2RadSigL",  fFcnTime2RadSigL);
  l->add("FcnTime2RadSigH",  fFcnTime2RadSigH);
  l->add("FcnRad2TimeMean",  fFcnRad2TimeMean);
  l->add("FcnRad2TimeSigL",  fFcnRad2TimeSigL);
  l->add("FcnRad2TimeSigH",  fFcnRad2TimeSigH);
  
  l->add("ResFactTime",      fResFactTime);
  l->add("ResFactRad",       fResFactRad);
  l->add("ResIsoIdeal",      fResIsoIdeal);
  l->add("SignalSpeed",      fSignalSpeed);
  l->add("ReadoutZ",         fReadoutZ);
}

Bool_t PndStt2DigiPar::getParams(FairParamList *l)
{
  if (!l)
    return kFALSE;
  
  if (!l->fill("FcnTime2RadMean", fFcnTime2RadMean, 255)) { 
    cout << "FcnTime2RadMean not found"<<endl; 
    return kFALSE; 
  }
  
  if (!l->fill("FcnTime2RadSigL", fFcnTime2RadSigL, 255)) {
     cout << "FcnTime2RadSigL not found"<<endl; 
     return kFALSE; 
  }
  
  if (!l->fill("FcnTime2RadSigH", fFcnTime2RadSigH, 255)) { 
    cout << "FcnTime2RadSigH not found"<<endl; 
    return kFALSE; 
  }
  
  if (!l->fill("FcnRad2TimeMean", fFcnRad2TimeMean, 255)) { 
    cout << "FcnRad2TimeMean not found"<<endl; 
    return kFALSE; 
  }
  
  if (!l->fill("FcnRad2TimeSigL", fFcnRad2TimeSigL, 255)) { 
    cout << "FcnRad2TimeSigL not found"<<endl; 
    return kFALSE; 
  }
  
  if (!l->fill("FcnRad2TimeSigH", fFcnRad2TimeSigH, 255)) { 
    cout << "FcnRad2TimeSigH not found"<<endl; 
    return kFALSE; 
  }
  
  if (!l->fill("ResFactTime", &fResFactTime)) { 
    cout << "ResFactTime not found. Using default "<<fResFactTime<<endl; 
    return kFALSE; 
  }
  
  if (!l->fill("ResFactRad", &fResFactRad)) { 
    cout << "ResFactRad not found. Using default "<< fResFactRad<<endl; 
    return kFALSE; 
  }
  
  if (!l->fill("ResIsoIdeal", &fResIsoIdeal)) { 
    cout << "ResIsoIdeal not found. Using default "<< fResIsoIdeal<<endl; 
    return kFALSE; 
  }
  
  if (!l->fill("SignalSpeed", &fSignalSpeed)) { 
    cout << "SignalSpeed not found. Using default "<< fSignalSpeed<<endl; 
    return kFALSE; 
  }
  
  if (!l->fill("ReadoutZ", &fReadoutZ)) { 
    cout << "ReadoutZ not found. Using default "<< fReadoutZ<<endl; 
    return kFALSE; 
  }
  
  return kTRUE;
}
