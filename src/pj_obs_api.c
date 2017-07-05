/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implement a (currently minimalistic) proj API based primarily
 *           on the PJ_OBS generic geodetic data type.
 *
 *           proj thread contexts have not seen widespread use, so one of the
 *           intentions with this new API is to make them less visible on the
 *           API surface: Contexts do not have a life by themselves, they are
 *           visible only through their associated PJs, and the number of
 *           functions supporting them is limited.
 *
 * Author:   Thomas Knudsen,  thokn@sdfe.dk,  2016-06-09/2016-11-06
 *
 ******************************************************************************
 * Copyright (c) 2016, Thomas Knudsen/SDFE
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
 *****************************************************************************/
#define PJ_OBS_API_C
#include <proj.h>
#include <projects.h>
#include <geodesic.h>

#include <stddef.h>
#include <errno.h>


static PJ_OBS pj_fwdobs (PJ_OBS obs, PJ *P);
static PJ_OBS pj_invobs (PJ_OBS obs, PJ *P);
static PJ_COORD pj_fwdcoord (PJ_COORD coo, PJ *P);
static PJ_COORD pj_invcoord (PJ_COORD coo, PJ *P);


/* Used for zero-initializing new objects */
const PJ_COORD proj_coord_null = {{0, 0, 0, 0}};
const PJ_OBS   proj_obs_null = {
    {{0, 0, 0, 0}},
    {{0, 0, 0}},
    0, 0
};

/* Magic object signaling proj system shutdown mode to routines taking a PJ * arg */
const PJ *proj_shutdown = (PJ *) &proj_shutdown;

PJ_COORD proj_coord (double x, double y, double z, double t) {
    PJ_COORD res;
    res.v[0] = x;
    res.v[1] = y;
    res.v[2] = z;
    res.v[3] = t;
    return res;
}

/* Geodesic distance between two points with angular 2D coordinates */
double proj_lp_dist (PJ *P, LP a, LP b) {
    double s12, azi1, azi2;
    /* Note: the geodesic code takes arguments in degrees */
    geod_inverse (P->geod, PJ_TODEG(a.phi), PJ_TODEG(a.lam), PJ_TODEG(b.phi), PJ_TODEG(b.lam), &s12, &azi1, &azi2);
    return s12;
}

/* Euclidean distance between two points with linear 2D coordinates */
double proj_xy_dist (XY a, XY b) {
    return hypot (a.x - b.x, a.y - b.y);
}

/* Euclidean distance between two points with linear 3D coordinates */
double proj_xyz_dist (XYZ a, XYZ b) {
    return hypot (hypot (a.x - b.x, a.y - b.y), a.z - b.z);
}



/* Measure numerical deviation after n roundtrips fwd-inv (or inv-fwd) */
double proj_roundtrip (PJ *P, enum proj_direction direction, int n, PJ_OBS obs) {
    int i;
    PJ_OBS o, u;

    if (0==P)
        return HUGE_VAL;

    if (n < 1) {
        proj_errno_set (P, EINVAL);
        return HUGE_VAL;
    }

    o.coo = obs.coo;

    for (i = 0;  i < n;  i++) {
        switch (direction) {
            case PJ_FWD:
                u  =  pj_fwdobs (o, P);
                o  =  pj_invobs (u, P);
                break;
            case PJ_INV:
                u  =  pj_invobs (o, P);
                o  =  pj_fwdobs (u, P);
                break;
            default:
                proj_errno_set (P, EINVAL);
                return HUGE_VAL;
        }
    }

    return proj_xyz_dist (o.coo.xyz, obs.coo.xyz);
}




/* Work around non-constness of MSVC HUGE_VAL by providing functions rather than constants */
PJ_COORD proj_coord_error (void) {
    PJ_COORD c;
    c.v[0] = c.v[1] = c.v[2] = c.v[3] = HUGE_VAL;
    return c;
}

PJ_OBS proj_obs_error (void) {
    PJ_OBS obs;
    obs.coo = proj_coord_error ();
    obs.anc.v[0] = obs.anc.v[1] = obs.anc.v[2] = HUGE_VAL;
    obs.id = obs.flags = 0;
    return obs;
}



static PJ_OBS pj_fwdobs (PJ_OBS obs, PJ *P) {
    if (0!=P->fwdobs) {
        obs  =  P->fwdobs (obs, P);
        return obs;
    }
    if (0!=P->fwd3d) {
        obs.coo.xyz  =  pj_fwd3d (obs.coo.lpz, P);
        return obs;
    }
    if (0!=P->fwd) {
        obs.coo.xy  =  pj_fwd (obs.coo.lp, P);
        return obs;
    }
    proj_errno_set (P, EINVAL);
    return proj_obs_error ();
}


static PJ_OBS pj_invobs (PJ_OBS obs, PJ *P) {
    if (0!=P->invobs) {
        obs  =  P->invobs (obs, P);
        return obs;
    }
    if (0!=P->inv3d) {
        obs.coo.lpz  =  pj_inv3d (obs.coo.xyz, P);
        return obs;
    }
    if (0!=P->inv) {
        obs.coo.lp  =  pj_inv (obs.coo.xy, P);
        return obs;
    }
    proj_errno_set (P, EINVAL);
    return proj_obs_error ();
}



static PJ_COORD pj_fwdcoord (PJ_COORD coo, PJ *P) {
    if (0!=P->fwdcoord)
        return P->fwdcoord (coo, P);
    if (0!=P->fwdobs) {
        PJ_OBS obs = proj_obs_null;
        obs.coo = coo;
        obs  =  P->fwdobs (obs, P);
        return obs.coo;
    }
    if (0!=P->fwd3d) {
        coo.xyz  =  pj_fwd3d (coo.lpz, P);
        return coo;
    }
    if (0!=P->fwd) {
        coo.xy  =  pj_fwd (coo.lp, P);
        return coo;
    }
    proj_errno_set (P, EINVAL);
    return proj_coord_error ();
}


static PJ_COORD pj_invcoord (PJ_COORD coo, PJ *P) {
    if (0!=P->invcoord)
        return P->invcoord (coo, P);
    if (0!=P->invobs) {
        PJ_OBS obs = proj_obs_null;
        obs.coo = coo;
        obs  =  P->invobs (obs, P);
        return obs.coo;
    }
    if (0!=P->inv3d) {
        coo.lpz  =  pj_inv3d (coo.xyz, P);
        return coo;
    }
    if (0!=P->inv) {
        coo.lp  =  pj_inv (coo.xy, P);
        return coo;
    }
    proj_errno_set (P, EINVAL);
    return proj_coord_error ();
}


/* Apply the transformation P to the coordinate coo */
PJ_OBS proj_trans_obs (PJ *P, enum proj_direction direction, PJ_OBS obs) {
    if (0==P)
        return obs;

    switch (direction) {
        case PJ_FWD:
            return pj_fwdobs (obs, P);
        case PJ_INV:
            return  pj_invobs (obs, P);
        case PJ_IDENT:
            return obs;
        default:
            break;
    }

    proj_errno_set (P, EINVAL);
    return proj_obs_error ();
}



/* Apply the transformation P to the coordinate coo */
PJ_COORD proj_trans_coord (PJ *P, enum proj_direction direction, PJ_COORD coo) {
    if (0==P)
        return coo;

    switch (direction) {
        case PJ_FWD:
            return pj_fwdcoord (coo, P);
        case PJ_INV:
            return  pj_invcoord (coo, P);
        case PJ_IDENT:
            return coo;
        default:
            break;
    }

    proj_errno_set (P, EINVAL);
    return proj_coord_error ();
}



/*************************************************************************************/
size_t proj_transform (
    PJ *P,
    enum proj_direction direction,
    double *x, size_t sx, size_t nx,
    double *y, size_t sy, size_t ny,
    double *z, size_t sz, size_t nz,
    double *t, size_t st, size_t nt
) {
/**************************************************************************************

    Transform a series of coordinates, where the individual coordinate dimension
    may be represented by an array that is either

        1. fully populated
        2. a null pointer and/or a length of zero, which will be treated as a
           fully populated array of zeroes
        3. of length one, i.e. a constant, which will be treated as a fully
           populated array of that constant value

    The strides, sx, sy, sz, st, represent the step length, in bytes, between
    consecutive elements of the corresponding array. This makes it possible for
    proj_transform to handle transformation of a large class of application
    specific data structures, without necessarily understanding the data structure
    format, as in:

        typedef struct {double x, y; int quality_level; char surveyor_name[134];} XYQS;
        XYQS survey[345];
        double height = 23.45;
        PJ *P = {...};
        size_t stride = sizeof (XYQS);
        ...
        proj_transform (
            P, PJ_INV, sizeof(XYQS),
            &(survey[0].x), stride, 345,  (*  We have 345 eastings  *)
            &(survey[0].y), stride, 345,  (*  ...and 345 northings. *)
            &height, 1,                   (*  The height is the constant  23.45 m *)
            0, 0                          (*  and the time is the constant 0.00 s *)
        );

    This is similar to the inner workings of the pj_transform function, but the
    stride functionality has been generalized to work for any size of basic unit,
    not just a fixed number of doubles.

    In most cases, the stride will be identical for x, y,z, and t, since they will
    typically be either individual arrays (stride = sizeof(double)), or strided
    views into an array of application specific data structures (stride = sizeof (...)).
    
    But in order to support cases where x, y, z, and t come from heterogeneous
    sources, individual strides, sx, sy, sz, st, are used.

    Caveat: Since proj_transform does its work *in place*, this means that even the
    supposedly constants (i.e. length 1 arrays) will return from the call in altered
    state. Hence, remember to reinitialize between repeated calls.

    Return value: Number of transformations completed.

**************************************************************************************/
    PJ_COORD coord = proj_coord_null;
    size_t i, nmin;
    double null_broadcast = 0;
    if (0==P)
        return 0;

    /* ignore lengths of null arrays */
    if (0==x) nx = 0;
    if (0==y) ny = 0;
    if (0==z) nz = 0;
    if (0==t) nt = 0;

    /* and make the nullities point to some real world memory for broadcasting nulls */
    if (0==nx) x = &null_broadcast;
    if (0==ny) y = &null_broadcast;
    if (0==nz) z = &null_broadcast;
    if (0==nt) t = &null_broadcast;
    
    /* nothing to do? */
    if (0==nx+ny+nz+nt)
        return 0;

    /* arrays of length 1 are constants, which we broadcast along the longer arrays */
    /* so we need to find the length of the shortest non-unity array to figure out  */
    /* how many coordinate pairs we must transform */
    nmin = (nx > 1)? nx: (ny > 1)? ny: (nz > 1)? nz: (nt > 1)? nt: 1;
    if ((nx > 1) && (nx < nmin))  nmin = nx;
    if ((ny > 1) && (ny < nmin))  nmin = ny;
    if ((nz > 1) && (nz < nmin))  nmin = nz;
    if ((nt > 1) && (nt < nmin))  nmin = nt;

    /* Check validity of direction flag */
    switch (direction) {
        case PJ_FWD:
        case PJ_INV:
            break;
        case PJ_IDENT:
            return nmin;
        default:
            proj_errno_set (P, EINVAL);
            return 0;
    }

    /* Arrays of length==0 are broadcast as the constant 0               */
    /* Arrays of length==1 are broadcast as their single value           */
    /* Arrays of length >1 are iterated over (for the first nmin values) */
    /* The slightly convolved incremental indexing is used due           */
    /* to the stride, which may be any size supported by the platform    */
    for (i = 0;  i < nmin;  i++) {
        coord.xyzt.x = *x;
        coord.xyzt.y = *y;
        coord.xyzt.z = *z;
        coord.xyzt.t = *t;

        if (PJ_FWD==direction)
            coord = pj_fwdcoord (coord, P);
        else
            coord = pj_invcoord (coord, P);

        /* in all full length cases, we overwrite the input with the output */
        if (nx > 1)  {
            *x = coord.xyzt.x;
            x =  (double *) ( ((char *) x) + sx);
        }
        if (ny > 1)  {
            *y = coord.xyzt.y;
            y =  (double *) ( ((char *) y) + sy);
        }
        if (nz > 1)  {
            *z = coord.xyzt.z;
            z =  (double *) ( ((char *) z) + sz);
        }
        if (nt > 1)  {
            *t = coord.xyzt.t;
            t =  (double *) ( ((char *) t) + st);
        }
    }
    /* Last time around, we update the length 1 cases with their transformed alter egos */
    /* ... or would we rather not? Then what about the nmin==1 case?                    */
    /* perhaps signalling the non-array case by setting all strides to 0?               */
    if (nx==1)
        *x = coord.xyzt.x;
    if (ny==1)
        *y = coord.xyzt.y;
    if (nz==1)
        *z = coord.xyzt.z;
    if (nt==1)
        *t = coord.xyzt.t;
    
    return i;
}





PJ *proj_create (PJ_CONTEXT *ctx, const char *definition) {
    if (0==ctx)
        ctx = pj_get_default_ctx ();
    return pj_init_plus_ctx (ctx, definition);
}

PJ *proj_create_argv (PJ_CONTEXT *ctx, int argc, char **argv) {
    if (0==ctx)
        ctx = pj_get_default_ctx ();
    return pj_init_ctx (ctx, argc, argv);
}

PJ *proj_destroy (PJ *P) {
    pj_free (P);
    return 0;
}

/* For now, if PJ itself is clean, we return the thread local error level. */
/* This may change as OBS_API error reporting matures */
int proj_errno (PJ *P) {
    if (0==P)
        return pj_ctx_get_errno (pj_get_default_ctx ());
    if (0 != P->last_errno)
        return P->last_errno;
    return pj_ctx_get_errno (pj_get_ctx (P));
}

/*****************************************************************************/
void proj_errno_set (PJ *P, int err) {
/******************************************************************************
    Sets errno in the PJ, and bubbles it up to the context and pj_errno levels
    through the low level pj_ctx interface.

    Has a synonym defined in proj.h: proj_errno_restore (P, err), but perhaps
    the naming should be the other way round, since the errno reset/restore
    paradigm will probably be the primary mode of operation.

    See usage example under proj_errno_reset ()
******************************************************************************/
    if (0==P) {
        errno = EINVAL;
        return;
    }

    /* Use proj_errno_reset to explicitly clear the error status */
    if (0==err)
        return;

    /* set local error level */
    P->last_errno = err;
    /* and let it bubble up */
    proj_context_errno_set (pj_get_ctx (P), err);
    errno = err;
    return;
}


/*****************************************************************************/
int proj_errno_reset (PJ *P) {
/******************************************************************************
    Clears errno in the PJ, and bubbles it up to the context and
    pj_errno levels through the low level pj_ctx interface.
    
    Returns the previous value of the errno, for convenient reset/restore
    operations:

    void foo (PJ *P) {
        int last_errno = proj_errno_reset (P);
        
        do_something_with_P (P);

        (* failure - keep latest error status *)
        if (proj_errno(P))
            return;
        (* restore previous error status *)
        proj_errno_restore (P, last_errno);
        return;
    }
******************************************************************************/
    int last_errno;
    if (0==P) {
        errno = EINVAL;
        return EINVAL;
    }
    last_errno = proj_errno (P);

    /* set local error level */
    P->last_errno = 0;
    /* and let it bubble up */
    pj_ctx_set_errno (pj_get_ctx (P), 0);
    errno = 0;
    return last_errno;
}



/* Create a new context - or provide a pointer to the default context */
PJ_CONTEXT *proj_context_create (int multithreaded) {
    /* Single threaded */
    if (0==multithreaded)
        return pj_get_default_ctx ();
    return pj_ctx_alloc ();
}


/* Move P to a new context - or to the default context i 0 is specified */
void proj_context_set (PJ *P, PJ_CONTEXT *ctx) {
    if (0==ctx)
        ctx = pj_get_default_ctx ();
    pj_set_ctx (P, ctx);
    return;
}

void proj_context_inherit (PJ *parent, PJ *child) {
    if (0==parent)
        pj_set_ctx (child, pj_get_default_ctx());
    else
        pj_set_ctx (child, pj_get_ctx(parent));
    return;
}


void proj_context_destroy (PJ_CONTEXT *ctx) {
    if (0==ctx)
        return;

    /* Trying to free the default context is a no-op (since it is statically allocated) */
    if (pj_get_default_ctx ()==ctx)
        return;

    pj_ctx_free (ctx);
}






/* stuff below is *not* considered API, and will be moved to an "internal plumbing toolset" */
#include <stdarg.h>




void proj_context_errno_set (PJ_CONTEXT *ctx, int err) {
    if (0==ctx)
        ctx = pj_get_default_ctx();
    pj_ctx_set_errno (ctx, err);
    return;
}



/* Set logging level 0-3. Higher number means more debug info. 0 turns it off */
enum proj_log_level proj_log_level (PJ_CONTEXT *ctx, enum proj_log_level log_level) {
    enum proj_log_level previous;
    if (0==ctx)
        ctx = pj_get_default_ctx();
    if (0==ctx)
        return PJ_LOG_TELL;
    previous = ctx->debug_level;
    if (PJ_LOG_TELL==log_level)
        return previous;
    ctx->debug_level = log_level;
    return previous;
}



/*  logging  */

/* pj_vlog resides in pj_log.c and relates to pj_log as vsprintf relates to sprintf */
void pj_vlog( projCtx ctx, int level, const char *fmt, va_list args );

void proj_log_error (PJ *P, const char *fmt, ...) {
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_ERROR , fmt, args);
    va_end( args );
}

void proj_log_debug (PJ *P, const char *fmt, ...) {
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_DEBUG_MAJOR , fmt, args);
    va_end( args );
}

void proj_log_trace (PJ *P, const char *fmt, ...) {
    va_list args;
    va_start( args, fmt );
    pj_vlog (pj_get_ctx (P), PJ_LOG_DEBUG_MINOR , fmt, args);
    va_end( args );
}

/* Put a new logging function into P's context. The opaque object app_data is passed as first arg at each call to the logger */
void proj_log_func (PJ_CONTEXT *ctx, void *app_data, PJ_LOG_FUNCTION log) {
    if (0==ctx)
        pj_get_default_ctx ();
    if (0==ctx)
        return;
    ctx->app_data = app_data;
    if (0!=log)
        ctx->logger = log;
}
