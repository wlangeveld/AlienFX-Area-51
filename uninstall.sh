#!/bin/bash
# ------------------------------------------------------------------
# [Freedom] uninstall.sh
#           Uninstalls AlienFXArea51 to user applications
# ------------------------------------------------------------------

# --- Delete files -------------------------------------------------------
rm "$HOME/.local/share/applications/org.gnome.AlienFXArea51.desktop"
rm -r "$HOME/.local/share/AlienFXArea51/"
