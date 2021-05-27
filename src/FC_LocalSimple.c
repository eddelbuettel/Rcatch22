#include <math.h>
#include <string.h>
#include <R.h>
#define USE_RINTERNALS
#include <Rinternals.h>
#include <Rversion.h>
#include "stats.h"
#include "CO_AutoCorr.h"

static void abs_diff(const double a[], const int size, double b[])
{
    for (int i = 1; i < size; i++) {
        b[i - 1] = fabs(a[i] - a[i - 1]);
    }
}

double fc_local_simple(const double y[], const int size, const int train_length)
{
    double * y1 = malloc((size - 1) * sizeof *y1);
    abs_diff(y, size, y1);
    double m = mean(y1, size - 1);
    free(y1);
    return m;
}

SEXP FC_LocalSimple_mean_tauresrat(SEXP y[], const int train_length)
{

    // we use int in loops
    if (xlength(y) >= INT_MAX) {
        error("y was a long vector, not supported.");
    }
    int size = xlength(y);
    // Don't accept integer vectors -- will be wrong pointer below
    if (TYPEOF(y) != REALSXP) {
        error("y was not a REAL vector.");
    }
    const double * x = REAL(y);
    // NaN check
    for(int i = 0; i < size; i++)
    {
        if(ISNAN(x[i]))
        {
            return ScalarReal(NA_REAL);
        }
    }

    double * res = malloc((size - train_length) * sizeof *res);

    for (int i = 0; i < size - train_length; i++)
    {
        double yest = 0;
        for (int j = 0; j < train_length; j++)
        {
            yest += y[i+j];

        }
        yest /= train_length;

        res[i] = y[i+train_length] - yest;
    }

    double resAC1stZ = co_firstzero(res, size - train_length, size - train_length);
    double yAC1stZ = co_firstzero(y, size, size);
    double output = resAC1stZ/yAC1stZ;

    free(res);
    return ScalarReal(output);

}

SEXP FC_LocalSimple_mean_stderr(SEXP y[], const int train_length)
{
    // we use int in loops
    if (xlength(y) >= INT_MAX) {
        error("y was a long vector, not supported.");
    }
    int size = xlength(y);
    // Don't accept integer vectors -- will be wrong pointer below
    if (TYPEOF(y) != REALSXP) {
        error("y was not a REAL vector.");
    }
    const double * x = REAL(y);
    // NaN check
    for(int i = 0; i < size; i++)
    {
        if(ISNAN(x[i]))
        {
            return ScalarReal(NA_REAL);
        }
    }

    double * res = malloc((size - train_length) * sizeof *res);

    for (int i = 0; i < size - train_length; i++)
    {
        double yest = 0;
        for (int j = 0; j < train_length; j++)
        {
            yest += y[i+j];

        }
        yest /= train_length;

        res[i] = y[i+train_length] - yest;
    }

    double output = stddev(res, size - train_length);

    free(res);
    return ScalarReal(output);

}

SEXP C_FC_LocalSimple_mean3_stderr(SEXP y[])
{
    return FC_LocalSimple_mean_stderr(y, 3);
}

SEXP C_FC_LocalSimple_mean1_tauresrat(SEXP y[]){
    return FC_LocalSimple_mean_tauresrat(y, 1);
}

double FC_LocalSimple_mean_taures(const double y[], const int size, const int train_length)
{
    double * res = malloc((size - train_length) * sizeof *res);

    // first z-score
    // no, assume ts is z-scored!!
    //zscore_norm(y, size);

    for (int i = 0; i < size - train_length; i++)
    {
        double yest = 0;
        for (int j = 0; j < train_length; j++)
        {
            yest += y[i+j];

        }
        yest /= train_length;

        res[i] = y[i+train_length] - yest;
    }

    int output = co_firstzero(res, size - train_length, size - train_length);

    free(res);
    return output;

}

double FC_LocalSimple_lfit_taures(const double y[], const int size)
{
    // set tau from first AC zero crossing
    int train_length = co_firstzero(y, size, size);

    double * xReg = malloc(train_length * sizeof * xReg);
    // double * yReg = malloc(train_length * sizeof * yReg);
    for(int i = 1; i < train_length+1; i++)
    {
        xReg[i-1] = i;
    }

    double * res = malloc((size - train_length) * sizeof *res);

    double m = 0.0, b = 0.0;

    for (int i = 0; i < size - train_length; i++)
    {
        linreg(train_length, xReg, y+i, &m, &b);

        // fprintf(stdout, "i=%i, m=%f, b=%f\n", i, m, b);

        res[i] = y[i+train_length] - (m * (train_length+1) + b);
    }

    int output = co_firstzero(res, size - train_length, size - train_length);

    free(res);
    free(xReg);
    // free(yReg);

    return output;

}
