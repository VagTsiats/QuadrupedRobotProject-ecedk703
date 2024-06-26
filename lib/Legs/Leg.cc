#include "Leg.h"
using namespace BLA;
Leg::Leg(int t_pin_shoulder, int t_pin_knee, int t_pin_ankle, const std::vector<float> &t_zeros, const std::vector<float> &t_polar, bool t_left)
{
    pin_shoulder = t_pin_shoulder;
    pin_knee = t_pin_knee;
    pin_ankle = t_pin_ankle;
    left = t_left;

    theta = {0, 0, 0};

    for (int i = 0; i < 3; i++)
    {
        zeros[i] = t_zeros[i];
        polar[i] = t_polar[i];
    }

    BaseFrameTranslation = {0, 0, 1, 0,
                            0, 1, 0, 0,
                            -1, 0, 0, 0,
                            0, 0, 0, 1};
}

void Leg::attach_servos()
{
    shoulder.attach(pin_shoulder);
    knee.attach(pin_knee);
    ankle.attach(pin_ankle);
}

void Leg::setDh(BLA::Matrix<3> t_dh_a, BLA::Matrix<3> t_dh_alpha, BLA::Matrix<3> t_dh_d)
{
    dh_a = t_dh_a;
    dh_alpha = t_dh_alpha;
    dh_d = t_dh_d;
}

void Leg::driveLeg()
{
    update_leg();

    shoulder.write(polar[0] * rad2deg(theta(0)) + zeros[0]);
    knee.write(polar[1] * rad2deg(theta(1)) + zeros[1]);
    ankle.write(polar[2] * rad2deg(theta(2)) + zeros[2]);
}

void Leg::driveLeg(BLA::Matrix<3> th)
{
    setTheta(th);
    driveLeg();
}

void Leg::update_leg()
{
    ForwardKinematics();

    z0 = {BaseFrameTranslation(0, 2), BaseFrameTranslation(1, 2), BaseFrameTranslation(2, 2)};
    z1 = {T01(0, 2), T01(1, 2), T01(2, 2)};
    z2 = {T02(0, 2), T02(1, 2), T02(2, 2)};

    p1 = {T01(0, 3), T01(1, 3), T01(2, 3)};
    p2 = {T02(0, 3), T02(1, 3), T02(2, 3)};
    pe = {T03(0, 3), T03(1, 3), T03(2, 3)};

    computeJacobian();
}

Matrix<4, 4> Leg::dhTransform(float a, float alpha, float d, float theta)
{
    Matrix<4, 4> T = {cos(theta), -sin(theta) * cos(alpha), sin(theta) * sin(alpha), a * cos(theta),
                      sin(theta), cos(theta) * cos(alpha), -cos(theta) * sin(alpha), a * sin(theta),
                      0, sin(alpha), cos(alpha), d,
                      0, 0, 0, 1};
    return T;
}

void Leg::ForwardKinematics()
{
    T01 = BaseFrameTranslation * dhTransform(dh_a(0), dh_alpha(0), dh_d(0), theta(0));
    T12 = dhTransform(dh_a(1), dh_alpha(1), dh_d(1), theta(1));
    T23 = dhTransform(dh_a(2), dh_alpha(2), dh_d(2), theta(2));

    T02 = T01 * T12;
    T03 = T02 * T23;

    return;
}

const Matrix<3> Leg::getEndEffectorPosition()
{
    return pe;
}

void Leg::computeJacobian()
{

    Jo = z0 || z1 || z2;

    Jp = crossProduct(z0, pe) || crossProduct(z1, pe - p1) || crossProduct(z2, pe - p2);

    Jacobian = Jo && Jp;
    return;
}

const Matrix<6, 3> Leg::getJacobian()
{
    return Jacobian;
}

const Matrix<3, 3> Leg::getJacobianPos()
{
    return Jp;
}

const BLA::Matrix<3> Leg::getTheta()
{
    return theta;
}

void Leg::setTheta(BLA::Matrix<3> t_theta)
{
    theta = t_theta;
}

// void Leg::JInvIK(Matrix<3> theta0, Matrix<3> xd, Matrix<3> xd_dot)
const BLA::Matrix<3> Leg::JInvIK(BLA::Matrix<3> x_des, BLA::Matrix<3> xd_des, BLA::Matrix<3, 3> t_gain, float t_dt, BLA::Matrix<3> t_initial_configuration)
{
    // if (initialisation)
    // {
    //     theta = t_initial_configuration;
    //     initialisation = false;
    // }

    // For theta 0 -pi/4 -pi/4 -> 7.95 0 -9.95 so we want to move on the z axis only with inverse dif kinematics:
    Matrix<3> error = x_des - getEndEffectorPosition();
    theta += (BLApseudoInverse(getJacobianPos()) * (xd_des + t_gain * error)) * t_dt;

    return error;
}

const BLA::Matrix<3> Leg::JTranspIK(BLA::Matrix<3> x_des, BLA::Matrix<3, 3> t_gain, float t_dt, BLA::Matrix<3> t_initial_configuration)
{

    if (initialisation)
    {
        theta = t_initial_configuration;
        initialisation = false;
    }

    Matrix<3> error = x_des - getEndEffectorPosition();

    theta += (~(getJacobianPos()) * (t_gain * error)) * t_dt;

    return error;
}
// DONE
Matrix<3> Leg::InverseKinematics(Matrix<3> pos)
{
    float x = pos(0);
    float y;
    if (left)
        y = pos(1);
    else
        y = -pos(1);
    float z = -pos(2);
    // Matrix<3> theta;
    float l1 = 5;
    float l2 = 11;
    float l3 = 13;

    float A = sqrt(z * z + y * y);
    float a1 = atan2(y, z);
    float a3 = M_PI_2 - asin(l1 / A);
    if (left)
        theta(0) = a1 + a3 - M_PI_2;
    else
        theta(0) = -(a1 + a3 - M_PI_2);

    float c1 = cos(theta(0));
    float s1 = sin(theta(0));
    float z1 = z + l1 * s1;
    z = z1 / c1;

    float A2 = sqrt(z * z + x * x);
    float R1 = atan2(x, z);
    float R2 = CosineTheoremAngle(A2, l2, l3);
    theta(1) = -(R2 - R1);

    float R3 = CosineTheoremAngle(l2, l3, A2);
    theta(2) = (M_PI - R3);
    return theta;
}

// NOTE - Unused Functions
// void Leg::resetInitialPos()
// {
//     initialisation = false;
//     return;
// }
//
// BLA::Matrix<3> Leg::getDh(int a)
// {
//     if (a == 0)
//     {
//         return dh_a;
//     }
//     else if (a == 1)
//     {
//         return dh_alpha;
//     }
//     else
//     {
//         return dh_d;
//     }
// }