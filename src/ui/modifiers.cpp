// SPDX-License-Identifier: GPL-2.0-or-later
/** @file
 * Modifiers for inkscape
 *
 * The file provides a definition of all the ways shift/ctrl/alt modifiers
 * are used in Inkscape, and allows users to customise them in keys.xml
 *
 *//*
 * Authors:
 * 2020 Martin Owens <doctormo@geek-2.com>
 *
 * Copyright (C) 2018 Authors
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */

#include <cstring>
#include <string>
#include <bitset>
#include <glibmm/i18n.h>

#include "modifiers.h"
#include "ui/tools/tool-base.h"

namespace Inkscape {
namespace Modifiers {

Modifier::Lookup Modifier::_modifier_lookup;

// these must be in the same order as the * enum in "modifiers.h"
decltype(Modifier::_modifiers) Modifier::_modifiers {
    // Canvas modifiers
    {Type::CANVAS_PAN_Y, new Modifier("canvas-pan-y", _("Vertical pan"), _("Pan/Scroll up and down"), ALWAYS, CANVAS, SCROLL)},
    {Type::CANVAS_PAN_X, new Modifier("canvas-pan-x", _("Horizontal pan"), _("Pan/Scroll left and right"), SHIFT, CANVAS, SCROLL)},
    {Type::CANVAS_ZOOM, new Modifier("canvas-zoom", _("Canvas zoom"), _("Zoom in and out with scroll wheel"), CTRL, CANVAS, SCROLL)},
    {Type::CANVAS_ROTATE, new Modifier("canvas-rotate", _("Canvas rotate"), _("Rotate the canvas with scroll wheel"), SHIFT | CTRL, CANVAS, SCROLL)},

    // Select tool modifiers (minus transforms)
    {Type::SELECT_ADD_TO, new Modifier("select-add-to", _("Add to selection"), _("Add items to existing selection"), SHIFT, SELECT, CLICK)},
    {Type::SELECT_IN_GROUPS, new Modifier("select-in-groups", _("Select inside groups"), _("Ignore groups when selecting items"), CTRL, SELECT, CLICK)},
    {Type::SELECT_TOUCH_PATH, new Modifier("select-touch-path", _("Select with touch-path"), _("Draw a band around items to select them"), ALT, SELECT, DRAG)},
    {Type::SELECT_ALWAYS_BOX, new Modifier("select-always-box", _("Select with box"), _("Don't drag items, select more with a box"), SHIFT, SELECT, DRAG)},
    {Type::SELECT_FIRST_HIT, new Modifier("select-first-hit", _("Select the first"), _("Drag the first item the mouse hits"), CTRL, SELECT, DRAG)},
    {Type::SELECT_FORCE_DRAG, new Modifier("select-force-drag", _("Forced Drag"), _("Drag objects even if the mouse isn't over them"), ALT, SELECT, DRAG)},
    {Type::SELECT_CYCLE, new Modifier("select-cycle", _("Cycle through objects"), _("Scroll through objects under the cursor"), ALT, SELECT, SCROLL)},

    // Transform handle modifiers (applies to multiple tools)
    {Type::MOVE_CONFINE, new Modifier("move-confine", _("Move one axis only"), _("When dragging items, confine to either x or y axis"), CTRL, MOVE, DRAG)},
    {Type::MOVE_INCREMENT, new Modifier("move-increment", _("Move in increments"), _("Move the objects by set increments when dragging"), ALT, MOVE, DRAG)},
    {Type::MOVE_SNAPPING, new Modifier("move-snapping", _("No Move Snapping"), _("Disable snapping when moving objects"), SHIFT, MOVE, DRAG)},
    {Type::TRANS_CONFINE, new Modifier("trans-confine", _("Keep aspect ratio"), _("When resizing objects, confine the aspect ratio"), CTRL, TRANSFORM, DRAG)},
    {Type::TRANS_INCREMENT, new Modifier("trans-increment", _("Transform in increments"), _("Scale, rotate or skew by set increments"), ALT, TRANSFORM, DRAG)},
    {Type::TRANS_OFF_CENTER, new Modifier("trans-off-center", _("Transform around center"), _("When scaling, scale selection symmetrically around its rotation center. When rotating/skewing, transform relative to opposite corner/edge."), SHIFT, TRANSFORM, DRAG)},
    {Type::TRANS_SNAPPING, new Modifier("trans-snapping", _("No Transform Snapping"), _("Disable snapping when transforming object."), SHIFT, TRANSFORM, DRAG)},
    // Center handle click: seltrans.cpp:734 SHIFT
    // Align handle click: seltrans.cpp:1365 SHIFT
    {Type::BOOL_SHIFT, new Modifier("bool-shift", _("Switch mode"), _("Change shape builder mode temporarily by holding a modifier key."), SHIFT, BOOLEANS_TOOL, DRAG)},

    {Type::NODE_GROW_LINEAR, new Modifier("node-grow-linear", _("Linear node selection"), _("Select the next nodes with scroll wheel or keyboard"), CTRL, NODE_TOOL, SCROLL)},
    {Type::NODE_GROW_SPATIAL, new Modifier("node-grow-spatial", _("Spatial node selection"), _("Select more nodes with scroll wheel or keyboard"), ALWAYS, NODE_TOOL, SCROLL)},
};

decltype(Modifier::_category_names) Modifier::_category_names {
    {NO_CATEGORY, _("No Category")},
    {CANVAS, _("Canvas")},
    {SELECT, _("Selection")},
    {MOVE, _("Movement")},
    {TRANSFORM, _("Transformations")},
    {NODE_TOOL, _("Node Tool")},
    {BOOLEANS_TOOL, _("Shape Builder")},
};


/**
 * Given a Trigger, find which modifier is active (category lookup)
 *
 * @param  trigger - The Modifier::Trigger category in the form "CANVAS | DRAG".
 * @param  button_state - The Gdk button state from an event.
 * @return - Returns the best matching modifier id by the most number of keys.
 */
Type Modifier::which(Trigger trigger, int button_state)
{
    // Record each active modifier with it's weight
    std::map<Type, unsigned long> scales;
    for (auto const& [key, val] : _modifiers) {
        if (val->get_trigger() == trigger) {
            if(val->active(button_state)) {
                scales[key] = val->get_weight();
            }
        }
    }
    // Sort the weightings
    using pair_type = decltype(scales)::value_type;
    auto sorted = std::max_element
    (
        std::begin(scales), std::end(scales),
        [] (const pair_type & p1, const pair_type & p2) {
            return p1.second < p2.second;
        }
    );
    return sorted->first;
}

/**
  * List all the modifiers available. Used in UI listing.
  *
  * @return a vector of Modifier objects.
  */
std::vector<Modifier *>
Modifier::getList () {

    std::vector<Modifier *> modifiers;
    // Go through the dynamic modifier table
    for( auto const& [key, val] : _modifiers ) {
        modifiers.push_back(val);
    }

    return modifiers;
};

/**
 * Test if this modifier is currently active.
 *
 * @param  state - The GDK button state from an event
 * @return a boolean, true if the modifiers for this action are active.
 */
bool Modifier::active(int state)
{
    // TODO:
    //  * ALT key is sometimes MOD1, MOD2 etc, if we find other ALT keys, set the ALT bit
    //  * SUPER key could be HYPER or META, these cases need to be considered.
    auto and_mask = get_and_mask();
    auto not_mask = get_not_mask();
    auto active = Key::ALL_MODS & state;
    // Check that all keys in AND mask are pressed, and NONE of the NOT mask are.
    return and_mask != NEVER && ((active & and_mask) == and_mask) && (not_mask == NOT_SET || (active & not_mask) == 0);
}

/**
 * Test if this modifier is currently active, adding or subtracting keyval
 * during a key press or key release operation.
 *
 * @param state - The GDK button state from an event
 * @param keyval - The GDK keyval from a key press/release event
 * @param release - Boolean, if true the keyval is removed instead
 *
 * @return a boolean, true if the modifiers for this action are active.
 */
bool Modifier::active(int state, int keyval, bool release)
{
    return active(add_keyval(state, keyval, release));
}

/**
 * Generate a label for any modifier keys based on the mask
 *
 * @param  mask - The Modifier Mask such as {SHIFT & CTRL}
 * @return a string of the keys needed for this mask to be true.
 */
std::string generate_label(KeyMask mask, std::string sep)
{
    auto ret = std::string();
    if(mask == NOT_SET) {
        return "-";
    }
    if(mask == NEVER) {
        ret.append("[NEVER]");
        return ret;
    }
    if(mask & CTRL) ret.append("Ctrl");
    if(mask & SHIFT) {
        if(!ret.empty()) ret.append(sep);
        ret.append("Shift");
    }
    if(mask & ALT) {
        if(!ret.empty()) ret.append(sep);
        ret.append("Alt");
    }
    if(mask & SUPER) {
        if(!ret.empty()) ret.append(sep);
        ret.append("Super");
    }
    if(mask & HYPER) {
        if(!ret.empty()) ret.append(sep);
        ret.append("Hyper");
    }
    if(mask & META) {
        if(!ret.empty()) ret.append(sep);
        ret.append("Meta");
    }
    return ret;
}

/**
 * Calculate the weight of this mask based on how many bits are set.
 *
 * @param  mask - The Modifier Mask such as {SHIFT & CTRL}
 * @return count of all modifiers being pressed (or excluded)
 */
unsigned long calculate_weight(KeyMask mask)
{

    if (mask < 0)
        return 0;
    std::bitset<sizeof(mask)> bit_mask(mask);
    return bit_mask.count();
}

/**
 * Set the responsive tooltip for this tool, given the selected types.
 *
 * @param message_context - The desktop's message context for showing tooltips
 * @param event - The current event status (which keys are pressed)
 * @param num_args - Number of Modifier::Type arguments to follow.
 * @param ... - One or more Modifier::Type arguments.
 */
void responsive_tooltip(MessageContext *message_context, KeyEvent const &event, int num_args, ...)
{
    std::string ctrl_msg = "<b>Ctrl</b>: ";
    std::string shift_msg = "<b>Shift</b>: ";
    std::string alt_msg = "<b>Alt</b>: ";

    // NOTE: This will hide any keys changed to SUPER or multiple keys such as CTRL+SHIFT
    va_list args;
    va_start(args, num_args);
    for(int i = 0; i < num_args; i++) {
        auto modifier = Modifier::get(va_arg(args, Type));
        auto name = std::string(_(modifier->get_name()));
        switch (modifier->get_and_mask()) {
            case CTRL:
                ctrl_msg += name + ", ";
                break;
            case SHIFT:
                shift_msg += name + ", ";
                break;
            case ALT:
                alt_msg += name + ", ";
                break;
            default:
                g_warning("Unhandled responsivle tooltip: %s", name.c_str());
        }
    }
    va_end(args);
    ctrl_msg.erase(ctrl_msg.size() - 2);
    shift_msg.erase(shift_msg.size() - 2);
    alt_msg.erase(alt_msg.size() - 2);

    UI::Tools::sp_event_show_modifier_tip(message_context, event,
        ctrl_msg.c_str(), shift_msg.c_str(), alt_msg.c_str());
}

static const std::map<int, int> key_map = {
    {GDK_KEY_Alt_L, GDK_MOD1_MASK},
    {GDK_KEY_Alt_R, GDK_MOD1_MASK},
    {GDK_KEY_Control_L, GDK_CONTROL_MASK},
    {GDK_KEY_Control_R, GDK_CONTROL_MASK},
    {GDK_KEY_Shift_L, GDK_SHIFT_MASK},
    {GDK_KEY_Shift_R, GDK_SHIFT_MASK},
    {GDK_KEY_Meta_L, GDK_META_MASK},
    {GDK_KEY_Meta_R, GDK_META_MASK},
};

/**
 * Add or remove the GDK keyval to the button state if it's one of the
 * keys that define the key mask. Useful for PRESS and RELEASE events.
 *
 * @param state - The GDK button state from an event
 * @param keyval - The GDK keyval from a key press/release event
 * @param release - Boolean, if true the keyval is removed instead
 *
 * @return a new state including the requested change.
 */
int add_keyval(int state, int keyval, bool release)
{
    if (auto it = key_map.find(keyval); it != key_map.end()) {
        if (release)
            state &= ~it->second;
        else
            state |= it->second;
    }
    return state;
}

} // namespace Modifiers
} // namespace Inkscape

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
