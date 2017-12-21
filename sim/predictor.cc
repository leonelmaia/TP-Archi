#include "predictor.h"

/////////////// STORAGE BUDGET JUSTIFICATION ////////////////

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

// Constructeur du prédicteur
PREDICTOR::PREDICTOR(char *prog, int argc, char *argv[])
{
   // La trace est tjs présente, et les arguments sont ceux que l'on désire
   if (argc != 2) {
      fprintf(stderr, "usage: %s <trace> pcbits countbits\n", prog);
      exit(-1);
   }

   uint32_t pcbits    = strtoul(argv[0], NULL, 0);
   uint32_t countbits = strtoul(argv[1], NULL, 0);
  
   nentries = (1 << pcbits);        // nombre d'entrées dans la table
   pcmask   = (nentries - 1);       // masque pour n'accéder qu'aux bits significatifs de PC
   countmax = (1 << countbits) - 1; // valeur max atteinte par le compteur à saturation
   table    = new uint32_t[nentries]();
   history = 0; // Question 2
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

bool PREDICTOR::GetPrediction(UINT64 PC)
{
   uint32_t v = table[PC & pcmask];
   return (v > (countmax / 2)) ? TAKEN : NOT_TAKEN;
}

bool PREDICTOR::GetPredictionGshare(UINT64 PC)
{
   uint32_t v = table[PC & pcmask] ^ (history & 0x00000011)
   return (v > (countmax / 2)) ? TAKEN : NOT_TAKEN;
}


bool PREDICTOR::GetPredictionGlobal(UINT64 PC)
{
  uint32_t v = (history & 0x00000011);
  return (v > (countmax / 2)) ? TAKEN : NOT_TAKEN;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void PREDICTOR::UpdatePredictor(UINT64 PC, OpType opType, bool resolveDir, bool predDir, UINT64 branchTarget)
{
   uint32_t v = table[PC & pcmask];
   table[PC & pcmask] = (resolveDir == TAKEN) ? SatIncrement(v, countmax) : SatDecrement(v);
}


void PREDICTOR::UpdatePredictorBimodal(UINT64 PC, OpType opType, bool resolveDir, bool predDir, UINT64 branchTarget)
{  
   uint32_t v = table[PC & pcmask];
   
	if (predDir == TAKEN){  
		if (resolveDir == TAKEN)table[PC & pcmask] = SatIncrement(v, countmax);
		else { 
			table[PC & pcmask] = SatDecrement(v);
			table[PC & pcmask] = SatDecrement(v);
		}	
	}

	if (predDir == NOT_TAKEN){  
		if (resolveDir == NOT_TAKEN)table[PC & pcmask] = SatDecrement(v);
		else { 
			table[PC & pcmask] = SatIncrement(v, countmax);
			table[PC & pcmask] = SatIncrement(v, countmax);		
		}	
	}

}
void PREDICTOR::UpdatePredictorGlobal(UINT64 PC, OpType opType, bool resolveDir, bool predDir, UINT64 branchTarget)
{
   uint32_t v = history;
   history = history << 1;
   history = (resolveDir == TAKEN) ? SatIncrement(history, countmax) : SatDecrement(history);
}

void PREDICTOR::UpdatePredictorGshare(UINT64 PC, OpType opType, bool resolveDir, bool predDir, UINT64 branchTarget)
{
   uint32_t v = table[PC & pcmask];
   uint32_t v = history;
   history = history << 1;
   history = (resolveDir == TAKEN) ? SatIncrement(history, countmax) : history = history;
   table[PC & pcmask] = (resolveDir == TAKEN) ? SatIncrement(v, countmax) : SatDecrement(v);
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void PREDICTOR::TrackOtherInst(UINT64 PC, OpType opType, bool branchDir, UINT64 branchTarget)
{
   // This function is called for instructions which are not
   // conditional branches, just in case someone decides to design
   // a predictor that uses information from such instructions.
   // We expect most contestants to leave this function untouched.
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


/***********************************************************/
