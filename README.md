# QTBD5Viewer

### Copyright(C) 2022 RIKEN

## Synopsis
----
A visualizer for BD5-format files. 

## Execution environment
---
The application interface was developed on QT.
It was tested on computers with next Operating Systems:

1. Windows 11
2. Mac OSX Monterey
3. Linux Ubuntu distro

## Setup
---
An installer program is provided for Windows users.

For Mac OSX and Linux users, it is necessary to compile the application. <br>
Requirements for compilation:
1. A C++ compiler with C++17 support
2. Make
3. HDF5 library. The version 1.12.2 was used during development. Download available at https://www.hdfgroup.org/downloads/hdf5/
4. Qt SDK. The version 6.3.1 was used during development. Download available at https://www.qt.io/download

The source code contains a file titled INSTALL.txt where detailed instruction are described for the
three mentioned Operating Systems.

## Usage
---
From the main application menu select "Open" and search for a file with extension h5 (BD5 file extensions).
If the application is able to correctly interpret the file content it will generate a visualization. Then, you can manipulate it with the mouse.
If an error is found in the middle of the reading process, you can check the bd5Viewer.log log file for searching an explanation of the error. 
