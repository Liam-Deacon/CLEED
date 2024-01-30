#ifndef CC_ATTRIBUTE_H
#define CC_ATTRIBUTE_H

#if !__GNUC__
#define __attribute__(x) /* empty */
#endif

#if (__GNUC__)
#define PURE __attribute__ ((pure))
#else
#define PURE
#endif

#endif /* CC_ATTRIBUTE_H */