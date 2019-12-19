/******************************************************************************
 * Project:  PROJ
 * Purpose:  Grid management
 * Author:   Even Rouault, <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2019, Even Rouault, <even.rouault at spatialys.com>
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

#ifndef GRIDS_HPP_INCLUDED
#define GRIDS_HPP_INCLUDED

#include <memory>
#include <vector>

#include "proj.h"
#include "proj/util.hpp"

NS_PROJ_START

struct ExtentAndRes {
    double westLon;  // in radian
    double southLat; // in radian
    double eastLon;  // in radian
    double northLat; // in radian
    double resLon;   // in radian
    double resLat;   // in radian

    bool fullWorldLongitude() const;
    bool contains(const ExtentAndRes &other) const;
    bool intersects(const ExtentAndRes &other) const;
};

// ---------------------------------------------------------------------------

class Grid {
  protected:
    std::string m_name;
    int m_width;
    int m_height;
    ExtentAndRes m_extent;

    Grid(const std::string &nameIn, int widthIn, int heightIn,
         const ExtentAndRes &extentIn);

  public:
    virtual ~Grid();

    int width() const { return m_width; }
    int height() const { return m_height; }
    const ExtentAndRes &extentAndRes() const { return m_extent; }
    const std::string &name() const { return m_name; }

    virtual bool isNullGrid() const { return false; }
};

// ---------------------------------------------------------------------------

class VerticalShiftGrid : public Grid {
  protected:
    std::vector<std::unique_ptr<VerticalShiftGrid>> m_children{};

  public:
    VerticalShiftGrid(const std::string &nameIn, int widthIn, int heightIn,
                      const ExtentAndRes &extentIn);

    const VerticalShiftGrid *gridAt(double lon, double lat) const;

    virtual bool isNodata(float /*val*/, double /* multiplier */) const;

    // x = 0 is western-most column, y = 0 is southern-most line
    virtual bool valueAt(int x, int y, float &out) const = 0;
};

// ---------------------------------------------------------------------------

class VerticalShiftGridSet {
  protected:
    std::string m_name{};
    std::string m_format{};
    std::vector<std::unique_ptr<VerticalShiftGrid>> m_grids{};

    VerticalShiftGridSet();

  public:
    virtual ~VerticalShiftGridSet();

    static std::unique_ptr<VerticalShiftGridSet>
    open(PJ_CONTEXT *ctx, const std::string &filename);

    const std::string &name() const { return m_name; }
    const std::string &format() const { return m_format; }
    const std::vector<std::unique_ptr<VerticalShiftGrid>> &grids() const {
        return m_grids;
    }
    const VerticalShiftGrid *gridAt(double lon, double lat) const;
};

// ---------------------------------------------------------------------------

class HorizontalShiftGrid : public Grid {
  protected:
    std::vector<std::unique_ptr<HorizontalShiftGrid>> m_children{};

  public:
    HorizontalShiftGrid(const std::string &nameIn, int widthIn, int heightIn,
                        const ExtentAndRes &extentIn);
    ~HorizontalShiftGrid() override;

    const HorizontalShiftGrid *gridAt(double lon, double lat) const;

    // x = 0 is western-most column, y = 0 is southern-most line
    virtual bool valueAt(int x, int y, float &lonShift,
                         float &latShift) const = 0;
};

// ---------------------------------------------------------------------------

class HorizontalShiftGridSet {
  protected:
    std::string m_name{};
    std::string m_format{};
    std::vector<std::unique_ptr<HorizontalShiftGrid>> m_grids{};

    HorizontalShiftGridSet();

  public:
    virtual ~HorizontalShiftGridSet();

    static std::unique_ptr<HorizontalShiftGridSet>
    open(PJ_CONTEXT *ctx, const std::string &filename);

    const std::string &name() const { return m_name; }
    const std::string &format() const { return m_format; }
    const std::vector<std::unique_ptr<HorizontalShiftGrid>> &grids() const {
        return m_grids;
    }
    const HorizontalShiftGrid *gridAt(double lon, double lat) const;
};

// ---------------------------------------------------------------------------

class GenericShiftGrid : public Grid {
  protected:
    std::vector<std::unique_ptr<GenericShiftGrid>> m_children{};

  public:
    GenericShiftGrid(const std::string &nameIn, int widthIn, int heightIn,
                     const ExtentAndRes &extentIn);

    ~GenericShiftGrid() override;

    const GenericShiftGrid *gridAt(double lon, double lat) const;

    virtual std::string unit(int sample) const = 0;

    virtual std::string description(int sample) const = 0;

    virtual std::string metadataItem(const std::string &key,
                                     int sample = -1) const = 0;

    virtual int samplesPerPixel() const = 0;

    // x = 0 is western-most column, y = 0 is southern-most line
    virtual bool valueAt(int x, int y, int sample, float &out) const = 0;
};

// ---------------------------------------------------------------------------

class GenericShiftGridSet {
  protected:
    std::string m_name{};
    std::string m_format{};
    std::vector<std::unique_ptr<GenericShiftGrid>> m_grids{};

    GenericShiftGridSet();

  public:
    virtual ~GenericShiftGridSet();

    static std::unique_ptr<GenericShiftGridSet>
    open(PJ_CONTEXT *ctx, const std::string &filename);

    const std::string &name() const { return m_name; }
    const std::string &format() const { return m_format; }
    const std::vector<std::unique_ptr<GenericShiftGrid>> &grids() const {
        return m_grids;
    }
    const GenericShiftGrid *gridAt(double lon, double lat) const;
};

// ---------------------------------------------------------------------------

typedef std::vector<std::unique_ptr<HorizontalShiftGridSet>> ListOfHGrids;
typedef std::vector<std::unique_ptr<VerticalShiftGridSet>> ListOfVGrids;
typedef std::vector<std::unique_ptr<GenericShiftGridSet>> ListOfGenericGrids;

ListOfVGrids proj_vgrid_init(PJ *P, const char *grids);
ListOfHGrids proj_hgrid_init(PJ *P, const char *grids);
ListOfGenericGrids proj_generic_grid_init(PJ *P, const char *grids);

double proj_vgrid_value(PJ *P, const ListOfVGrids &, PJ_LP lp,
                        double vmultiplier);
PJ_LP proj_hgrid_value(PJ *P, const ListOfHGrids &, PJ_LP lp);
PJ_LP proj_hgrid_apply(PJ *P, const ListOfHGrids &, PJ_LP lp,
                       PJ_DIRECTION direction);

NS_PROJ_END

#endif // GRIDS_HPP_INCLUDED