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

#ifndef PNDLMDTRKSFILTERTASK_H
#define PNDLMDTRKSFILTERTASK_H

#include "FairTask.h"

#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TVector3.h"

class TClonesArray;

class PndLmdTrksFilterTask: public FairTask {
public:
	PndLmdTrksFilterTask(double LabMomentum, bool UseKinematicsFilter, double IPXMean = 0.0,
	    double IPYMean = 0.0);
#ifndef __CINT__
  PndLmdTrksFilterTask(const PndLmdTrksFilterTask &) = delete;
  PndLmdTrksFilterTask &operator=(const PndLmdTrksFilterTask &) = delete;
#endif
	virtual ~PndLmdTrksFilterTask() = default;
	void SetVerboseLevel(int verbose) {
		verboseLevel = verbose;
	}
	InitStatus Init();
	void Finish();

	void Exec(Option_t* opt);

private:
	std::vector<int> getHitIDSequence(unsigned int TrackID) const;
	unsigned int getNumberOfIdenticalHits(const std::vector<int> &seq1,
	    const std::vector<int> &seq2) const;
	int verboseLevel;

	// Input Data------------
	TClonesArray* fTrkCandArray;
	TClonesArray* fTrkArray;
	TClonesArray* fTrkOutArray;

	TString fTrkCandName;
	TString fTrkName;
	TString fTrkOutName;
	int fEventNr;
	bool fUseKinematicsFilter;

	double fIPXMean;  // beam shift in X
	double fIPYMean;  // beam shift in Y

	// mean and sigma are determined from parameterization (simulations)
	double PhiDiffMean;
	double PhiDiffSigma;
	double MagnitudeRatioMean;
	double MagnitudeRatioSigma;

	TH1D *RadialPhiDiffBefore;
	TH1D *RadialMagnitudeRatioBefore;
	TH1D *RadialPhiDiffAfter;
	TH1D *RadialMagnitudeRatioAfter;
	TH2D *RadialPhiDiffMagnitudeRatioBefore;
	TH2D *RadialPhiDiffMagnitudeRatioAfter;

ClassDef(PndLmdTrksFilterTask, 4)
	;
};

#endif
