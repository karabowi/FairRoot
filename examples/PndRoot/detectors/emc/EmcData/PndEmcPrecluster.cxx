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

//---------------------------------------------------------------------
// Description:
//     Precluster object that also stores the base digi information.
//
// Author List:
//	Marcel Tiemens	University of Groningen

//---------------------------------------------------------------------

#include "PndEmcPrecluster.h"

#include "FairRunAna.h"
#include "FairLogger.h"

#include "PndEmcDigi.h"
#include "PndEmcXtal.h"
#include "PndEmcStructure.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cfloat>
#include <vector>
#include <utility>
#include "assert.h"

using std::vector;


//----------------
// Constructors --
//----------------

PndEmcPrecluster::PndEmcPrecluster()  : 
	fDigiTimeArray(),
	fDigiEnergyArray(),
	fDigiXposArray(),
	fDigiYposArray(),
	fDigiZposArray()
{
	fDigiTimeArray.clear();
	fDigiEnergyArray.clear();
	fDigiXposArray.clear();
	fDigiYposArray.clear();
	fDigiZposArray.clear();
}


//--------------
// Destructor --
//--------------

PndEmcPrecluster::~PndEmcPrecluster()
{
}

// implement only in this class?
/*
Double_t
PndEmcPrecluster::GetRadius() const
{
  if (fRadiusValid)
    return fRadius;
  else
  {
    LOG(error) << " in PndEmcPrecluster::GetRadius(): Radius of cluster is not defined";
    abort();
  }
}*/

//-------------
// Modifiers --
//-------------

	void
PndEmcPrecluster::addDigi(const TClonesArray *digiArray, Int_t iDigi)
{
	fDigiList.push_back(iDigi);
	PndEmcDigi *digi= (PndEmcDigi *) digiArray->At(iDigi);

	fTimeStamp = (digi->GetTimeStamp() > this->GetTimeStamp()) ? digi->GetTimeStamp() : this->GetTimeStamp();

	fDigiTimeArray.push_back(fTimeStamp);
	fDigiEnergyArray.push_back(digi->GetEnergy());
	if (fDigiPosType==0) {
	// use XPad, YPad
	fDigiXposArray.push_back(digi->GetXPad());
	fDigiYposArray.push_back(digi->GetYPad());
	}
	else if (fDigiPosType==1) {
// use real position
	fDigiXposArray.push_back(digi->where().x());
	fDigiYposArray.push_back(digi->where().y());
	}

	fDigiZposArray.push_back(digi->where().z()); // always use real z position

	invalidateCache(kFALSE);
	SetInsertHistory(kFALSE);
	if(FairRunAna::Instance()->IsTimeStamp()) {
		AddLink((static_cast<PndEmcDigi*>(digiArray->At(iDigi))->GetEntryNr()));
	} else {
		AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "EmcDigi", iDigi));
	}
	SetInsertHistory(kTRUE);
}

ClassImp(PndEmcPrecluster)
