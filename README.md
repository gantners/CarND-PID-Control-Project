# CarND-Controls-PID

Self-Driving Car Engineer Nanodegree Program

---

## Instructions

The code is depending on the hardware, so it turned out that even
the recording on high resolution has an impact on how well the car drives on the road.

So the final parameters are a mix between good driving and recording. As seen in the video
the car is able to drive around the track. 

Depending on the underlying hardware this can differ and
the parameters of the speed controller can be chosen more conservative, like changing P from 0.39 to 0.45. That
will led to slow down more often in curves.

## Reflections

### Steering Controller

First i started implementing the P controller following the lessons and slightly approximate all PID params.

The steering angle tau is a factor which is proportional to the Cross Track Error (CTE) that makes the car oscillating 
around the reference track because it overshoots basically all the time.
That makes the car able to do sharp turns.
So P has been chosen to be able to oscillate around the track without leaving it.

As we do not want to oscillate on the straight parts of the track we need a factor that 
reduces the oscillation where the CTE is getting closer to the reference.
The temporal derivate of the CTE, the "D" term, is added to the steering angle, which basically recognizes when
the car is getting closer again to the reference and starts to countersteer the oscillating of the "P" term and
allow a graceful approaching of our target projectory.
So i increased the "D" term slightly to visually reduce the oscillation in a way, that the car
is still able to drive around the track but without wild oscillations.

Usually in the real world, the steering of the car is not 100% ideal, that means there is always a
systematic bias which most of us unconsciously correct by steering stronger.

As in our simulation we do have a ideal steering conditions and not any other disturbances like constant wind which could add such a bias,
so we do not necessarily need it for the solution.

I used it for increasing the angle of the steering a little bit earlier for sharp turns.
As the track has mostly left curves i choose the "I" term factor so that the car is more to the inner side of the track and
thus is able to drive a bit faster.

### Speed Controller

As the car was already able to drive around the track with a speed constant of 0.5, i wanted to add another pid controller for 
reducing the throttle when CTE of steering is to high.
So i created another instance of my current steering controller and change the PID params accordingly.

#### How the final params have been chosen

For the steering controller it's already described in the steering controller section above.
In general and especially for the speed controller it was mostly trial and error starting from increasing P and D alternatingly.

I tried to implement twiddle for the steering controller but did not very much succeed with it,
as it seemed odd to me to restart the sim for each param change.

So the way i went about it, was to implement a sort of on the fly twiddle where the params are 
constantly changed and set back to the last known good configuration in case the CTE went too high.

But this approach did no lead to better params than how i did it manually with trial and error.

### Summary

* P (Proportional) = Proportion to the error itself
* D (Differential) = Proportion to the change of the error
* I (Integral)     = Proportion to the Integral or sum of all errors observed.
 

## Video

[![Watch the video](https://img.youtube.com/vi/d1PKWdPvfYE/0.jpg)](https://youtu.be/d1PKWdPvfYE)

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

There's an experimental patch for windows in this [PR](https://github.com/udacity/CarND-PID-Control-Project/pull/3)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

## Editor Settings

We've purposefully kept editor configuration files out of this repo in order to
keep it as simple and environment agnostic as possible. However, we recommend
using the following settings:

* indent using spaces
* set tab width to 2 spaces (keeps the matrices in source code aligned)

## Code Style

Please (do your best to) stick to [Google's C++ style guide](https://google.github.io/styleguide/cppguide.html).

## Project Instructions and Rubric

Note: regardless of the changes you make, your project must be buildable using
cmake and make!

More information is only accessible by people who are already enrolled in Term 2
of CarND. If you are enrolled, see [the project page](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/f1820894-8322-4bb3-81aa-b26b3c6dcbaf/lessons/e8235395-22dd-4b87-88e0-d108c5e5bbf4/concepts/6a4d8d42-6a04-4aa6-b284-1697c0fd6562)
for instructions and the project rubric.

## Hints!

* You don't have to follow this directory structure, but if you do, your work
  will span all of the .cpp files here. Keep an eye out for TODOs.

## Call for IDE Profiles Pull Requests

Help your fellow students!

We decided to create Makefiles with cmake to keep this project as platform
agnostic as possible. Similarly, we omitted IDE profiles in order to we ensure
that students don't feel pressured to use one IDE or another.

However! I'd love to help people get up and running with their IDEs of choice.
If you've created a profile for an IDE that you think other students would
appreciate, we'd love to have you add the requisite profile files and
instructions to ide_profiles/. For example if you wanted to add a VS Code
profile, you'd add:

* /ide_profiles/vscode/.vscode
* /ide_profiles/vscode/README.md

The README should explain what the profile does, how to take advantage of it,
and how to install it.

Frankly, I've never been involved in a project with multiple IDE profiles
before. I believe the best way to handle this would be to keep them out of the
repo root to avoid clutter. My expectation is that most profiles will include
instructions to copy files to a new location to get picked up by the IDE, but
that's just a guess.

One last note here: regardless of the IDE used, every submitted project must
still be compilable with cmake and make./
