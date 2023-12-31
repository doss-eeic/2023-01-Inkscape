// SPDX-License-Identifier: GPL-2.0-or-later
/* Authors:
 *   Thomas Holder
 *   Anshudhar Kumar Singh <anshudhar2001@gmail.com>
 *
 * Copyright (C) 2020-2021 Authors
 *
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */

#ifndef SEEN_INKSCAPE_UI_WIDGET_SCROLLPROTECTED_H
#define SEEN_INKSCAPE_UI_WIDGET_SCROLLPROTECTED_H

#include <glibmm/refptr.h>
#include "scroll-utils.h"

namespace Gtk {
class Builder;
} // namespace Gtk

namespace Inkscape::UI::Widget {

// TODO: GTK4: shouldnʼt be needed as in-progress scroll on window is not ‘stolen’ by child anymore
/**
 * A class decorator which blocks the scroll event if the widget does not have
 * focus and any ancestor is a scrollable window, and SHIFT is not pressed.
 *
 * For custom scroll event handlers, derived classes must implement
 * on_safe_scroll_event instead of on_scroll_event. Directly connecting to
 * signal_scroll_event() will bypass the scroll protection.
 *
 * @tparam Base A subclass of Gtk::Widget
 */
template <typename Base>
class ScrollProtected : public Base
{
public:
    using Base::Base;
    using typename Base::BaseObjectType;

    ScrollProtected()
        : Base()
    {}

    ScrollProtected(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &refGlade)
        : Base(cobject){};

private:
    /**
     * Event handler for "safe" scroll events which are only triggered if:
     * - the widget has focus
     * - or the widget has no scrolled window ancestor
     * - or the Shift key is pressed
     */
    virtual bool on_safe_scroll_event(GdkEventScroll *event)
    {
        return Base::on_scroll_event(event);
    }

    bool on_scroll_event(GdkEventScroll *event) final
    {
        if (!scrolling_allowed(this, event)) {
            return false;
        }
        return on_safe_scroll_event(event);
    }
};

} // namespace Inkscape::UI::Widget

#endif // SEEN_INKSCAPE_UI_WIDGET_SCROLLPROTECTED_H

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
