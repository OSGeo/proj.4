/* general forward projection */
#define PJ_LIB__
#include <projects.h>
#include <PJ_pipeline.h>
#include <errno.h>
# define EPS 1.0e-12


XYZ pj_fwd3d(LPZ lpz, PJ *P) {
	XYZ xyz;
	XYZ xyz_error = {HUGE_VAL, HUGE_VAL, HUGE_VAL};
	double t;

	if (0==P->fwd3d)
    	return xyz_error;

    if (pj_is_isomorphic (P))
	    return  (*P->fwd3d)(lpz, P);

	/* check for forward and latitude or longitude overange */
<<<<<<< HEAD
	if ((t = fabs(lpz.phi)-HALFPI) > EPS || fabs(lpz.lam) > 10.) {
		pj_ctx_set_errno( P->ctx, -14);
		return xyz_error;
=======
	if ((t = fabs(lpz.phi)-M_HALFPI) > EPS || fabs(lpz.lam) > 10.) {
		xyz.x = xyz.y = xyz.z = HUGE_VAL;
		pj_ctx_set_errno( P->ctx, -14);
	} else { /* proceed with projection */
                P->ctx->last_errno = 0;
                pj_errno = 0;
                errno = 0;

		if (fabs(t) <= EPS)
			lpz.phi = lpz.phi < 0. ? -M_HALFPI : M_HALFPI;
		else if (P->geoc)   /* Maybe redundant and never used. */
			lpz.phi = atan(P->rone_es * tan(lpz.phi));
		lpz.lam -= P->lam0;	/* compute del lp.lam */
		if (!P->over)
			lpz.lam = adjlon(lpz.lam); /* adjust del longitude */

                /* Check for NULL pointer */
                if (P->fwd3d != NULL)
                {
		    xyz = (*P->fwd3d)(lpz, P); /* project */
		    if ( P->ctx->last_errno )
			xyz.x = xyz.y = xyz.z = HUGE_VAL;
		    /* adjust for major axis and easting/northings */
		    else {
			xyz.x = P->fr_meter * (P->a * xyz.x + P->x0);
			xyz.y = P->fr_meter * (P->a * xyz.y + P->y0);
                        /* z is not scaled since this handled by vto_meter outside */
		    }
                }
                else
                {
                    xyz.x = xyz.y = xyz.z = HUGE_VAL;
                }
>>>>>>> refs/remotes/OSGeo/master
	}

	P->ctx->last_errno = 0;
	/* Not sure these should be zeroed out too? */
    pj_errno = 0;
    errno = 0;

	if (fabs(t) <= EPS)
		lpz.phi = lpz.phi < 0. ? -HALFPI : HALFPI;
	else if (P->geoc)   /* Maybe redundant and never used. */
		lpz.phi = atan(P->rone_es * tan(lpz.phi));

	lpz.lam -= P->lam0;	/* compute del lp.lam */
	if (!P->over)
		lpz.lam = adjlon (lpz.lam); /* adjust del longitude */

    xyz = (*P->fwd3d)(lpz, P); /* project */

    if ( P->ctx->last_errno )
		return xyz_error;

    /* adjust for major axis and easting/northings */
    xyz.x = P->fr_meter * (P->a * xyz.x + P->x0);
    xyz.y = P->fr_meter * (P->a * xyz.y + P->y0);
    /* z is not scaled since this handled by vto_meter outside */

	return xyz;
}
