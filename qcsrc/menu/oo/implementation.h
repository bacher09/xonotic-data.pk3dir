#ifdef INTERFACE
#undef INTERFACE
#endif

#ifndef IMPLEMENTATION
#define IMPLEMENTATION
#endif

#ifdef CLASS
#undef CLASS
#undef EXTENDS
#undef METHOD
#undef ATTRIB
#undef ATTRIBARRAY
#undef ENDCLASS
#undef SUPER
#endif

#define SUPER(cname) (vtbl##cname.vtblbase)
