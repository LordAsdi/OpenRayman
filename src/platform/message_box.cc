#include <platform/message_box.h>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <gtk/gtk.h>
#endif

namespace openrayman
{
    void message_box::display(const std::string& title, const std::string& msg, bool error)
    {
        std::cout <<
        std::endl << "#################" << std::endl << std::endl <<
        title << std::endl << std::endl << msg << std::endl <<
        std::endl << "#################" << std::endl << std::endl;
    #ifdef _WIN32
        MessageBox(nullptr, std::wstring(title).c_str(), std::wstring(msg).c_str(), MB_OK | (error ? MB_ICONERROR : MB_ICONINFORMATION));
    #else
        GtkDialogFlags flags = (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR);
        GtkWidget* dialog = gtk_message_dialog_new(nullptr, flags, error ? GTK_MESSAGE_ERROR : GTK_MESSAGE_INFO, GTK_BUTTONS_OK, msg.c_str());
        gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    #endif
    }
}
