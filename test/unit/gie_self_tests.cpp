/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Test
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2017, Thomas Knudsen
 * Copyright (c) 2017, SDFE
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
 ****************************************************************************/

#include "gtest_include.h"

// PROJ include order is sensitive
// clang-format off
#include "proj.h"
#include "proj_internal.h"
// clang-format on

#include <cmath>
#include <string>

namespace {

// ---------------------------------------------------------------------------

TEST(gie, cart_selftest) {
    PJ_CONTEXT *ctx;
    PJ *P;
    PJ_COORD a, b, obs[2];
    PJ_COORD coord[3];

    size_t n, sz;
    double dist, h, t;
    const char *const args[3] = {"proj=utm", "zone=32", "ellps=GRS80"};
    char arg[50] = {"+proj=utm; +zone=32; +ellps=GRS80"};

    /* An utm projection on the GRS80 ellipsoid */
    P = proj_create(PJ_DEFAULT_CTX, arg);
    ASSERT_TRUE(P != nullptr);

    /* Clean up */
    proj_destroy(P);

    /* Same projection, now using argc/argv style initialization */
    P = proj_create_argv(PJ_DEFAULT_CTX, 3, const_cast<char **>(args));
    ASSERT_TRUE(P != nullptr);

    /* zero initialize everything, then set (longitude, latitude) to (12, 55) */
    a = proj_coord(0, 0, 0, 0);
    /* a.lp: The coordinate part of a, interpreted as a classic LP pair */
    a.lp.lam = proj_torad(12);
    a.lp.phi = proj_torad(55);

    /* Forward projection */
    b = proj_trans(P, PJ_FWD, a);

    /* Inverse projection */
    a = proj_trans(P, PJ_INV, b);

    /* Null projection */
    a = proj_trans(P, PJ_IDENT, a);

    /* Forward again, to get two linear items for comparison */
    a = proj_trans(P, PJ_FWD, a);

    dist = proj_xy_dist(a, b);
    ASSERT_LE(dist, 2e-9);

    /* Clear any previous error */
    proj_errno_reset(P);

    /* Clean up */
    proj_destroy(P);

    /* Now do some 3D transformations */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=cart +ellps=GRS80");
    ASSERT_TRUE(P != nullptr);

    /* zero initialize everything, then set (longitude, latitude, height) to
     * (12, 55, 100) */
    a = b = proj_coord(0, 0, 0, 0);
    a.lpz.lam = proj_torad(12);
    a.lpz.phi = proj_torad(55);
    a.lpz.z = 100;

    /* Forward projection: 3D-Cartesian-to-Ellipsoidal */
    b = proj_trans(P, PJ_FWD, a);

    /* Check roundtrip precision for 10000 iterations each way */
    dist = proj_roundtrip(P, PJ_FWD, 10000, &a);
    dist += proj_roundtrip(P, PJ_INV, 10000, &b);
    ASSERT_LE(dist, 4e-9);

    /* Test at the North Pole */
    a = b = proj_coord(0, 0, 0, 0);
    a.lpz.lam = proj_torad(0);
    a.lpz.phi = proj_torad(90);
    a.lpz.z = 100;

    /* Forward projection: Ellipsoidal-to-3D-Cartesian */
    dist = proj_roundtrip(P, PJ_FWD, 1, &a);
    ASSERT_LE(dist, 1e-9);

    /* Test at the South Pole */
    a = b = proj_coord(0, 0, 0, 0);
    a.lpz.lam = proj_torad(0);
    a.lpz.phi = proj_torad(-90);
    a.lpz.z = 100;
    b = a;

    /* Forward projection: Ellipsoidal-to-3D-Cartesian */
    dist = proj_roundtrip(P, PJ_FWD, 1, &a);
    ASSERT_LE(dist, 4e-9);

    /* Inverse projection: 3D-Cartesian-to-Ellipsoidal */
    b = proj_trans(P, PJ_INV, b);

    /* Move p to another context */
    ctx = proj_context_create();
    ASSERT_NE(ctx, pj_get_default_ctx());

    proj_context_set(P, ctx);
    ASSERT_EQ(ctx, P->ctx);

    b = proj_trans(P, PJ_FWD, b);

    /* Move it back to the default context */
    proj_context_set(P, 0);
    ASSERT_EQ(pj_get_default_ctx(), P->ctx);

    proj_context_destroy(ctx);

    /* We go on with the work - now back on the default context */
    b = proj_trans(P, PJ_INV, b);
    proj_destroy(P);

    /* Testing proj_trans_generic () */

    /* An utm projection on the GRS80 ellipsoid */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=utm +zone=32 +ellps=GRS80");
    ASSERT_TRUE(P != nullptr);

    obs[0] = proj_coord(proj_torad(12), proj_torad(55), 45, 0);
    obs[1] = proj_coord(proj_torad(12), proj_torad(56), 50, 0);
    sz = sizeof(PJ_COORD);

    /* Forward projection */
    a = proj_trans(P, PJ_FWD, obs[0]);
    b = proj_trans(P, PJ_FWD, obs[1]);

    n = proj_trans_generic(P, PJ_FWD, &(obs[0].lpz.lam), sz, 2,
                           &(obs[0].lpz.phi), sz, 2, &(obs[0].lpz.z), sz, 2, 0,
                           sz, 0);
    ASSERT_EQ(n, 2U);

    ASSERT_EQ(a.lpz.lam, obs[0].lpz.lam);
    ASSERT_EQ(a.lpz.phi, obs[0].lpz.phi);
    ASSERT_EQ(a.lpz.z, obs[0].lpz.z);
    ASSERT_EQ(b.lpz.lam, obs[1].lpz.lam);
    ASSERT_EQ(b.lpz.phi, obs[1].lpz.phi);
    ASSERT_EQ(b.lpz.z, obs[1].lpz.z);

    /* now test the case of constant z */
    obs[0] = proj_coord(proj_torad(12), proj_torad(55), 45, 0);
    obs[1] = proj_coord(proj_torad(12), proj_torad(56), 50, 0);
    h = 27;
    t = 33;
    n = proj_trans_generic(P, PJ_FWD, &(obs[0].lpz.lam), sz, 2,
                           &(obs[0].lpz.phi), sz, 2, &h, 0, 1, &t, 0, 1);
    ASSERT_EQ(n, 2U);

    ASSERT_EQ(a.lpz.lam, obs[0].lpz.lam);
    ASSERT_EQ(a.lpz.phi, obs[0].lpz.phi);
    ASSERT_EQ(45, obs[0].lpz.z);
    ASSERT_EQ(b.lpz.lam, obs[1].lpz.lam);
    ASSERT_EQ(b.lpz.phi, obs[1].lpz.phi);
    ASSERT_EQ(50, obs[1].lpz.z);
    ASSERT_NE(50, h);

    /* test proj_trans_array () */

    coord[0] = proj_coord(proj_torad(12), proj_torad(55), 45, 0);
    coord[1] = proj_coord(proj_torad(12), proj_torad(56), 50, 0);
    ASSERT_EQ(proj_trans_array(P, PJ_FWD, 2, coord), 0);

    ASSERT_EQ(a.lpz.lam, coord[0].lpz.lam);
    ASSERT_EQ(a.lpz.phi, coord[0].lpz.phi);
    ASSERT_EQ(a.lpz.z, coord[0].lpz.z);
    ASSERT_EQ(b.lpz.lam, coord[1].lpz.lam);
    ASSERT_EQ(b.lpz.phi, coord[1].lpz.phi);
    ASSERT_EQ(b.lpz.z, coord[1].lpz.z);

    /* test proj_trans_array () with two failed points for the same reason */

    coord[0] =
        proj_coord(proj_torad(12), proj_torad(95), 45, 0); // invalid latitude
    coord[1] = proj_coord(proj_torad(12), proj_torad(56), 50, 0);
    coord[2] =
        proj_coord(proj_torad(12), proj_torad(95), 45, 0); // invalid latitude
    ASSERT_EQ(proj_trans_array(P, PJ_FWD, 3, coord),
              PROJ_ERR_COORD_TRANSFM_INVALID_COORD);

    ASSERT_EQ(HUGE_VAL, coord[0].lpz.lam);
    ASSERT_EQ(HUGE_VAL, coord[0].lpz.phi);
    ASSERT_EQ(HUGE_VAL, coord[0].lpz.z);
    ASSERT_EQ(b.lpz.lam, coord[1].lpz.lam);
    ASSERT_EQ(b.lpz.phi, coord[1].lpz.phi);
    ASSERT_EQ(b.lpz.z, coord[1].lpz.z);
    ASSERT_EQ(HUGE_VAL, coord[2].lpz.lam);
    ASSERT_EQ(HUGE_VAL, coord[2].lpz.phi);
    ASSERT_EQ(HUGE_VAL, coord[2].lpz.z);

    /* test proj_trans_array () with two failed points for different reasons */

    coord[0] =
        proj_coord(proj_torad(12), proj_torad(95), 45, 0); // invalid latitude
    coord[1] =
        proj_coord(proj_torad(105), proj_torad(0), 45,
                   0); // in the equatorial axis, at 90° of the central meridian
    ASSERT_EQ(proj_trans_array(P, PJ_FWD, 2, coord), PROJ_ERR_COORD_TRANSFM);

    /* Clean up  after proj_trans_* tests */
    proj_destroy(P);
}

// ---------------------------------------------------------------------------

class gieTest : public ::testing::Test {

    static void DummyLogFunction(void *, int, const char *) {}

  protected:
    void SetUp() override {
        m_ctxt = proj_context_create();
        proj_log_func(m_ctxt, nullptr, DummyLogFunction);
    }

    void TearDown() override { proj_context_destroy(m_ctxt); }

    PJ_CONTEXT *m_ctxt = nullptr;
};

// ---------------------------------------------------------------------------

TEST_F(gieTest, proj_create_crs_to_crs) {
    /* test proj_create_crs_to_crs() */
    auto P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, "epsg:25832", "epsg:25833",
                                    NULL);
    ASSERT_TRUE(P != nullptr);
    PJ_COORD a, b;

    a.xyzt.x = 700000.0;
    a.xyzt.y = 6000000.0;
    a.xyzt.z = 0;
    a.xyzt.t = HUGE_VAL;
    b.xy.x = 307788.8761171057;
    b.xy.y = 5999669.3036037628;

    a = proj_trans(P, PJ_FWD, a);
    EXPECT_NEAR(a.xy.x, b.xy.x, 1e-8);
    EXPECT_NEAR(a.xy.y, b.xy.y, 1e-8);

    auto src = proj_get_source_crs(PJ_DEFAULT_CTX, P);
    ASSERT_TRUE(src != nullptr);
    EXPECT_EQ(proj_get_name(src), std::string("ETRS89 / UTM zone 32N"));
    proj_destroy(src);

    proj_destroy(P);

    /* we can also allow PROJ strings as a usable PJ */
    P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, "proj=utm +zone=32 +datum=WGS84",
                               "proj=utm +zone=33 +datum=WGS84", NULL);
    ASSERT_TRUE(P != nullptr);
    proj_destroy(P);

    EXPECT_TRUE(proj_create_crs_to_crs(m_ctxt, "invalid", "EPSG:25833", NULL) ==
                nullptr);
    EXPECT_TRUE(proj_create_crs_to_crs(m_ctxt, "EPSG:25832", "invalid", NULL) ==
                nullptr);
}

// ---------------------------------------------------------------------------

TEST_F(gieTest, proj_create_crs_to_crs_EPSG_4326) {

    auto P =
        proj_create_crs_to_crs(PJ_DEFAULT_CTX, "EPSG:4326", "EPSG:32631", NULL);
    ASSERT_TRUE(P != nullptr);
    PJ_COORD a, b;

    // Lat, long degrees
    a.xyzt.x = 0.0;
    a.xyzt.y = 3.0;
    a.xyzt.z = 0;
    a.xyzt.t = HUGE_VAL;

    b.xy.x = 500000.0;
    b.xy.y = 0.0;

    a = proj_trans(P, PJ_FWD, a);
    EXPECT_NEAR(a.xy.x, b.xy.x, 1e-9);
    EXPECT_NEAR(a.xy.y, b.xy.y, 1e-9);
    proj_destroy(P);
}

// ---------------------------------------------------------------------------

TEST_F(gieTest, proj_create_crs_to_crs_proj_longlat) {

    auto P = proj_create_crs_to_crs(
        PJ_DEFAULT_CTX, "+proj=longlat +datum=WGS84", "EPSG:32631", NULL);
    ASSERT_TRUE(P != nullptr);
    PJ_COORD a, b;

    // Long, lat degrees
    a.xyzt.x = 3.0;
    a.xyzt.y = 0;
    a.xyzt.z = 0;
    a.xyzt.t = HUGE_VAL;

    b.xy.x = 500000.0;
    b.xy.y = 0.0;

    a = proj_trans(P, PJ_FWD, a);
    EXPECT_NEAR(a.xy.x, b.xy.x, 1e-9);
    EXPECT_NEAR(a.xy.y, b.xy.y, 1e-9);
    proj_destroy(P);
}

// ---------------------------------------------------------------------------

TEST(gie, info_functions) {
    PJ_INFO info;
    PJ_PROJ_INFO pj_info;
    PJ_GRID_INFO grid_info;
    PJ_INIT_INFO init_info;

    PJ_FACTORS factors;

    const PJ_OPERATIONS *oper_list;
    const PJ_ELLPS *ellps_list;
    const PJ_PRIME_MERIDIANS *pm_list;

    char buf[40];
    PJ *P;
    char arg[50] = {"+proj=utm; +zone=32; +ellps=GRS80"};
    PJ_COORD a;

    /* ********************************************************************** */
    /*                          Test info functions                           */
    /* ********************************************************************** */

    /* proj_info()                                                            */
    /* this one is difficult to test, since the output changes with the setup */
    putenv(const_cast<char *>("PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY="));
    info = proj_info();
    putenv(const_cast<char *>("PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES"));

    if (info.version[0] != '\0') {
        char tmpstr[64];
        sprintf(tmpstr, "%d.%d.%d", info.major, info.minor, info.patch);
        ASSERT_EQ(std::string(info.version), std::string(tmpstr));
    }
    ASSERT_NE(std::string(info.release), "");
    if (getenv("HOME") || getenv("PROJ_LIB")) {
        ASSERT_NE(std::string(info.searchpath), std::string());
    }

    ASSERT_TRUE(std::string(info.searchpath).find("/proj") !=
                std::string::npos);

    /* proj_pj_info() */
    {
        P = proj_create(PJ_DEFAULT_CTX,
                        "+proj=august"); /* august has no inverse */
        auto has_inverse = proj_pj_info(P).has_inverse;
        proj_destroy(P);
        ASSERT_FALSE(has_inverse);
    }

    P = proj_create(PJ_DEFAULT_CTX, arg);
    pj_info = proj_pj_info(P);
    ASSERT_TRUE(pj_info.has_inverse);
    pj_shrink(arg);
    ASSERT_EQ(std::string(pj_info.definition), arg);
    ASSERT_EQ(std::string(pj_info.id), "utm");

    proj_destroy(P);

    /* proj_grid_info() */
    grid_info = proj_grid_info("tests/test_hgrid.tif");
    ASSERT_NE(std::string(grid_info.filename), "");
    ASSERT_EQ(std::string(grid_info.gridname), "tests/test_hgrid.tif");
    ASSERT_EQ(std::string(grid_info.format), "gtiff");

    grid_info = proj_grid_info("nonexistinggrid");
    ASSERT_EQ(std::string(grid_info.filename), "");

    // File exists, but is not a grid
    grid_info = proj_grid_info("proj.db");
    ASSERT_EQ(std::string(grid_info.filename), "");

    /* proj_init_info() */
    init_info = proj_init_info("unknowninit");
    ASSERT_EQ(std::string(init_info.filename), "");

    init_info = proj_init_info("epsg");
    /* Need to allow for "Unknown" until all commonly distributed EPSG-files
     * comes with a metadata section */
    ASSERT_TRUE(std::string(init_info.origin) == "EPSG" ||
                std::string(init_info.origin) == "Unknown")
        << std::string(init_info.origin);
    ASSERT_EQ(std::string(init_info.name), "epsg");

    /* test proj_rtodms() and proj_dmstor() */
    ASSERT_EQ(std::string("180dN"), proj_rtodms(buf, M_PI, 'N', 'S'));

    ASSERT_EQ(proj_dmstor(&buf[0], NULL), M_PI);

    ASSERT_EQ(std::string("114d35'29.612\"S"),
              proj_rtodms(buf, -2.0, 'N', 'S'));

    /* we can't expect perfect numerical accuracy so testing with a tolerance */
    ASSERT_NEAR(-2.0, proj_dmstor(&buf[0], NULL), 1e-7);

    /* test proj_derivatives_retrieve() and proj_factors_retrieve() */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=merc +ellps=WGS84");
    a = proj_coord(0, 0, 0, 0);
    a.lp.lam = proj_torad(12);
    a.lp.phi = proj_torad(55);

    factors = proj_factors(P, a);
    ASSERT_FALSE(proj_errno(P)); /* factors not created correctly */

    /* check a few key characteristics of the Mercator projection */
    EXPECT_NEAR(factors.angular_distortion, 0.0, 1e-7)
        << factors.angular_distortion; /* angular distortion should be 0 */

    /* Meridian/parallel angle should be 90 deg */
    EXPECT_NEAR(factors.meridian_parallel_angle, M_PI_2, 1e-7)
        << factors.meridian_parallel_angle;

    EXPECT_EQ(factors.meridian_convergence,
              0.0); /* meridian convergence should be 0 */

    proj_destroy(P);

    /* Check that proj_list_* functions work by looping through them */
    size_t n = 0;
    for (oper_list = proj_list_operations(); oper_list->id; ++oper_list)
        n++;
    ASSERT_NE(n, 0U);

    n = 0;
    for (ellps_list = proj_list_ellps(); ellps_list->id; ++ellps_list)
        n++;
    ASSERT_NE(n, 0U);

    n = 0;
    for (pm_list = proj_list_prime_meridians(); pm_list->id; ++pm_list)
        n++;
    ASSERT_NE(n, 0U);
}

// ---------------------------------------------------------------------------

TEST(gie, io_predicates) {
    /* check io-predicates */

    /* angular in on fwd, linear out */
    auto P = proj_create(PJ_DEFAULT_CTX, "+proj=cart +ellps=GRS80");
    ASSERT_TRUE(P != nullptr);
    ASSERT_TRUE(proj_angular_input(P, PJ_FWD));
    ASSERT_FALSE(proj_angular_input(P, PJ_INV));
    ASSERT_FALSE(proj_angular_output(P, PJ_FWD));
    ASSERT_TRUE(proj_angular_output(P, PJ_INV));
    P->inverted = 1;
    ASSERT_FALSE(proj_angular_input(P, PJ_FWD));
    ASSERT_TRUE(proj_angular_input(P, PJ_INV));
    ASSERT_TRUE(proj_angular_output(P, PJ_FWD));
    ASSERT_FALSE(proj_angular_output(P, PJ_INV));
    proj_destroy(P);

    /* angular in and out */
    P = proj_create(PJ_DEFAULT_CTX,
                    "+proj=molodensky +a=6378160 +rf=298.25 "
                    "+da=-23 +df=-8.120449e-8 +dx=-134 +dy=-48 +dz=149 "
                    "+abridged ");
    ASSERT_TRUE(P != nullptr);
    ASSERT_TRUE(proj_angular_input(P, PJ_FWD));
    ASSERT_TRUE(proj_angular_input(P, PJ_INV));
    ASSERT_TRUE(proj_angular_output(P, PJ_FWD));
    ASSERT_TRUE(proj_angular_output(P, PJ_INV));
    P->inverted = 1;
    ASSERT_TRUE(proj_angular_input(P, PJ_FWD));
    ASSERT_TRUE(proj_angular_input(P, PJ_INV));
    ASSERT_TRUE(proj_angular_output(P, PJ_FWD));
    ASSERT_TRUE(proj_angular_output(P, PJ_INV));
    proj_destroy(P);

    /* linear in and out */
    P = proj_create(PJ_DEFAULT_CTX,
                    " +proj=helmert"
                    " +x=0.0127 +y=0.0065 +z=-0.0209 +s=0.00195"
                    " +rx=-0.00039 +ry=0.00080 +rz=-0.00114"
                    " +dx=-0.0029 +dy=-0.0002 +dz=-0.0006 +ds=0.00001"
                    " +drx=-0.00011 +dry=-0.00019 +drz=0.00007"
                    " +t_epoch=1988.0 +convention=coordinate_frame");
    ASSERT_TRUE(P != nullptr);
    ASSERT_FALSE(proj_angular_input(P, PJ_FWD));
    ASSERT_FALSE(proj_angular_input(P, PJ_INV));
    ASSERT_FALSE(proj_angular_output(P, PJ_FWD));
    ASSERT_FALSE(proj_angular_output(P, PJ_INV));
    P->inverted = 1;
    ASSERT_FALSE(proj_angular_input(P, PJ_FWD));
    ASSERT_FALSE(proj_angular_input(P, PJ_INV));
    ASSERT_FALSE(proj_angular_output(P, PJ_FWD));
    ASSERT_FALSE(proj_angular_output(P, PJ_INV));

    /* pj_init_ctx should default to GRS80 */
    ASSERT_EQ(P->a, 6378137.0);
    ASSERT_EQ(P->f, 1.0 / 298.257222101);
    proj_destroy(P);

    /* Test that pj_fwd* and pj_inv* returns NaNs when receiving NaN input */
    P = proj_create(PJ_DEFAULT_CTX, "+proj=merc +ellps=WGS84");
    ASSERT_TRUE(P != nullptr);
    auto a = proj_coord(NAN, NAN, NAN, NAN);
    a = proj_trans(P, PJ_FWD, a);
    ASSERT_TRUE((std::isnan(a.v[0]) && std::isnan(a.v[1]) &&
                 std::isnan(a.v[2]) && std::isnan(a.v[3])));

    a = proj_coord(NAN, NAN, NAN, NAN);
    a = proj_trans(P, PJ_INV, a);
    ASSERT_TRUE((std::isnan(a.v[0]) && std::isnan(a.v[1]) &&
                 std::isnan(a.v[2]) && std::isnan(a.v[3])));
    proj_destroy(P);
}

// ---------------------------------------------------------------------------

static void test_time(const char *args, double tol, double t_in, double t_exp) {
    PJ_COORD in, out;
    PJ *P = proj_create(PJ_DEFAULT_CTX, args);

    ASSERT_TRUE(P != 0);

    in = proj_coord(0.0, 0.0, 0.0, t_in);

    out = proj_trans(P, PJ_FWD, in);
    EXPECT_NEAR(out.xyzt.t, t_exp, tol);

    out = proj_trans(P, PJ_INV, out);
    EXPECT_NEAR(out.xyzt.t, t_in, tol);

    proj_destroy(P);

    proj_log_level(NULL, PJ_LOG_NONE);
}

// ---------------------------------------------------------------------------

TEST(gie, unitconvert_selftest) {

    char args1[] = "+proj=unitconvert +t_in=decimalyear +t_out=decimalyear";
    double in1 = 2004.25;

    char args2[] = "+proj=unitconvert +t_in=gps_week +t_out=gps_week";
    double in2 = 1782.0;

    char args3[] = "+proj=unitconvert +t_in=mjd +t_out=mjd";
    double in3 = 57390.0;

    char args4[] = "+proj=unitconvert +t_in=gps_week +t_out=decimalyear";
    double in4 = 1877.71428, exp4 = 2016.0;

    char args5[] = "+proj=unitconvert +t_in=yyyymmdd +t_out=yyyymmdd";
    double in5 = 20170131;

    test_time(args1, 1e-6, in1, in1);
    test_time(args2, 1e-6, in2, in2);
    test_time(args3, 1e-6, in3, in3);
    test_time(args4, 1e-6, in4, exp4);
    test_time(args5, 1e-6, in5, in5);
}

static const char tc32_utm32[] = {
    " +proj=horner"
    " +ellps=intl"
    " +range=500000"
    " +fwd_origin=877605.269066,6125810.306769"
    " +inv_origin=877605.760036,6125811.281773"
    " +deg=4"
    " +fwd_v=6.1258112678e+06,9.9999971567e-01,1.5372750011e-10,5.9300860915e-"
    "15,2.2609497633e-19,4.3188227445e-05,2.8225130416e-10,7.8740007114e-16,-1."
    "7453997279e-19,1.6877465415e-10,-1.1234649773e-14,-1.7042333358e-18,-7."
    "9303467953e-15,-5.2906832535e-19,3.9984284847e-19"
    " +fwd_u=8.7760574982e+05,9.9999752475e-01,2.8817299305e-10,5.5641310680e-"
    "15,-1.5544700949e-18,-4.1357045890e-05,4.2106213519e-11,2.8525551629e-14,-"
    "1.9107771273e-18,3.3615590093e-10,2.4380247154e-14,-2.0241230315e-18,1."
    "2429019719e-15,5.3886155968e-19,-1.0167505000e-18"
    " +inv_v=6.1258103208e+06,1.0000002826e+00,-1.5372762184e-10,-5."
    "9304261011e-15,-2.2612705361e-19,-4.3188331419e-05,-2.8225549995e-10,-7."
    "8529116371e-16,1.7476576773e-19,-1.6875687989e-10,1.1236475299e-14,1."
    "7042518057e-18,7.9300735257e-15,5.2881862699e-19,-3.9990736798e-19"
    " +inv_u=8.7760527928e+05,1.0000024735e+00,-2.8817540032e-10,-5."
    "5627059451e-15,1.5543637570e-18,4.1357152105e-05,-4.2114813612e-11,-2."
    "8523713454e-14,1.9109017837e-18,-3.3616407783e-10,-2.4382678126e-14,2."
    "0245020199e-18,-1.2441377565e-15,-5.3885232238e-19,1.0167203661e-18"};

static const char sb_utm32[] = {
    " +proj=horner"
    " +ellps=intl"
    " +range=500000"
    " +tolerance=0.0005"
    " +fwd_origin=4.94690026817276e+05,6.13342113183056e+06"
    " +inv_origin=6.19480258923588e+05,6.13258568148837e+06"
    " +deg=3"
    " +fwd_c=6.13258562111350e+06,6.19480105709997e+05,9.99378966275206e-01,-2."
    "82153291753490e-02,-2.27089979140026e-10,-1.77019590701470e-09,1."
    "08522286274070e-14,2.11430298751604e-15"
    " +inv_c=6.13342118787027e+06,4.94690181709311e+05,9.99824464710368e-01,2."
    "82279070814774e-02,7.66123542220864e-11,1.78425334628927e-09,-1."
    "05584823306400e-14,-3.32554258683744e-15"};

// ---------------------------------------------------------------------------

TEST(gie, horner_selftest) {

    PJ *P;
    PJ_COORD a, b, c;
    double dist;

    /* Real polynonia relating the technical coordinate system TC32 to "System
     * 45 Bornholm" */
    P = proj_create(PJ_DEFAULT_CTX, tc32_utm32);
    ASSERT_TRUE(P != nullptr);

    a = b = proj_coord(0, 0, 0, 0);
    a.uv.v = 6125305.4245;
    a.uv.u = 878354.8539;
    c = a;

    /* Check roundtrip precision for 1 iteration each way, starting in forward
     * direction */
    dist = proj_roundtrip(P, PJ_FWD, 1, &c);
    EXPECT_LE(dist, 0.01);
    proj_destroy(P);

    /* The complex polynomial transformation between the "System Storebaelt" and
     * utm32/ed50 */
    P = proj_create(PJ_DEFAULT_CTX, sb_utm32);
    ASSERT_TRUE(P != nullptr);

    /* Test value: utm32_ed50(620000, 6130000) = sb_ed50(495136.8544,
     * 6130821.2945) */
    a = b = c = proj_coord(0, 0, 0, 0);
    a.uv.v = 6130821.2945;
    a.uv.u = 495136.8544;
    c.uv.v = 6130000.0000;
    c.uv.u = 620000.0000;

    /* Forward projection */
    b = proj_trans(P, PJ_FWD, a);
    dist = proj_xy_dist(b, c);
    EXPECT_LE(dist, 0.001);

    /* Inverse projection */
    b = proj_trans(P, PJ_INV, c);
    dist = proj_xy_dist(b, a);
    EXPECT_LE(dist, 0.001);

    /* Check roundtrip precision for 1 iteration each way */
    dist = proj_roundtrip(P, PJ_FWD, 1, &a);
    EXPECT_LE(dist, 0.01);

    proj_destroy(P);
}

// ---------------------------------------------------------------------------

TEST(gie, proj_create_crs_to_crs_PULKOVO42_ETRS89) {
    auto P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, "EPSG:4179", "EPSG:4258",
                                    nullptr);
    ASSERT_TRUE(P != nullptr);
    PJ_COORD c;

    EXPECT_EQ(std::string(proj_pj_info(P).definition),
              "unavailable until proj_trans is called");
    EXPECT_EQ(proj_get_name(P), nullptr);
    EXPECT_EQ(P->fwd, nullptr);
    EXPECT_EQ(P->fwd3d, nullptr);
    EXPECT_EQ(P->fwd4d, nullptr);

    // get source CRS even if the P object is in a dummy state
    auto src_crs = proj_get_source_crs(PJ_DEFAULT_CTX, P);
    EXPECT_TRUE(src_crs != nullptr);
    EXPECT_EQ(proj_get_name(src_crs), std::string("Pulkovo 1942(58)"));
    proj_destroy(src_crs);

    // get target CRS even if the P object is in a dummy state
    auto target_crs = proj_get_target_crs(PJ_DEFAULT_CTX, P);
    EXPECT_TRUE(target_crs != nullptr);
    EXPECT_EQ(proj_get_name(target_crs), std::string("ETRS89"));
    proj_destroy(target_crs);

    // Romania
    c.xyzt.x = 45; // Lat
    c.xyzt.y = 25; // Long
    c.xyzt.z = 0;
    c.xyzt.t = HUGE_VAL;
    c = proj_trans(P, PJ_FWD, c);
    EXPECT_NEAR(c.xy.x, 44.999701238, 1e-9);
    EXPECT_NEAR(c.xy.y, 24.998474948, 1e-9);
    EXPECT_EQ(std::string(proj_pj_info(P).definition),
              "proj=pipeline step proj=axisswap order=2,1 "
              "step proj=unitconvert xy_in=deg xy_out=rad "
              "step proj=push v_3 "
              "step proj=cart "
              "ellps=krass step proj=helmert x=2.3287 y=-147.0425 z=-92.0802 "
              "rx=0.3092483 ry=-0.32482185 rz=-0.49729934 s=5.68906266 "
              "convention=coordinate_frame step inv proj=cart ellps=GRS80 "
              "step proj=pop v_3 "
              "step proj=unitconvert xy_in=rad xy_out=deg step proj=axisswap "
              "order=2,1");

    c = proj_trans(P, PJ_INV, c);
    EXPECT_NEAR(c.xy.x, 45, 1e-8);
    EXPECT_NEAR(c.xy.y, 25, 1e-8);

    c.xyzt.x = 45; // Lat
    c.xyzt.y = 25; // Long
    c.xyzt.z = 0;
    c.xyzt.t = HUGE_VAL;
    proj_trans_generic(P, PJ_FWD, &(c.xyz.x), sizeof(double), 1, &(c.xyz.y),
                       sizeof(double), 1, &(c.xyz.z), sizeof(double), 1,
                       nullptr, 0, 0);
    EXPECT_NEAR(c.xy.x, 44.999701238, 1e-9);
    EXPECT_NEAR(c.xy.y, 24.998474948, 1e-9);

    // Poland
    c.xyz.x = 52; // Lat
    c.xyz.y = 20; // Long
    c.xyz.z = 0;
    c = proj_trans(P, PJ_FWD, c);
    EXPECT_NEAR(c.xy.x, 51.999714150, 1e-9);
    EXPECT_NEAR(c.xy.y, 19.998187811, 1e-9);
    EXPECT_EQ(std::string(proj_pj_info(P).definition),
              "proj=pipeline step proj=axisswap order=2,1 "
              "step proj=unitconvert xy_in=deg xy_out=rad "
              "step proj=push v_3 "
              "step proj=cart "
              "ellps=krass step proj=helmert x=33.4 y=-146.6 z=-76.3 rx=-0.359 "
              "ry=-0.053 rz=0.844 s=-0.84 convention=position_vector step inv "
              "proj=cart ellps=GRS80 step proj=pop v_3 "
              "step proj=unitconvert xy_in=rad "
              "xy_out=deg step proj=axisswap order=2,1");

    proj_destroy(P);
}

// ---------------------------------------------------------------------------

TEST(gie, proj_create_crs_to_crs_outside_area_of_use) {

    // See https://github.com/OSGeo/proj.4/issues/1329
    auto P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, "EPSG:4275", "EPSG:4807",
                                    nullptr);
    ASSERT_TRUE(P != nullptr);
    PJ_COORD c;

    EXPECT_EQ(P->fwd, nullptr);

    // Test point outside area of use of both candidate coordinate operations
    c.xyzt.x = 58; // Lat in deg
    c.xyzt.y = 5;  // Long in deg
    c.xyzt.z = 0;
    c.xyzt.t = HUGE_VAL;
    c = proj_trans(P, PJ_FWD, c);
    EXPECT_NEAR(c.xy.x, 64.44444444444444, 1e-9); // Lat in grad
    EXPECT_NEAR(c.xy.y, 2.958634259259258, 1e-9); // Long in grad

    proj_destroy(P);
}

// ---------------------------------------------------------------------------

TEST(gie, proj_create_crs_to_crs_with_area_large) {

    // Test bugfix for https://github.com/OSGeo/gdal/issues/3695
    auto area = proj_area_create();
    proj_area_set_bbox(area, -14.1324, 49.5614, 3.76488, 62.1463);
    auto P =
        proj_create_crs_to_crs(PJ_DEFAULT_CTX, "EPSG:4277", "EPSG:4326", area);
    proj_area_destroy(area);
    ASSERT_TRUE(P != nullptr);
    PJ_COORD c;

    c.xyzt.x = 50; // Lat in deg
    c.xyzt.y = -2; // Long in deg
    c.xyzt.z = 0;
    c.xyzt.t = HUGE_VAL;
    c = proj_trans(P, PJ_FWD, c);
    EXPECT_NEAR(c.xy.x, 50.00065628, 1e-8);
    EXPECT_NEAR(c.xy.y, -2.00133989, 1e-8);

    proj_destroy(P);
}

// ---------------------------------------------------------------------------

TEST(gie, proj_trans_generic) {
    // GDA2020 to WGS84 (G1762)
    auto P = proj_create(
        PJ_DEFAULT_CTX,
        "+proj=pipeline +step +proj=axisswap +order=2,1 "
        "+step +proj=unitconvert +xy_in=deg +xy_out=rad "
        "+step +proj=cart +ellps=GRS80 "
        "+step +proj=helmert +x=0 +y=0 +z=0 +rx=0 +ry=0 +rz=0 +s=0 +dx=0 "
        "+dy=0 +dz=0 +drx=-0.00150379 +dry=-0.00118346 +drz=-0.00120716 "
        "+ds=0 +t_epoch=2020 +convention=coordinate_frame "
        "+step +inv +proj=cart +ellps=WGS84 "
        "+step +proj=unitconvert +xy_in=rad +xy_out=deg "
        "+step +proj=axisswap +order=2,1");
    double lat = -60;
    double lon = 120;
    proj_trans_generic(P, PJ_FWD, &lat, sizeof(double), 1, &lon, sizeof(double),
                       1, nullptr, 0, 0, nullptr, 0, 0);
    // Should be a no-op when the time is unknown (or equal to 2020)
    EXPECT_NEAR(lat, -60, 1e-9);
    EXPECT_NEAR(lon, 120, 1e-9);

    proj_destroy(P);
}

TEST(gie, proj_create_crs_to_crs_force_over) {

    PJ_CONTEXT* ctx;

    ctx = proj_context_create();
    ASSERT_TRUE(ctx != nullptr);
    proj_context_set_force_over(ctx, 1);
    ASSERT_TRUE(ctx->forceOver);

    auto P = proj_create_crs_to_crs(ctx, "EPSG:4326", "EPSG:3857", nullptr);
    ASSERT_TRUE(P != nullptr);
    PJ_COORD input;
    PJ_COORD input_over;

    //Test a point along the equator. 
    //The same point, but in two different representations.
    input.xyz.x = 0; // Lat in deg
    input.xyz.y = 140;  // Long in deg
    input.xyz.z = 0;

    input_over.xyz.x = 0; // Lat in deg
    input_over.xyz.y = -220;  // Long in deg
    input_over.xyz.z = 0;

    auto output = proj_trans(P, PJ_FWD, input);
    auto output_over = proj_trans(P, PJ_FWD, input_over);

    auto input_inv = proj_trans(P, PJ_INV, output);
    auto input_over_inv = proj_trans(P, PJ_INV, output_over);

    //Web Mercator x's between 0 and 180 longitude come out positive.
    //But when forcing the over flag, the -220 calculation makes it flip.
    EXPECT_GT(output.xyz.x, 0);
    EXPECT_LT(output_over.xyz.x, 0);

    EXPECT_NEAR(output.xyz.x, 15584728.711058298, 1e-8);
    EXPECT_NEAR(output_over.xyz.x, -24490287.974520184, 1e-8);

    //The distance from 140 to 180 and -220 to -180 should be pretty much the same.
    auto dx_o = fabs(output.xyz.x - 20037508.342789244);
    auto dx_over = fabs(output_over.xyz.x + 20037508.342789244);
    auto dx = fabs(dx_o - dx_over);

    EXPECT_NEAR(dx, 0, 1e-8);

    //Check the inverse operations get us back close to our original input values.
    EXPECT_NEAR(input.xyz.x, input_inv.xyz.x, 1e-8);
    EXPECT_NEAR(input.xyz.y, input_inv.xyz.y, 1e-8);
    EXPECT_NEAR(input_over.xyz.x, input_over_inv.xyz.x, 1e-8);
    EXPECT_NEAR(input_over.xyz.y, input_over_inv.xyz.y, 1e-8);

    proj_destroy(P);

    proj_context_destroy(ctx);
}

} // namespace
