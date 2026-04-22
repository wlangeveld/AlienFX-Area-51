/* Derived from AlienFX-Aurora */
/* alienfx-area51-window.c
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

#include "humanfx.h"
#include <math.h>

#include "alienfx-area51-window.h"

#include <ctype.h>
FILE *config_file;
const char filename[] = ".afxaconfig";
uint16_t breathe_time;
uint16_t spectrum_time;

static void btnApplyClicked(GtkButton *self, gpointer user_data);

static void load_options(AlienfxArea51Window *self);

static void load_option(GtkColorDialogButton *btn, char *line,
                        GtkCheckButton *chkStatic, GtkCheckButton *chkBreathe,
                        GtkCheckButton *chkSpectrum,
                        GtkAdjustment *adjBrightness);

static uint32_t rgb_to_hex(int r, int g, int b);

static void set_static_zone(uint32_t color, uint8_t zone[], size_t length);

static void set_breathe_zone(uint32_t color, uint8_t zone[], size_t length);

static void set_spectrum_zone(uint8_t zone[], size_t length);

static void start_transaction(void);

static void end_transaction(void);

static void apply_option(GtkColorDialogButton *btn, GtkCheckButton *chkStatic,
                         GtkCheckButton *chkBreathe,
                         GtkAdjustment *adjBrightness, uint8_t *zone,
                         size_t zone_size, bool skip);

static void btnTestZonesClicked(GtkButton *self, gpointer user_data);

static void btnTestClicked(GtkButton *self, gpointer user_data);

struct _AlienfxArea51Window {
  AdwApplicationWindow parent_instance;

  /* Template widgets */
  AdwHeaderBar *header_bar;

  GtkColorDialogButton *btnPower;
  GtkColorDialogButton *btnFront;
  GtkColorDialogButton *btnRight;
  GtkColorDialogButton *btnLeft;

  GtkCheckButton *chkPowerStatic, *chkPowerBreathe, *chkPowerSpectrum;
  GtkCheckButton *chkFrontStatic, *chkFrontBreathe, *chkFrontSpectrum;
  GtkCheckButton *chkRightStatic, *chkRightBreathe, *chkRightSpectrum;
  GtkCheckButton *chkLeftStatic, *chkLeftBreathe, *chkLeftSpectrum;

  GtkAdjustment *adjPower;
  GtkAdjustment *adjFront;
  GtkAdjustment *adjRight;
  GtkAdjustment *adjLeft;
  GtkAdjustment *adjBreatheTime;
  GtkAdjustment *adjSpectrumTime;

  GtkButton *btnApply;
  GtkButton *btnTestZones;

  GtkEntry *from;
  GtkEntry *to;
  GtkColorDialogButton *testColorBtn;
};

G_DEFINE_FINAL_TYPE(AlienfxArea51Window, alienfx_area51_window,
                    ADW_TYPE_APPLICATION_WINDOW)

static void alienfx_area51_window_class_init(AlienfxArea51WindowClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class, "/org/gnome/AlienFXArea51/alienfx-area51-window.ui");
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       header_bar);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       btnPower);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       btnFront);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       btnRight);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       btnLeft);

  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkPowerStatic);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkPowerBreathe);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkPowerSpectrum);

  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkFrontStatic);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkFrontBreathe);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkFrontSpectrum);

  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkRightStatic);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkRightBreathe);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkRightSpectrum);

  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkLeftStatic);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkLeftBreathe);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       chkLeftSpectrum);

  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       adjPower);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       adjFront);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       adjRight);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       adjLeft);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       adjBreatheTime);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       adjSpectrumTime);

  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       btnApply);
  gtk_widget_class_bind_template_child(widget_class, AlienfxArea51Window,
                                       btnTestZones);
}

static void alienfx_area51_window_init(AlienfxArea51Window *self) {
  gtk_widget_init_template(GTK_WIDGET(self));
  g_signal_connect(self->btnApply, "clicked", G_CALLBACK(btnApplyClicked),
                   self);
  g_signal_connect(self->btnTestZones, "clicked",
                   G_CALLBACK(btnTestZonesClicked), self);
  load_options(self);
}

static void load_options(AlienfxArea51Window *self) {
  char line[40];
  char *token;
  char *value;

  config_file = fopen(filename, "r");
  if (config_file == NULL) {
    return;
  }

  while (fgets(line, sizeof(line), config_file)) {
    token = strtok(line, "=");
    if (strcmp(token, gtk_widget_get_name((GtkWidget *)self->btnPower)) == 0) {
      load_option(self->btnPower, strtok(NULL, "="), self->chkPowerStatic,
                  self->chkPowerBreathe, self->chkPowerSpectrum,
                  self->adjPower);
    } else if (strcmp(token,
                      gtk_widget_get_name((GtkWidget *)self->btnFront)) == 0) {
      load_option(self->btnFront, strtok(NULL, "="), self->chkFrontStatic,
                  self->chkFrontBreathe, self->chkFrontSpectrum,
                  self->adjFront);
    } else if (strcmp(token,
                      gtk_widget_get_name((GtkWidget *)self->btnRight)) == 0) {
      load_option(self->btnRight, strtok(NULL, "="), self->chkRightStatic,
                  self->chkRightBreathe, self->chkRightSpectrum,
                  self->adjRight);
    } else if (strcmp(token, gtk_widget_get_name((GtkWidget *)self->btnLeft)) ==
               0) {
      load_option(self->btnLeft, strtok(NULL, "="), self->chkLeftStatic,
                  self->chkLeftBreathe, self->chkLeftSpectrum, self->adjLeft);
    } else if (strcmp(token, "BreatheTime") == 0) {
      value = strtok(NULL, "=");
      gtk_adjustment_set_value(self->adjBreatheTime, atoi(value));
    } else if (strcmp(token, "SpectrumTime") == 0) {
      value = strtok(NULL, "=");
      gtk_adjustment_set_value(self->adjSpectrumTime, atoi(value));
    }
  }

  fclose(config_file);
}

static void load_option(GtkColorDialogButton *btn, char *line,
                        GtkCheckButton *chkStatic, GtkCheckButton *chkBreathe,
                        GtkCheckButton *chkSpectrum,
                        GtkAdjustment *adjBrightness) {
  int value;
  char *token;
  GdkRGBA *color = malloc(sizeof(GdkRGBA));
  ;
  token = strtok(line, " ");
  gdk_rgba_parse(color, token);
  gtk_color_dialog_button_set_rgba(btn, color);
  token = strtok(NULL, " ");
  value = atoi(token);

  if (value == 1) {
    gtk_check_button_set_active(chkStatic, true);
  } else if (value == 2) {
    gtk_check_button_set_active(chkBreathe, true);
  } else {
    gtk_check_button_set_active(chkSpectrum, true);
  }

  token = strtok(NULL, " ");
  value = atoi(token);

  gtk_adjustment_set_value(adjBrightness, value);

  free(color);
}

static void btnApplyClicked(GtkButton *self, gpointer user_data) {
  AlienfxArea51Window *form = user_data;
  char buff[20] = "";
  config_file = fopen(filename, "w");
  if (config_file == NULL) {
    g_error("Could not open config file %s, %s", filename,
            gtk_button_get_label(GTK_BUTTON(self)));
  }

  breathe_time = (uint16_t)gtk_adjustment_get_value(form->adjBreatheTime);
  spectrum_time = (uint16_t)gtk_adjustment_get_value(form->adjSpectrumTime);

  start_transaction();
  apply_option(form->btnPower, form->chkPowerStatic, form->chkPowerBreathe,
               form->adjPower, AREA_POWER, AREA_POWER_SIZE, false);
  apply_option(form->btnFront, form->chkFrontStatic, form->chkFrontBreathe,
               form->adjFront, AREA_FRONT, AREA_FRONT_SIZE, false);
  apply_option(form->btnRight, form->chkRightStatic, form->chkRightBreathe,
               form->adjRight, AREA_RIGHT, AREA_RIGHT_SIZE, false);
  apply_option(form->btnLeft, form->chkLeftStatic, form->chkLeftBreathe,
               form->adjLeft, AREA_LEFT, AREA_LEFT_SIZE, false);
  end_transaction();

  sprintf(buff, "BreatheTime=%d ", breathe_time);
  fprintf(config_file, "%s\n", buff);
  sprintf(buff, "SpectrumTime=%d ", spectrum_time);
  fprintf(config_file, "%s\n", buff);

  fclose(config_file);
}

static void apply_option(GtkColorDialogButton *btn, GtkCheckButton *chkStatic,
                         GtkCheckButton *chkBreathe,
                         GtkAdjustment *adjBrightness, uint8_t *zone,
                         size_t zone_size, bool skip) {
  uint32_t color_hex;
  char config_line[40] = "";
  char buff[40] = "";
  const GdkRGBA *color = gtk_color_dialog_button_get_rgba(btn);
  color_hex = rgb_to_hex((int)(0.5 + pow(CLAMP(color->red, 0., 1.), 2.2) * 255.),
                         (int)(0.5 + pow(CLAMP(color->green, 0., 1.), 2.2) * 255.),
                         (int)(0.5 + pow(CLAMP(color->blue, 0., 1.), 2.2) * 255.));

  sprintf(buff, "%s=", gtk_widget_get_name((GtkWidget *)btn));
  strcat(config_line, buff);
  sprintf(buff, "%s ", gdk_rgba_to_string(color));
  strcat(config_line, buff);
  if (gtk_check_button_get_active(chkStatic)) {
    set_static_zone(color_hex, zone, zone_size);
    sprintf(buff, "%d ", 1);
  } else if (gtk_check_button_get_active(chkBreathe)) {
    set_breathe_zone(color_hex, zone, zone_size);
    sprintf(buff, "%d ", 2);
  } else {
    set_spectrum_zone(zone, zone_size);
    sprintf(buff, "%d ", 3);
  }
  strcat(config_line, buff);

  send_set_dim_arr(100 - (int)gtk_adjustment_get_value(adjBrightness),
                   zone_size, zone);
  sprintf(buff, "%d ", (int)gtk_adjustment_get_value(adjBrightness));
  strcat(config_line, buff);

  if (!skip)
    fprintf(config_file, "%s\n", config_line);
}

static uint32_t rgb_to_hex(int r, int g, int b) {
  return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

static void set_static_zone(uint32_t color, uint8_t zone[], size_t length) {
  send_zone_select_arr(1, length, zone);
  send_add_action(ACTION_COLOR, 1, 2, color);
}

static void set_breathe_zone(uint32_t color, uint8_t zone[], size_t length) {
  send_zone_select_arr(1, length, zone);
  
  // Calculate a 5% dim version of the color to prevent LEDs from snapping to pitch black
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  uint32_t dim_color = rgb_to_hex(r / 20, g / 20, b / 20);

  send_add_action(ACTION_MORPH, breathe_time, 64, color);
  send_add_action(ACTION_MORPH, breathe_time, 64, dim_color);
}

static void set_spectrum_zone(uint8_t zone[], size_t length) {
  send_zone_select_arr(1, length, zone);
  send_add_action(ACTION_MORPH, spectrum_time, 64, 0xFF0000);
  send_add_action(ACTION_MORPH, spectrum_time, 64, 0xFFa500);
  send_add_action(ACTION_MORPH, spectrum_time, 64, 0xFFFF00);
  send_add_action(ACTION_MORPH, spectrum_time, 64, 0x008000);
  send_add_action(ACTION_MORPH, spectrum_time, 64, 0x00BFFF);
  send_add_action(ACTION_MORPH, spectrum_time, 64, 0x0000FF);
  send_add_action(ACTION_MORPH, spectrum_time, 64, 0x800080);
}

static void start_transaction(void) {
  device_open();
  device_acquire();
  send_animation_remove(1);
  send_animation_config_start(1);
}

static void end_transaction(void) {
  send_animation_config_save(1);
  send_animation_set_default(1);
  device_release();
  device_close();
}

static void btnTestZonesClicked(GtkButton *self, gpointer user_data) {
  AlienfxArea51Window *form = user_data;
  (void)self;
  GtkWidget *window = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(window), "Test Zones");
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 50);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_box_set_homogeneous(GTK_BOX(box), true);
  gtk_window_set_child(GTK_WINDOW(window), box);
  gtk_widget_set_margin_start(box, 10);
  gtk_widget_set_margin_end(box, 10);
  gtk_widget_set_margin_top(box, 10);
  gtk_widget_set_margin_bottom(box, 10);

  GtkWidget *label = gtk_label_new("Zone Start");
  gtk_box_append(GTK_BOX(box), label);

  // Create an entry widget for user input
  GtkWidget *from = gtk_entry_new();
  gtk_box_append(GTK_BOX(box), from);
  form->from = GTK_ENTRY(from);

  GtkWidget *label2 = gtk_label_new("Zone End");
  gtk_box_append(GTK_BOX(box), label2);

  // Create an entry widget for user input
  GtkWidget *to = gtk_entry_new();
  gtk_box_append(GTK_BOX(box), to);
  form->to = GTK_ENTRY(to);

  GtkWidget *label3 = gtk_label_new("Color");
  gtk_box_append(GTK_BOX(box), label3);

  GtkColorDialog *dialog = gtk_color_dialog_new();
  GtkWidget *color_btn = gtk_color_dialog_button_new(dialog);
  GdkRGBA default_color = {1.0, 1.0, 1.0, 1.0};
  gtk_color_dialog_button_set_rgba(GTK_COLOR_DIALOG_BUTTON(color_btn),
                                   &default_color);
  gtk_box_append(GTK_BOX(box), color_btn);
  form->testColorBtn = GTK_COLOR_DIALOG_BUTTON(color_btn);

  GtkWidget *button = gtk_button_new_with_label("Test");
  gtk_box_append(GTK_BOX(box), button);

  g_signal_connect(button, "clicked", G_CALLBACK(btnTestClicked), form);

  gtk_window_present(GTK_WINDOW(window));
}

static void btnTestClicked(GtkButton *self, gpointer user_data) {
  AlienfxArea51Window *form = user_data;
  (void)self;
  uint8_t from_n;
  uint8_t to_n;
  uint8_t start;
  uint8_t end;
  const char *from = gtk_editable_get_text(GTK_EDITABLE(form->from));
  const char *to = gtk_editable_get_text(GTK_EDITABLE(form->to));

  for (int i = 0; i < (int)strlen(from); i++) {
    if (!isdigit(from[i])) {
      return;
    }
  }

  for (int i = 0; i < (int)strlen(to); i++) {
    if (!isdigit(to[i])) {
      return;
    }
  }

  from_n = (uint8_t)atoi(from);
  to_n = (uint8_t)atoi(to);

  if (from_n < to_n) {
    start = from_n;
    end = to_n;
  } else {
    start = to_n;
    end = from_n;
  }
  const GdkRGBA *color = gtk_color_dialog_button_get_rgba(form->testColorBtn);
  uint32_t color_hex =
      rgb_to_hex((int)(0.5 + pow(CLAMP(color->red, 0., 1.), 2.2) * 255.),
                 (int)(0.5 + pow(CLAMP(color->green, 0., 1.), 2.2) * 255.),
                 (int)(0.5 + pow(CLAMP(color->blue, 0., 1.), 2.2) * 255.));

  start_transaction();
  for (int i = start; i <= end; i++) {
    send_zone_select(1, 1, i);
    send_add_action(ACTION_COLOR, 1, 2, color_hex);
  }
  end_transaction();
}