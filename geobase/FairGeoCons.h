#ifndef FAIRGEOCONS_H
#define FAIRGEOCONS_H

#include "FairGeoBasicShape.h"          // for FairGeoBasicShape

#include "Riosfwd.h"                    // for fstream
#include "Rtypes.h"                     // for FairGeoCons::Class, Bool_t, etc

#include <iosfwd>                       // for fstream

class FairGeoTransform;
class FairGeoVolume;
class TArrayD;
/**
 * class for geometry shape CONS
 * @author ilse koenig
 */
class FairGeoCons : public FairGeoBasicShape
{
  public:
    FairGeoCons();
    ~FairGeoCons();
    TArrayD* calcVoluParam(FairGeoVolume*);
    void calcVoluPosition(FairGeoVolume*,
                          const FairGeoTransform&,const FairGeoTransform&);
    Int_t readPoints(fstream*,FairGeoVolume*);
    Bool_t writePoints(fstream*,FairGeoVolume*);
    void printPoints(FairGeoVolume* volu);
    ClassDef(FairGeoCons,0) //
};

#endif  /* !FAIRGEOCONS_H */
