/**
 * From the code of Liam P.White from his Power Stroke Knot dialog
 *
 * Released under GNU GPL.  Read the file 'COPYING' for more information
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtkmm.h>
#include "lpe-fillet-chamfer-properties.h"
#include <boost/lexical_cast.hpp>
#include <glibmm/main.h>
#include <glibmm/i18n.h>
#include "inkscape.h"
#include "desktop.h"
#include "document.h"
#include "document-undo.h"
#include "layer-manager.h"
#include "message-stack.h"

#include "sp-object.h"
#include "sp-item.h"
#include "verbs.h"
#include "selection.h"
#include "selection-chemistry.h"
#include "ui/icon-names.h"
#include "ui/widget/imagetoggler.h"
#include <cmath>

//#include "event-context.h"

namespace Inkscape {
namespace UI {
namespace Dialogs {

FilletChamferPropertiesDialog::FilletChamferPropertiesDialog()
    : _desktop(NULL), _knotpoint(NULL), _position_visible(false)
{
    Gtk::Box *mainVBox = get_vbox();
    mainVBox->set_homogeneous(false);
    _layout_table.set_spacings(4);
    _layout_table.resize(3, 3);

    // Layer name widgets
    _fillet_chamfer_position_numeric.set_digits(4);
    _fillet_chamfer_position_numeric.set_increments(1,1);
    //todo: get tha max aloable infinity freeze the widget
    _fillet_chamfer_position_numeric.set_range(0., 999999999999999999.);

    _fillet_chamfer_position_label.set_label(_("Radius (pixels):"));
    _fillet_chamfer_position_label.set_alignment(1.0, 0.5);

    _layout_table.attach(_fillet_chamfer_position_label, 0, 1, 0, 1, Gtk::FILL,
                         Gtk::FILL);
    _layout_table.attach(_fillet_chamfer_position_numeric, 1, 2, 0, 1,
                         Gtk::FILL | Gtk::EXPAND, Gtk::FILL);
    _fillet_chamfer_chamfer_subdivisions.set_digits(0);
    _fillet_chamfer_chamfer_subdivisions.set_increments(1,1);
    //todo: get tha max aloable infinity freeze the widget
    _fillet_chamfer_chamfer_subdivisions.set_range(1, 4294967295);

    _fillet_chamfer_chamfer_subdivisions_label.set_label(_("Chamfer subdivisions:"));
    _fillet_chamfer_chamfer_subdivisions_label.set_alignment(1.0, 0.5);

    _layout_table.attach(_fillet_chamfer_chamfer_subdivisions_label, 0, 1, 1, 2, Gtk::FILL,
                         Gtk::FILL);
    _layout_table.attach(_fillet_chamfer_chamfer_subdivisions, 1, 2, 1, 2,
                         Gtk::FILL | Gtk::EXPAND, Gtk::FILL);
    _fillet_chamfer_type_fillet.set_label(_("Fillet"));
    _fillet_chamfer_type_fillet.set_group(_fillet_chamfer_type_group);
    _fillet_chamfer_type_inverse_fillet.set_label(_("Inverse fillet"));
    _fillet_chamfer_type_inverse_fillet.set_group(_fillet_chamfer_type_group);
    _fillet_chamfer_type_chamfer.set_label(_("Chamfer"));
    _fillet_chamfer_type_chamfer.set_group(_fillet_chamfer_type_group);
    _fillet_chamfer_type_inverse_chamfer.set_label(_("Inverse chamfer"));
    _fillet_chamfer_type_inverse_chamfer.set_group(_fillet_chamfer_type_group);


    mainVBox->pack_start(_layout_table, true, true, 4);
    mainVBox->pack_start(_fillet_chamfer_type_fillet, true, true, 4);
    mainVBox->pack_start(_fillet_chamfer_type_inverse_fillet, true, true, 4);
    mainVBox->pack_start(_fillet_chamfer_type_chamfer, true, true, 4);
    mainVBox->pack_start(_fillet_chamfer_type_inverse_chamfer, true, true, 4);

    // Buttons
    _close_button.set_use_stock(true);
    _close_button.set_label(Gtk::Stock::CANCEL.id);
    _close_button.set_can_default();

    _apply_button.set_use_underline(true);
    _apply_button.set_can_default();

    _close_button.signal_clicked()
    .connect(sigc::mem_fun(*this, &FilletChamferPropertiesDialog::_close));
    _apply_button.signal_clicked()
    .connect(sigc::mem_fun(*this, &FilletChamferPropertiesDialog::_apply));

    signal_delete_event().connect(sigc::bind_return(
                                      sigc::hide(sigc::mem_fun(*this, &FilletChamferPropertiesDialog::_close)),
                                      true));

    add_action_widget(_close_button, Gtk::RESPONSE_CLOSE);
    add_action_widget(_apply_button, Gtk::RESPONSE_APPLY);

    _apply_button.grab_default();

    show_all_children();

    set_focus(_fillet_chamfer_position_numeric);
}

FilletChamferPropertiesDialog::~FilletChamferPropertiesDialog()
{

    _setDesktop(NULL);
}

void FilletChamferPropertiesDialog::showDialog(
    SPDesktop *desktop, double _amount,
    const Inkscape::LivePathEffect::
    FilletChamferKnotHolderEntity *pt,
    bool _use_distance,
    bool _aprox_radius,
    Satellite _satellite)
{
    FilletChamferPropertiesDialog *dialog = new FilletChamferPropertiesDialog();

    dialog->_setDesktop(desktop);
    dialog->_setUseDistance(_use_distance);
    dialog->_setAprox(_aprox_radius);
    dialog->_setAmount(_amount);
    dialog->_setSatellite(_satellite);
    dialog->_setPt(pt);

    dialog->set_title(_("Modify Fillet-Chamfer"));
    dialog->_apply_button.set_label(_("_Modify"));

    dialog->set_modal(true);
    desktop->setWindowTransient(dialog->gobj());
    dialog->property_destroy_with_parent() = true;

    dialog->show();
    dialog->present();
}

void FilletChamferPropertiesDialog::_apply()
{

    double d_pos =  _fillet_chamfer_position_numeric.get_value();
    if (d_pos >= 0) {
        if (_fillet_chamfer_type_fillet.get_active() == true) {
            _satellite.satelliteType = Geom::F;
        } else if (_fillet_chamfer_type_inverse_fillet.get_active() == true) {
            _satellite.satelliteType = Geom::IF;
        } else if (_fillet_chamfer_type_inverse_chamfer.get_active() == true) {
            _satellite.satelliteType = Geom::IC;
        } else {
            _satellite.satelliteType = Geom::C;
        }
        if (_flexible) {
            if (d_pos > 99.99999 || d_pos < 0) {
                d_pos = 0;
            }
            d_pos = d_pos / 100;
        }
        _satellite.amount = d_pos;
        size_t steps = (size_t)_fillet_chamfer_chamfer_subdivisions.get_value();
        if(steps < 1) {
            steps = 1;
        }
        _satellite.steps = steps;
        _knotpoint->knot_set_offset(_satellite);
    }
    _close();
}

void FilletChamferPropertiesDialog::_close()
{
    _setDesktop(NULL);
    destroy_();
    Glib::signal_idle().connect(
        sigc::bind_return(
            sigc::bind(sigc::ptr_fun(&::operator delete), this),
            false
        )
    );
}

bool FilletChamferPropertiesDialog::_handleKeyEvent(GdkEventKey * /*event*/)
{
    return false;
}

void FilletChamferPropertiesDialog::_handleButtonEvent(GdkEventButton *event)
{
    if ((event->type == GDK_2BUTTON_PRESS) && (event->button == 1)) {
        _apply();
    }
}

void FilletChamferPropertiesDialog::_setSatellite(Satellite satellite)
{
    double position;
    std::string distance_or_radius = std::string(_("Radius"));
    if(_aprox) {
        distance_or_radius = std::string(_("Radius approximated"));
    }
    if(_use_distance) {
        distance_or_radius = std::string(_("Knot distance"));
    }
    if (satellite.isTime) {
        position = _amount * 100;
        _flexible = true;
        _fillet_chamfer_position_label.set_label(_("Position (%):"));
    } else {
        _flexible = false;
        std::string posConcat = Glib::ustring::compose (_("%1:"), distance_or_radius);
        _fillet_chamfer_position_label.set_label(_(posConcat.c_str()));
        position = _amount;
    }
    _fillet_chamfer_position_numeric.set_value(position);
    _fillet_chamfer_chamfer_subdivisions.set_value(satellite.steps);
    if (satellite.satelliteType == Geom::F) {
        _fillet_chamfer_type_fillet.set_active(true);
    } else if (satellite.satelliteType == Geom::IF) {
        _fillet_chamfer_type_inverse_fillet.set_active(true);
    } else if (satellite.satelliteType == Geom::C) {
        _fillet_chamfer_type_chamfer.set_active(true);
    } else if (satellite.satelliteType == Geom::IC) {
        _fillet_chamfer_type_inverse_chamfer.set_active(true);
    }
    _satellite = satellite;
}

void FilletChamferPropertiesDialog::_setPt(
    const Inkscape::LivePathEffect::
    FilletChamferKnotHolderEntity *pt)
{
    _knotpoint = const_cast<
                 Inkscape::LivePathEffect::FilletChamferKnotHolderEntity *>(
                     pt);
}


void FilletChamferPropertiesDialog::_setAmount(double amm)
{
    _amount = amm;
}



void FilletChamferPropertiesDialog::_setUseDistance(bool use_knot_distance)
{
    _use_distance = use_knot_distance;
}

void FilletChamferPropertiesDialog::_setAprox(bool _aprox_radius)
{
    _aprox = _aprox_radius;
}

void FilletChamferPropertiesDialog::_setDesktop(SPDesktop *desktop)
{
    if (desktop) {
        Inkscape::GC::anchor(desktop);
    }
    if (_desktop) {
        Inkscape::GC::release(_desktop);
    }
    _desktop = desktop;
}

} // namespace
} // namespace
} // namespace

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim:
// filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:fileencoding=utf-8:textwidth=99
// :
