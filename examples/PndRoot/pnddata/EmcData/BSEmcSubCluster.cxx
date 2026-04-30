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

#include "BSEmcSubCluster.h"

#include "BSEmcCluster.h"

BSEmcSubCluster::BSEmcSubCluster() : BSEmcCluster() {}

BSEmcSubCluster::BSEmcSubCluster(const BSEmcSubCluster &t_other) : BSEmcCluster(t_other), fClusterIndex(t_other.fClusterIndex), fCentralCrystalId(t_other.fCentralCrystalId) {}

BSEmcSubCluster::~BSEmcSubCluster() {}
