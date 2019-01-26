#include "libhue_data.h"

// RGB Models

const hue_XYZ_RGB_model hue_XYZ_RGB_model_sRGB = (hue_XYZ_RGB_model){
    .to_XYZ = {
        0.4124564391f, 0.2126728514f, 0.0193338956f,
        0.3575760776f, 0.7151521553f, 0.1191920259f,
        0.1804374833f, 0.0721749933f, 0.9503040785f,
    },
    .to_RGB = {
         3.2404541621f, -0.9692660305f,  0.0556434310f,
        -1.5371385128f,  1.8760108454f, -0.2040259135f,
        -0.4985314096f,  0.0415560175f,  1.0572251882f,
    },
    .reference_white = {0.95047f, 1.0f, 1.08883f},
    .gamma = -2.2f,
};

// Adaptations

const hue_XYZ_adaptation hue_XYZ_adaptation_bradford = (hue_XYZ_adaptation){
    .matrix = {
        0.8951f, -0.7502f, 0.0389f,
        0.2664f, 1.7135f, -0.0685f,
        -0.1614f, 0.0367f, 1.0296f,
    },
    .inverse = {
        0.986993f, 0.432305f, -0.00852866f,
        -0.147054f, 0.518360f, 0.0400428f,
        0.159963f, 0.0492912f, 0.968487f,
    },
};

const hue_XYZ_adaptation hue_XYZ_adaptation_von_kries = (hue_XYZ_adaptation){
    .matrix = {
        0.40024f, -0.22630f, 0.0f,
        0.70760f, 1.16532f, 0.0f,
        -0.08081f, 0.04570f, 0.91822f,
    },
    .inverse = {
        1.85994f, 0.361191f, 0.0f,
        -1.12938f, 0.638812f, 0.0f,
        0.219897f, -6.3706e-06f, 1.08906f,
    },
};

const hue_XYZ_adaptation hue_XYZ_adaptation_none = (hue_XYZ_adaptation){
    .matrix = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    },
    .inverse = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    },
};

// White points

const hue_XYZ hue_XYZ_ref_white_d50 = (hue_XYZ){0.96422f, 1.0f, 0.82521f};
const hue_XYZ hue_XYZ_ref_white_d65 = (hue_XYZ){0.95047f, 1.0f, 1.08883f};
