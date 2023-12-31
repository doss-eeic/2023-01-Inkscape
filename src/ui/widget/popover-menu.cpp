// SPDX-License-Identifier: GPL-2.0-or-later
/** @file
 * A replacement for GTK3ʼs Gtk::Menu, as removed in GTK4.
 */
/*
 * Authors:
 *   Daniel Boles <dboles.src+inkscape@gmail.com>
 *
 * Copyright (C) 2023 Daniel Boles
 *
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */

#include "ui/widget/popover-menu.h"

#include <glibmm/main.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/stylecontext.h>

#include "ui/menuize.h"
#include "ui/popup-menu.h"
#include "ui/widget/css-name-class-init.h"
#include "ui/widget/popover-menu-item.h"

namespace Inkscape::UI::Widget {

// Make our Grid have CSS name `menu` to try to piggyback “real” Menusʼ theming.
// Ditto, we leave Popover as `popover` so we don't lose normal Popover theming.
class PopoverMenuGrid final
    : public CssNameClassInit
    , public Gtk::Grid
{
public:
    [[nodiscard]] PopoverMenuGrid()
        : Glib::ObjectBase{"PopoverMenuGrid"}
        , CssNameClassInit{"menu"}
        , Gtk::Grid{}
    {
        get_style_context()->add_class("menu");
        set_orientation(Gtk::ORIENTATION_VERTICAL);
    }
};

PopoverMenu::PopoverMenu(Gtk::Widget &parent, Gtk::PositionType const position)
    : Glib::ObjectBase{"PopoverMenu"}
    , Gtk::Popover{}
    , _grid{*Gtk::make_managed<PopoverMenuGrid>()}
{
    auto const style_context = get_style_context();
    style_context->add_class("popover-menu");
    style_context->add_class("menu");

    set_relative_to(parent);
    set_position(position);
    add(_grid);

    signal_show().connect([this]
    {
        // Check no one (accidentally?) removes Grid.
        g_return_if_fail(_grid.get_parent() == this);

        // FIXME: Initially focused item is sometimes wrong on first popup. GTK bug?
        // Grabbing focus in ::show does not always work & sometimes even crashes :(
        // For now, just remove possibly wrong, visible selection until hover/keynav
        // This is also nicer for menus with only 1 item, like the ToolToolbar popup
        Glib::signal_idle().connect_once( [this]{ unset_items_focus_hover(nullptr); });
    });

    // Temporarily hide tooltip of relative-to widget to avoid it covering us up
    UI::autohide_tooltip(*this);
}

void PopoverMenu::attach(Gtk::Widget &item,
                         int const left_attach, int const right_attach,
                         int const top_attach, int const bottom_attach)
{
    // Check no one (accidentally?) removes Grid.
    g_return_if_fail(_grid.get_parent() == this);

    auto const width = right_attach - left_attach;
    auto const height = bottom_attach - top_attach;
    _grid.attach(item, left_attach, top_attach, width, height);
    _items.push_back(&item);
}

void PopoverMenu::append(Gtk::Widget &item)
{
    g_return_if_fail(_grid.get_parent() == this);

    _grid.attach_next_to(item, Gtk::POS_BOTTOM);
    _items.push_back(&item);
}

void PopoverMenu::prepend(Gtk::Widget &item)
{
    g_return_if_fail(_grid.get_parent() == this);

    _grid.attach_next_to(item, Gtk::POS_TOP);
    _items.push_back(&item);
}

void PopoverMenu::remove(Gtk::Widget &item)
{
    // Check was added with one of our methods, is not Grid, etc.
    auto const it = std::find(_items.begin(), _items.end(), &item);
    g_return_if_fail(it != _items.end());

    _grid.remove(item);
    _items.erase(it);
}

void PopoverMenu::append_section_label(Glib::ustring const &markup)
{
    auto const label = Gtk::make_managed<Gtk::Label>();
    label->set_markup(markup);
    auto const item = Gtk::make_managed<PopoverMenuItem>();
    item->add(*label);
    item->set_sensitive(false);
    append(*item);
}

void PopoverMenu::append_separator()
{
    append(*Gtk::make_managed<Gtk::Separator>(Gtk::ORIENTATION_HORIZONTAL));
}

void PopoverMenu::popup_at(Gtk::Widget &widget,
                           int const x_offset, int const y_offset)
{
    ::Inkscape::UI::popup_at(*this, widget, x_offset, y_offset);
}

void PopoverMenu::popup_at_center(Gtk::Widget &widget)
{
    ::Inkscape::UI::popup_at_center(*this, widget);
}

std::vector<Gtk::Widget *> const &PopoverMenu::get_items()
{
    return _items;
}

void PopoverMenu::unset_items_focus_hover(Gtk::Widget * const except_active)
{
    for (auto const item : _items) {
        if (item != except_active) {
            item->unset_state_flags(Gtk::STATE_FLAG_FOCUSED | Gtk::STATE_FLAG_PRELIGHT);
        }
    }
}

} // namespace Inkscape::UI::Widget

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
