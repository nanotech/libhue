#ifndef LIBHUE_H_
#define LIBHUE_H_

#include <stdint.h>

typedef float hue_float;

typedef struct { hue_float r, g, b; } hue_RGB;
typedef struct { hue_float hue, saturation, brightness; } hue_HSB;
typedef struct { hue_float h, s, l; } hue_HSL;
typedef struct { hue_float x, y, z; } hue_XYZ;
typedef struct { hue_float l, u, v; } hue_Luv;
typedef struct { hue_float l, c, h; } hue_LCHuv;
typedef struct { uint8_t r, g, b, a; } hue_RGBA8;

#define CONVERSION_DECL(A, B) hue_##B hue_##A##_to_##B(hue_##A)
#define CONVERSION_DECL_PAIR(A, B) \
  CONVERSION_DECL(A, B); CONVERSION_DECL(B, A)

CONVERSION_DECL_PAIR(RGB, HSB);
CONVERSION_DECL_PAIR(RGB, XYZ);
CONVERSION_DECL_PAIR(XYZ, Luv);
CONVERSION_DECL_PAIR(Luv, LCHuv);

CONVERSION_DECL(HSL, RGB);

#undef CONVERSION_DECL
#undef CONVERSION_DECL_PAIR

hue_RGBA8 hue_RGB_to_RGBA8(hue_RGB RGB, hue_float alpha);
uint32_t hue_HSB_to_BGRAU32(hue_HSB HSB, hue_float alpha);

hue_Luv hue_XYZ_to_Luv_with_white_point(hue_XYZ XYZ, const hue_XYZ *ref_white);
hue_XYZ hue_Luv_to_XYZ_with_white_point(hue_Luv Luv, const hue_XYZ *ref_white);

__attribute__ ((unused))
static inline hue_float hue_interpolate(hue_float a, hue_float b, hue_float p) {
  return a + ((b-a) * p);
}

// usually what you want
hue_HSB hue_HSB_interpolate(hue_HSB a, hue_HSB b, hue_float p);
// ...but not always. this one makes rainbows.
hue_HSB hue_HSB_naive_interpolate(hue_HSB a, hue_HSB b, hue_float p);

#endif
