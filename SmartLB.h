/**
 *  \addtogroup CkLdb
 */

/*@{*/

#ifndef _SMARTLB_H_
#define _SMARTLB_H_

#include "CentralLB.h"
#include "SmartLB.decl.h"

void CreateSmartLB();

class SmartLB : public CentralLB {
  public:
    SmartLB(const CkLBOptions &opt);
    SmartLB(CkMigrateMessage *m) : CentralLB (m) { };
    void procuraProc(LDStats *stats, double *pe_loads, int highestProcessor, int lessProcessor, int n_pes);
    //int * getMoreAndLessThanAverage(int npes, double *pe_loads);
    void work(LDStats *stats);
    void pup(PUP::er &p) { CentralLB::pup(p); }
    void FreeProcs(double* bufs);
    void FreeProcs(int* bufs);

  private:
    CmiBool QueryBalanceNow(int _step);
};

#endif /* _GRAPHPARTLB_H_ */

/*@}*/
