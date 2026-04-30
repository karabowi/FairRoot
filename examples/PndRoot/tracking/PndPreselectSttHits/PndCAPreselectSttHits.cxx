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

/*
 * PndCAPreselectSttHits.cxx
 *
 *  Created on: 14.09.2021
 *      Author: tstockmanns
 */

#include "PndCAPreselectSttHits.h"
#include "PndSttCATrackletGenerator.h"

void PndCAPreselectSttHits::Init(TClonesArray* sttHits, PndStt2GeoHandler *geoH)
//void PndCAPreselectSttHits::Init(TClonesArray* sttHits, TClonesArray* tubeArray)
{
	SetSttHitsArray(sttHits);
	fCADataMap = new PndSttCAData(geoH);
	//fCADataMap = new PndSttCAData(tubeArray);
}

std::vector<std::vector<PndSttHit*>> PndCAPreselectSttHits::PreselectSttHits() {
	fCADataMap->SetAllowDoubleHits(true);
	fCADataMap->AddHits(fSttHits, "STTHit");
	fCADataMap->GenerateNeighborhoodData();

	PndSttCATrackletGenerator trackletGenerator(fCADataMap);
	trackletGenerator.GenerateClusters();
	std::map<int, int> states = trackletGenerator.GetStates();

	std::vector<std::vector<PndSttHit*>> result;
	std::map<int, std::vector<PndSttHit*>> sttGroups;
	auto mapTubeIdToHit = fCADataMap->GetMapTubeIdToHit();
	std::vector<int> groupUsed;

	for (auto state : states) {
		sttGroups[state.second].push_back(static_cast<PndSttHit*>(fSttHits->At(mapTubeIdToHit[state.first])));
	}

	for (auto state : sttGroups) {
		result.push_back(state.second);
	}

	return result;
}





