// [HK] defines BE_EXPORTED_CLASS and BE_EXPORTED_METHOD depending on the version of GCC, to workaround an uncompatibility between gcc-2.95 and gcc-3.3

#if __GNUC__ >=3
#define BE_EXPORTED_CLASS BE_EXPORTED
#define BE_EXPORTED_METHOD
#else
#define BE_EXPORTED_CLASS 
#define BE_EXPORTED_METHOD BE_EXPORTED
#endif // __GNUC__ >=3
