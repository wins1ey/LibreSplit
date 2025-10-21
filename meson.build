project(
    'libresplit',
    'c',
    'cpp',
    default_options: ['c_std=gnu23', 'cpp_std=c++20'],
    meson_version: '>=1.1',
)

if get_option('buildtype') == 'debug'
    add_project_arguments('-DDEBUG', language: 'cpp')
endif

threads = dependency('threads')
gtk = dependency('gtk+-3.0')
luajit = dependency('luajit')
x11 = dependency('x11')
jansson = dependency('jansson')

executable(
    'libresplit',
    [
        'src/main.c',
        'src/auto-splitter.c',
        'src/bind.c',
        'src/memory.c',
        'src/process.c',
        'src/settings.c',
        'src/timer.c',
        'src/component/best-sum.c',
        'src/component/clock.c',
        'src/component/components.c',
        'src/component/pb.c',
        'src/component/prev-segment.c',
        'src/component/splits.c',
        'src/component/title.c',
        'src/component/wr.c',
        'src/component/detailed-timer.c',
    ],
    dependencies: [threads, gtk, luajit, x11, jansson],
    c_args: [
        '-DPREFIX="' + get_option('prefix') + '"',
        '-DDATADIR="' + get_option('datadir') + '"',
        '-Werror',
    ],
    install: true,
)

message('prefix: ' + get_option('prefix')) # /usr/local by default
message('datadir: ' + get_option('datadir')) # share by default
message('buildtype: ' + get_option('buildtype')) 

install_data(
    'assets/libresplit.desktop',
    install_dir: get_option('prefix') / get_option('datadir') / 'applications',
)
install_data(
    'assets/com.github.wins1ey.libresplit.gschema.xml',
    install_dir: get_option('prefix') / get_option('datadir') / 'glib-2.0/schemas',
)
install_data(
    'README.md',
    install_dir: get_option('prefix') / get_option('datadir') / 'doc' / meson.project_name(),
)
install_data(
    'LICENSE',
    install_dir: get_option('prefix') / get_option('datadir') / 'licenses' / meson.project_name(),
)

# TODO: better install dir for this to make desktop and gtk icons work on debug builds/uninstalled tests
foreach size : [16, 22, 24, 32, 36, 48, 64, 72, 96, 128, 256, 512, 1024]
    install_data(
        'assets/icons/libresplit-@0@.png'.format(size),
        install_dir: get_option('prefix') / get_option('datadir') / 'icons/hicolor/@0@x@0@/apps/'.format(size),
        rename: 'libresplit.png',
        install_mode: 'rw-r--r--',
    )
endforeach

gnome = import('gnome')
gnome.post_install(glib_compile_schemas: true, gtk_update_icon_cache: true)
