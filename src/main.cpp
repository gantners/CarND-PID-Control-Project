#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

#define M_PI 3.14159265358979323846

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }

double deg2rad(double x) { return x * pi() / 180; }

double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
    auto found_null = s.find("null");
    auto b1 = s.find_first_of("[");
    auto b2 = s.find_last_of("]");
    if (found_null != std::string::npos) {
        return "";
    } else if (b1 != std::string::npos && b2 != std::string::npos) {
        return s.substr(b1, b2 - b1 + 1);
    }
    return "";
}

//https://discussions.udacity.com/t/twiddle-application-in-pid-controller/243427/9
void Restart(uWS::WebSocket <uWS::SERVER> ws) {
    std::string reset_msg = "42[\"reset\",{}]";
    ws.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);
}

int main() {
    uWS::Hub h;

    PID pid_steer;
    PID pid_speed;
    // TODO: Initialize the pid variable
    // Taken from lessons
    //pid_steer.Init(0.25, 3.0, 0.0001); //Initial manual guess
    //with twiddle P: 0.1882, D: 3.57564, I: 0.000363842
    //pid_steer.Init(0.0995, 2.975, 0.001);
    pid_steer.Init(0.0995, 2.975, 0.0005);
    // pid_speed.Init(0.4, 0.05, 0.00003);
    pid_speed.Init(0.39, 0.15, 0.00002);
    double target_throttle = 1.0;

    h.onMessage(
            [&pid_steer, &pid_speed, &target_throttle](uWS::WebSocket <uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
                // "42" at the start of the message means there's a websocket message event.
                // The 4 signifies a websocket message
                // The 2 signifies a websocket event
                if (length && length > 2 && data[0] == '4' && data[1] == '2') {
                    auto s = hasData(std::string(data).substr(0, length));
                    if (s != "") {
                        auto j = json::parse(s);
                        std::string event = j[0].get<std::string>();
                        if (event == "telemetry") {

                            // j[1] is the data JSON object
                            double cte = std::stod(j[1]["cte"].get<std::string>());
                            double speed = std::stod(j[1]["speed"].get<std::string>());
                            double angle = std::stod(j[1]["steering_angle"].get<std::string>());
                            double steer_value;

                            pid_steer.UpdateError(cte);
                            steer_value = pid_steer.TotalError(pid_steer.Kp, pid_steer.Kd, pid_steer.Ki);

                            pid_speed.UpdateError(fabs(cte));
                            double new_throttle = target_throttle + pid_speed.TotalError(pid_speed.Kp, pid_speed.Kd, pid_speed.Ki);

                            json msgJson;
                            msgJson["steering_angle"] = steer_value;
                            msgJson["throttle"] = new_throttle;
                            auto msg = "42[\"steer\"," + msgJson.dump() + "]";
                            //std::cout << msg << std::endl;
                            ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);

                            /*if (pid_steer.current_step % 10 == 0) {
                                std::cout << "Step: " << pid_steer.current_step
                                          << "\tCTE: " << cte
                                          << "\tSteering: " << steer_value
                                          << "\tAngle: " << angle
                                          << "\tSpeed: " << speed
                                          << "\tThrottle: " << new_throttle
                                          << std::endl;
                            }*/

                            //Reset on too large values for twiddle
                            /*if (pid_steer.total_error > 0.01) {
                                std::cout << "Reset to last known good configuration" << std::endl;
                                std::cout << "P: " << pid_steer.Kp << ", I: " << pid_steer.Ki << ", D: " << pid_steer.Kd
                                          << ", best_err: " << pid_steer.best_err << ", total_err: "
                                          << pid_steer.total_error << std::endl;
                                pid_steer.dp = pid_steer.last_good;
                                Restart(ws);
                            }*/

                        }
                    } else {
                        // Manual driving
                        std::string msg = "42[\"manual\",{}]";
                        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                    }

                }
            });

    // We don't need this since we're not using HTTP but if it's removed the program
    // doesn't compile :-(
    h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
        const std::string s = "<h1>Hello world!</h1>";
        if (req.getUrl().valueLength == 1) {
            res->end(s.data(), s.length());
        } else {
            // i guess this should be done more gracefully?
            res->end(nullptr, 0);
        }
    });

    h.onConnection([&h](uWS::WebSocket <uWS::SERVER> ws, uWS::HttpRequest req) {
        std::cout << "Connected!!!" << std::endl;
    });

    h.onDisconnection([&h](uWS::WebSocket <uWS::SERVER> ws, int code, char *message, size_t length) {
        ws.close();
        std::cout << "Disconnected" << std::endl;
    });

    int port = 4567;
    if (h.listen(port)) {
        std::cout << "Listening to port " << port << std::endl;
    } else {
        std::cerr << "Failed to listen to port" << std::endl;
        return -1;
    }
    h.run();
}
