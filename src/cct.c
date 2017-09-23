/***********************************************************************

                 The cct 4D Transformation program

************************************************************************

cct is a first attempt at a 4D equivalent to the "proj" projection
program.

cct is an acromyn meaning "Coordinate Conversion and Transformation".

The acronym refers to definitions given in the OGC 08-015r2/ISO-19111
standard "Geographical Information -- Spatial Referencing by Coordinates",
which defines two different classes of coordinate operations:

*Coordinate Conversions*, which are coordinate operations where input
and output datum are identical (e.g. conversion from geographical to
cartesian coordinates) and

*Coordinate Transformations*, which are coordinate operations where
input and output datums differ (e.g. change of reference frame).

cct, however, also refers to Carl Christian Tscherning (1942--2014),
professor of Geodesy at the University of Copenhagen, mentor and advisor
for a generation of Danish geodesists, colleague and collaborator for
two generations of global geodesists, Secretary General for the
International Association of Geodesy, IAG (1995--2007), fellow of the
Amercan Geophysical Union (1991), recipient of the IAG Levallois Medal
(2007), the European Geosciences Union Vening Meinesz Medal (2008), and
of numerous other honours.

cct, or Christian, as he was known to most of us, was recognized for his
good mood, his sharp wit, his tireless work, and his great commitment to
the development of geodesy - both through his scientific contributions,
comprising more than 250 publications, and by his mentoring and teaching
of the next generations of geodesists.

As Christian was an avid Fortran programmer, and a keen Unix connoiseur,
he would have enjoyed to know that his initials would be used to name a
modest Unix style transformation filter, hinting at the tireless aspect
of his personality, which was certainly one of the reasons he accomplished
so much, and meant so much to so many people.

Hence, in honour of cct (the geodesist) this is cct (the program).

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-25/2017-09-19

************************************************************************

* Copyright (c) 2016, 2017 Thomas Knudsen
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

***********************************************************************/

#include "optargpm.h"
#include <proj.h>
#include <projects.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

double proj_strtod(const char *str, char **endptr);
double proj_atof(const char *str);

char *column (char *buf, int n);
PJ_COORD parse_input_line (char *buf, int *columns, double fixed_height, double fixed_time);
int print_output_line (FILE *fout, char *buf, PJ_COORD point);
int main(int argc, char **argv);
    


static const char usage[] = {
    "--------------------------------------------------------------------------------\n"
    "Usage: %s [-options]... [+operator_specs]... infile...\n"
    "--------------------------------------------------------------------------------\n"
    "Options:\n"
    "--------------------------------------------------------------------------------\n"
    "    -o /path/to/file  Specify output file name\n"
    "    -c x,y,z,t        Specify input columns for (up to) 4 input parameters.\n"
    "                      Defaults to 1,2,3,4\n"
    "    -z value          Provide a fixed z value for all input data (e.g. -z 0)\n"
    "    -t value          Provide a fixed t value for all input data (e.g. -t 0)\n"
    "    -v                Verbose: Provide non-essential informational output.\n"
    "                      Repeat -v for more verbosity (e.g. -vv)\n"
    "--------------------------------------------------------------------------------\n"
    "Long Options:\n"
    "--------------------------------------------------------------------------------\n"
    "    --output          Alias for -o\n"
    "    --columns         Alias for -c\n"
    "    --height          Alias for -z\n"
    "    --time            Alias for -t\n"
    "    --verbose         Alias for -v\n"
    "    --help            Alias for -h\n"
    "--------------------------------------------------------------------------------\n"
    "Operator Specs:\n"
    "--------------------------------------------------------------------------------\n"
    "The operator specs describe the action to be performed by cct, e.g:\n"
    "\n"
    "        +proj=utm  +ellps=GRS80  +zone=32\n"
    "\n"
    "instructs cct to convert input data to Universal Transverse Mercator, zone 32\n"
    "coordinates, based on the GRS80 ellipsoid.\n"
    "\n"
    "Hence, the command\n"
    "\n"
    "        echo 12 55 | cct -z0 -t0 +proj=utm +zone=32 +ellps=GRS80\n"
    "\n"
    "Should give results comparable to the classic proj command\n"
    "\n"
    "        echo 12 55 | proj +proj=utm +zone=32 +ellps=GRS80\n"
    "--------------------------------------------------------------------------------\n"
    "Examples:\n"
    "--------------------------------------------------------------------------------\n"
    "1. convert geographical input to UTM zone 32 on the GRS80 ellipsoid:\n"
    "    cct +ellps=GRS80 +zone=32 +proj=utm\n"
    "2. roundtrip accuract check for the case above:\n"
    "    cct +proj=pipeline +proj=utm +ellps=GRS80 +zone=32 +step +step +inv\n"
    "3. as (1) but specify input columns for longitude, latitude, height and time:\n"
    "    cct -c 5,2,1,4  +proj=utm +ellps=GRS80 +zone=32\n"
    "4. as (1) but specify fixed height and time, hence needing only 2 cols in input:\n"
    "    cct -t 0 -z 0  +proj=utm  +ellps=GRS80  +zone=32\n"
    "--------------------------------------------------------------------------------\n"    
};

int main(int argc, char **argv) {
    PJ *P;
    PJ_COORD point;
    OPTARGS *o;
    FILE *fout = stdout;
    char *buf;
    int input_unit, output_unit, nfields = 4, direction = 1, verbose;
    double fixed_z = HUGE_VAL, fixed_time = HUGE_VAL;
    int columns_xyzt[] = {1, 2, 3, 4};
    const char *longflags[]  = {"v=verbose", "h=help", 0};
    const char *longkeys[]   = {"o=output",  "c=columns", "z=height", "t=time", 0};
    
    o = opt_parse (argc, argv, "hv", "cozt", longflags, longkeys);
    if (0==o)
        return 0;

    if (opt_given (o, "h")) {
        printf (usage, o->progname);
        exit (0);
    }


    direction = opt_given (o, "I")? -1: 1;
    verbose   = opt_given (o, "v");

    if (opt_given (o, "o"))
        fout = fopen (opt_arg (o, "output"), "rt");
    if (0==fout) {
        fprintf (stderr, "%s: Cannot open '%s' for output\n", o->progname, opt_arg (o, "output"));
        free (o);
        return 1;
    }
    if (verbose > 3)
        fprintf (fout, "%s: Running in very verbose mode\n", o->progname);
    
    

    if (opt_given (o, "z")) {
        fixed_z = proj_atof (opt_arg (o, "z"));
        nfields--;
    }
    
    if (opt_given (o, "t")) {
        fixed_time = proj_atof (opt_arg (o, "t"));
        nfields--;
    }

    if (opt_given (o, "c")) {
        int ncols = sscanf (opt_arg (o, "c"), "%d,%d,%d,%d", columns_xyzt, columns_xyzt+1, columns_xyzt+3, columns_xyzt+3);
        if (ncols != nfields) {
            fprintf (stderr, "%s: Too few input columns given: '%s'\n", o->progname, opt_arg (o, "c"));
            free (o);
            return 1;
        }
    }
    
    /* Setup transformation */
    P = proj_create_argv (0, o->pargc, o->pargv);
    if ((0==P) || (0==o->pargc)) {
        fprintf (stderr, "%s: Bad transformation arguments. '%s -h' for help\n", o->progname, o->progname);
        free (o);
        return 1;
    }

    input_unit   =  P->left;
    output_unit  =  P->right;
    if (PJ_IO_UNITS_CLASSIC==P->left)
        input_unit = PJ_IO_UNITS_RADIANS;
    if (PJ_IO_UNITS_CLASSIC==P->right)
        output_unit = PJ_IO_UNITS_METERS;
    if (direction==-1) {
        enum pj_io_units swap = input_unit;
        input_unit = output_unit;
        output_unit = swap;
    }

    /* Allocate input buffer */
    buf = calloc (1, 10000);
    if (0==buf) {
        fprintf (stderr, "%s: Out of memory\n", o->progname);
        pj_free (P);
        free (o);
        return 1;
    }


    /* Loop over all lines of all input files */
    while (opt_input_loop (o, optargs_file_format_text)) {
        void *ret = fgets (buf, 10000, o->input);
        int res;
        opt_eof_handler (o);
        if (0==ret) {
            fprintf (stderr, "Read error in record %d\n", (int) o->record_index);
            continue;
        }
        point = parse_input_line (buf, columns_xyzt, fixed_z, fixed_time);
        if (PJ_IO_UNITS_RADIANS==input_unit) {
            point.lpzt.lam = proj_torad (point.lpzt.lam);
            point.lpzt.phi = proj_torad (point.lpzt.phi);
        }
        point = proj_trans_coord (P, direction, point);
        if (PJ_IO_UNITS_RADIANS==output_unit) {
            point.lpzt.lam = proj_todeg (point.lpzt.lam);
            point.lpzt.phi = proj_todeg (point.lpzt.phi);
        }
        res = print_output_line (fout, buf, point);
        if (0==res) {
            fprintf (fout, "# UNREADABLE: %s", buf);
            if (verbose)
                fprintf (stderr, "%s: Could not parse file '%s' line %d\n", o->progname, opt_filename (o), opt_record (o));
        }
    }

    return 0;
}





/* return a pointer to the n'th column of buf */
char *column (char *buf, int n) {
    int i;
    if (n <= 0)
        return buf;
    for (i = 0;  i < n;  i++) {
        while (isspace(*buf))
            buf++;
        if (i == n - 1)
            break;
        while ((0 != *buf) && !isspace(*buf))
            buf++;
    }
    return buf;
}


PJ_COORD parse_input_line (char *buf, int *columns, double fixed_height, double fixed_time) {
    PJ_COORD err = proj_coord (HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL);
    PJ_COORD result = err;
    int prev_errno = errno;
    char *endptr = 0;
    errno = 0;

    result.xyzt.z = fixed_height;
    result.xyzt.t = fixed_time;
    result.xyzt.x = proj_strtod (column (buf, columns[0]), &endptr);
    result.xyzt.y = proj_strtod (column (buf, columns[1]), &endptr);
    if (result.xyzt.z==HUGE_VAL)
        result.xyzt.z = proj_strtod (column (buf, columns[2]), &endptr);
    if (result.xyzt.t==HUGE_VAL)
        result.xyzt.t = proj_strtod (column (buf, columns[3]), &endptr);

    if (0!=errno)
        return err;

    errno = prev_errno;
    return result;
}


int print_output_line (FILE *fout, char *buf, PJ_COORD point) {
    char *c;
    if (HUGE_VAL!=point.xyzt.x)
        return fprintf (fout, "%20.15f  %20.15f  %20.15f  %20.15f\n", point.xyzt.x, point.xyzt.y, point.xyzt.z, point.xyzt.t);
    c = column (buf, 1);
    /* reflect comments and blanks */
    if (c && ((*c=='\0') || (*c=='#')))
        return fprintf (fout, "%s\n", buf);
    return 0;
}
