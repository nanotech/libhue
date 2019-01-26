#include "libhue.h"
#include "libhue_data.c"
#include <math.h>
#include <stdio.h>
#include <assert.h>

#if USE_ACCELERATE
#include <Accelerate/Accelerate.h>
#endif

#if !defined(MIN)
#define MIN(A,B) ((A) < (B) ? (A) : (B))
#endif
#if !defined(MAX)
#define MAX(A,B) ((A) > (B) ? (A) : (B))
#endif

// from http://www.cs.rit.edu/~ncs/color/t_convert.html
hue_HSB hue_RGB_to_HSB(hue_RGB rgb) {
    hue_float h, s, v;
    hue_float r = rgb.r, g = rgb.g, b = rgb.b;
    hue_float min = MIN(MIN(r, g), b);
    hue_float max = MAX(MAX(r, g), b);
    v = max;
    hue_float delta = max - min;
    if (max != 0) {
        s = delta / max;
    } else {
        return (hue_HSB){0, 0, 0};
    }
    if (r == max) {
        h = (g - b) / delta; // between yellow & magenta
    } else if (g == max) {
        h = 2 + (b - r) / delta; // between cyan & yellow
    } else {
        h = 4 + (r - g) / delta; // between magenta & cyan
    }

    h /= 6.0; // scale to 0..1
    if (h < 0) {
        h += 1.0;
    }
    return (hue_HSB){h, s, v};
}

hue_RGB hue_HSB_to_RGB(hue_HSB hsb) {
    hue_float s = hsb.saturation, v = hsb.brightness;
    if (s == 0) {
        return (hue_RGB){v, v, v};
    } else {
        hue_float h = hsb.hue * 6;
        hue_float i = (int)h;
        hue_float v1 = v * (1 - s);
        hue_float v2 = v * (1 - s * (h - i));
        hue_float v3 = v * (1 - s * (1 - (h - i)));

        hue_RGB rgb;
        if      (i == 0) { rgb.r = v  ; rgb.g = v3 ; rgb.b = v1; }
        else if (i == 1) { rgb.r = v2 ; rgb.g = v  ; rgb.b = v1; }
        else if (i == 2) { rgb.r = v1 ; rgb.g = v  ; rgb.b = v3; }
        else if (i == 3) { rgb.r = v1 ; rgb.g = v2 ; rgb.b = v;  }
        else if (i == 4) { rgb.r = v3 ; rgb.g = v1 ; rgb.b = v;  }
        else             { rgb.r = v  ; rgb.g = v1 ; rgb.b = v2; }
        return rgb;
    }
}

static hue_float hue_HSL_to_RGB_map(hue_float p, hue_float q, hue_float t) {
    if      (t < 0) t += 6;
    else if (t > 6) t -= 6;

    if      (t < 1) return p + (q - p) * t;
    else if (t < 3) return q;
    else if (t < 4) return p + (q - p) * (4 - t);
    else            return p;
}

hue_RGB hue_HSL_to_RGB(hue_HSL hsl) {
    hue_float s = hsl.s, l = hsl.l;
    if (s == 0) return (hue_RGB){l, l, l};

    hue_float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    hue_float p = 2 * l - q;
    hue_float h = hsl.h * 6;

    return (hue_RGB){
        hue_HSL_to_RGB_map(p, q, h + 2),
        hue_HSL_to_RGB_map(p, q, h),
        hue_HSL_to_RGB_map(p, q, h - 2),
    };
}

static hue_XYZ hue_XYZ_sgemv(const hue_float *matrix, const hue_float *vec) {
#if USE_ACCELERATE
    hue_XYZ result = {0};
    const size_t w = 3;
    cblas_sgemv(CblasColMajor, CblasNoTrans,
                w, w, 1.0f, matrix,
                w, vec, 1,
                0, (hue_float *)&result, 1);
#else
    hue_float r[3] = {0};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            r[j] += matrix[3 * i + j] * vec[i];
        }
    }
    hue_XYZ result = {.x = r[0], .y = r[1], .z = r[2]};
#endif
    return result;
}

// begin translated code from http://www.brucelindbloom.com/javascript/ColorConv.js

static hue_float compand(hue_float linear, hue_float gamma) {
    hue_float companded;
    if (gamma > 0.0) {
        companded = (linear >= 0.0) ? powf(linear, 1.0 / gamma)
                                    : -powf(-linear, 1.0 / gamma);
    } else if (gamma < 0.0) {
        // sRGB
        hue_float sign = 1.0;
        if (linear < 0.0) {
            sign = -1.0;
            linear = -linear;
        }
        companded = (linear <= 0.0031308) ? (linear * 12.92)
                                          : (1.055 * powf(linear, 1.0 / 2.4) - 0.055);
        companded *= sign;
    } else {
        // L*
        hue_float sign = 1.0;
        if (linear < 0.0) {
            sign = -1.0;
            linear = -linear;
        }
        companded = (linear <= (216.0 / 24389.0))
                        ? (linear * 24389.0 / 2700.0)
                        : (1.16 * powf(linear, 1.0 / 3.0) - 0.16);
        companded *= sign;
    }
    return companded;
}

static hue_float inv_compand(hue_float companded, hue_float gamma) {
    hue_float linear;
    if (gamma > 0.0) {
        linear = (companded >= 0.0) ? powf(companded, gamma) : -powf(-companded, gamma);
    } else if (gamma < 0.0) {
        /* sRGB */
        hue_float sign = 1.0;
        if (companded < 0.0) {
            sign = -1.0;
            companded = -companded;
        }
        linear = companded <= 0.04045 ? companded / 12.92
                                      : powf((companded + 0.055) / 1.055, 2.4);
        linear *= sign;
    } else {
        /* L* */
        hue_float sign = 1.0;
        if (companded < 0.0) {
            sign = -1.0;
            companded = -companded;
        }
        linear = (companded <= 0.08)
            ? 2700.0 * companded / 24389.0
            : (((1000000.0 * companded + 480000.0)
                        * companded + 76800.0)
                    * companded + 4096.0) / 1560896.0;
        linear *= sign;
    }
    return linear;
}

static inline hue_RGB generic_compand_RGB(hue_float (*func)(hue_float, hue_float),
                                          hue_RGB linear,
                                          hue_float gamma) {
    return (hue_RGB){
        .r = func(linear.r, gamma),
        .g = func(linear.g, gamma),
        .b = func(linear.b, gamma),
    };
}

hue_XYZ hue_RGB_to_XYZ_with_options(hue_RGB rgb,
                                    const hue_XYZ_RGB_model *model,
                                    const hue_XYZ_adaptation *adaptation,
                                    const hue_XYZ *ref_white) {
    rgb = generic_compand_RGB(inv_compand, rgb, model->gamma);
    hue_XYZ xyz = hue_XYZ_sgemv(model->to_XYZ, (const hue_float *)&rgb);

    if (adaptation != NULL) {
        hue_XYZ d = hue_XYZ_sgemv(adaptation->matrix, (const hue_float *)ref_white);
        hue_XYZ s = hue_XYZ_sgemv(adaptation->matrix,
                (const hue_float *)&hue_XYZ_RGB_model_sRGB.reference_white);
        hue_XYZ t = hue_XYZ_sgemv(adaptation->matrix, (const hue_float *)&xyz);

        t.x *= (d.x / s.x);
        t.y *= (d.y / s.y);
        t.z *= (d.z / s.z);

        xyz = hue_XYZ_sgemv(adaptation->inverse, (const hue_float *)&t);
    }
    return xyz;
}

hue_RGB hue_XYZ_to_RGB_with_options(hue_XYZ xyz,
                                    const hue_XYZ_RGB_model *model,
                                    const hue_XYZ_adaptation *adaptation,
                                    const hue_XYZ *ref_white) {
    if (adaptation != NULL) {
        hue_XYZ d = hue_XYZ_sgemv(adaptation->matrix,
                (const hue_float *)&hue_XYZ_RGB_model_sRGB.reference_white);
        hue_XYZ s = hue_XYZ_sgemv(adaptation->matrix, (const hue_float *)ref_white);
        hue_XYZ t = hue_XYZ_sgemv(adaptation->matrix, (const hue_float *)&xyz);

        t.x *= (d.x / s.x);
        t.y *= (d.y / s.y);
        t.z *= (d.z / s.z);

        xyz = hue_XYZ_sgemv(adaptation->inverse, (const hue_float *)&t);
    }

    hue_XYZ XYZrgb = hue_XYZ_sgemv(model->to_RGB, (const hue_float *)&xyz);
    hue_RGB rgb = {XYZrgb.x, XYZrgb.y, XYZrgb.z};
    rgb = generic_compand_RGB(compand, rgb, model->gamma);
    return rgb;
}

hue_XYZ hue_RGB_to_XYZ(hue_RGB rgb) {
    return hue_RGB_to_XYZ_with_options(rgb, &hue_XYZ_RGB_model_sRGB, NULL, NULL);
}

hue_RGB hue_XYZ_to_RGB(hue_XYZ xyz) {
    return hue_XYZ_to_RGB_with_options(xyz, &hue_XYZ_RGB_model_sRGB, NULL, NULL);
}

static const hue_float K = 24389.0 / 27.0;
static const hue_float E = 216.0 / 24389.0;
static const hue_float KE = 8.0;

static void uv_white_point(const hue_XYZ *ref_white, hue_float *u, hue_float *v) {
    *u = (4.0 * ref_white->x) / (ref_white->x + 15.0 * ref_white->y + 3.0 * ref_white->z);
    *v = (9.0 * ref_white->y) / (ref_white->x + 15.0 * ref_white->y + 3.0 * ref_white->z);
}

hue_Luv hue_XYZ_to_Luv_with_white_point(hue_XYZ xyz, const hue_XYZ *ref_white) {
    hue_float Den = xyz.x + 15.0 * xyz.y + 3.0 * xyz.z;
    hue_float up = (Den > 0.0) ? ((4.0 * xyz.x) / (xyz.x + 15.0 * xyz.y + 3.0 * xyz.z)) : 0.0;
    hue_float vp = (Den > 0.0) ? ((9.0 * xyz.y) / (xyz.x + 15.0 * xyz.y + 3.0 * xyz.z)) : 0.0;

    hue_float urp, vrp;
    uv_white_point(ref_white, &urp, &vrp);

    hue_float yr = xyz.y / ref_white->y;

    hue_Luv luv;
    luv.l = (yr > E) ? (116.0 * powf(yr, 1.0 / 3.0) - 16.0) : (K * yr);
    luv.u = 13.0 * luv.l * (up - urp);
    luv.v = 13.0 * luv.l * (vp - vrp);
    return luv;
}

hue_XYZ hue_Luv_to_XYZ_with_white_point(hue_Luv Luv, const hue_XYZ *ref_white) {
    hue_XYZ xyz;
    xyz.y = (Luv.l > KE) ? powf((Luv.l + 16.0) / 116.0, 3.0) : (Luv.l / K);
    hue_float u0, v0;
    uv_white_point(ref_white, &u0, &v0);

    hue_float a = (((52.0 * Luv.l) / (Luv.u + 13.0 * Luv.l * u0)) - 1.0) / 3.0;
    hue_float b = -5.0 * xyz.y;
    hue_float c = -1.0 / 3.0;
    hue_float d = xyz.y * (((39.0 * Luv.l) / (Luv.v + 13.0 * Luv.l * v0)) - 5.0);

    xyz.x = (d - b) / (a - c);
    xyz.z = xyz.x * a + b;
    return xyz;
}

hue_Luv hue_XYZ_to_Luv(hue_XYZ xyz) {
    return hue_XYZ_to_Luv_with_white_point(xyz, &hue_XYZ_ref_white_d65);
}

hue_XYZ hue_Luv_to_XYZ(hue_Luv Luv) {
    return hue_Luv_to_XYZ_with_white_point(Luv, &hue_XYZ_ref_white_d65);
}

hue_LCHuv hue_Luv_to_LCHuv(hue_Luv Luv) {
    hue_LCHuv LCHuv;
    LCHuv.l = Luv.l;
    LCHuv.c = sqrtf(Luv.u * Luv.u + Luv.v * Luv.v);
    LCHuv.h = 0.5 * atan2f(Luv.v, Luv.u) / M_PI;
    if (LCHuv.h < 0.0) {
        LCHuv.h += 1.0;
    }
    return LCHuv;
}

hue_Luv hue_LCHuv_to_Luv(hue_LCHuv LCHuv) {
    return (hue_Luv){
        .l = LCHuv.l,
        .u = LCHuv.c * cosf(LCHuv.h * M_PI * 2),
        .v = LCHuv.c * sinf(LCHuv.h * M_PI * 2),
    };
}

// end translated code

#define INTERPOLATE_HSB_PROP(prop) hue_interpolate(a.prop, b.prop, p)

hue_HSB hue_HSB_interpolate(hue_HSB a, hue_HSB b, hue_float p) {
    // Black and white have no hue or saturation.
    if (a.saturation == 0 && (a.brightness == 0 || a.brightness == 1)) {
        a.hue = b.hue;
        a.saturation = b.saturation;
    } else if (b.saturation == 0 && (b.brightness == 0 || b.brightness == 1)) {
        b.hue = a.hue;
        b.saturation = a.saturation;
    }

    hue_HSB r;

    float distCCW = (a.hue >= b.hue) ? a.hue - b.hue : 1 + a.hue - b.hue;
    float distCW = (a.hue >= b.hue) ? 1 + b.hue - a.hue : b.hue - a.hue;

    r.hue = (distCW <= distCCW) ? a.hue + (distCW * p) : a.hue - (distCCW * p);
    if (r.hue < 0) {
        r.hue = 1 + r.hue;
    } else if (r.hue > 1) {
        r.hue = r.hue - 1;
    }
    r.saturation = INTERPOLATE_HSB_PROP(saturation);
    r.brightness = INTERPOLATE_HSB_PROP(brightness);
    return r;
}

hue_HSB hue_HSB_naive_interpolate(hue_HSB a, hue_HSB b, hue_float p) {
    hue_HSB r = {
        .hue = INTERPOLATE_HSB_PROP(hue),
        .saturation = INTERPOLATE_HSB_PROP(saturation),
        .brightness = INTERPOLATE_HSB_PROP(brightness),
    };
    return r;
}

hue_RGBA8 hue_RGB_to_RGBA8(hue_RGB rgb, hue_float alpha) {
    hue_RGBA8 c = {
        rgb.r * 255,
        rgb.g * 255,
        rgb.b * 255,
        alpha * 255,
    };
    return c;
}

uint32_t hue_HSB_to_BGRAU32(hue_HSB hsb, hue_float alpha) {
    uint32_t r, g, b, a;
    hue_RGB rgb = hue_HSB_to_RGB(hsb);
    r = rgb.r * 255;
    g = rgb.g * 255;
    b = rgb.b * 255;
    a = alpha * 255;

    b = b << 24;
    g = g << 16;
    r = r << 8;

    return (b | g | r | a);
}
