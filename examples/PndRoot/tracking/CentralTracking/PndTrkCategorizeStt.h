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

#ifndef PndTrkCategorizeStt_H
#define PndTrkCategorizeStt_H 1

#include "TClonesArray.h"

class PndTrkCategorizeStt {

 public:
  /** Default constructor **/
  PndTrkCategorizeStt();

  /** Destructor **/
  virtual ~PndTrkCategorizeStt();

  void CategorizeStt(
    // in this geametry there are 4542 Stt Straws (axial and skew);
    const Short_t NUMBER_STRAWS, // number of Stt Straws in total;
    TClonesArray *SttTubeArray,  // input; array of the Stt tubes;
    Short_t &nAxialOuterRight,   // output; number of axial Stt, outer, on the right (looking into the beam);
    Short_t &nAxialInnerRight,   // output; number of axial Stt, inner, on the right (looking into the beam);
    Short_t &nAxialOuterLeft,    // output; number of axial Stt, outer, on the left (looking into the beam);
    Short_t &nAxialInnerLeft,    // output; number of axial Stt, inner, on the left (looking into the beam);

    Short_t *ListAxialOuterRight, // output; list of axial Stt, outer, on the right (looking into the beam);
    Short_t *ListAxialInnerRight, // output; list of axial Stt, inner, on the lright (looking into the beam);
    Short_t *ListAxialOuterLeft,  // output; list of axial Stt, outer, on the left (looking into the beam);
    Short_t *ListAxialInnerLeft,  // output; list of axial Stt, inner, on the left (looking into the beam);

    Short_t &nSkewRight,    // output; number of skew Stt, on the right (looking into the beam);
    Short_t &nSkewLeft,     // output; number of skew Stt, on the right (looking into the beam);
    Short_t *ListSkewRight, // output; list of axial Stt, inner, on the lright (looking into the beam);
    Short_t *ListSkewLeft   // output; list of axial Stt, outer, on the left (looking into the beam);
  );

  ClassDef(PndTrkCategorizeStt, 1);
};

#endif
