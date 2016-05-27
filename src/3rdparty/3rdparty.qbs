import qbs

Project {
    name: "3rdparty"
    references: [
        "../angle/angle.qbs",
        "double-conversion",
        "egl.qbs",
        "fontconfig.qbs",
        "freetype/freetype2.qbs",
        "forkfd/forkfd.qbs",
        "gbm.qbs",
        "glesv2.qbs",
        "glib.qbs",
        "gl.qbs",
        "gtk+.qbs",
        "harfbuzz/harfbuzz.qbs",
        "ice.qbs",
        "libdrm.qbs",
        "libjpeg/libjpeg.qbs",
        "libpng/libpng.qbs",
        "libinput.qbs",
        "libudev.qbs",
        "mtdev.qbs",
        "pcre/pcre.qbs",
        "sm.qbs",
        "tslib.qbs",
        "x11.qbs",
        "x11-xcb.qbs",
        "xcb/xcb.qbs",
        "xcb/xcb-icccm.qbs",
        "xcb/xcb-image.qbs",
        "xcb/xcb-xkb.qbs",
        "xcb/xcb-keysyms.qbs",
        "xcb/xcb-randr.qbs",
        "xcb/xcb-sync.qbs",
        "xcb/xcb-xfixes.qbs",
        "xcb/xcb-xinerama.qbs",
        "xkbcommon.qbs",
        "xkbcommon-x11.qbs",
        "zlib/zlib.qbs",
    ]
}
