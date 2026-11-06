# University of Leeds 2026-27
# COMP 5893M Assignment 1 Triangle Soup Renderer

This is a very simple renderer for triangle soup. It makes no attempt to be efficient, which keeps the code clearer.  It should work with either QT 5 or QT 6.

LINUX:
=================

To compile on Linux machines, you will need to do the following:

[userid@machine triangle_renderer]$ qmake -project "QT += core gui widgets opengl openglwidgets" "LIBS += -lGL -lGLU"
[userid@machine triangle_renderer]$ qmake
[userid@machine triangle_renderer]$ make

You should see some compiler warnings, which can be ignored.

To execute the renderer, pass the file name on the command line:

[userid@machine triangle_renderer]$ ./triangle_renderer ../handout_/models/tetrahedron.tri

For Red Hat in the lab, you will need to use:
[userid@redhatmachine triangle_renderer]$ QT_QPA_PLATFORM=xcb ./triangle_renderer ../handout_/models/tetrahedron.tri
