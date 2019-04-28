#include <stdint.h>

#define UTF_INVALID   0xFFFD
#define UTF_SIZ       4

typedef uint_least32_t Rune;

size_t mandecode(Rune *, const char *, size_t);
