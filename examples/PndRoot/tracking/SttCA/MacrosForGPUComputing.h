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
 * MacrosForGPUComputing.h
 *
 *  Created on: Sep 1, 2015
 *      Author: schumann
 */

#ifndef MACROSFORGPUCOMPUTING_H_
#define MACROSFORGPUCOMPUTING_H_

// Values for gpu algorithm
#define NUM_STRAWS 4542
#define MAX_SKEWED_NEIGHBORS 22
#define MAX_UNSKEWED_NEIGHBORS 6
#define START_TUBE_ID_SKEWED 855
#define END_TUBE_ID_SKEWED 2956
#define NUM_SKEWED_STRAWS (END_TUBE_ID_SKEWED - START_TUBE_ID_SKEWED + 1)
#define NUM_UNSKEWED_STRAWS (NUM_STRAWS - NUM_SKEWED_STRAWS)

#define MAX_THREADS_PER_BLOCK 1024
#define MAX_MULTISTATE_NUM 20

#define WARP_SIZE 32

#endif /* MACROSFORGPUCOMPUTING_H_ */
