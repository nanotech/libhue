#include "libhue_data.h"

// RGB Models

const hue_XYZ_RGB_model *hue_XYZ_RGB_model_sRGB = &(hue_XYZ_RGB_model){
    .to_XYZ = {
        0.4124564391, 0.2126728514, 0.0193338956,
        0.3575760776, 0.7151521553, 0.1191920259,
        0.1804374833, 0.0721749933, 0.9503040785,
    },
    .to_RGB = {
         3.2404541621, -0.9692660305,  0.0556434310,
        -1.5371385128,  1.8760108454, -0.2040259135,
        -0.4985314096,  0.0415560175,  1.0572251882,
    },
    .reference_white = {0.95047, 1.0, 1.08883},
    .gamma = -2.2,
};

// Adaptations

const hue_XYZ_adaptation *hue_XYZ_adaptation_bradford = &(hue_XYZ_adaptation){
    .matrix = {
        0.8951, -0.7502, 0.0389,
        0.2664, 1.7135, -0.0685,
        -0.1614, 0.0367, 1.0296,
    },
    .inverse = {
        0.986993, 0.432305, -0.00852866,
        -0.147054, 0.518360, 0.0400428,
        0.159963, 0.0492912, 0.968487,
    },
};

const hue_XYZ_adaptation *hue_XYZ_adaptation_von_kries = &(hue_XYZ_adaptation){
    .matrix = {
        0.40024, -0.22630, 0.0,
        0.70760, 1.16532, 0.0,
        -0.08081, 0.04570, 0.91822,
    },
    .inverse = {
        1.85994, 0.361191, 0.0,
        -1.12938, 0.638812, 0.0,
        0.219897, -6.3706e-06, 1.08906,
    },
};

const hue_XYZ_adaptation *hue_XYZ_adaptation_none = &(hue_XYZ_adaptation){
    .matrix = {
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
    },
    .inverse = {
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
    },
};

// White points

const hue_XYZ *hue_XYZ_ref_white_d50 = &(hue_XYZ){0.96422, 1.0, 0.82521};
const hue_XYZ *hue_XYZ_ref_white_d65 = &(hue_XYZ){0.95047, 1.0, 1.08883};
