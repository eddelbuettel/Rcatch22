//
//  SB_TransitionMatrix.h
//
//
//  Created by Carl Henning Lubba on 23/09/2018.
//

#ifndef SB_TransitionMatrix_h
#define SB_TransitionMatrix_h

#include <stdio.h>
#include <R.h>
#define USE_RINTERNALS
#include <Rinternals.h>
#include <Rversion.h>

extern SEXP C_SB_TransitionMatrix_3ac_sumdiagcov(SEXP y[]);

#endif /* SB_TransitionMatrix_h */
