# Poly FLIF
A browser poly fill for [FLIF](http://flif.info) image format. Since this image
format is not supported natively by browsers yet, this project makes it possible to
decode FLIF images with Javascript and to then render them on HTML5 Canvas.

Try the [interactive demo](https://uprootlabs.github.io/poly-flif/) if you are
new to FLIF. The demo allows you to truncate the bitstream and to compare FLIF
with other image formats.

For an example of how to use the [PolyFLIF API](https://uprootlabs.github.io/poly-flif/api.html) to render FLIF images,
see this [page](https://uprootlabs.github.io/poly-flif/polyflif-sample.html).

## Status

Mostly stable but needs more testing.

Currently, has been tested manually on latest versions of Chromium and Firefox. No regression tests / CI setup yet.

## JS payload size

As of version 0.3, the gzipped and minified payload size is 77KB.

## Roadmap

* Async processing (so as to not block the browser event loop during decode)
* Animation speed and loop control. [#2](https://github.com/UprootLabs/poly-flif/issues/2)
* Code size reduction and performance optimisation.

## License
GNU Lesser General Public License (LGPLv3+) in accordance with the upstream license.

## Contributing

### How to Build

* Install [emscripten](https://github.com/kripken/emscripten/)
* Install Java (for the closure compiler)
* `cd src/`
* `emmake make`

### CLA
When you send us a pull request, you acknowledge that
* You either hold the copyright to your contribution or you have the permission from the copyright holder to submit the contribution to this project.
* You grant the right to Uproot Labs to use your contribution in the project without any strings attached.

## About FLIF

FLIF is a lossless image format based on MANIAC compression. MANIAC (Meta-Adaptive Near-zero Integer Arithmetic Coding) is a variant of CABAC (context-adaptive binary arithmetic coding), where the contexts are nodes of decision trees which are dynamically learned at encode time.

[![Join the chat at https://gitter.im/FLIF-hub/FLIF](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/FLIF-hub/FLIF)
