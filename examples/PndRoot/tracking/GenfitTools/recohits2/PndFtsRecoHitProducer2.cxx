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

// ========================================
// PndFtsRecoHitProducer2
//
// Class for kalman fitting with genfit for FTS
//
// This FTS specific reco hit producer was created to be able
// to access the tube array TCA from in addition to the hit TCA
// when constructing the reco hit.
//
// It inherits from MeasurementProducer and has been written
// following the MeasurementProducer
//
// modified by Elisabetta Prencipe 19/5/2014
// ========================================

#include "PndFtsRecoHitProducer2.h"

#include <iostream>
