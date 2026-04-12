#!/bin/bash
# ------------------------------------------------------------------
# [Freedom] uninstall.sh
#           Uninstalls AlienFXArea51 to user applications
# ------------------------------------------------------------------

# --- Delete files -------------------------------------------------------
rm "$HOME/.local/share/applications/AlienFX-Area51.desktop"
rm -r "$HOME/.local/share/AlienFXArea51/"
