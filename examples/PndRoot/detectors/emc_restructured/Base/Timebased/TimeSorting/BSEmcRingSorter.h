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
 * BSEmcRingSorter.h
 */

#ifndef BSEMCRINGSORTER_HH
#define BSEMCRINGSORTER_HH

#include <FairRingSorter.h>

/**
 * @class BSEmcRingSorter
 * @brief templated version of a ring sorter
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
template <class T>
class BSEmcRingSorter : public FairRingSorter {
 public:
  BSEmcRingSorter(Int_t t_size = 100, Double_t t_width = 10) : FairRingSorter(t_size, t_width){};
  virtual ~BSEmcRingSorter(){};

  virtual FairTimeStamp *CreateElement(FairTimeStamp *t_data) /*override*/ { return new T(*(dynamic_cast<T *>(t_data))); }

  ClassDef(BSEmcRingSorter, 1);
};

#endif /*BSEMCRINGSORTER_HH*/
