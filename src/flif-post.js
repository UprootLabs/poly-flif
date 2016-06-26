
  window["PolyFlif"] = Module["PolyFlif"].extend("PolyFlif", {
    "__construct": function(options) {

      this.options = options;
      this.buf = options["buf"];
      this["__parent"]["__construct"].call(this);
    },

    "begin": function(truncation, rw, rh) {
      this.rw = rw;
      this.rh = rh;
      this.start(truncation, rw, rh);
    },

    "stop": function() {
      if (this.nAnims) {
        this.nAnims.play = false;
      }
    },

    "bufGetSize": function () {
      return this.buf.length;
    },
    "bufGetC": function (idx) {
      return this.buf[idx];
    },

    "prepareCanvas": function(aw, ah) {
      var canvas = this.options["canvas"];

      var width = Math.max(this.rw, aw);
      canvas.width = width;
      var height = Math.max(this.rh, ah);
      canvas.height = height;

      this.ctx = canvas.getContext("2d");
      this.imgData = this.ctx.getImageData(0,0,width,1);
      this.imgDataData = this.imgData.data;
    },

    "putPixel": function(indx, r, g, b, a) {
      var idd = this.imgDataData;
      idd[indx    ] = r;
      idd[indx + 1] = g;
      idd[indx + 2] = b;
      idd[indx + 3] = a;
    },

    "showRow": function(i) {
      this.ctx.putImageData(this.imgData, 0, i);
    },

    "finishCanvasDraw": function() {
      this.ctx = undefined;
      this.imgData = undefined;
      this.imgDataData = undefined;
      this.buf = undefined;
    },

    "initAnimImage": function (n, aw, ah) {
      var width = Math.max(this.rw, aw);
      var height = Math.max(this.rh, ah);

      if (n == 0) {
        var canvas = this.options["canvas"];
        canvas.width = width;
        canvas.height = height;

        this.nAnims = [];
        this.nAnims.width = width;
        this.nAnims.height = height;
      }
      this.nAnims[n] = new ImageData(width, height);
    },

    "putRow": function (n, i, data) {
      var heap = Module["HEAPU8"].subarray(data, data + (this.nAnims.width * 4));
      this.nAnims[n].data.set(heap, i * this.nAnims.width * 4);
    },

    "finishAnimTx": function() {
      var anims = this.nAnims;
      anims.play = true;
      var canvas = this.options["canvas"];
      var ctx = canvas.getContext('2d');
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
      var pf = new PolyFlif({"buf": content, "canvas": elem});
      setTimeout(function() { pf["begin"](0, rw, rh) }, 0);
    });
  }
  

  function processBg(elem) {
    var src = elem.getAttribute("data-polyflif-bg-src");
    var bytes = elem.getAttribute("data-polyflif-bytes");
    var cElem = document.createElement("canvas");
    load(src, bytes, function(content) {
      var pf = new PolyFlif({"buf": content, "canvas": cElem});
      setTimeout(function() {
        pf["begin"](0, 0, 0);
        cElem["toBlob"](function(blob) {
           elem.style.backgroundImage = "url('" + URL.createObjectURL(blob) + "')";
        }, 'image/png');
      }, 0);
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
