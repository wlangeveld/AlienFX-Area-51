/* Derived from AlienFX-Aurora */
/* alienfx-area51-application.c
 *
 * Copyright 2024 Freedom
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "alienfx-area51-application.h"
#include "alienfx-area51-window.h"

struct _AlienfxArea51Application {
  AdwApplication parent_instance;
};

G_DEFINE_FINAL_TYPE(AlienfxArea51Application, alienfx_area51_application,
                    ADW_TYPE_APPLICATION)

AlienfxArea51Application *
alienfx_area51_application_new(const char *application_id,
                               GApplicationFlags flags) {
  g_return_val_if_fail(application_id != NULL, NULL);

  return g_object_new(ALIENFX_AREA51_TYPE_APPLICATION, "application-id",
                      application_id, "flags", flags, NULL);
}

static void alienfx_area51_application_activate(GApplication *app) {
  GtkWindow *window;

  g_assert(ALIENFX_AREA51_IS_APPLICATION(app));

  window = gtk_application_get_active_window(GTK_APPLICATION(app));

  if (window == NULL)
    window = g_object_new(ALIENFX_AREA51_TYPE_WINDOW, "application", app, NULL);

  gtk_window_present(window);
}

static void
alienfx_area51_application_class_init(AlienfxArea51ApplicationClass *klass) {
  GApplicationClass *app_class = G_APPLICATION_CLASS(klass);

  app_class->activate = alienfx_area51_application_activate;
}

static void alienfx_area51_application_init(AlienfxArea51Application *self) {
  if (self == NULL)
    quick_exit(-1);
}
