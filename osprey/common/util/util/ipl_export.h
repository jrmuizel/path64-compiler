// [HK] defines IPL_EXPORTED_CLASS and IPL_EXPORTED_METHOD depending on the version of GCC, to workaround an uncompatibility between gcc-2.95 and gcc-3.3

#if __GNUC__ >=3
#define IPL_EXPORTED_CLASS IPL_EXPORTED
#define IPL_EXPORTED_METHOD
#else
#define IPL_EXPORTED_CLASS 
#define IPL_EXPORTED_METHOD IPL_EXPORTED
#endif // __GNUC__ >=3
