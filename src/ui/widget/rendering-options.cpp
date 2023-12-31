// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Author:
 *   Kees Cook <kees@outflux.net>
 *
 * Copyright (C) 2007 Kees Cook
 * Copyright (C) 2004 Bryce Harrington
 *
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */

#include "rendering-options.h"

#include <glibmm/i18n.h>

#include "preferences.h"
#include "util/units.h"

namespace Inkscape::UI::Widget {

void RenderingOptions::_toggled()
{
    _frame_bitmap.set_sensitive(as_bitmap());
}

RenderingOptions::RenderingOptions () :
      Gtk::Box (Gtk::ORIENTATION_VERTICAL),
      _frame_backends ( Glib::ustring(_("Backend")) ),
      _radio_vector ( Glib::ustring(_("Vector")) ),
      _radio_bitmap ( Glib::ustring(_("Bitmap")) ),
      _frame_bitmap ( Glib::ustring(_("Bitmap options")) ),
      _dpi( _("DPI"),
            Glib::ustring(_("Preferred resolution of rendering, "
                            "in dots per inch.")),
            1,
            Glib::ustring{},
            false)
{
    Inkscape::Preferences *prefs = Inkscape::Preferences::get();
    // set up tooltips
    _radio_vector.set_tooltip_text(
                        _("Render using Cairo vector operations.  "
                        "The resulting image is usually smaller in file "
                        "size and can be arbitrarily scaled, but some "
                        "filter effects will not be correctly rendered."));
    _radio_bitmap.set_tooltip_text(
                        _("Render everything as bitmap.  The resulting image "
                        "is usually larger in file size and cannot be "
                        "arbitrarily scaled without quality loss, but all "
                        "objects will be rendered exactly as displayed."));

    property_margin().set_value(2);

    Gtk::RadioButtonGroup group = _radio_vector.get_group ();
    _radio_bitmap.set_group (group);
    _radio_bitmap.signal_toggled().connect(sigc::mem_fun(*this, &RenderingOptions::_toggled));
    
    // default to vector operations
    if (prefs->getBool("/dialogs/printing/asbitmap", false)) {
        _radio_bitmap.set_active();
    } else {
        _radio_vector.set_active();
    }
    
    // configure default DPI
    _dpi.setRange(Inkscape::Util::Quantity::convert(1, "in", "pt"),2400.0);
    _dpi.setValue(prefs->getDouble("/dialogs/printing/dpi", 
                                   Inkscape::Util::Quantity::convert(1, "in", "pt")));
    _dpi.setIncrements(1.0,10.0);
    _dpi.setDigits(0);
    _dpi.update();

    // fill frames
    auto const box_vector = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);
    box_vector->set_border_width (2);
    box_vector->add (_radio_vector);
    box_vector->add (_radio_bitmap);
    _frame_backends.add (*box_vector);

    auto const box_bitmap = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    box_bitmap->set_border_width (2);
    box_bitmap->add (_dpi);
    _frame_bitmap.add (*box_bitmap);

    // fill up container
    add (_frame_backends);
    add (_frame_bitmap);

    // initialize states
    _toggled();

    show_all_children ();
}

bool
RenderingOptions::as_bitmap ()
{
    return _radio_bitmap.get_active();
}

double
RenderingOptions::bitmap_dpi ()
{
    return _dpi.getValue();
}

} // namespace Inkscape::UI::Widget

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:fileencoding=utf-8:textwidth=99 :
