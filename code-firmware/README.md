# Attention Circuits Control Laboratory - I/O SynchBox project
# README documentation.
Written by Christopher Thomas.


## Important things to note:

* Arduino's development environment will auto-detect and include any file
that has a `.ino`, `.c`, `.cpp`, or `.h` extension.

* The Arduino IDE doesn't like hyphens in filenames; use underscores instead.

* I gave the Makefile a customized name to avoid confusing the Arduino IDE.
It will generate its own Makefile when building the project.

* **Building with the Arduino IDE is possible but not officially supported.**
There are shims in place for it, and it may be officially supported again
in the future, but for now it's not guaranteed to work.



## Building using the NeurAVR library and "`make`":

To build using the NeurAVR library, use:

`make -f Makefile.neuravr NEURAVRPATH=(path) (targets)`

Type `make` for a list of available targets. Typical targets are:

* `clean` -- Removes binaries so that the Arduino IDE doesn't complain
about them.
* `hex` -- Builds the hex files (and copies them to the top-level hex
folder).
* `burn` -- Calls `avrdude` to write the hex image to `/dev/ttyACM0`.
* `test` -- Connects to `/dev/ttyACM0` using the `cu` serial client.

The `NEURAVRPATH` variable defaults to looking for `neuravr` in the build
directory. This is a symlink to the `NeurAVR` project tree; the symlink
can be changed as an alternative to specifying a path variable.


## Building using the Arduino IDE:

* Copy (or symlink) the project directory into your Arduino sketchbook
  directory. The directory name must match the name of the top-level .ino file
  (so, "synchbox" for this project).

* Open and edit the project per normal.

* On my machine, under Linux, the Arduino IDE sometimes can't open the serial
port to burn the sketch. Once you've found the hex file, the sketch can be
burned manually using:

`avrdude -c stk500 -p m2560 -P (port) -D -U flash:w:(file):i`

...Where "`(port)`" is usually `/dev/ttyACM0` or `/dev/ttyUSB0` under Linux,
and `(file)` is the hex file.

The Arduino IDE puts the hex file in:

`/tmp/build(gibberish).tmp/(filename).hex`

Use "`ls -t /tmp|grep build|head -1`" to find the right build directory;
every build attempt will create a new directory, so there will be several
of them.


_This is the end of the file._
