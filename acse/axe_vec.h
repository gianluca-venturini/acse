/*
 * Ettore Speziale
 * Politecnico di Milano, 2010
 * 
 * axe_vec.h
 * Formal Languages & Compilers Machine, 2009/2010
 * 
 */

#ifndef _AXE_EXPRESSIONS_H
#define _AXE_EXPRESSIONS_H

#include "axe_engine.h"

/* Generates instructions for vectorial operations. */
extern void handle_vec_op(t_program_infos *program,
        char *destID, char *sourceID1, char *sourceID2, int vec_op);

#endif 
