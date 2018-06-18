/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  ISO19111:2018 implementation
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Even Rouault <even dot rouault at spatialys dot com>
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

#ifndef FROM_PROJ_CPP
#define FROM_PROJ_CPP
#endif

#include "proj/crs.hpp"
#include "proj/common.hpp"
#include "proj/coordinateoperation.hpp"
#include "proj/coordinatesystem.hpp"
#include "proj/coordinatesystem_internal.hpp"
#include "proj/internal.hpp"
#include "proj/io.hpp"
#include "proj/io_internal.hpp"
#include "proj/util.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace NS_PROJ::common;
using namespace NS_PROJ::crs;
using namespace NS_PROJ::cs;
using namespace NS_PROJ::datum;
using namespace NS_PROJ::internal;
using namespace NS_PROJ::io;
using namespace NS_PROJ::metadata;
using namespace NS_PROJ::operation;
using namespace NS_PROJ::util;

// ---------------------------------------------------------------------------

CRS::CRS() = default;

// ---------------------------------------------------------------------------

CRS::~CRS() = default;

// ---------------------------------------------------------------------------

GeographicCRSPtr CRS::extractGeographicCRS(CRSNNPtr crs) {
    CRSPtr transformSourceCRS;
    auto geogCRS = util::nn_dynamic_pointer_cast<GeographicCRS>(crs);
    if (geogCRS) {
        return geogCRS;
    }
    auto projCRS = util::nn_dynamic_pointer_cast<ProjectedCRS>(crs);
    if (projCRS) {
        return util::nn_dynamic_pointer_cast<GeographicCRS>(projCRS->baseCRS());
    }
    auto compoundCRS = util::nn_dynamic_pointer_cast<CompoundCRS>(crs);
    if (compoundCRS) {
        for (const auto &subCrs : compoundCRS->componentReferenceSystems()) {
            geogCRS = util::nn_dynamic_pointer_cast<GeographicCRS>(subCrs);
            if (geogCRS) {
                return geogCRS;
            }
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct SingleCRS::Private {
    DatumPtr datum{};
    DatumEnsemblePtr datumEnsemble{};
    CoordinateSystemNNPtr coordinateSystem;

    Private(const DatumNNPtr &datumIn, const CoordinateSystemNNPtr &csIn)
        : datum(datumIn), coordinateSystem(csIn) {}
    Private(const DatumPtr &datumIn, const CoordinateSystemNNPtr &csIn)
        : datum(datumIn), coordinateSystem(csIn) {}
};
//! @endcond

// ---------------------------------------------------------------------------

SingleCRS::SingleCRS(const DatumNNPtr &datumIn,
                     const CoordinateSystemNNPtr &csIn)
    : d(internal::make_unique<Private>(datumIn, csIn)) {}

// ---------------------------------------------------------------------------

SingleCRS::SingleCRS(const DatumPtr &datumIn, const CoordinateSystemNNPtr &csIn)
    : d(internal::make_unique<Private>(datumIn, csIn)) {}

// ---------------------------------------------------------------------------

#ifdef notdef
SingleCRS::SingleCRS(const SingleCRS &other)
    : // CRS(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

SingleCRS::~SingleCRS() = default;

// ---------------------------------------------------------------------------

const DatumPtr &SingleCRS::datum() const { return d->datum; }

// ---------------------------------------------------------------------------

const DatumEnsemblePtr &SingleCRS::datumEnsemble() const {
    return d->datumEnsemble;
}

// ---------------------------------------------------------------------------

const CoordinateSystemNNPtr &SingleCRS::coordinateSystem() const {
    return d->coordinateSystem;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeodeticCRS::Private {
    std::vector<PointMotionOperationNNPtr> velocityModel{};
};
//! @endcond

// ---------------------------------------------------------------------------

GeodeticCRS::GeodeticCRS(const GeodeticReferenceFrameNNPtr &datumIn,
                         const EllipsoidalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeodeticCRS::GeodeticCRS(const GeodeticReferenceFrameNNPtr &datumIn,
                         const SphericalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

GeodeticCRS::GeodeticCRS(const GeodeticReferenceFrameNNPtr &datumIn,
                         const CartesianCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

#ifdef notdef
GeodeticCRS::GeodeticCRS(const GeodeticCRS &other)
    : SingleCRS(other), d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

GeodeticCRS::~GeodeticCRS() = default;

// ---------------------------------------------------------------------------

const GeodeticReferenceFrameNNPtr GeodeticCRS::datum() const {
    return NN_CHECK_THROW(std::dynamic_pointer_cast<GeodeticReferenceFrame>(
        SingleCRS::getPrivate()->datum));
}

// ---------------------------------------------------------------------------

const std::vector<PointMotionOperationNNPtr> &
GeodeticCRS::velocityModel() const {
    return d->velocityModel;
}

// ---------------------------------------------------------------------------

bool GeodeticCRS::isGeocentric() const {
    return coordinateSystem()->getWKT2Type() == "Cartesian" &&
           coordinateSystem()->axisList().size() == 3 &&
           coordinateSystem()->axisList()[0]->axisDirection() ==
               AxisDirection::GEOCENTRIC_X &&
           coordinateSystem()->axisList()[1]->axisDirection() ==
               AxisDirection::GEOCENTRIC_Y &&
           coordinateSystem()->axisList()[2]->axisDirection() ==
               AxisDirection::GEOCENTRIC_Z;
}

// ---------------------------------------------------------------------------

GeodeticCRSNNPtr GeodeticCRS::create(const PropertyMap &properties,
                                     const GeodeticReferenceFrameNNPtr &datum,
                                     const EllipsoidalCSNNPtr &cs) {
    auto crs(GeodeticCRS::nn_make_shared<GeodeticCRS>(datum, cs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

GeodeticCRSNNPtr GeodeticCRS::create(const PropertyMap &properties,
                                     const GeodeticReferenceFrameNNPtr &datum,
                                     const SphericalCSNNPtr &cs) {
    auto crs(GeodeticCRS::nn_make_shared<GeodeticCRS>(datum, cs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

GeodeticCRSNNPtr GeodeticCRS::create(const PropertyMap &properties,
                                     const GeodeticReferenceFrameNNPtr &datum,
                                     const CartesianCSNNPtr &cs) {
    auto crs(GeodeticCRS::nn_make_shared<GeodeticCRS>(datum, cs));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

std::string GeodeticCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? ((formatter->use2018Keywords() &&
                                    dynamic_cast<const GeographicCRS *>(this))
                                       ? WKTConstants::GEOGCRS
                                       : WKTConstants::GEODCRS)
                                : isGeocentric() ? WKTConstants::GEOCCS
                                                 : WKTConstants::GEOGCS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));
    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty()) {
        formatter->pushAxisAngularUnit(
            util::nn_make_shared<UnitOfMeasure>(axisList[0]->axisUnitID()));
    }
    datum()->exportToWKT(formatter);
    datum()->primeMeridian()->exportToWKT(formatter);
    if (!axisList.empty()) {
        formatter->popAxisAngularUnit();
    }
    if (!isWKT2) {
        if (!axisList.empty()) {
            axisList[0]->axisUnitID().exportToWKT(formatter);
        }
    }
    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct GeographicCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

GeographicCRS::GeographicCRS(const GeodeticReferenceFrameNNPtr &datumIn,
                             const EllipsoidalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), GeodeticCRS(datumIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

#ifdef notdef
GeographicCRS::GeographicCRS(const GeographicCRS &other)
    : SingleCRS(other), GeodeticCRS(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

GeographicCRS::~GeographicCRS() = default;

// ---------------------------------------------------------------------------

const EllipsoidalCSNNPtr GeographicCRS::coordinateSystem() const {
    return NN_CHECK_THROW(util::nn_dynamic_pointer_cast<EllipsoidalCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr
GeographicCRS::create(const PropertyMap &properties,
                      const GeodeticReferenceFrameNNPtr &datum,
                      const EllipsoidalCSNNPtr &cs) {
    GeographicCRSNNPtr gcrs(
        GeographicCRS::nn_make_shared<GeographicCRS>(datum, cs));
    gcrs->setProperties(properties);
    return gcrs;
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4326() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4326)
        .set(IdentifiedObject::NAME_KEY, "WGS 84");
    return create(
        propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
        EllipsoidalCS::createLatitudeLongitude(UnitOfMeasure::DEGREE));
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4979() {
    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4979)
        .set(IdentifiedObject::NAME_KEY, "WGS 84");
    return create(propertiesCRS, GeodeticReferenceFrame::EPSG_6326,
                  EllipsoidalCS::createLatitudeLongitudeEllipsoidalHeight(
                      UnitOfMeasure::DEGREE, UnitOfMeasure::METRE));
}

// ---------------------------------------------------------------------------

GeographicCRSNNPtr GeographicCRS::createEPSG_4807() {
    PropertyMap propertiesEllps;
    propertiesEllps.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 6807)
        .set(IdentifiedObject::NAME_KEY, "Clarke 1880 (IGN)");
    auto ellps(Ellipsoid::createFlattenedSphere(
        propertiesEllps, Length(6378249.2), Scale(293.4660212936269)));

    auto pm(PrimeMeridian::create(PropertyMap()
                                      .set(Identifier::CODESPACE_KEY, "EPSG")
                                      .set(Identifier::CODE_KEY, 8903)
                                      .set(IdentifiedObject::NAME_KEY, "Paris"),
                                  Angle(2.5969213, UnitOfMeasure::GRAD)));

    auto axisLat(CoordinateSystemAxis::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, AxisName::Latitude),
        AxisAbbreviation::lat, AxisDirection::NORTH, UnitOfMeasure::GRAD));

    auto axisLong(CoordinateSystemAxis::create(
        PropertyMap().set(IdentifiedObject::NAME_KEY, AxisName::Longitude),
        AxisAbbreviation::lon, AxisDirection::EAST, UnitOfMeasure::GRAD));

    auto cs(EllipsoidalCS::create(PropertyMap(), axisLat, axisLong));

    PropertyMap propertiesDatum;
    propertiesDatum.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 6807)
        .set(IdentifiedObject::NAME_KEY,
             "Nouvelle_Triangulation_Francaise_Paris");
    auto datum(GeodeticReferenceFrame::create(propertiesDatum, ellps,
                                              optional<std::string>(), pm));

    PropertyMap propertiesCRS;
    propertiesCRS.set(Identifier::CODESPACE_KEY, "EPSG")
        .set(Identifier::CODE_KEY, 4807)
        .set(IdentifiedObject::NAME_KEY, "NTF (Paris)");
    auto gcrs(create(propertiesCRS, datum, cs));
    return gcrs;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct VerticalCRS::Private {
    std::vector<TransformationNNPtr> geoidModel{};
    std::vector<PointMotionOperationNNPtr> velocityModel{};
};
//! @endcond

// ---------------------------------------------------------------------------

VerticalCRS::VerticalCRS(const VerticalReferenceFrameNNPtr &datumIn,
                         const VerticalCSNNPtr &csIn)
    : SingleCRS(datumIn, csIn), d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

#ifdef notdef
VerticalCRS::VerticalCRS(const VerticalCRS &other)
    : SingleCRS(other), d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

VerticalCRS::~VerticalCRS() = default;

// ---------------------------------------------------------------------------

const VerticalReferenceFramePtr VerticalCRS::datum() const {
    return std::dynamic_pointer_cast<VerticalReferenceFrame>(
        SingleCRS::getPrivate()->datum);
}

// ---------------------------------------------------------------------------

const std::vector<TransformationNNPtr> &VerticalCRS::geoidModel() const {
    return d->geoidModel;
}

// ---------------------------------------------------------------------------

const std::vector<PointMotionOperationNNPtr> &
VerticalCRS::velocityModel() const {
    return d->velocityModel;
}

// ---------------------------------------------------------------------------

const VerticalCSNNPtr VerticalCRS::coordinateSystem() const {
    return NN_CHECK_THROW(util::nn_dynamic_pointer_cast<VerticalCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

std::string VerticalCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? WKTConstants::VERTCRS : WKTConstants::VERT_CS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));
    if (datum()) {
        datum()->exportToWKT(formatter);
    } else {
        throw FormattingException("Missing VDATUM");
    }
    auto &axisList = coordinateSystem()->axisList();
    if (!isWKT2 && !axisList.empty()) {
        axisList[0]->axisUnitID().exportToWKT(formatter);
    }
    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

VerticalCRSNNPtr VerticalCRS::create(const PropertyMap &properties,
                                     const VerticalReferenceFrameNNPtr &datumIn,
                                     const VerticalCSNNPtr &csIn) {
    auto crs(VerticalCRS::nn_make_shared<VerticalCRS>(datumIn, csIn));
    crs->setProperties(properties);
    return crs;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct DerivedCRS::Private {
    SingleCRSNNPtr baseCRS_;
    ConversionNNPtr derivingConversion_;

    Private(const SingleCRSNNPtr &baseCRSIn,
            const operation::ConversionNNPtr &derivingConversionIn)
        : baseCRS_(baseCRSIn), derivingConversion_(derivingConversionIn) {}
};

//! @endcond

// ---------------------------------------------------------------------------

// DerivedCRS is an abstract class, that virtually inherits from SingleCRS
// Consequently the base constructor in SingleCRS will never be called by
// that constructor. clang -Wabstract-vbase-init and VC++ underline this, but
// other
// compilers will complain if we don't call the base constructor.

DerivedCRS::DerivedCRS(const SingleCRSNNPtr &baseCRSIn,
                       const ConversionNNPtr &derivingConversionIn,
                       const CoordinateSystemNNPtr &
#if !defined(COMPILER_WARNS_ABOUT_ABSTRACT_VBASE_INIT)
                           cs
#endif
                       )
    :
#if !defined(COMPILER_WARNS_ABOUT_ABSTRACT_VBASE_INIT)
      SingleCRS(baseCRSIn->datum(), cs),
#endif
      d(internal::make_unique<Private>(
          baseCRSIn, Conversion::create(derivingConversionIn))) {
}

// ---------------------------------------------------------------------------

#ifdef notdef
DerivedCRS::DerivedCRS(const DerivedCRS &other)
    :
#if !defined(COMPILER_WARNS_ABOUT_ABSTRACT_VBASE_INIT)
      SingleCRS(other),
#endif
      d(internal::make_unique<Private>(*other.d)) {
}
#endif

// ---------------------------------------------------------------------------

DerivedCRS::~DerivedCRS() = default;

// ---------------------------------------------------------------------------

const SingleCRSNNPtr &DerivedCRS::baseCRS() const { return d->baseCRS_; }

// ---------------------------------------------------------------------------

const ConversionNNPtr &DerivedCRS::derivingConversion() const {
    return d->derivingConversion_;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct ProjectedCRS::Private {};
//! @endcond

// ---------------------------------------------------------------------------

ProjectedCRS::ProjectedCRS(const GeodeticCRSNNPtr &baseCRSIn,
                           const ConversionNNPtr &derivingConversionIn,
                           const CartesianCSNNPtr &csIn)
    : SingleCRS(baseCRSIn->datum(), csIn),
      DerivedCRS(baseCRSIn, derivingConversionIn, csIn),
      d(internal::make_unique<Private>()) {}

// ---------------------------------------------------------------------------

#ifdef notdef
ProjectedCRS::ProjectedCRS(const ProjectedCRS &other)
    : SingleCRS(other), DerivedCRS(other),
      d(internal::make_unique<Private>(*other.d)) {}
#endif

// ---------------------------------------------------------------------------

ProjectedCRS::~ProjectedCRS() = default;

// ---------------------------------------------------------------------------

const GeodeticCRSNNPtr ProjectedCRS::baseCRS() const {
    return NN_CHECK_ASSERT(util::nn_dynamic_pointer_cast<GeodeticCRS>(
        DerivedCRS::getPrivate()->baseCRS_));
}

// ---------------------------------------------------------------------------

const CartesianCSNNPtr ProjectedCRS::coordinateSystem() const {
    return NN_CHECK_THROW(util::nn_dynamic_pointer_cast<CartesianCS>(
        SingleCRS::getPrivate()->coordinateSystem));
}

// ---------------------------------------------------------------------------

std::string ProjectedCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? WKTConstants::PROJCRS : WKTConstants::PROJCS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));

    auto &geodeticCRSAxisList = baseCRS()->coordinateSystem()->axisList();

    if (isWKT2) {
        formatter->startNode(
            (formatter->use2018Keywords() &&
             dynamic_cast<const GeographicCRS *>(baseCRS().get()))
                ? WKTConstants::BASEGEOGCRS
                : WKTConstants::BASEGEODCRS,
            !baseCRS()->identifiers().empty());
        formatter->addQuotedString(*(baseCRS()->name()->description()));
        baseCRS()->datum()->exportToWKT(formatter);
        // insert ellipsoidal cs unit when the units of the map
        // projection angular parameters are not explicitly given within those
        // parameters. See
        // http://docs.opengeospatial.org/is/12-063r5/12-063r5.html#61
        if (formatter->primeMeridianOrParameterUnitOmittedIfSameAsAxis() &&
            !geodeticCRSAxisList.empty()) {
            geodeticCRSAxisList[0]->axisUnitID().exportToWKT(formatter);
        }
        baseCRS()->datum()->primeMeridian()->exportToWKT(formatter);
        formatter->endNode();
    } else {
        baseCRS()->exportToWKT(formatter);
    }

    if (!isWKT2) {
        formatter->setOutputConversionNode(false);
    }

    auto &axisList = coordinateSystem()->axisList();
    if (!axisList.empty()) {
        formatter->pushAxisLinearUnit(
            util::nn_make_shared<UnitOfMeasure>(axisList[0]->axisUnitID()));
    }
    if (!geodeticCRSAxisList.empty()) {
        formatter->pushAxisAngularUnit(util::nn_make_shared<UnitOfMeasure>(
            geodeticCRSAxisList[0]->axisUnitID()));
    }

    derivingConversion()->exportToWKT(formatter);

    if (!geodeticCRSAxisList.empty()) {
        formatter->popAxisAngularUnit();
    }
    if (!axisList.empty()) {
        formatter->popAxisLinearUnit();
    }

    if (!isWKT2) {
        formatter->setOutputConversionNode(true);
        if (!axisList.empty()) {
            axisList[0]->axisUnitID().exportToWKT(formatter);
        }
    }

    coordinateSystem()->exportToWKT(formatter);
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

ProjectedCRSNNPtr ProjectedCRS::create(
    const PropertyMap &properties, const GeodeticCRSNNPtr &baseCRSIn,
    const ConversionNNPtr &derivingConversionIn, const CartesianCSNNPtr &csIn) {
    auto crs = ProjectedCRS::nn_make_shared<ProjectedCRS>(
        baseCRSIn, derivingConversionIn, csIn);
    crs->setProperties(properties);
    crs->derivingConversion()->setWeakSourceTargetCRS(
        static_cast<const GeodeticCRSPtr>(baseCRSIn),
        static_cast<const ProjectedCRSPtr>(crs));
    return crs;
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct CompoundCRS::Private {
    std::vector<CRSNNPtr> components_{};
};
//! @endcond

// ---------------------------------------------------------------------------

CompoundCRS::CompoundCRS(const std::vector<CRSNNPtr> &components)
    : CRS(), d(internal::make_unique<Private>()) {
    d->components_ = components;
}

// ---------------------------------------------------------------------------

CompoundCRS::~CompoundCRS() = default;

// ---------------------------------------------------------------------------

std::vector<SingleCRSNNPtr> CompoundCRS::componentReferenceSystems() const {
    // Flatten the potential hierarchy to return only SingleCRS
    std::vector<SingleCRSNNPtr> res;
    for (const auto &crs : d->components_) {
        auto childCompound = util::nn_dynamic_pointer_cast<CompoundCRS>(crs);
        if (childCompound) {
            auto childFlattened = childCompound->componentReferenceSystems();
            res.insert(res.end(), childFlattened.begin(), childFlattened.end());
        } else {
            auto singleCRS = util::nn_dynamic_pointer_cast<SingleCRS>(crs);
            if (singleCRS) {
                res.push_back(NN_CHECK_ASSERT(singleCRS));
            }
        }
    }
    return res;
}

// ---------------------------------------------------------------------------

CompoundCRSNNPtr CompoundCRS::create(const PropertyMap &properties,
                                     const std::vector<CRSNNPtr> &components) {
    auto compoundCRS(CompoundCRS::nn_make_shared<CompoundCRS>(components));
    compoundCRS->setProperties(properties);
    if (properties.find(IdentifiedObject::NAME_KEY) == properties.end()) {
        std::string name;
        for (const auto &crs : components) {
            if (!name.empty()) {
                name += " + ";
            }
            if (crs->name()->description()) {
                name += *crs->name()->description();
            } else {
                name += "unnamed";
            }
        }
        PropertyMap propertyName;
        propertyName.set(IdentifiedObject::NAME_KEY, name);
        compoundCRS->setProperties(propertyName);
    }

    return compoundCRS;
}

// ---------------------------------------------------------------------------

std::string CompoundCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    formatter->startNode(isWKT2 ? WKTConstants::COMPOUNDCRS
                                : WKTConstants::COMPD_CS,
                         !identifiers().empty());
    formatter->addQuotedString(*(name()->description()));
    if (isWKT2) {
        for (const auto &crs : componentReferenceSystems()) {
            crs->exportToWKT(formatter);
        }
    } else {
        for (const auto &crs : d->components_) {
            crs->exportToWKT(formatter);
        }
    }
    ObjectUsage::_exportToWKT(formatter);
    formatter->endNode();
    return formatter->toString();
}

// ---------------------------------------------------------------------------

//! @cond Doxygen_Suppress
struct BoundCRS::Private {
    CRSNNPtr baseCRS_;
    CRSNNPtr hubCRS_;
    TransformationNNPtr transformation_;

    Private(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
            const TransformationNNPtr &transformationIn)
        : baseCRS_(baseCRSIn), hubCRS_(hubCRSIn),
          transformation_(transformationIn) {}
};

// ---------------------------------------------------------------------------

BoundCRS::BoundCRS(const CRSNNPtr &baseCRSIn, const CRSNNPtr &hubCRSIn,
                   const TransformationNNPtr &transformationIn)
    : d(internal::make_unique<Private>(baseCRSIn, hubCRSIn, transformationIn)) {
}

// ---------------------------------------------------------------------------

BoundCRS::~BoundCRS() = default;

// ---------------------------------------------------------------------------

const CRSNNPtr &BoundCRS::baseCRS() const { return d->baseCRS_; }

// ---------------------------------------------------------------------------

const CRSNNPtr &BoundCRS::hubCRS() const { return d->hubCRS_; }

// ---------------------------------------------------------------------------

const TransformationNNPtr &BoundCRS::transformation() const {
    return d->transformation_;
}

// ---------------------------------------------------------------------------

BoundCRSNNPtr BoundCRS::create(const CRSNNPtr &baseCRSIn,
                               const CRSNNPtr &hubCRSIn,
                               const TransformationNNPtr &transformationIn) {
    return BoundCRS::nn_make_shared<BoundCRS>(baseCRSIn, hubCRSIn,
                                              transformationIn);
}

// ---------------------------------------------------------------------------

BoundCRSNNPtr
BoundCRS::createFromTOWGS84(const CRSNNPtr &baseCRSIn,
                            const std::vector<double> TOWGS84Parameters) {
    return BoundCRS::nn_make_shared<BoundCRS>(
        baseCRSIn, GeographicCRS::EPSG_4326,
        Transformation::createTOWGS84(baseCRSIn, TOWGS84Parameters));
}

// ---------------------------------------------------------------------------

std::string BoundCRS::exportToWKT(WKTFormatterNNPtr formatter) const {
    const bool isWKT2 = formatter->version() == WKTFormatter::Version::WKT2;
    if (isWKT2) {
        formatter->startNode(WKTConstants::BOUNDCRS, false);
        formatter->startNode(WKTConstants::SOURCECRS, false);
        baseCRS()->exportToWKT(formatter);
        formatter->endNode();
        formatter->startNode(WKTConstants::TARGETCRS, false);
        hubCRS()->exportToWKT(formatter);
        formatter->endNode();
        formatter->setAbridgedTransformation(true);
        transformation()->exportToWKT(formatter);
        formatter->setAbridgedTransformation(false);
        formatter->endNode();
    } else {
        if (nn_dynamic_pointer_cast<GeographicCRS>(hubCRS()) == nullptr ||
            *(hubCRS()->name()->description()) != "WGS 84") {
            throw FormattingException(
                "Cannot export BoundCRS with non-WGS 84 hub CRS in WKT1");
        }
        auto params = transformation()->getTOWGS84Parameters();
        formatter->setTOWGS84Parameters(params);
        baseCRS()->exportToWKT(formatter);
        formatter->setTOWGS84Parameters(std::vector<double>());
    }
    return formatter->toString();
}
// ---------------------------------------------------------------------------

//! @endcond
