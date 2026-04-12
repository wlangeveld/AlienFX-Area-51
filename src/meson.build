alienfx_area51_sources = [
  'main.c',
  'alienfx-area51-application.c',
  'alienfx-area51-window.c',
  'humanfx.c',
]

alienfx_area51_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.4'),
  dependency('libusb-1.0')
]

alienfx_area51_sources += gnome.compile_resources('alienfx-area51-resources',
  'alienfx-area51.gresource.xml',
  c_name: 'alienfx_area51'
)

executable('alienfx-area51', alienfx_area51_sources,
           dependencies: alienfx_area51_deps,
           install: true,
           install_dir : get_option('prefix') / '.local/share/AlienFXArea51'
)

install_data(sources : 'AlienFXArea51Logo.svg', install_dir : get_option('prefix') / '.local/share/AlienFXArea51')
install_data(sources : 'AlienFX-Area51.desktop', install_dir : get_option('prefix') / '.local/share/applications')


