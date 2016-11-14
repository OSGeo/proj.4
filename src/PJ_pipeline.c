/***********************************************************************

               Transformation pipeline manager

                 Thomas Knudsen, 2016-05-20/2016-11-14

************************************************************************

**The Problem**

**The Solution**

**The hack...:**

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-20

************************************************************************
* Copyright (c) 2016, Thomas Knudsen / SDFE
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*
***********************************************************************/

#define PJ_LIB__
#include <proj.h>
#include <projects.h>

#include <assert.h>
#include <stddef.h>
#include <errno.h>
PROJ_HEAD(pipeline,         "Transformation pipeline manager");

/* Projection specific elements for the PJ object */
#define PIPELINE_STACK_SIZE 100
struct pj_opaque {
    int reversible;
    int steps;
    int depth;
    int verbose;
    int *reverse_step;
    int *omit_forward;
    int *omit_inverse;
    PJ_OBS stack[PIPELINE_STACK_SIZE];
};


static PJ_OBS pipeline_forward_obs (PJ_OBS, PJ *P);
static PJ_OBS pipeline_reverse_obs (PJ_OBS, PJ *P);
static XYZ    pipeline_forward_3d (LPZ lpz, PJ *P);
static LPZ    pipeline_reverse_3d (XYZ xyz, PJ *P);
static XY     pipeline_forward (LP lpz, PJ *P);
static LP     pipeline_reverse (XY xyz, PJ *P);



/********************************************************************

                  ISOMORPHIC TRANSFORMATIONS

*********************************************************************

    In this context, an isomorphic transformation is a proj PJ
    object returning the same kind of coordinates that it
    receives, i.e. a transformation from angular to angular or
    linear to linear coordinates.

    The degrees-to-radians operation is an example of the former,
    while the latter is typical for most of the datum shift
    operations used in geodesy, e.g. the Helmert 7-parameter shift.

    Isomorphic transformations trips up the pj_inv/pj_fwd
    functions, which try to check input sanity and scale output to
    internal proj units, under the assumption that if input is of
    angular type, then output must be of linear (or vice versa).

    Hence, to avoid having pj_inv/pj_fwd stomping on output (or
    choking on input), we need a way to tell them that they should
    accept whatever is being handed to them.

    The P->left and P->right elements indicate isomorphism.

    For classic proj style projections, they both have the
    value PJ_IO_UNITS_CLASSIC (default initialization), indicating
    that the forward driver expects angular input coordinates, and
    provides linear output coordinates.

    Newer projections may set P->left and P->right to either
    PJ_IO_UNITS_METERS, PJ_IO_UNITS_RADIANS or PJ_IO_UNITS_ANY,
    to indicate their I/O style.

    For the forward driver, left indicates input coordinate
    type, while right indicates output coordinate type.

    For the inverse driver, left indicates output coordinate
    type, while right indicates input coordinate type.

*********************************************************************/


/********************************************************************/
int pj_is_isomorphic (PJ *P) {
/********************************************************************/
    if (P->left==PJ_IO_UNITS_CLASSIC)
        return 0;
    if (P->left==P->right)
        return 1;
    return 0;
}




static PJ_OBS pipeline_forward_obs (PJ_OBS point, PJ *P) {
    int i, first_step, last_step, incr;

    first_step = 1;
    last_step  = P->opaque->steps + 1;
    incr  = 1;

    for (i = first_step; i != last_step; i += incr) {
        if (P->opaque->omit_forward[i])
            continue;
        if (P->opaque->reverse_step[i])
            point = pj_apply (P + i, PJ_INV, point);
        else
            point = pj_apply (P + i, PJ_FWD, point);
        if (P->opaque->depth < PIPELINE_STACK_SIZE)
            P->opaque->stack[P->opaque->depth++] = point;
    }

    P->opaque->depth = 0;    /* Clear the stack */
    return point;
}

static PJ_OBS pipeline_reverse_obs (PJ_OBS point, PJ *P) {
    int i, first_step, last_step, incr;

    first_step = P->opaque->steps;
    last_step  =  0;
    incr  = -1;

    for (i = first_step; i != last_step; i += incr) {
        if (P->opaque->omit_inverse[i])
            continue;
        if (P->opaque->reverse_step[i])
            point = pj_apply (P + i, PJ_FWD, point);
        else
            point = pj_apply (P + i, PJ_INV, point);
        if (P->opaque->depth < PIPELINE_STACK_SIZE)
            P->opaque->stack[P->opaque->depth++] = point;
    }

    P->opaque->depth = 0;    /* Clear the stack */
    return point;
}

static XYZ pipeline_forward_3d (LPZ lpz, PJ *P) {
    PJ_OBS point = pj_obs_null;
    point.coo.lpz = lpz;
    point = pipeline_forward_obs (point, P);
    return point.coo.xyz;
}

static LPZ pipeline_reverse_3d (XYZ xyz, PJ *P) {
    PJ_OBS point = pj_obs_null;
    point.coo.xyz = xyz;
    point = pipeline_reverse_obs (point, P);
    return point.coo.lpz;
}

static XY pipeline_forward (LP lp, PJ *P) {
    PJ_OBS point = pj_obs_null;
    point.coo.lp = lp;
    point = pipeline_forward_obs (point, P);
    return point.coo.xy;
}

static LP pipeline_reverse (XY xy, PJ *P) {
    PJ_OBS point = pj_obs_null;
    point.coo.xy = xy;
    point = pipeline_reverse_obs (point, P);
    return point.coo.lp;
}

static void freeup(PJ *P) {                                    /* Destructor */
    if (P==0)
        return;
    /* Projection specific deallocation goes here */
    pj_dealloc (P->opaque);
    pj_dealloc (P);
    return;
}


static void *pipeline_freeup (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;

    pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque->reverse_step);
    pj_dealloc (P->opaque->omit_forward);
    pj_dealloc (P->opaque->omit_inverse);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/* Adapts pipeline_freeup to the format defined for the PJ object */
static void pipeline_freeup_wrapper (PJ *P) {
    pipeline_freeup (P, 0);
    return;
}

/* Indicator function with just enough dummy functionality to silence compiler warnings */
static XY pipe_end_indicator (LP lp, PJ *P) {
    XY xy;

    xy.x = lp.lam;
    xy.y = lp.phi;
    if (P)
        return xy;
    xy.x++;
    return xy;
}


static PJ *pj_create_pipeline (PJ *P, size_t steps) {
    PJ *pipeline;
    size_t i;

    /*  Room for the pipeline: An array of PJ (not of PJ *) */
    pipeline = pj_calloc (steps + 2, sizeof(PJ));

    P->opaque->steps = steps;
    P->opaque->reverse_step =  pj_calloc (steps + 2, sizeof(int));
    if (0==P->opaque->reverse_step)
        return pipeline_freeup (P, ENOMEM);

    P->opaque->omit_forward =  pj_calloc (steps + 2, sizeof(int));
    if (0==P->opaque->omit_forward)
        return pipeline_freeup (P, ENOMEM);

    P->opaque->omit_inverse =  pj_calloc (steps + 2, sizeof(int));
    if (0==P->opaque->omit_inverse)
        return pipeline_freeup (P, ENOMEM);

    /* First element is the pipeline manager herself */
    *pipeline = *P;

    /* Fill the rest of the pipeline with pipe_ends */
    P->fwd = pipe_end_indicator;
    for (i = 1;  i < steps + 2;  i++)
        pipeline[i] = *P;

    /* This is a shallow copy, so we just release P, without calling P->pfree */
    /* The actual deallocation will be done by pipeline_freeup */
    pj_dealloc (P);

    return pipeline;
}


static void pj_push_to_pipeline (PJ *pipeline, PJ *P) {
    while (pipeline->fwd != pipe_end_indicator)
        pipeline++;
    *pipeline = *P;
    pj_dealloc (P);
}

/* count the number of args in pipeline definition */
size_t argc_params (paralist *params) {
    size_t argc = 0;
    for (; params != 0; params = params->next)
        argc++;
    return ++argc;  /* one extra for the sentinel */
}

/* Sentinel for argument list */
static char argv_sentinel[5] = "step";

/* turn paralist int argc/argv style argument list */
char **argv_params (paralist *params) {
    char **argv;
    size_t argc = 0;
    argv = pj_calloc (argc_params (params), sizeof (char *));
    if (0==argv)
        return 0;
    for (; params != 0; params = params->next) {
        argv[argc++] = params->param;
        puts (argv[argc - 1]);
    }
    argv[argc++] = argv_sentinel;
    return argv;
}


PJ *PROJECTION(pipeline) {

    int i, nsteps = 0, argc;
    int i_pipeline = -1, i_first_step = -1, i_next_step, i_current_step;
    char **argv, **current_argv;

    PJ     *pipeline  = 0;

    P->fwdobs =  pipeline_forward_obs;
    P->invobs =  pipeline_reverse_obs;
    P->fwd3d  =  pipeline_forward_3d;
    P->inv3d  =  pipeline_reverse_3d;
    P->fwd    =  pipeline_forward;
    P->inv    =  pipeline_reverse;
    P->pfree  =  pipeline_freeup_wrapper;

    P->opaque = pj_calloc (1, sizeof(struct pj_opaque));
    if (0==P->opaque)
        return 0;

    argc = argc_params (P->params);
    argv = argv_params (P->params);
    if (0==argv)
        return pipeline_freeup (P, ENOMEM);
    current_argv = argv_params (P->params);
    if (0==current_argv)
        return pipeline_freeup (P, ENOMEM);

    /* Do some syntactic sanity checking */
    for (i = 0, i_next_step = argc - 1;  i < argc;  i++) {
        if (0==strcmp ("step", argv[i])) {
            if (-1==i_pipeline)
                return pipeline_freeup (P, 51); /* ERROR: +step before +proj=pipeline */
            nsteps++;
            switch (nsteps) {
                case 1:
                    i_first_step = i;
                    break;
                case 2:
                    if (i_next_step == argc - 1)
                        i_next_step = i;
                    break;
            }
        }
        if (0==strcmp ("proj=pipeline", argv[i])) {
            if (-1 != i_pipeline)
                return pipeline_freeup (P, 52); /* ERROR: nested pipelines */
            i_pipeline = i;
        }
    }
    if (-1==i_pipeline)
        return pipeline_freeup (P, 50); /* ERROR: no pipeline def */
    if (0==nsteps)
        return pipeline_freeup (P, 50); /* ERROR: no pipeline def */

    /*  Room for the pipeline: An array of PJ (note: NOT of PJ *) */
    pipeline = pj_create_pipeline (P, nsteps);

    /* Now loop over all steps, building a new set of arguments for each init */
    for (i_current_step = i_first_step, i = 0;  i < nsteps;  i++) {
        int j;
        int  current_argc = 0;
        PJ     *next_step = 0;

        /* Build a set of initialization args for the current step */

        /* First add the step specific args */
        for (j = i_current_step + 1;  0 != strcmp ("step", argv[j]); j++)
            current_argv[current_argc++] = argv[j];

        i_current_step = j;

        /* Then add the global args */
        for (j = i_pipeline + 1;  0 != strcmp ("step", argv[j]); j++)
            current_argv[current_argc++] = argv[j];

        /* Finally handle non-symmetric steps and inverted steps */
        for (j = 0;  j < current_argc; j++) {
            if (0==strcmp("omit_inv", current_argv[j])) {
                pipeline->opaque->omit_inverse[i+1] = 1;
                pipeline->opaque->omit_forward[i+1] = 0;
            }
            if (0==strcmp("omit_fwd", current_argv[j])) {
                pipeline->opaque->omit_inverse[i+1] = 0;
                pipeline->opaque->omit_forward[i+1] = 1;
            }
            if (0==strcmp("inv", current_argv[j]))
                pipeline->opaque->reverse_step[i+1] = 1;
        }

        next_step = pj_init (current_argc, current_argv);
        if (0==next_step)
            return pipeline_freeup (P, 50); /* ERROR: bad pipeline def */
        pj_push_to_pipeline (pipeline, next_step);
    }

    return pipeline;
}

/* selftest stub */
int pj_pipeline_selftest (void) {return 0;}
