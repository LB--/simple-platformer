#ifndef simplat_config_Header
#define simplat_config_Header

#ifdef CORRADE_IS_DEBUG_BUILD
#define MAGNUM_PLUGINS_FONT_DIR "${MAGNUM_PLUGINS_FONT_DEBUG_DIR}"
#else
#define MAGNUM_PLUGINS_FONT_DIR "${MAGNUM_PLUGINS_FONT_DIR}"
#endif

#endif
