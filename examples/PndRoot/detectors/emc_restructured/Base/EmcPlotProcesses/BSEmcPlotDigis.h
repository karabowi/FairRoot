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

#ifndef BSEMCPLOTDIGIS_HH
#define BSEMCPLOTDIGIS_HH

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "BSEmcPlotProcess.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPlotDigis
 * @brief Process to plot some digi values, such as number of digis per event, energy of each digi, time, etc
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotDigis : public BSEmcPlotProcess {
 public:
  BSEmcPlotDigis() : BSEmcPlotProcess("BSEmcPlotDigis") {}
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  virtual void InitHistos(const TString &t_folderprefix) /*override*/;

 private:
  Int_t fEvent{0};

  ClassDef(BSEmcPlotDigis, 1);
};

#endif /*BSEMCPLOTDIGIS_HH*/
