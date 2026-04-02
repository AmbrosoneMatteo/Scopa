scopa_sources = [
  'main.c',
  'scopa-application.c',
  'scopa-window.c',
  'server/server.c',
  'server/game-manager.c',
  'engine/game-helper.c',
  'engine/game-assets.c',
  'local/local_game.c',
  'new-game.c'
]

scopa_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.4'),
]

scopa_sources += gnome.compile_resources('scopa-resources',
  'scopa.gresource.xml',
  c_name: 'scopa',
  source_dir: [
    meson.project_source_root() / 'data',
  ],
)

executable('scopa', scopa_sources,
  dependencies: scopa_deps,
       install: true,
)
