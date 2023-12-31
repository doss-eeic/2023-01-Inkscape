// SPDX-License-Identifier: GPL-2.0-or-later
/** \file
 * @brief LPE sketch effect implementation, see lpe-sketch.cpp.
 */
/* Authors:
 *   Jean-Francois Barraud <jf.barraud@gmail.com>
 *   Johan Engelen <j.b.c.engelen@utwente.nl>
 *
 * Copyright (C) 2007 Authors
 *
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */

#ifndef INKSCAPE_LPE_SKETCH_H
#define INKSCAPE_LPE_SKETCH_H

#include "live_effects/effect.h"
#include "live_effects/parameter/parameter.h"
#include "live_effects/parameter/random.h"
#include "live_effects/parameter/point.h"

#define LPE_SKETCH_USE_CONSTRUCTION_LINES
//#define LPE_SKETCH_USE_CURVATURE

namespace Inkscape {
namespace LivePathEffect {

class LPESketch : public Effect {
    
public:
    LPESketch(LivePathEffectObject *lpeobject);
    ~LPESketch() override;

    Geom::Piecewise<Geom::D2<Geom::SBasis> > doEffect_pwd2 (Geom::Piecewise<Geom::D2<Geom::SBasis> > const & pwd2_in) override;

    void doBeforeEffect (SPLPEItem const* lpeitem) override;
    Gtk::Widget *newWidget() override;

private:
    // add the parameters for your effect here:
    //PointParam testpointA;
    ScalarParam nbiter_approxstrokes;
    ScalarParam strokelength;
    RandomParam strokelength_rdm;
    ScalarParam strokeoverlap;
    RandomParam strokeoverlap_rdm;
    RandomParam ends_tolerance;
    RandomParam parallel_offset;
    RandomParam tremble_size;
    ScalarParam tremble_frequency;

#ifdef LPE_SKETCH_USE_CONSTRUCTION_LINES
    ScalarParam nbtangents;
    ScalarParam tgtscale;
    ScalarParam tgtlength;
    RandomParam tgtlength_rdm;
    RandomParam tgt_places_rdmness;
#ifdef LPE_SKETCH_USE_CURVATURE
    ScalarParam min_curvature;
    ScalarParam max_curvature;
#endif
#endif
    LPESketch(const LPESketch&) = delete;
    LPESketch& operator=(const LPESketch&) = delete;

    Geom::Piecewise<Geom::D2<Geom::SBasis> > computePerturbation (double s0, double s1);

};

} //namespace LivePathEffect
} //namespace Inkscape

#endif

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:fileencoding=utf-8:textwidth=99 :
