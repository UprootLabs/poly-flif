  function CanvasManager(canvas) {
    this.setRequestedDimensions = function(rw, rh) {
      this.rw = rw;
      this.rh = rh;
    }

    this.prepareCanvas = function(aw, ah) {
      var width = Math.max(this.rw, aw);
      canvas.width = width;
      var height = Math.max(this.rh, ah);
      canvas.height = height;

      this.ctx = canvas.getContext("2d");
      this.imgData = this.ctx.getImageData(0,0,width,1);
      this.imgDataData = this.imgData.data;
    }

    this.showRow = function(row, data) {
      this.imgDataData.set(data, 0);
      this.ctx.putImageData(this.imgData, 0, row);
    }

    this.finish = function() {
      this.ctx = undefined;
      this.imgData = undefined;
      this.imgDataData = undefined;
    }

    this.initAnimImage = function(n, aw, ah) {
      var width = Math.max(this.rw, aw);
      var height = Math.max(this.rh, ah);

      if (n == 0) {
        canvas.width = width;
        canvas.height = height;

        this.nAnims = [];
        this.nAnims.width = width;
        this.nAnims.height = height;
      }
      this.nAnims[n] = new ImageData(width, height);
    }

    this.animPutRow = function(n, row, heap) {
      this.nAnims[n].data.set(heap, row * this.nAnims.width * 4);
    }

    this.animStartPlay = function() {
      var anims = this.nAnims;
      anims.play = true;
      var ctx = this.ctx;
      var currFrame = 0;
      var lastStep = 0;
      function step(now) {
        if (anims.play) {
          if ((now - lastStep) > 33) {
            ctx.putImageData(anims[currFrame], 0, 0);
            currFrame++;
            if (currFrame >= anims.length) {
              currFrame = 0;
            }
            lastStep = now;
          }
          window.requestAnimationFrame(step);
        } else {
          console.log("Stopping playback");
        }
      }
      window.requestAnimationFrame(step);
    }
  }

  window["PolyFlif"] = Module["PolyFlif"].extend("PolyFlif", {
    "__construct": function(options) {

      this.options = options;
      this.buf = options["buf"];
      this["__parent"]["__construct"].call(this);
      this.cm = new CanvasManager(this.options["canvas"]);
    },

    "beginCount": function(truncation, rw, rh) {
      this.cm.setRequestedDimensions(rw, rh);
      this["startCount"](truncation, rw, rh);
    },

    "beginPercent": function(truncationPercent, rw, rh) {
      this.cm.setRequestedDimensions(rw, rh);
      this["startPercent"](truncationPercent, rw, rh);
    },

    "stop": function() {
      if (this.nAnims) {
        this.nAnims.play = false;
      }
    },

    // --------------------
    // called from C++ code

    "bufGetSize": function () {
      return this.buf.length;
    },

    "readBuffer": function(from, to, size) {
      var rem = Math.min(this.buf.length - from, size);
      var sub = this.buf.subarray(from, from + rem);
      Module["HEAPU8"].set(sub, to);
    },

    "prepareCanvas": function(aw, ah) {
      this.cm.prepareCanvas(aw, ah);
    },

    "showRow": function(row, dataPtr, width) {
      var data = Module["HEAPU8"].subarray(dataPtr, dataPtr + width*4);
      this.cm.showRow(row, data);
    },

    "finishCanvasDraw": function() {
      this.cm.finish();
      this.buf = undefined;
    },

    "initAnimImage": function (n, aw, ah) {
      this.cm.initAnimImage(n, aw, ah);
    },

    "putRow": function (n, row, dataPtr) {
      var data = Module["HEAPU8"].subarray(dataPtr, dataPtr + (this.cm.nAnims.width * 4));
      this.cm.animPutRow(n, row, data);
    },

    "finishAnimTx": function() {
      this.cm.animStartPlay();
    }

  });

})();

document.addEventListener("DOMContentLoaded", function() {
  function load(src, bytes, andThen) {
    var xhr = new XMLHttpRequest();
    xhr.responseType = "arraybuffer";
    xhr.open("GET", src);
    if (bytes) {
      xhr.setRequestHeader('Range', 'bytes=0-' + bytes);
    }
    xhr.onload = function() {
      var content = bytes ? new Uint8Array(this.response, 0, Number.parseInt(bytes)) : new Uint8Array(this.response);
      andThen(content);
    };
    xhr.send();
  }

  function process(elem) {
    var src = elem.getAttribute("data-polyflif-src");
    // const wAttrib = elem.getAttribute("width");
    // const hAttrib = elem.getAttribute("height");
    var bytes = elem.getAttribute("data-polyflif-bytes");

    var cs = window.getComputedStyle(elem);
    var wAttrib = cs.width;
    var hAttrib = cs.height;
    var scaleRequested = elem.hasAttribute("data-polyflif-scale");
    var rw = scaleRequested && wAttrib.length > 0 ? Number.parseInt(wAttrib) : 0;
    var rh  = scaleRequested && hAttrib.length > 0 ? Number.parseInt(hAttrib) : 0;
    load(src, bytes, function(content) {
      setTimeout(function() {
        var pf = new PolyFlif({"buf": content, "canvas": elem});
        pf["beginCount"](-1, rw, rh);
      }, 1);
    });
  }
  

  function processBg(elem) {
    var src = elem.getAttribute("data-polyflif-bg-src");
    var bytes = elem.getAttribute("data-polyflif-bytes");
    var cElem = document.createElement("canvas");
    load(src, bytes, function(content) {
      setTimeout(function() {
        var pf = new PolyFlif({"buf": content, "canvas": cElem});
        pf["beginCount"](-1, 0, 0);
        cElem["toBlob"](function(blob) {
           elem.style.backgroundImage = "url('" + URL.createObjectURL(blob) + "')";
        }, 'image/png');
      }, 1);
    });
  }

  // Poly-fill for toBlob, inspired by MDN docs
  if (!HTMLCanvasElement.prototype.toBlob) {
    Object.defineProperty(HTMLCanvasElement.prototype, 'toBlob', {
      value: function (callback, type, quality) {

        var binStr = atob(this.toDataURL(type, quality).split(',')[1] ),
            len = binStr.length,
            arr = new Uint8Array(len);

        for (var i=0; i<len; i++ ) {
          arr[i] = binStr.charCodeAt(i);
        }

        callback( new Blob( [arr], {type: type || 'image/png'} ) );
      }
    });
  }

  var canvasElements = document.querySelectorAll("canvas[data-polyflif-src]");
  for (var i = 0; i < canvasElements.length; i++) {
    process(canvasElements[i]);
  }

  var bgElements = document.querySelectorAll("*[data-polyflif-bg-src]");
  for (var i = 0; i < bgElements.length; i++) {
    processBg(bgElements[i]);
  }
});
