# Groover

Groover is a simple remote for the [Groove Basin](http://groovebasin.com)
music player daemon. It uses GTK+ and WebKitGTK+ to provide a window where
Groove Basin's standard web interface is loaded — honestly I do not see the
whole point of re-creating the UI from scratch, so the idea is to keep
Groover simple, and focus on desktop integration instead.


## Features

- Shows the Groove Basin UI as a standalone application (and its own icon, 
  and so on), without the need to launch a complete web browser to access
  it. This is not very different from the “web application mode” of Web;
  but does not require it to be installed.


## (Planned) Features

- Simple “Now Playing” screen mode which uses only GTK+ widgets, without
  requiring WebKitGTK+ to load Groove Basin's web UI.
- Support acting as a command-line remote for Groove Basin.
- MPRIS2 provider, so other applications can control Groove Basin via a
  compatible client.
