/*
 * Copyright 2009-2011 IPOL Image Processing On Line http://www.ipol.im/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file colorbalance_lib.c
 * @brief color balance algorithms
 *
 * @author Nicolas Limare <nicolas.limare@cmla.ens-cachan.fr>
 * @author Jose-Luis Lisani <joseluis.lisani@uib.es>
 * @author Catalina Sbert <catalina.sbert@uib.es>
 */

#include <stdlib.h>
#include <string.h>

#include "balance_lib.h"
#include "colorspace_lib.h"

/**
 * @brief simplest color balance on RGB channels
 *
 * The input image is normalized by affine transformation on each RGB
 * channel, saturating a percentage of the pixels at the beginning and
 * end of the color space on each channel.
 *
 * @todo integer and histogram-based alternative
 */
float *colorbalance_rgb_f32(float *rgb, size_t size,
                            size_t nb_min, size_t nb_max)
{
    (void) balance_f32(rgb, size, nb_min, nb_max);
    (void) balance_f32(rgb + size, size, nb_min, nb_max);
    (void) balance_f32(rgb + 2 * size, size, nb_min, nb_max);
    return rgb;
}

/**
 * @brief simplest color balance in the HSL color space, L axis
 *
 * The input image is normalized by affine transformation on the L
 * axis, saturating a percentage of the pixels at the beginning and
 * end of the axis.
 *
 * @todo compute L from RGB, then compute new RGB from RGB and new L
 */
float *colorbalance_hsl_f32(float *rgb, size_t size,
                            size_t nb_min, size_t nb_max)
{
    float *hsl;
    /* convert to HSL */
    hsl = (float *) malloc(3 * size * sizeof(float));
    rgb2hsl(rgb, hsl, size);
    /* normalize the L channel */
    (void) balance_f32(hsl + 2 * size, size, nb_min, nb_max);
    /* convert back to RGB */
    hsl2rgb(hsl, rgb, size);
    free(hsl);
    return rgb;
}

/**
 * @brief simplest color balance in the HSV color space, V axis
 *
 * The input image is normalized by affine transformation on the V
 * axis, saturating a percentage of the pixels at the beginning and
 * end of the axis.
 *
 * @todo compute V from RGB, then compute new RGB from RGB and new V
 */
float *colorbalance_hsv_f32(float *rgb, size_t size,
                            size_t nb_min, size_t nb_max)
{
    float *hsv;
    /* convert to HSV */
    hsv = (float *) malloc(3 * size * sizeof(float));
    rgb2hsv(rgb, hsv, size);
    /* normalize the V channel */
    (void) balance_f32(hsv + 2 * size, size, nb_min, nb_max);
    /* convert back to RGB */
    hsv2rgb(hsv, rgb, size);
    free(hsv);
    return rgb;
}

/**
 * @brief simplest color balance in the HSI color space, I axis
 *
 * The input image is normalized by affine transformation on the I
 * axis, saturating a percentage of the pixels at the beginning and
 * end of the axis. The HSI cube is not stable by this operation, so
 * when the result is oput of the RGB cube, a clipping will be
 * performed independantly on each RGB channel (thus with somne color
 * distortion).
 *
 * @todo add another mode: clip before hsi2rgb
 * @todo compute I from RGB, then compute new RGB from RGB and new I
 */
float *colorbalance_hsi_f32(float *rgb, size_t size,
                            size_t nb_min, size_t nb_max)
{
    float *hsi;
    size_t i;
    /* convert to HSI */
    hsi = (float *) malloc(3 * size * sizeof(float));
    rgb2hsi(rgb, hsi, size);
    /* normalize the I channel */
    (void) balance_f32(hsi + 2 * size, size, nb_min, nb_max);
    /* convert back to RGB */
    hsi2rgb(hsi, rgb, size);
    free(hsi);
    /* clip RGB values to [0, 1] */
    for (i = 0; i < 3 * size; i++) {
        /** @todo use boolean arithmetics, avoid if.. branching */
        if (rgb[i] > 1.)
            rgb[i] = 1.;
        if (rgb[i] < 0.)
            rgb[i] = 0.;
    }
    return rgb;
}

/**
 * @brief simplest color balance based on the I axis applied to the
 * RGB channels, bounded
 *
 * The input image is normalized by affine transformation on the I
 * axis, saturating a percentage of the pixels at the beginning and
 * end of the axis. This transformation is linearly applied to the R,
 * G and B channels. The HSI cube is not stable by this operation, so
 * some clipping will happen when the result is out of the RGB cube.
 */
float *colorbalance_irgb_bounded_f32(float *rgb, size_t size,
                                     size_t nb_min, size_t nb_max)
{
    float *irgb, *inorm;        /* intensity scale */
    double s;
    size_t i;
    /** @todo work with I=R+G+B instead of (R+G+B)/3 to save a division */
    irgb = (float *) malloc(size * sizeof(float));
    for (i = 0; i < size; i++)
        irgb[i] = (rgb[i] + rgb[i + size] + rgb[i + 2 * size]) / 3;
    /* copy and normalize I */
    inorm = (float *) malloc(size * sizeof(float));
    memcpy(inorm, irgb, size * sizeof(float));
    (void) balance_f32(inorm, size, nb_min, nb_max);
    /*
     * apply the I normalization to the RGB channels:
     * RGB = RGB * Inorm / I
     */
    for (i = 0; i < size; i++) {
        s = inorm[i] / irgb[i];
        rgb[i] *= s;
        rgb[i + size] *= s;
        rgb[i + 2 * size] *= s;
    }
    free(irgb);
    free(inorm);
    return rgb;
}