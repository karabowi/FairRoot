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

/** CbmStsMapsHit
 ** Class for MAPS detector hit
 **@author Michael Deveaux <m.deveaux@gsi.de>
 ** Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken
 ** Version beta 0.1 (02.02.2005)
 ** Slight modifications by V. Friese to match coding conventions
 **
 ** Meaning of RefIndex:  Index of corresponding MCPoint
 **                       -1 if fake or background hit
 **
 ** Meaning of Flag:       0 = Hit ok
 **                       -1 : Hit lost due to detection inefficiency
 **/

#ifndef PndSttSkewedHit_H
#define PndSttSkewedHit_H 1

#include "TVector3.h"
#include "FairHit.h"
#include "PndTrack.h"

class PndSttSkewedHit : public FairHit {

 public:
  /** Default constructor **/
  PndSttSkewedHit();

  /** Standard constructor
   *@param detID     Detector unique volume ID
   *@param tubeID1   Unique tube ID
   *@param mcindex   Index of corresponding MCPoint
   **/

  // THIS ONE!
  PndSttSkewedHit(Int_t detID, Int_t tubeID1, Int_t tubeID2, Int_t mcindex, TVector3 &pos, TVector3 &dpos);

  /** Destructor **/
  virtual ~PndSttSkewedHit();

  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t *opt = nullptr) const
  {
    std::cout << " opt=" << opt << std::endl;
    return;
  }

  /** Public method Clear
   ** Resets the isochrone and it's error to 0
   **/
  void Clear() { SetTubeIDs(-1, -1); }

  /** Accessors **/

  // tube ID // CHECK added
  void SetTubeIDs(Int_t tubeid1, Int_t tubeid2) { fTubeIDs = std::make_pair<int, int>((int)tubeid1, (int)tubeid2); }
  std::pair<Int_t, Int_t> GetTubeIDs() const { return fTubeIDs; }

  friend std::ostream &operator<<(std::ostream &out, PndSttSkewedHit &digi)
  {
    out << "PndSttSkewedHit in Tubes: " << digi.GetTubeIDs().first << "/" << digi.GetTubeIDs().second << std::endl;
    return out;
  }

 protected:
  /** tube id **/
  std::pair<Int_t, Int_t> fTubeIDs; // CHECK added

  ClassDef(PndSttSkewedHit, 1);
};

#endif
