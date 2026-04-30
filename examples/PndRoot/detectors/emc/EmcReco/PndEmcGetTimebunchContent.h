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

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: $
//---------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCGETTIMEBUNCHCONTENT_H
#define PNDEMCGETTIMEBUNCHCONTENT_H

#include "FairTask.h"
#include <vector>		

#include "FairTSBufferFunctional.h"
#include "TStopwatch.h"
#include "TH1.h"

class TClonesArray;
class TObjectArray;
class PndEmcDigi;
class PndEmcGeoPar;
class PndEmcRecoPar;
class BinaryFunctor;


class PndEmcGetTimebunchContent : public FairTask
{
public:

  // Constructors

  PndEmcGetTimebunchContent(Int_t verbose=0);

  // Destructor

  virtual ~PndEmcGetTimebunchContent( );

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void FinishTask();
 
  ///  Override EmcRecoPar active time parameter ..to be set in ns!!! 
  void SetClusterActiveTime(Double_t time) { fClusterActiveTime = time; } 
  
private:
	/** Input array of CbmDigis **/
	TClonesArray* fDigiArray;

	PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	
	Double_t fDigiEnergyTresholdBarrel; 
	Double_t fDigiEnergyTresholdFWD; 
	Double_t fDigiEnergyTresholdBWD; 
	Double_t fDigiEnergyTresholdShashlyk;

	Double_t fClusterActiveTime; 
	BinaryFunctor* fDigiFunctor;

	TH1I* hEventMultiplicity;

	Int_t fNrOfEvents;

	/** Get parameter containers **/
	virtual void SetParContainers();

	TStopwatch fTimer;

	ClassDef(PndEmcGetTimebunchContent, 1)
};
#endif // PNDEMCGETTIMEBUNCHCONTENT_HH
