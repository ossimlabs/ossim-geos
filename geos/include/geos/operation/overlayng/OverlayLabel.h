/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2020 Paul Ramsey <pramsey@cleverelephant.ca>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#pragma once

#include <geos/geom/Location.h>
#include <geos/geom/Position.h>
#include <geos/export.h>


namespace geos {      // geos.
namespace operation { // geos.operation
namespace overlayng { // geos.operation.overlayng

/**
* A label for a pair of {@link OverlayEdge}s which records
* the topological information for the edge
* in the {@link OverlayGraph} containing it.
* The label is shared between both OverlayEdges
* of a symmetric pair.
* Accessors for orientation-sensitive information
* require the orientation of the containing OverlayEdge.
*
* A label contains the topological {@link Location}s for
* the two overlay input geometries.
* A labelled input geometry may be either a Line or an Area.
* In both cases, the label locations are populated
* with the locations for the edge {@link Position}s
* once they are computed by topological evaluation.
* The label also records the dimension of each geometry,
* and in the case of area boundary edges, the role
* of the originating ring (which allows
* determination of the edge role in collapse cases).
*
* For each input geometry, the label indicates that an edge is in one of the following states
* (denoted by the <code>dim</code> field).
* Each state has some additional information about the edge.
*
* <li>A <b>Boundary</b> edge of an input Area (polygon)
*   <ul>
*   <li><code>dim</code> = DIM_BOUNDARY</li>
*   <li><code>locLeft, locRight</code> : the locations of the edge sides for the input Area</li>
*   <li><code>isHole</code> : whether the
* edge was in a shell or a hole</li>
*   </ul>
* </li>
* <li>A <b>Collapsed</b> edge of an input Area
* (which had two or more parent edges)
*   <ul>
*   <li><code>dim</code> = DIM_COLLAPSE</li>
*   <li><code>locLine</code> : the location of the
* edge relative to the input Area</li>
*   <li><code>isHole</code> : whether some
* contributing edge was in a shell (<code>false</code>),
* or otherwise that all were in holes</li> (<code>true</code>)
*   </ul>
* </li>
* <li>An edge from an input <b>Line</b>
*   <ul>
*   <li><code>dim</code> = DIM_LINE</li>
*   <li><code>locLine</code> : initialized to LOC_UNKNOWN,
*          to simplify logic.</li>
*   </ul>
* </li>
* <li>An edge which is <b>Not Part</b> of an input geometry
* (and thus must be part of the other geometry).
*   <ul>
*   <li><code>dim</code> = NOT_PART</li>
*   </ul>
* </li>
* </ul>
* Note that:
* <ul>
* <li>an edge cannot be both a Collapse edge and a Line edge in the same input geometry,
* because each input geometry must be homogeneous.
* <li>an edge may be an Boundary edge in one input geometry
* and a Line or Collapse edge in the other input.
* </ul>
* @author Martin Davis
*/

using geos::geom::Location;
using geos::geom::Position;


class GEOS_DLL OverlayLabel {

private:

    // Members
    int aDim = DIM_NOT_PART;
    bool aIsHole = false;
    Location aLocLeft = LOC_UNKNOWN;
    Location aLocRight = LOC_UNKNOWN;
    Location aLocLine = LOC_UNKNOWN;
    int bDim = DIM_NOT_PART;
    bool bIsHole = false;
    Location bLocLeft = LOC_UNKNOWN;
    Location bLocRight = LOC_UNKNOWN;
    Location bLocLine = LOC_UNKNOWN;


    std::string dimensionSymbol(int dim) const;
    void locationString(int index, bool isForward, std::ostream& os) const;


public:

    static constexpr Location LOC_UNKNOWN = Location::UNDEF;

    enum {
        DIM_UNKNOWN = -1,
        DIM_NOT_PART = -1,
        DIM_LINE = 1,
        DIM_BOUNDARY = 2,
        DIM_COLLAPSE = 3
    };

    OverlayLabel()
        : aDim(DIM_NOT_PART)
        , aIsHole(false)
        , aLocLeft(LOC_UNKNOWN)
        , aLocRight(LOC_UNKNOWN)
        , aLocLine(LOC_UNKNOWN)
        , bDim(DIM_NOT_PART)
        , bIsHole(false)
        , bLocLeft(LOC_UNKNOWN)
        , bLocRight(LOC_UNKNOWN)
        , bLocLine(LOC_UNKNOWN) {};

    OverlayLabel(int p_index)
        : OverlayLabel()
    {
        initLine(p_index);
    };

    OverlayLabel(int p_index, Location p_locLeft, Location p_locRight, bool p_isHole)
        : OverlayLabel()
    {
        initBoundary(p_index, p_locLeft, p_locRight, p_isHole);
    };

    int dimension(int index) const { return index == 0 ? aDim : bDim; };
    void initBoundary(int index, Location locLeft, Location locRight, bool p_isHole);
    void initCollapse(int index, bool p_isHole);
    void initLine(int index);
    void initNotPart(int index);

    /**
    * Sets the line location.
    *
    * This is used to set the locations for linear edges
    * encountered during area label propagation.
    *
    * @param index source to update
    * @param loc location to set
    */
    void setLocationLine(int index, Location loc);
    void setLocationAll(int index, Location loc);
    void setLocationCollapse(int index);

    /*
    * Tests whether at least one of the sources is a Line.
    *
    * @return true if at least one source is a line
    */
    bool isLine() const;
    bool isLine(int index) const;
    bool isLinear(int index) const;
    bool isKnown(int index) const;
    bool isNotPart(int index) const;
    bool isBoundaryEither() const;
    bool isBoundaryBoth() const;

    /**
    * Tests if the label is for a collapsed
    * edge of an area
    * which is coincident with the boundary of the other area.
    *
    * @return true if the label is for a collapse coincident with a boundary
    */
    bool isBoundaryCollapse() const;
    bool isBoundary(int index) const;
    bool isLineLocationUnknown(int index) const;

    /**
    * Tests if a line edge is inside
    * @param index
    * @return
    */
    bool isLineInArea(int index) const;
    bool isHole(int index) const;
    bool isCollapse(int index) const;
    Location getLineLocation(int index) const;

    /**
    * Tests if a line is in the interior of a source geometry.
    *
    * @param index source geometry
    * @return true if the label is a line and is interior
    */
    bool isLineInterior(int index) const;

    /**
    * Gets the location for this label for either
    * a Boundary or a Line edge.
    * This supports a simple determination of
    * whether the edge should be included as a result edge.
    *
    * @param index the source index
    * @param position the position for a boundary label
    * @param isForward the direction for a boundary label
    * @return the location for the specified position
    */
    Location getLocationBoundaryOrLine(int index, int position, bool isForward) const;

    /**
    * Gets the linear location for the given source.
    *
    * @param index the source index
    * @return the linear location for the source
    */
    Location getLocation(int index) const;
    Location getLocation(int index, int position, bool isForward) const;
    bool hasSides(int index) const;

    OverlayLabel copyFlip() const;
    OverlayLabel copy() const;


    friend std::ostream& operator<<(std::ostream& os, const OverlayLabel& ol);
    void toString(bool isForward, std::ostream& os) const;


};


} // namespace geos.operation.overlayng
} // namespace geos.operation
} // namespace geos

