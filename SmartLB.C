
/**
 *  \addtogroup CkLdb
 */

/*@{*/

#include "SmartLB.h"
#include "ckgraph.h"
#include "stdio.h"

double aux = 0.0;

CreateLBFunc_Def(SmartLB, "My first load balancer")

SmartLB::SmartLB(const CkLBOptions &opt) : CentralLB(opt) {
  lbname = "SmartLB";
  if(CkMyPe() == 0)
    CkPrintf("SmartLB created on processor %d\n",CkMyPe());
}

CmiBool SmartLB::QueryBalanceNow(int _step) {
  return CmiTrue;
}

int * getMoreAndLessThanAverage(int npes, double *pe_loads) {
	int more = 0, less = 0;
	static int array[1];
	for (int i = 0; i < npes; i++){
		if (pe_loads[more] < pe_loads[i]) {
			more = i;
		}
		if (pe_loads[less] > pe_loads[i]) {
			less = i;
		}
	}
	array[0] = less;
	array[1] = more;

	return array;
}

int nmoves=0, taskCount=0, migrationCount=0;

void SmartLB::procuraProc(LDStats *stats, double *pe_loads, int highestProcessor, int lessProcessor, int n_pes) {
	double walltime, diference, wallDiff;

	/*
		a ideia: se a diferença de carga entre o core
		mais carregado e menos carregado for maior que
		5% ele avança uma etapa, e desta vez verifica
		se a carga do objeto é menor que a difença entre
		a carga dos cores extremos, caso seja menor ele
		ira migrar o processo (sempre do maior para o menor),
		assim evitando migrações com uma carga acima do normal
		ou desnecessarias.
	*/

	if (pe_loads[lessProcessor] <= (pe_loads[highestProcessor]*0.95)) {
		for (int j=0; j<stats->n_objs; j++) {
			if (stats->from_proc[j] == highestProcessor) {
				LDObjData &oData = stats->objData[j]; //carrega dados do processo atual
				walltime = oData.wallTime;//carga do objeto
				diference = pe_loads[highestProcessor]-pe_loads[lessProcessor];
				taskCount++;

				if(taskCount >= (n_pes*0.95) and taskCount <= (n_pes*0.90) and migrationCount < 1) {
					wallDiff = (diference*0.15);
				} else if(taskCount > (n_pes*0.90) and migrationCount < (n_pes*0.98)) {
					wallDiff = (diference*0.30);
				} else {
					wallDiff = (diference*0.5);
				}

				if (walltime <= (diference+wallDiff)) { //se a carga do objeto for menor que a diferença entre os cores entra
					migrationCount++;
					stats->to_proc[j] = lessProcessor; //migra stats->from_proc[j] para o core com menor carga
					pe_loads[lessProcessor] += walltime;
					pe_loads[stats->from_proc[j]] -= walltime;
					nmoves ++;  //incrementa num de migrações

				}
			}
		}
	}
}

void SmartLB::work(LDStats *stats) {
	/** ========================== INITIALIZATION ============================= */
	int n_pes = stats->nprocs(); //n_pes = numero de processadores, nmoves = numero de migrações
	double pe_loads[n_pes]; //carga total de cada Pe

	for (int i=0; i<n_pes; i++) {
		pe_loads[i] = 0;
	}

	for(int i = 0; i < stats->n_objs; i++) {
		LDObjData &oData = stats->objData[i];
		pe_loads[stats->from_proc[i]] += oData.wallTime; //calcula a carga total de cada Pe
	}


	  /** ============================= STRATEGY ================================ */
	int *highestLessProcessor = getMoreAndLessThanAverage(stats->nprocs(), pe_loads);
	int highestProcessor = highestLessProcessor[1]; // pega processador com a maior carga
	int lessProcessor = highestLessProcessor[0]; // pega processador com a menor carga
	taskCount=0;
	migrationCount=0;

	procuraProc(stats, pe_loads, highestProcessor, lessProcessor, n_pes);

	//verifica novamente qual é o core com maior e menor carga
	highestLessProcessor = getMoreAndLessThanAverage(stats->nprocs(), pe_loads);
	lessProcessor = highestLessProcessor[0];
	highestProcessor = highestLessProcessor[1];

	CkPrintf("maior[%d] = %f\n", highestProcessor, pe_loads[highestProcessor]);
	CkPrintf("menor[%d] = %f\n", lessProcessor, pe_loads[lessProcessor]);
	/** ============================== CLEANUP ================================ */
}
#include "SmartLB.def.h"
/*@}*/
