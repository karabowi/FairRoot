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
// -----                   PndGemTrackFinder header file               -----
// -----                  Created 19.03.2009 by R. Karabowicz          -----
// -----                according to the PndDchTrackFinder             -----
// -------------------------------------------------------------------------

/** \class PndGemTrackFinder
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief Abstract base class for concrete Gem track finding algorithm.
 *
 *  Each derived class must implement the method DoFind. This has
 *  to operate on the TClonesArrays of Pnd hits/digis and to fill
 *  the PndGemTrackArray.
 **/

#ifndef PNDGEMTRACKFINDER
#define PNDGEMTRACKFINDER 1

#include "TObject.h"

class TClonesArray;

class PndGemTrackFinder : public TObject {

 public:
  /** Default constructor **/
  PndGemTrackFinder(){};

  /** Destructor **/
  virtual ~PndGemTrackFinder(){};

  /** Virtual method Init. If needed, to be implemented in the
   *  concrete class.
   **/
  virtual void Init(){};
  virtual void Finish(){};

  /** Virtual method SetParContainers. If needed, to be implemented in the
   *  concrete class.
   **/
  virtual void SetParContainers(){};

  /** Abstract method DoFind. To be implemented in the concrete class.
   *  Task: Read the hit/digi array and fill the track array,
   *  pointers to which are given as arguments
   *
   *  \param hitOrDigiArray   Array of Pnd hits/digis
   *  \param trackArray       Array of PndGemTrack
   *  \return Number of tracks created
   **/
  virtual Int_t DoFind(TClonesArray *hitOrDigiArray, TClonesArray *trackArray, TClonesArray *trackCandArray) = 0;

  ClassDef(PndGemTrackFinder, 1);
};

#endif
