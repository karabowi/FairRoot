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

#include "BSEmcRecoHit.h"

#include "RtypesCore.h"

#include "FairHit.h" // for FairHit

BSEmcRecoHit::BSEmcRecoHit() : FairHit() {}

BSEmcRecoHit::BSEmcRecoHit(const BSEmcRecoHit &t_other)
  : FairHit(t_other), fPosition(t_other.fPosition), fCorrectedPosition(t_other.fCorrectedPosition), fRawEnergy(t_other.fRawEnergy), fCalEnergy(t_other.fCalEnergy),
    fNumberOfCrystals(t_other.fNumberOfCrystals), fNumberOfSubCluster(t_other.fNumberOfSubCluster), fNumberOfSubClusterCrystals(t_other.fNumberOfSubClusterCrystals),
    fModule(t_other.fModule), fClusterIndex(t_other.fClusterIndex), fSubClusterIndex(t_other.fSubClusterIndex), fCentralCrystalId(t_other.fCentralCrystalId), fZ20(t_other.fZ20),
    fZ53(t_other.fZ53), fLat(t_other.fLat), fE1(t_other.fE1), fE9(t_other.fE9), fE25(t_other.fE25), fCov7(t_other.fCov7), fCovP4(t_other.fCovP4), fIsSplitOff(t_other.fIsSplitOff), fMcList(t_other.fMcList),
    fTrackEntering(t_other.fTrackEntering), fTrackExiting(t_other.fTrackExiting)
{
}

BSEmcRecoHit::~BSEmcRecoHit() {}
