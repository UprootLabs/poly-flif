# Poly FLIF

An attempt to create a poly fill for FLIF in browsers, using emscripten.

Status: Alpha quality. Performance hasn't been optimised yet.

[Demo](https://uprootlabs.github.io/poly-flif/)

# Roadmap

* Support animations. [#2](https://github.com/UprootLabs/poly-flif/issues/2)
* Fix memory leaks. [#3](https://github.com/UprootLabs/poly-flif/issues/3)
* Design a proper API [#6](https://github.com/UprootLabs/poly-flif/issues/6)

# What is FLIF

FLIF is a lossless image format based on MANIAC compression. MANIAC (Meta-Adaptive Near-zero Integer Arithmetic Coding) is a variant of CABAC (context-adaptive binary arithmetic coding), where the contexts are nodes of decision trees which are dynamically learned at encode time.

FLIF outperforms PNG, FFV1, lossless WebP, lossless BPG and lossless JPEG2000 in terms of compression ratio.

Moreover, FLIF supports a form of progressive interlacing (essentially a generalization/improvement of PNG's Adam7) which means that any prefix (e.g. partial download) of a compressed file can be used as a reasonable lossy encoding of the entire image.

For more information on FLIF, visit http://flif.info

