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

// -----------------------------------------
// PndDrcLutNode.h
//
// Created on: 09.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcLutNode.h"

ClassImp(PndDrcLutNode)

  // -----   Default constructor   -------------------------------------------
  PndDrcLutNode::PndDrcLutNode()
  : TObject(), fDetectorId(0), fSize(0), fNodePos(TVector3()), fPosArray(), fNodeArray(), fPathIdArray(), fPathArray(), fTimeArray()
{
}

// -----   Standard constructors   -----------------------------------------
PndDrcLutNode::PndDrcLutNode(Int_t nodeId) : TObject(), fDetectorId(nodeId), fSize(0), fNodePos(TVector3()), fPosArray(), fNodeArray(), fPathIdArray(), fPathArray(), fTimeArray()
{
}

void PndDrcLutNode::AddEntry(Int_t detectorId, TVector3 dir, Double_t path, Int_t pathid, Double_t time, TVector3 pos)
{
  fDetectorId = detectorId;
  fNodeArray.push_back(dir);
  fPathArray.push_back(path);
  fPathIdArray.push_back(pathid);
  fTimeArray.push_back(time);
  fPosArray.push_back(pos);
  fSize++;
}
