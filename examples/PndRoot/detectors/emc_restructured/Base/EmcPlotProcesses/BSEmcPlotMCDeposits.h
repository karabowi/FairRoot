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

#ifndef BSEMCPLOTMCDEPOSITS_HH
#define BSEMCPLOTMCDEPOSITS_HH

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "BSEmcPlotProcess.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPlotMCDeposits
 * @brief Process to plot some values for MCDeposits such as energy or time
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotMCDeposits : public BSEmcPlotProcess {
 public:
  BSEmcPlotMCDeposits() : BSEmcPlotProcess("BSEmcPlotMCDeposits") {}
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  virtual void InitHistos(const TString &t_folderprefix);

 private:
  Int_t fEvent{0};
  ClassDef(BSEmcPlotMCDeposits, 1);
};

#endif /*BSEMCPLOTMCDEPOSITS_HH*/
