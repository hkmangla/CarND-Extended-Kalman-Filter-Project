#include "kalman_filter.h"
#include "tools.h"
#include <iostream>
using Eigen::MatrixXd;
using Eigen::VectorXd;

/* 
 * Please note that the Eigen library does not initialize 
 *   VectorXd or MatrixXd objects with zeros upon creation.
 */

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
   * TODO: predict the state
   */
  x_ = F_ * x_;
  P_ = (F_ * P_) * F_.transpose() + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
   * TODO: update the state by using Kalman Filter equations
   */
  VectorXd y = z - H_ * x_;
  UpdateUsingY(y);
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
   * TODO: update the state by using Extended Kalman Filter equations
   */
  VectorXd hx = VectorXd(3);
  float px = x_(0), py = x_(1);
  float vx = x_(2), vy = x_(3);
  float rho = sqrt(px*px + py*py);
  if (fabs(rho) < 0.0001) {
      px += 0.001;
      py += 0.001;
      rho = sqrt(px*px + py*py);
  }
  float theta = atan2(py, px);
  float rho_dot = (px*vx + py*vy) / rho;

  hx << rho, theta, rho_dot;
  VectorXd y = z - hx;
  while (y(1)>M_PI) y(1) -= 2 * M_PI;
  while (y(1)<-M_PI) y(1) += 2 * M_PI;

  UpdateUsingY(y);
}

void KalmanFilter::UpdateUsingY(const VectorXd &y) {
  
  MatrixXd Ht = H_.transpose();
  MatrixXd S = (H_ * P_) * Ht + R_;
  MatrixXd K = (P_ * Ht) * S.inverse();
  MatrixXd I = MatrixXd::Identity(4, 4);

  x_ = x_ + (K * y);
  P_ = (I - K * H_) * P_;
} 
