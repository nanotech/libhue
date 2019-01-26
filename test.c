#include "libhue.h"

#include <math.h>
#include <stdio.h>

static void assert_eq(hue_float a, hue_float b) {
    if (a != b) {
        fprintf(stderr, "%f != %f\n", a, b);
    }
}

static void assert_eq_with_accuracy(hue_float a, hue_float b, hue_float diff) {
    if (fabsf(a - b) > diff) {
        fprintf(stderr, "%f != %f (within %f)\n", a, b, diff);
    }
}

static const hue_float epsilon = 0.0001f;

static void testRGB_HSB(void) {
    hue_RGB rgb = {1,0,0}; // red
    hue_HSB hsb = hue_RGB_to_HSB(rgb);
    assert_eq(hsb.hue, 0.0f);
    assert_eq(hsb.saturation, 1.0f);
    assert_eq(hsb.brightness, 1.0f);
    hue_RGB rgb2 = hue_HSB_to_RGB(hsb);
    assert_eq(rgb2.r, rgb.r);
    assert_eq(rgb2.g, rgb.g);
    assert_eq(rgb2.b, rgb.b);
}

static void testHSB_RGB(void) {
    hue_HSB hsb = {0.13f, 0.77f, 0.83f};
    hue_HSB hsb2 = hue_RGB_to_HSB(hue_HSB_to_RGB(hue_RGB_to_HSB(hue_HSB_to_RGB(hsb))));
    assert_eq_with_accuracy(hsb.hue,        hsb2.hue,        0.000001f);
    assert_eq_with_accuracy(hsb.saturation, hsb2.saturation, 0.000001f);
    assert_eq_with_accuracy(hsb.brightness, hsb2.brightness, 0.000001f);
}

static void testXYZ(void) {
    hue_RGB rgb = {1,0,0}; // red
    hue_XYZ xyz = hue_RGB_to_XYZ(rgb);
    assert_eq_with_accuracy(xyz.x, 0.412456f, epsilon);
    assert_eq_with_accuracy(xyz.y, 0.212673f, epsilon);
    assert_eq_with_accuracy(xyz.z, 0.019334f, epsilon);
}

static void testLuv(void) {
    hue_RGB rgb = {1,0,0}; // red
    hue_Luv luv = hue_XYZ_to_Luv(hue_RGB_to_XYZ(rgb));
    assert_eq_with_accuracy(luv.l, 53.2408f, epsilon);
    assert_eq_with_accuracy(luv.u, 175.0151f, epsilon);
    assert_eq_with_accuracy(luv.v, 37.7564f, epsilon);

    hue_LCHuv lch = {54.5793f, 116.8585f, 255.5050f/360.0f}; // aqua
    luv = hue_LCHuv_to_Luv(lch);
    assert_eq_with_accuracy(luv.l, 54.5793f, 0.0f);
    assert_eq_with_accuracy(luv.u, -29.2492f, epsilon);
    assert_eq_with_accuracy(luv.v, -113.1388f, epsilon);
    hue_LCHuv lch2 = hue_Luv_to_LCHuv(luv);
    assert_eq_with_accuracy(lch.l, lch2.l, 0.0f);
    assert_eq_with_accuracy(lch.c, lch2.c, 0.0f);
    assert_eq_with_accuracy(lch.h, lch2.h, 0.0f);
}

static void testLCHuv(void) {
    hue_RGB rgb = {1,0,0}; // red
    hue_LCHuv lch = hue_Luv_to_LCHuv(hue_XYZ_to_Luv(hue_RGB_to_XYZ(rgb)));
    assert_eq_with_accuracy(lch.l, 53.2408f, epsilon);
    assert_eq_with_accuracy(lch.c, 179.0414f, epsilon);
    assert_eq_with_accuracy(lch.h, 12.1740f/360.0f, epsilon);
}

int main(void) {
    testHSB_RGB();
    testRGB_HSB();
    testXYZ();
    testLuv();
    testLCHuv();
    return 0;
}
