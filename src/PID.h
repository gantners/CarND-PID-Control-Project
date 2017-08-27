#ifndef PID_H
#define PID_H
#include "math.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iterator>

class PID {
public:
    /*
    * Errors
    */
    double last_error;
    double sum_error;
    double change_error;

    /*
    * Coefficients
    */
    double Kp;
    double Ki;
    double Kd;

    bool pActive;
    bool dActive;
    bool iActive;

    double best_err;

    //track current iteration
    int current_step;

    //twiddle max iterations
    int iterations;

    //patience
    int patience_steps;

    //stage for twiddle
    int stage;

    //current param to tune
    int pIndex;

    double total_error;

    std::vector<double> dp;

    std::vector<double> last_good;

    /*
    * Constructor
    */
    PID();

    /*
    * Destructor.
    */
    virtual ~PID();

    /*
    * Initialize PID.
    */
    void Init(double Kp, double Ki, double Kd);

    /*
    * Update the PID error variables given cross track error.
    */
    void UpdateError(double current_error);

    /*
    * Calculate the total PID error.
    */
    double TotalError(double Kp, double Ki, double Kd);

    /*
     * Tune params with twiddle algorithm
     */
    void twiddle();

};

#endif /* PID_H */
