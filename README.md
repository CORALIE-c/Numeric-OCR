# Numeric-Digit-OCR

An Optical Character Recognition (OCR) system designed to recognize numeric 
digits using statistical methods.

This project was implemented during University. The design
of this does not necessarily reflect the quality of decisions that 
would be made if this were to be done again from scratch.

_**This project is no longer maintained**_

## Features

- [x] Color to grayscale conversion for BMP images 
- [x] Applies filters (in the form of matrix kernels) to images
- [x] Scan images to create database of glyphs (handwritten black/white 
  image strokes)
- [x] Detects hand-written numeric digits based, using database files 
  (`data/database`) for feature analysis

## How to build

This project is set up using the CMake build system.
Building is as simple as making a directory to build, configuring the project,
and building it:

```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

The binary will be found in the root of the build directory.

## <a name="license"></a>License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The contents of this library are licensed under the 
[MIT License](http://opensource.org/licenses/MIT):

Copyright &copy; 2015 Matthew Rodusek

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.