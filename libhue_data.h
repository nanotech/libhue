#ifndef LIBHUE_DATA_H_
#define LIBHUE_DATA_H_

#include "libhue.h"

typedef struct {
    const hue_float to_XYZ[9];
    const hue_float to_RGB[9];
    hue_XYZ reference_white;
    hue_float gamma;
} hue_XYZ_RGB_model;

typedef struct {
    const hue_float matrix[9];
    const hue_float inverse[9];
} hue_XYZ_adaptation;


extern const hue_XYZ_RGB_model *hue_XYZ_RGB_model_sRGB;

extern const hue_XYZ_adaptation *hue_XYZ_adaptation_none;
extern const hue_XYZ_adaptation *hue_XYZ_adaptation_von_kries;
extern const hue_XYZ_adaptation *hue_XYZ_adaptation_bradford;

extern const hue_XYZ *hue_XYZ_ref_white_d50;
extern const hue_XYZ *hue_XYZ_ref_white_d65;

#endif
