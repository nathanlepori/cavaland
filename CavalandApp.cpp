//
// Created by simrat39 on 12/26/23.
//

#include "CavalandApp.hpp"
#include "CavaWindow.hpp"
#include "ConfigManager.hpp"
#include "gtk4-layer-shell.h"
#include <format>
#include <gtkmm/cssprovider.h>
#include <gtkmm.h>

#include "CavaWindow.hpp"

CavalandApp::CavalandApp() : Gtk::Application() {}

Glib::RefPtr<CavalandApp> CavalandApp::create() {
  return Glib::make_refptr_for_instance(new CavalandApp());
}

void CavalandApp::on_activate() {
  Application::on_activate();

  auto window = Gtk::make_managed<CavaWindow>();
  window->set_size_request(WIDTH, HEIGHT);

  gtk_layer_init_for_window(window->gobj());
  gtk_layer_set_layer(window->gobj(),
                      GtkLayerShellLayer::GTK_LAYER_SHELL_LAYER_BACKGROUND);
  gtk_layer_set_exclusive_zone(window->gobj(), -1);

  static const gboolean anchors[] = {TRUE, TRUE, TRUE, TRUE};
  for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
    gtk_layer_set_anchor(window->gobj(), static_cast<GtkLayerShellEdge>(i),
                         anchors[i]);
  }

  auto provider = Gtk::CssProvider::create();
  auto css = "window{background:none;} *{transform: rotate(" +
             ConfigManager::get_instance().get_or_default("rotation", "0deg") +
             ");}";
  provider->load_from_string(css);

  Gtk::StyleContext::add_provider_for_display(
      Gdk::Display::get_default(), provider,
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  // Assign to monitor
  auto monitorIndex = ConfigManager::get_instance().get_or_default("monitor", "");
  auto monitors = gdk_display_get_monitors(Gdk::Display::get_default()->gobj());
  if (!monitorIndex.empty()) {
    auto monitor = static_cast<GdkMonitor *>(g_list_model_get_item(monitors, std::stoi(monitorIndex)));
    gtk_layer_set_monitor(window->gobj(), monitor);
  }

  add_window(*window);
  window->present();
  window->show();
}
