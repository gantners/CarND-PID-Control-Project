#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

/*
* Initialize PID.
*/
void PID::Init(double Kp, double Kd, double Ki) {
    this->Kp = Kp;
    this->Kd = Kd;
    this->Ki = Ki;

    this->pActive = true;
    this->dActive = true;
    this->iActive = true;

    this->last_error = 0.0;
    this->sum_error = 0.0;
    this->change_error = 0.0;

    this->best_err = 1000.0;

    //count iterations passed
    this->current_step = 0;

    //stop at bridge and reset
    this->iterations = 1000;

    //according to sebastian waiting at least x current_steps to have stable values
    this->patience_steps = 1000;

    this->stage = 0;

    this->pIndex = 0;

    this->total_error = 0.0;

    this->dp = std::vector<double>({1.0, 1.0, 1.0});

    this->last_good = std::vector<double>({Kp, Kd, Ki});
}

/*
  * Update the PID error variables given cross track error.
  */
void PID::UpdateError(double current_error) {
    if (current_step == 1) {
        last_error = current_error;
    }
    change_error = current_error - last_error;
    last_error = current_error;
    sum_error += current_error;

    //Parameter optimization L135
    //if (current_step % (patience_steps + iterations) >= patience_steps) {
        //total_error += pow(current_error, 2);
        //total_error /= iterations;
        //cout << "total_err: " << total_error << endl;
    //}

    if (current_step % (patience_steps + iterations) == 0) {
        if (false && total_error > 0.0)
            twiddle();
    }
    current_step++;
}

/*
  * Calculate the total PID error.
  */
double PID::TotalError(double Kp, double Kd, double Ki) {
    double val = 0.0;
    if(pActive)
        val -= Kp * last_error;
    if(dActive)
        val -= Kd * change_error;
    if(iActive)
        val -= Ki * sum_error;

    return val;
}

void PID::twiddle() {
    double total = std::accumulate(dp.begin(), dp.end(), 0);
    cout << "Total " << total << endl;

    bool save = false;

    double to_tweak = 1.0;
    switch (pIndex) {
        case 0:
            to_tweak = dp[0];
            break;
        case 1:
            to_tweak = dp[1];
            break;
        case 2:
            to_tweak = dp[2];
            break;
    }

    if (stage == 0) {
        to_tweak += dp[pIndex];

        if (total_error < best_err) {
            best_err = total_error;
            cout << "Improved1: " << total_error << endl;
            save = true;
            dp[pIndex] *= 1.1;
            stage = 0; //Stay in stage 0 as long as its improving
            cout << "-> Stage 0 improved, Param " << pIndex << endl;
        } else {
            to_tweak -= 2 * dp[pIndex];
            stage = 1;
            cout << "-> Stage 0 decreased, Param " << pIndex << endl;
        }
    } else if (stage == 1) {
        if (total_error < best_err) {
            best_err = total_error;
            cout << "Improved2: " << best_err << endl;
            save = true;
            dp[pIndex] *= 1.1;
            stage = 0; //Improving again -> level up!
            cout << "-> Stage 1, Param " << pIndex << endl;
        } else {
            to_tweak += dp[pIndex];
            dp[pIndex] *= 0.9; //Getting worse - slow down!
            stage = 1;
            cout << "-> Stage 1 Worst, Param " << pIndex << endl;
        }

    }

    switch (pIndex) {
        case 0:
            Kp = to_tweak;
            break;
        case 1:
            Kd = to_tweak;
            break;
        case 2:
            Ki = to_tweak;
            break;
    }

    //If we have a new best_err save the params used for it
    if (save) {
        this->last_good = std::vector<double>({dp[0], dp[1], dp[2]});
    }

    cout << "P: " << Kp << ", I: " << Ki << ", D: " << Kd << ", best_err: " << best_err << ", total_err: "
         << total_error << endl;
    total_error = 0;
    pIndex = pIndex == 2 ? 0 : pIndex + 1;
}
