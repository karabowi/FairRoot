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

// -------------------------------------------------------------------------
// -----                 PndLmdHitMergeTask header file             -----
// -------------------------------------------------------------------------

#ifndef PNDLMDHITMERGETASK_H
#define PNDLMDHITMERGETASK_H

#include "FairTask.h"

#include <string>
#include <vector>

class TH2;
class TClonesArray;

class PndLmdHitMergeTask: public FairTask {
public:
	/** Default constructor **/

	PndLmdHitMergeTask();
	PndLmdHitMergeTask(const char* name);

	/** Destructor **/
	virtual ~PndLmdHitMergeTask();

	/** Virtual method Init **/
	virtual InitStatus Init();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	void Finish();

protected:
	TClonesArray* fHitArray;        // Input array of PndSdsHits
	TClonesArray* fMergedHitArray;  // Output array of PndSdsHits

	TString fHitBranchName;

	TH2* hdxdy;

	ClassDef(PndLmdHitMergeTask, 0);
};

#endif /* PNDLMDHITMERGETASK_H */
