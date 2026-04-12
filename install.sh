#!/bin/bash
# ------------------------------------------------------------------
# [Freedom] install.sh
#           Installs AlienFXArea51 to user applications
# ------------------------------------------------------------------



# --- Setup Builddir -----------------------------------------------
rm -f -r ./builddir
meson setup --prefix=$HOME builddir
if [ ! $? -eq 0 ]; then
    exit 1
fi


# --- Compile -------------------------------------------------------
cd builddir || exit

meson compile
if [ ! $? -eq 0 ]; then
    exit 1
fi

# --- Install -------------------------------------------------------
meson install
if [ ! $? -eq 0 ]; then
    exit 1
fi

# --- Update .desktop -----------------------------------------------
file="$HOME/.local/share/applications/AlienFX-Area51.desktop"
sed -i -e "s/FXUSER/$USER/g" "$file"
if [ ! $? -eq 0 ]; then
    exit 1
fi