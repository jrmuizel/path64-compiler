#if defined(__GNUC__) && (__GNUC__==2) && (__GNUC_MINOR__>=95)
/*STL extensions were put in the std namespace ...*/
// namespace gnuext = std ;
#define __GNUEXT  std 
#elif defined(__GNUC__) && (__GNUC__>=3)
/* STL extensions have their own namespace */
#define __GNUEXT  __gnu_cxx 
// namespace gnuext = __gnu_cxx ;
#else
#error "This code relies on non-standard STL extensions as defined by GNU libraries."
#endif
