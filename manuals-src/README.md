# "I/O SynchBox" Data Logger and Timing Signal Generator - Manuals

## Quick-Start

The Makefile automates most operations involved in building the manuals.

* Type "`make clean`" to remove the manual PDFs.
* Type "`make all`" to rebuild the manual PDFs.
* Type "`make aux`" to rebuild symlinks to schematics, pinouts, etc.

The "guide" contains information needed for _using_ the SynchBox, and the
"manual" contains information needed for _developing_ the SynchBox (in
addition to a copy of the "guide" material).


## Notes

* **The manuals are very out of date.** They were last updated in 2018,
and were incomplete even then.

* The `rebuild-auxlinks.sh` script rebuilds "automated" symbolic links.
It's called by "`make aux`"; there's no need to call it directly.


## Folders

* `captures` -- Manually-edited capture text from SynchBox sessions.
* `figs` -- Diagrams drawn specifically for the manuals.
* `layouts` -- Symbolic links to layout plots from `packages` folders.
Automatically generated.
* `notes` -- Miscellaneous notes useful for writing the manuals.
* `photos` -- Symbolic links to selected images in `photos-dir`.
* `photos-dir` -- Symbolic links to the `photos` folder in the main tree.
* `pinouts` -- Symbolic links to pinout diagrams from `packages` folders.
Automatically generated.
* `schematics` -- Symbolic links to schematics from `packages` folders.
automatically generated.
* `screenshots` -- Manually-edited screenshots from SynchBox communication
sessions, firmware updating, etc.

_This is the end of the file._
