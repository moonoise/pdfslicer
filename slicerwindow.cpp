#include "slicerwindow.hpp"
#include <gtkmm/filechooserdialog.h>
#include <glibmm/main.h>

namespace Slicer {

Window::Window(std::string filePath)
    : m_document{filePath}
    , m_boxMenuRemoveOptions{Gtk::ORIENTATION_VERTICAL}
    , m_view{m_document}
    , m_labelDone{"Saved!"}
{
    // Widget setup
    set_titlebar(m_headerBar);
    set_default_size(800, 600);

    m_headerBar.set_title("PDF Slicer");
    m_headerBar.set_show_close_button(true);
    m_headerBar.set_has_subtitle(false);

    m_buttonSave.set_image_from_icon_name("document-save");
    m_buttonSave.set_tooltip_text("Save as...");
    m_headerBar.pack_start(m_buttonSave);

    // FIXME:
    // The commit that introduced this comment also introduced
    // a warning that Gtk is trying to destroy an already destroyed
    // widget when closing the window.
    // Look through the changes to find out what's wrong.
    m_buttonRemovePages.set_image_from_icon_name("edit-delete");
    m_buttonRemovePages.set_tooltip_text("Remove selected pages");
    m_boxRemovePages.pack_start(m_buttonRemovePages);

    m_buttonRemovePrevious.set_label("Remove previous pages");
    m_buttonRemoveNext.set_label("Remove next pages");

    m_boxMenuRemoveOptions.pack_start(m_buttonRemovePrevious);
    m_boxMenuRemoveOptions.pack_start(m_buttonRemoveNext);
    m_boxMenuRemoveOptions.set_margin_top(10);
    m_boxMenuRemoveOptions.set_margin_bottom(10);
    m_boxMenuRemoveOptions.set_margin_left(10);
    m_boxMenuRemoveOptions.set_margin_right(10);
    m_menuRemoveOptions.add(m_boxMenuRemoveOptions);
    m_menuRemoveOptions.show_all_children();

    m_buttonRemoveOptions.set_tooltip_text("More removing options");
    m_buttonRemoveOptions.set_popover(m_menuRemoveOptions);
    m_boxRemovePages.pack_start(m_buttonRemoveOptions);

    m_boxRemovePages.get_style_context()->add_class("linked");
    m_headerBar.pack_start(m_boxRemovePages);

    m_scroller.add(m_view);

    m_labelDone.set_margin_left(5);
    m_buttonDoneClose.set_image_from_icon_name("window-close",
                                               Gtk::ICON_SIZE_SMALL_TOOLBAR);
    m_boxDone.pack_start(m_labelDone, true, true, 10);
    m_boxDone.pack_start(m_buttonDoneClose, false, false);
    m_boxDone.get_style_context()->add_class("osd");
    m_boxDone.set_size_request(1, 36);
    m_revealerDone.add(m_boxDone);
    m_revealerDone.set_halign(Gtk::ALIGN_CENTER);
    m_revealerDone.set_valign(Gtk::ALIGN_START);

    m_overlay.add(m_scroller);
    m_overlay.add_overlay(m_revealerDone);

    add(m_overlay);

    // Signal handlers
    m_buttonSave.signal_clicked().connect([this]() {
        onSaveAction();
    });

    m_buttonRemovePages.signal_clicked().connect([this]() {
        removeSelectedPages();
    });

    m_buttonRemovePrevious.signal_clicked().connect([this]() {
        removePreviousPages();
    });

    m_buttonRemoveNext.signal_clicked().connect([this]() {
        removeNextPages();
    });

    m_view.signal_selected_children_changed().connect([this]() {
        const int numSelected = m_view.get_selected_children().size();

        if (numSelected == 0)
            m_buttonRemovePages.set_sensitive(false);
        else
            m_buttonRemovePages.set_sensitive(true);

        if (numSelected == 1) {
            m_buttonRemoveOptions.set_sensitive(true);

            const unsigned int index = m_view.get_selected_children().at(0)->get_index();
            if (index == 0)
                m_buttonRemovePrevious.set_sensitive(false);
            else
                m_buttonRemovePrevious.set_sensitive(true);

            if (index == m_view.get_children().size() - 1)
                m_buttonRemoveNext.set_sensitive(false);
            else
                m_buttonRemoveNext.set_sensitive(true);
        }
        else
            m_buttonRemoveOptions.set_sensitive(false);

    });

    m_buttonDoneClose.signal_clicked().connect([this]() {
        m_revealerDone.set_reveal_child(false);
    });

    m_signalSaved.connect([this]() {
        // Stop any previous hiding animation started
        if (m_connectionSaved.connected())
            m_connectionSaved.disconnect();

        // Show notification and automatically hide it later
        m_revealerDone.set_reveal_child(true);
        m_connectionSaved = Glib::signal_timeout().connect([this]() {
            m_revealerDone.set_reveal_child(false);
            return false;
        },
                                                           5000);
    });

    show_all_children();
}

void Window::removeSelectedPages()
{
    auto children = m_view.get_selected_children();

    // FIXME
    // The following won't work with a multiple selection!
    for (Gtk::FlowBoxChild* c : children) {
        const int index = c->get_index();
        m_document.removePage(index);
    }
}

void Window::removePreviousPages()
{
    auto selected = m_view.get_selected_children();

    if (selected.size() != 1)
        throw std::runtime_error(
            "Tried to remove previous pages with more "
            "than one page selected. This should never happen!");

    const int index = selected.at(0)->get_index();

    m_document.removePageRange(0, index - 1);
}

void Window::removeNextPages()
{
    auto selected = m_view.get_selected_children();

    if (selected.size() != 1)
        throw std::runtime_error(
            "Tried to remove next pages with more "
            "than one page selected. This should never happen!");

    const int index = selected.at(0)->get_index();

    m_document.removePageRange(index + 1, m_document.pages()->get_n_items() - 1);
}

void Window::onSaveAction()
{
    Gtk::FileChooserDialog dialog{*this,
                                  "Save file as",
                                  Gtk::FILE_CHOOSER_ACTION_SAVE};
    dialog.set_transient_for(*this);

    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("Save", Gtk::RESPONSE_OK);

    const int result = dialog.run();

    if (result == Gtk::RESPONSE_OK) {
        std::string filePath = dialog.get_filename();
        m_document.saveDocument(filePath);
        m_signalSaved.emit();
    }
}
}
