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
 * PndMasterRunCompactSim.h
 *
 *  Created on: 18.09.2024
 *      Author: Tobias Stockmanns
 */

#pragma once

#include "PndMasterRunSim.h"

class PndMasterRunCompactSim : public PndMasterRunSim {
public:
	PndMasterRunCompactSim();
	virtual ~PndMasterRunCompactSim();

	virtual void CreateGeometry();
	virtual void SetField();
	virtual void AddSimTasks();

private:
	void AddEmcGeometry();

/** @cond CLASSIMP */
ClassDef(PndMasterRunCompactSim, 1);
/** @endcond */

};

