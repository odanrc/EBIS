# EBIS - Eye Based Independance System

EBIS is a system to allow people with physical disabilities use computers. It emulates mouse and keyboard commands with facial gestures, such as blinking and eye movement. It is based on the article "Monocular Gaze Tracking System for Disabled People Assistance". If you use this system, please cite 

Carvalho, Daniel Rodrigues, and Maurcio Pamplona Segundo. "Monocular Gaze Tracking System for Disabled People Assistance."

The system is provided AS IS under the MIT License.

## SYSTEM REQUIREMENTS

Currently, only Linux is supported. The system uses OpenCV for the algorithms, and SDL 2.0 and X11 for command generation.

## HOW TO USE

Create an instance of the system and call update(cv::Mat image). The system will automatically detect faces and calculate gaze direction. Whenever a user closes both eyes, the system will change mode. Check src/exe/main.cc for an example, and the section **Modes** for command interpretation and mode description.

There's also a Pong game implemented, which can be played with the eyes by calling playPong(cv::Mat image).

## MODES

###Display
Normal use mode. The mouse does not move, and is not visible, in order to allow user to see the screen contents.

###Menu Mode
This mode shows all other modes, so that user can click on the mode to be started.

###Follow Mode
Free roam mode: mouse is moved freely on the screen horizontally and vertically. Clicks are performed by blink commands.

###Left Click Mode
Left clicks are performed at this mode. The magnification steps are performed, and after each click command, the screen is magnified, until there's enough click precision. i.e., for a 3x3 grid 6 magnifications are necessary before a click is emitted.

###Scroll Mode
Screen scrolling is activated once the mouse achieved a certain threshold on the screen. i.e., if the focal point is at 20% of the screen height, an upwards scrolling starts, and if it is at 80% of the screen height, downward scrolling happens. 

###Right Click Mode - Not implemented yet
Performs a right click. Can be a Left Click Mode followed by a screen with the options generated by the right click.

###Double Click Mode - Not implemented yet
Uses an approach analogous to the left click mode to perform a double click.

###Drag Mode - Not implemented yet
Allows the user to perform a drag operation by performing 2 Left Click Modes in a row: the first click to set the src position, and the second to set the dst position.

###Type Mode - Not implemented yet
Allows the user to type fast. Good results can be achieved by using something similiar to Ward's Dasher.

###Keyboard Mode - Not implemented yet
Different from the Type mode, this mode creates a virtual keyboard that allows not only keystrokes, but useful keystroke combinations, such as alt+tab, ctrl+alt+del, etc.

## TODO

The system still lacks a lot of functionality, as it does not fully allow human-computer interaction yet. Besides that, the algorithms used within the gaze detector should always be improved, in order to achieve maximum robustness.

- Improve blink detection. One small and simple option is to train a Haar Classifier for closed eyes.
- Create a calibration step to calculate user screen distance. It's suggested to create a dot on the screen and require the user to look at it. The system should estimate various screen distances, and the one that leads to a focal point that is the closest to the dot should be the base screen distance.
- Hack XSendEvent, or use another way to emulate clicks, as it is not accepted by all softwares.
- Use Kalman Filters to better estimate pupil position and gaze direction.
- Implement missing operation modes
- Improve desktop screenshot acquisition
- Create and Test Windows Version.
- Improve Makefile

## COPYRIGHT NOTICE

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer; redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution; neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
