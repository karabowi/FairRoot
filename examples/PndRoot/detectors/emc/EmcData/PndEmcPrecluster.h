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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcPreclusters. An extension of PndEmcCluster - for testing 
//          purposes only.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCPRECLUSTER_H
#define PNDEMCPRECLUSTER_H

//----------------------
// Base Class Headers --
//----------------------
#include "PndEmcCluster.h"
#include "TObject.h"

/**
 * @brief represents an emc precluster, formed in the (virtual) data concentrators
 * @ingroup PndEmc
 */
class PndEmcPrecluster: public PndEmcCluster
{

public:

	// Constructors
	PndEmcPrecluster();
	
	// Destructor
	virtual ~PndEmcPrecluster();
	
	/** Copy **/ //use compiler generated copy constructor
	//PndEmcPrecluster(const PndEmcPrecluster& copy);

	// Modifiers
	virtual void addDigi(const TClonesArray *digiArray, Int_t iDigi);

	// Getters
//	const std::vector<Int_t> &DigiList() const {return fDigiList;};
	Int_t GetNumberOfDigis() {return fDigiTimeArray.size();};
	 std::vector<Double_t> &GetMemberDigiTimes()  {return fDigiTimeArray;};
	 std::vector<Double_t> &GetMemberDigiEnergies()  {return fDigiEnergyArray;};
	 std::vector<Double_t> &GetMemberDigiXpos()  {return fDigiXposArray;};
	 std::vector<Double_t> &GetMemberDigiYpos()  {return fDigiYposArray;};
	 std::vector<Double_t> &GetMemberDigiZpos()  {return fDigiZposArray;};

	virtual void SetDigiPositionType(Int_t type=-1) { fDigiPosType = type; } // select whether to use mapped or real digi position


protected:
  // Data members
	std::vector<Double_t> fDigiTimeArray;
	std::vector<Double_t> fDigiEnergyArray;
	std::vector<Double_t> fDigiXposArray;
	std::vector<Double_t> fDigiYposArray;
	std::vector<Double_t> fDigiZposArray;

	Int_t fDigiPosType;

	ClassDef(PndEmcPrecluster,1)
private:
		PndEmcPrecluster& operator=(const PndEmcPrecluster& rv);
};
#endif // PNDEMCPRECLUSTER_HH
