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
 * PndEmcClusterRingSorter.h
 */

#ifndef PNDEMCCLUSTERRINGSORTER_H_
#define PNDEMCCLUSTERRINGSORTER_H_

#include <FairRingSorter.h>

class PndEmcClusterRingSorter: public FairRingSorter {
public:
	PndEmcClusterRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndEmcClusterRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	void SetClusterType(Int_t type=0) {fClusterType=type;};

private:
	Int_t fClusterType=0;

	ClassDef (PndEmcClusterRingSorter,1);
};

#endif /* PNDEMCCLUSTERRINGSORTER_H_ */
