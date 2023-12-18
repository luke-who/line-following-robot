#define GEAR_RATIO 358.3
#define WHEEL_RADIUS 57.03
#define BOTTOM_LENGTH 153.252
#define CLOSE_MIN 40

class Kinematics_c {
  public:
    double x_global, y_global, theta_global = 0;

    Kinematics_c(Encoders_c *encoders_in, Motors_c *motors_in) {
      encoders = encoders_in;
      motors = motors_in;
    }
    void reset() {
      x_global, y_global = 0;
      theta_global = 0;
    }
    void update() {
      double left_velocity = double(encoders->left_velocity());
      double right_velocity = double(encoders->right_velocity());
      double x_relative = WHEEL_RADIUS * left_velocity / 2 + WHEEL_RADIUS * right_velocity / 2;
      double theta_relative = WHEEL_RADIUS * left_velocity / (2 * BOTTOM_LENGTH) - WHEEL_RADIUS * right_velocity / (2 * BOTTOM_LENGTH);
      x_global = x_global + x_relative * cos(theta_global) / GEAR_RATIO;
      y_global = y_global + x_relative * sin(theta_global) / GEAR_RATIO;
      theta_global = theta_global + TWO_PI * theta_relative / GEAR_RATIO;
    }
    double point_to(double target_x, double target_y) {
      double relative_x, relative_y, target_theta, best_error;
      relative_x = target_x - x_global;
      relative_y = target_y - y_global;
      target_theta = atan2(relative_y, relative_x);
      best_error = target_theta - theta_global;
      
      if (abs(best_error - TWO_PI) < abs(best_error)) {
        best_error = best_error - TWO_PI;
      } else if (abs(best_error + TWO_PI) < abs(best_error)) {
        best_error = best_error + TWO_PI;
      }

      return best_error;
    }
    bool close_to(double target_x, double target_y) {
      return (abs(target_x - x_global) <= CLOSE_MIN && abs(target_y - y_global) <= CLOSE_MIN);
    }
  private:
    Encoders_c *encoders;
    Motors_c *motors;
};
