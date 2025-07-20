#include "TrajectoryFollower.h"

#include "TrajectoryLogger.h"
#include "SDCard.h"
#include "Router.h"
// #include "Loader.h"

#define LOG_INTERVAL_US 5000
#define COMMAND_INTERVAL_US 1000

namespace TrajectoryFollower {

/**
 * Performs linear interpolation between two values.
 * @param a The starting value.
 * @param b The ending value.
 * @param t0 The starting time.
 * @param t1 The ending time.
 * @param t The current time.
 * @return The interpolated value at the current time.
 */
float lerp(float a, float b, float t0, float t1, float t) {
  if (t <= t0)
    return a;
  if (t >= t1)
    return b;
  if (t0 == t1)
    return b; // immediately get to b
  return a + (b - a) * ((t - t0) / (t1 - t0));
}

/**
 * Follows a trajectory curve by interpolating between position values.
 */
void followPositionLerpCurve() {
  lerp_point_pos *lpc = Loader::lerp_position_curve;
  elapsedMicros timer = elapsedMicros();
  unsigned long lastlog = timer;
  unsigned long lastloop = timer;

  long counter = 0;

  for (int i = 0; i < Loader::header.num_points - 1; i++) {
    while (timer / 1000000.0 < lac[i + 1].time) {
      float seconds = timer / 1000000.0;
      float x_pos = lerp(lpc[i].x, lpc[i + 1].x, lpc[i].time, lpc[i + 1].time, seconds);
      float y_pos = lerp(lpc[i].y, lpc[i + 1].y, lpc[i].time, lpc[i + 1].time, seconds);
      float z_pos = lerp(lpc[i].z, lpc[i + 1].z, lpc[i].time, lpc[i + 1].time, seconds);

      // TODO: Code to send lerped positions to Driver
      // Driver::loxODrive.setPos(lox_pos);
      // Driver::ipaODrive.setPos(ipa_pos);

      if (timer - lastlog > LOG_INTERVAL_US) {
        lastlog += LOG_INTERVAL_US;
        TrajectoryLogger::log_trajectory_csv(seconds, i, x_pos, y_pos, z_pos);
      }
      counter++;

      unsigned long target_slp = COMMAND_INTERVAL_US - (timer - lastloop);
      delayMicroseconds(target_slp < COMMAND_INTERVAL_US ? target_slp : 0); // don't delay for too long
      lastloop += COMMAND_INTERVAL_US;
    }
  }
  Router::info_no_newline("Finished ");
  Router::info_no_newline(counter);
  Router::info(" loop iterations.");
}

// init CurveFollower and add relevant router cmds
void begin() {
  Router::add({arm, "arm"});
}

// prompt user for log file name, then follow curve
void arm() {
  if (!Loader::loaded_curve) {
    Router::info("ARMING FAILURE: no curve loaded.");
    return;
  }

  if (!PT::zeroed_since_boot) {
    Router::info("ARMING FAILURE: pt boards have not been zeroed.");
    return;
  }

  // filenames use DOS 8.3 standard
  Router::info_no_newline("Enter log filename (1-8 chars + '.' + 3 chars): ");
  String log_file_name = Router::read(50);
  TrajectoryLogger::create_trajectory_log(log_file_name.c_str()); // lower case files have issues on teensy

  Router::info_no_newline("ARMING COMPLETE. Type `y` and press enter to confirm. ");
  String final_check_str = Router::read(50);
  if (final_check_str != "y") {
    Router::info("ARMING FAILURE: Cancelled by operator.");
    TrajectoryLogger::close_trajectory_log();
    return;
  }

  // Loader::header.is_thrust ? followThrustLerpCurve(lox_start, ipa_start) : followAngleLerpCurve();

  Router::info("Finished following curve!");
  TrajectoryLogger::close_trajectory_log();
}

} // namespace TrajectoryFollower