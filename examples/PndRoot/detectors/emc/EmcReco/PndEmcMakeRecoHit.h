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
// Software developed for the PANDA Detector at GSI.		
//---------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCMAKERECOHIT_H
#define PNDEMCMAKERECOHIT_H

#include <PndPersistencyTask.h>
//#include <string>
//#include <vector>		

class TClonesArray;
class TObjectArray;
class PndEmcBump;
class PndEmcRecoPar;

/**
 * @brief Create PndEmcRecoHit from PndEmcBump
 * @ingroup PndEmc
 */
class PndEmcMakeRecoHit : public PndPersistencyTask
{
public:

  // Constructors

  PndEmcMakeRecoHit(Int_t verbose=0, Bool_t storerecohits=kTRUE);

  // Destructor

  virtual ~PndEmcMakeRecoHit( );

  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetStorageOfData(Bool_t val); // Method to specify whether recohits are stored or not.

protected:

  
private:
	/** Input array of PndEmcBumps **/
	TClonesArray* fBumpArray;
	
	/** Output array of PndEmcRecoHits **/
	TClonesArray* fRecoHitArray;
	  
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();
	
	/** Verbosity level **/
	Int_t fVerbose;

	Bool_t fStoreRecoHits;

        PndEmcMakeRecoHit(const  PndEmcMakeRecoHit& L);
        PndEmcMakeRecoHit& operator= (const  PndEmcMakeRecoHit&) {return *this;};
	
	ClassDef(PndEmcMakeRecoHit,1)
};
#endif // PNDEMCMAKERECOHIT_HH
