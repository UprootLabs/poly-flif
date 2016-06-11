# Poly FLIF
A browser poly fill for [FLIF](http://flif.info).

[Interactive Demo](https://uprootlabs.github.io/poly-flif/) of features.
[Sample](https://uprootlabs.github.io/poly-flif/) page with FLIF images on it.

## Status

Beta. Not tested on all browsers. Performance and size is still being optimised. 

## JS payload size

The Payload size currently varies between 50 to 80KB (gzipped) while we develop the library.

## Roadmap

* Code size reduction and performance optimisation.
* Async processing (so as to not block the browser event loop during decode)
* Animation speed and loop control. [#2](https://github.com/UprootLabs/poly-flif/issues/2)

## License
GNU Lesser General Public License (LGPLv3+) in accordance with the upstream license.

## About FLIF

FLIF is a lossless image format based on MANIAC compression. MANIAC (Meta-Adaptive Near-zero Integer Arithmetic Coding) is a variant of CABAC (context-adaptive binary arithmetic coding), where the contexts are nodes of decision trees which are dynamically learned at encode time.

[![Join the chat at https://gitter.im/FLIF-hub/FLIF](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/FLIF-hub/FLIF)
