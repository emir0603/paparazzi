#include "overo_estimator.h"

#include "booz/booz_imu.h"
#include <math.h>

struct OveroEstimator estimator;

void estimator_init(void) {
  estimator.tilt_lp_coeff = 0.9;
  estimator.elevation_lp_coeff = 0.9;
}

//bench sensors z,y,x values passed in
void estimator_run(uint16_t tilt_measure, uint16_t elevation_measure, uint16_t azimuth_measure) {
  
  const int32_t tilt_neutral = 1970;
  const float   tilt_scale = 1./580.;
  const int32_t azimuth_neutral = 2875;
  const float   azimuth_scale = 1./580.;
  const int32_t elevation_neutral = 670;
  const float   elevation_scale = 1./580.;


  estimator.tilt = -(tilt_neutral - (int32_t)tilt_measure ) * tilt_scale;
  Bound(estimator.tilt,-89,89);
  //low pass filter gyro values
  estimator.tilt_dot = estimator.tilt_dot + 
                         estimator.tilt_lp_coeff * (RATE_FLOAT_OF_BFP(booz_imu.gyro.q) - estimator.tilt_dot);
  /* Second order filter to be tested
  estimator.tilt_dot = estimator.tilt_dot * (2 - estimator.tilt_lp_coeff1 - estimator.tilt_lp_coeff2) +
                         estimator.tilt_lp_coeff1 * estimator.tilt_lp_coeff2 * RATE_FLOAT_OF_BFP(booz_imu.gyro.q) -
                         estimator.tilt_dot_old * (1 - estimator.tilt_lp_coeff1 - estimator.tilt_lp_coeff2 +
                         estimator.tilt_lp_coeff1*estimator.tilt_lp_coeff2);
  */

  estimator.elevation = (elevation_neutral - (int32_t)elevation_measure ) * elevation_scale;
  Bound(estimator.elevation,-45,45);

  //rotation compensation (mixing of two gyro values to generate a reading that reflects rate along beth axes
  float rotated_elev_dot = RATE_FLOAT_OF_BFP(booz_imu.gyro.p) * cos(estimator.tilt) +
                             RATE_FLOAT_OF_BFP(booz_imu.gyro.r) * sin(estimator.tilt);
  //low pass filter -- should probably increase order and maybe move filtering to measured values.
  estimator.elevation_dot = estimator.elevation_dot + 
                              estimator.elevation_lp_coeff * (rotated_elev_dot - estimator.elevation_dot);

  estimator.azimuth = (azimuth_neutral - (int32_t)azimuth_measure ) * azimuth_scale;
  estimator.azimuth_dot = RATE_FLOAT_OF_BFP(booz_imu.gyro.r);

}

