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
 * PndEmcClusterRingSorter.cxx
 */

#include "PndEmcClusterRingSorter.h"
#include "PndEmcCluster.h"
#include "PndEmcPrecluster.h"


ClassImp(PndEmcClusterRingSorter);


PndEmcClusterRingSorter::~PndEmcClusterRingSorter() {
	// TODO Auto-generated destructor stub
}

FairTimeStamp* PndEmcClusterRingSorter::CreateElement(FairTimeStamp* data) {

//	if (fClusterType==0) return new PndEmcCluster(*(PndEmcCluster*)data); // <-- Here (set to use PndEmcCluster)
//	else if (fClusterType==1) return new PndEmcPrecluster(*(PndEmcPrecluster*)data); // <-- and here (set to use PndEmcPrecluster)
	return new PndEmcCluster(*(PndEmcCluster*)data);
//	return new PndEmcPrecluster(*(PndEmcPrecluster*)data);

// It works fine when using just 1 type without the if statement
	
}
