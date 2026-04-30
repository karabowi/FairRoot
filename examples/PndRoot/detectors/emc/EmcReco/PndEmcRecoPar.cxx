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
// PndEmcRecoPar
//
// Container class for Reconstruction parameters
//
/////////////////////////////////////////////////////////////

#include "PndEmcRecoPar.h"


ClassImp(PndEmcRecoPar)

PndEmcRecoPar::PndEmcRecoPar(const char* name,const char* title,
			     const char* context)
    : FairParGenericSet(name,title,context), fEnergyThresholdBarrel(0), fEnergyThresholdFWD(0), fEnergyThresholdBWD(0), fEnergyThresholdShashlyk(0), fTimebunchCutTime(0.), fEmcClusterPosMethod(), fOffsetParmA(0), fOffsetParmB(0), fOffsetParmC(0), fEmcDigiPositionDepthPWO(0), fEmcDigiPositionDepthShashlyk(0), fMaxECut(0), fNeighbourECut(0), fCutSlope(0), fCutOffset(0), fERatioCorr(0), fTheNeighbourLevel(0), fMoliereRadius(0), fMoliereRadiusShashlyk(0), fExponentialConstant(0), fParArray1(TArrayD()), fParArray2(TArrayD()), fParArray3(TArrayD()), fParArray4(TArrayD()), fMaxIterations(0), fCentroidShift(0), fMaxBumps(0), fMinDigiEnergy(0) 
{
	clear();
}

void PndEmcRecoPar::putParams(FairParamList* list)
{
	if(!list) return;
	list->add("EnergyThresholdBarrel",fEnergyThresholdBarrel);
	list->add("EnergyThresholdFWD",fEnergyThresholdFWD);
	list->add("EnergyThresholdBWD",fEnergyThresholdBWD);
	list->add("EnergyThresholdShashlyk",fEnergyThresholdShashlyk);
	list->add("ClusterActiveTime",fTimebunchCutTime);
	list->add("EmcClusterPosMethod",fEmcClusterPosMethod);	
        list->add("OffsetParmA",fOffsetParmA);
	list->add("OffsetParmB",fOffsetParmB);
	list->add("OffsetParmC",fOffsetParmC);
	list->add("EmcDigiPositionDepthPWO",fEmcDigiPositionDepthPWO);
	list->add("EmcDigiPositionDepthShashlyk",fEmcDigiPositionDepthShashlyk);
	
	// parameters related to bump splitting
	list->add("MaxECut",fMaxECut);
	list->add("NeighbourECut",fNeighbourECut);
	list->add("CutSlope",fCutSlope);
	list->add("CutOffset",fCutOffset);
	list->add("ERatioCorr",fERatioCorr);
	list->add("TheNeighbourLevel",fTheNeighbourLevel);
	list->add("MoliereRadius",fMoliereRadius);
	list->add("MoliereRadiusShashlyk",fMoliereRadiusShashlyk);
	list->add("ExponentialConstant",fExponentialConstant);
	list->add("ParArray1",fParArray1);
	list->add("ParArray2",fParArray2);
	list->add("ParArray3",fParArray3);
	list->add("ParArray4",fParArray4);
	list->add("MaxIterations",fMaxIterations);
	list->add("CentroidShift",fCentroidShift);
	list->add("MaxBumps",fMaxBumps);
	list->add("MinDigiEnergy",fMinDigiEnergy);
}
 
Bool_t PndEmcRecoPar::getParams(FairParamList* list)
{
	if (!list) return kFALSE;
	if (!list->fill("EnergyThresholdBarrel",&fEnergyThresholdBarrel)) return kFALSE;
	if (!list->fill("EnergyThresholdFWD",&fEnergyThresholdFWD)) return kFALSE;
	if (!list->fill("EnergyThresholdBWD",&fEnergyThresholdBWD)) return kFALSE;
	if (!list->fill("EnergyThresholdShashlyk",&fEnergyThresholdShashlyk)) return kFALSE;
	if (!list->fill("ClusterActiveTime",&fTimebunchCutTime)) return kFALSE;
	if (!list->fill("EmcClusterPosMethod",fEmcClusterPosMethod,10)) return kFALSE;
	if (!list->fill("OffsetParmA",&fOffsetParmA)) return kFALSE;
	if (!list->fill("OffsetParmB",&fOffsetParmB)) return kFALSE;
	if (!list->fill("OffsetParmC",&fOffsetParmC)) return kFALSE;
	if (!list->fill("EmcDigiPositionDepthPWO",&fEmcDigiPositionDepthPWO)) return kFALSE;
	if (!list->fill("EmcDigiPositionDepthShashlyk",&fEmcDigiPositionDepthShashlyk)) return kFALSE;
	
	// parameters related to bump splitting
	if (!list->fill("MaxECut",&fMaxECut)) return kFALSE;
	if (!list->fill("NeighbourECut",&fNeighbourECut)) return kFALSE;
	if (!list->fill("CutSlope",&fCutSlope)) return kFALSE;
	if (!list->fill("CutOffset",&fCutOffset)) return kFALSE;
	if (!list->fill("ERatioCorr",&fERatioCorr)) return kFALSE;
	if (!list->fill("TheNeighbourLevel",&fTheNeighbourLevel)) return kFALSE;
	if (!list->fill("MoliereRadius",&fMoliereRadius)) return kFALSE;
	if (!list->fill("MoliereRadiusShashlyk",&fMoliereRadiusShashlyk)) return kFALSE;
	if (!list->fill("ExponentialConstant",&fExponentialConstant)) return kFALSE;
	if (!list->fill("ParArray1",&fParArray1)) return kFALSE;
	if (!list->fill("ParArray2",&fParArray2)) return kFALSE;
	if (!list->fill("ParArray3",&fParArray3)) return kFALSE;
	if (!list->fill("ParArray4",&fParArray4)) return kFALSE;
	if (!list->fill("MaxIterations",&fMaxIterations)) return kFALSE;
	if (!list->fill("CentroidShift",&fCentroidShift)) return kFALSE;
	if (!list->fill("MaxBumps",&fMaxBumps)) return kFALSE;
	if (!list->fill("MinDigiEnergy",&fMinDigiEnergy)) return kFALSE;

	return kTRUE;
}

