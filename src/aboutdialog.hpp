#ifndef ABOUTDIALOG_HPP
#define ABOUTDIALOG_HPP

#include <gtkmm/aboutdialog.h>

namespace Slicer {

class AboutDialog : public Gtk::AboutDialog {
public:
    AboutDialog();
    virtual ~AboutDialog();
};

} // namespace Slicer

#endif // ABOUTDIALOG_HPP
