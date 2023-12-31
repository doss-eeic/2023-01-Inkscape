// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Inkscape::UI::Widget::UnitTracker
 * Simple mediator to synchronize changes to unit menus
 *
 * Authors:
 *   Jon A. Cruz <jon@joncruz.org>
 *   Matthew Petroff <matthew@mpetroff.net>
 *
 * Copyright (C) 2007 Jon A. Cruz
 * Copyright (C) 2013 Matthew Petroff
 *
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */

#ifndef INKSCAPE_UI_WIDGET_UNIT_TRACKER_H
#define INKSCAPE_UI_WIDGET_UNIT_TRACKER_H

#include <map>
#include <vector>
#include <glibmm/refptr.h>

#include "util/units.h"

using Inkscape::Util::Unit;
using Inkscape::Util::UnitType;

typedef struct _GObject       GObject;
typedef struct _GtkAdjustment GtkAdjustment;
typedef struct _GtkListStore  GtkListStore;

namespace Gtk {
class ListStore;
} // namespace Gtk

namespace Inkscape::UI::Widget {

class ComboToolItem;

class UnitTracker {
public:
    UnitTracker(UnitType unit_type);
    virtual ~UnitTracker();

    bool isUpdating() const;

    void setActiveUnit(Inkscape::Util::Unit const *unit);
    void setActiveUnitByAbbr(gchar const *abbr);
    void setActiveUnitByLabel(Glib::ustring label);
    Inkscape::Util::Unit const * getActiveUnit() const;

    void addUnit(Inkscape::Util::Unit const *u);
    void addAdjustment(GtkAdjustment *adj);
    void prependUnit(Inkscape::Util::Unit const *u);
    void setFullVal(GtkAdjustment *adj, double val);
    Glib::ustring getCurrentLabel();
    void changeLabel(Glib::ustring new_label, gint pos, bool onlylabel = false);

    ComboToolItem *create_tool_item(Glib::ustring const &label,
                                    Glib::ustring const &tooltip);

protected:
    UnitType _type;

private:
    // Callbacks
    void _unitChangedCB(int active);
    static void _adjustmentFinalizedCB(gpointer data, GObject *where_the_object_was);

    void _setActive(gint index);
    void _fixupAdjustments(Inkscape::Util::Unit const *oldUnit, Inkscape::Util::Unit const *newUnit);

    // Cleanup
    void _adjustmentFinalized(GObject *where_the_object_was);

    gint _active;
    bool _isUpdating;
    Inkscape::Util::Unit const *_activeUnit;
    bool _activeUnitInitialized;

    Glib::RefPtr<Gtk::ListStore> _store;
    std::vector<ComboToolItem *> _combo_list;
    std::vector<GtkAdjustment*> _adjList;
    std::map <GtkAdjustment *, double> _priorValues;
};

} // namespace Inkscape::UI::Widget

#endif // INKSCAPE_UI_WIDGET_UNIT_TRACKER_H

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
