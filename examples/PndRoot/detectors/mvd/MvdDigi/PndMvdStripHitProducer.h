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

#ifndef PNDMVDSTRIPHITPRODUCER_H
#define PNDMVDSTRIPHITPRODUCER_H 1

#include "PndSdsStripHitProducer.h"

class TClonesArray;

//! Hit Producer Task for strip detectors
/**
 * The choice of the parameters used for Digitization depends on the invocation of the
 * different constructors of this task. Instantiating by the default constructor forces
 * the digitisation parameters initialized from the DigiPar-Database. On the other hand these
 * parameters may be overridden by invoking the constructor:
 * @code PndMvdStripHitProducer(Double_t, Double_t, Double_t, Double_t, TVector2, TVector2,
 * Int_t, Int_t, Int_t, Double_t, Double_t) @endcode \n
 * Basically, the sensors are thought to be rectangular (even if they are not).
 * Knowing the origin of the wafer, the strips can be described by an angle (orientation),
 * their separation with respect to each other (pitch) and one point that is known to be part of the first strip (anchor point).\n
 * The numbering scheme is as follows (assuming 128 channels per FE): \n
 *
   <table>
   <TR> <TD> strip index </TD> <TD>frontend #</TD> <TD> side </TD> </TR>
   <TR> <TD>     0       </TD> <TD>    0     </TD> <TD>  top </TD> </TR> <TD><- Top Anchor</TD>
   <TR> <TD>     1       </TD> <TD>    0     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    ...      </TD> <TD>    0     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    127      </TD> <TD>    0     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    128      </TD> <TD>    1     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    ...      </TD> <TD>    1     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    255      </TD> <TD>    1     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    256      </TD> <TD>    2     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    ...      </TD> <TD>   ...    </TD> <TD>  top </TD> </TR>
   <TR> <TD> topNrFE*128-1 </TD> <TD> topNrFE-1  </TD> <TD>  top </TD> </TR>
   <TR> <TD> topNrFE*128 </TD> <TD> topNrFE  </TD> <TD>  bottom </TD> </TR> <TD><- Bottom Anchor</TD>
   <TR> <TD>    ...      </TD> <TD> topNrFE  </TD> <TD>  bottom </TD> </TR>
   <TR> <TD> topNrFE*128+127 </TD> <TD> topNrFE  </TD> <TD>  bottom </TD> </TR>
   <TR> <TD> topNrFE*128+128 </TD> <TD> topNrFE+1  </TD> <TD>  bottom </TD> </TR>
   <TR> <TD>    ...      </TD> <TD>   ...    </TD> <TD>  bottom </TD> </TR>
   <TR> <TD> (topNrFE+botNrFE)*128-1 </TD> <TD> topNrFE+botNrFE-1  </TD> <TD>  bottom </TD> </TR>

   </table>
 * \n
 * The numbering starts from the strip containing the anchor point following the direction
 * orthogonal to the strips in mathematically positive sense (along x-axis in positive direction,
 * if the strip orientation equals 90 degrees).
 * \n
 * @author HG Zaunick <hg.zaunick@physik.tu-dresden.de>
 *
 * [04/2010] This functionality went into the SDS Package, only inheritance here. R.Kliemt
 *
 *
 *
 *
 *
 *
 **/
class PndMvdStripHitProducer : public PndSdsStripHitProducer {
 public:
  /** Default constructor **/
  PndMvdStripHitProducer();

  /** Destructor **/
  virtual ~PndMvdStripHitProducer();

  /** Specific Parameter set loading **/
  virtual void SetParContainers();

  virtual void SetCalculators();

  /** Specify the branch & folder names for the I/O **/
  void SetBranchNames(TString inBranchname, TString outBranchname, TString folderName);

  void SetBranchNames() { SetBranchNames("MVDPoint", "MVDStripDigis", "PndMvd"); }

  ClassDef(PndMvdStripHitProducer, 6);
};

#endif
