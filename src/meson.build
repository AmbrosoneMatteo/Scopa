scopa_sources = [
  'main.c',
  'scopa-application.c',
  'scopa-window.c',
  'server/server.c',
  'engine/game-assets.c',
  'server/game-manager.c',
  'engine/game-helper.c',
  'local/local_game.c',
  'local/bot_engine.c',
  'netutils/communication.c',
  'netutils/serializer.c',
  'new-game.c',
  'select-combination.c',
  'endgame-dialog.c'
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
