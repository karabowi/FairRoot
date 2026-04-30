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
 *      Author: stockman
 * PndDrcDigiRingSorter.h
 *
 */
// -------------------------------------------------------------------------
// -----                   PndDrcDigiRingSorter header file              -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDDRCDigiRINGSORTER_H_
#define PNDDRCDigiRINGSORTER_H_

#include <FairRingSorter.h>

class PndDrcDigiRingSorter : public FairRingSorter {
 public:
  PndDrcDigiRingSorter(int size = 100, double width = 10) : FairRingSorter(size, width){};
  virtual ~PndDrcDigiRingSorter();

  virtual FairTimeStamp *CreateElement(FairTimeStamp *data);

  ClassDef(PndDrcDigiRingSorter, 1);
};

#endif /* PNDDrcDigiRINGSORTER_H_ */
