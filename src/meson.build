scopa_sources = [
  'main.c',
  'scopa-application.c',
  'scopa-window.c',
]

scopa_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.4'),
]

scopa_sources += gnome.compile_resources('scopa-resources',
  'scopa.gresource.xml',
  c_name: 'scopa'
)

executable('scopa', scopa_sources,
  dependencies: scopa_deps,
       install: true,
)
