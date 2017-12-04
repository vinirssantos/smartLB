/**
 *  \addtogroup CkLdb
 */

/*@{*/

#ifndef _FIRSTLB_H_
#define _FIRSTLB_H_

#include "CentralLB.h"
#include "FirstLB.decl.h"

void CreateFirstLB();

class FirstLB : public CentralLB {
  public:
    FirstLB(const CkLBOptions &opt);
    FirstLB(CkMigrateMessage *m) : CentralLB (m) { };
    void procuraProc(LDStats *stats, double *pe_loads, int highestProcessor, int lessProcessor);
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
