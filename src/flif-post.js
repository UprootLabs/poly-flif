  var isAWorker = undefined !== typeof WorkerGlobalScope;
  var currScriptSrc = isAWorker ? null : document.currentScript.src;

  function DirectCanvasManager(canvas) {
    var animPlay = true;
    var animStarted = false;
    var nAnims = [];

    this["setRequestedDimensions"] = function(rw, rh) {
      this.rw = rw;
      this.rh = rh;
    }

    this["prepareCanvas"] = function(aw, ah) {
      var width = Math.max(this.rw, aw);
      if (canvas.width != width) {
        canvas.width = width;
      }
      var height = Math.max(this.rh, ah);
      if (canvas.height != height) {
        canvas.height = height;
      }

      if (!this.ctx) {
        this.ctx = canvas.getContext("2d");
      }
      this.imgData = this.ctx.getImageData(0,0,width,1);
      this.imgDataData = this.imgData.data;
    }

    this["showRow"] = function(row, data) {
      this.imgDataData.set(data, 0);
      this.ctx.putImageData(this.imgData, 0, row);
    }

    this["finish"] = function() {
      this.ctx = undefined;
      this.imgData = undefined;
      this.imgDataData = undefined;
    }

    this["initAnimImage"] = function(n, aw, ah) {
      var width = Math.max(this.rw, aw);
      var height = Math.max(this.rh, ah);

      if (n == 0) {
        if (canvas.width != width) {
          canvas.width = width;
        }
        if (canvas.height != height) {
          canvas.height = height;
        }

        this.animWidth = aw;
        // this.nAnims.height = height;
        if (!this.ctx) {
          this.ctx = canvas.getContext("2d");
        }
      }
      if (!nAnims[n]) {
        nAnims[n] = new ImageData(width, height);
      }
    }

    this["animPutRow"] = function(n, row, heap) {
      nAnims[n].data.set(heap, row * this.animWidth * 4);
    }

    this["animStartPlay"] = function() {
      if (animStarted) {
        return;
      }
      animStarted = true;

      var ctx = this.ctx;
      var currFrame = 0;
      var lastStep = 0;
      function step(now) {
        if (animPlay) {
          if ((now - lastStep) > 33) {
            ctx.putImageData(nAnims[currFrame], 0, 0);
            currFrame++;
            if (currFrame >= nAnims.length) {
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

    this["stop"] = function() {
      animPlay = false;
    }
  }

  function WorkerCanvasManager() {
    function replaceTypeArrays(args) {
      var arr = new Array(args.length);
      for (var i = 0; i < args.length; i++) {
        var e = args[i];
        if (e instanceof Uint8Array) {
          /* we wrap e in a new instance to avoid a performance bug in
           * chromium (the whole underlying array gets copied and doesn't get
           * garbage collected) */
          arr[i] = new Uint8Array(e);
        } else {
          arr[i] = e;
        }
      }
      return arr;
    }

    function send(name, args) {
      var arr = replaceTypeArrays(args);
      self.postMessage({type: name, args: arr});
    }

    this.setRequestedDimensions = function() { send("setRequestedDimensions", arguments); }
    this.prepareCanvas = function() { send("prepareCanvas", arguments); }
    this.showRow = function() { send("showRow", arguments); }
    this.finish = function() { send("finish", arguments); }

    this.initAnimImage = function(n, aw, ah) {
      if (n == 0) {
        this.animWidth = aw;
      }
      send("initAnimImage", arguments);
    }

    this.animPutRow = function() { send("animPutRow", arguments); }

    this.animStartPlay = function() { send("animStartPlay", arguments); }
  }


  var pfBase = {
    "beginCount": function(truncation, rw, rh) {
      this.cm["setRequestedDimensions"](rw, rh);
      this["startCount"](truncation, rw, rh);
    },

    "beginPercent": function(truncationPercent, rw, rh) {
      this.cm["setRequestedDimensions"](rw, rh);
      this["startPercent"](truncationPercent, rw, rh);
    },

    "stop": function() {
      this.cm["stop"]();
    },

    "loaded": function() {
      if (typeof this.options.onload == "function") {
        this.options.onload();
      }
    }

  }

  function pfProxyModule(options) {
    this.options = options;
    this.buf = options["buf"];
    var cm = new DirectCanvasManager(options["canvas"]);
    this.cm = cm;
    var module = this;
    this.worker = new Worker(currScriptSrc);
    this.worker.onmessage = function(m) {
      var data = m.data;
      if (data.type == "onload") {
        module.loaded();
        this.worker = undefined;
      } else {
        cm[data.type].apply(cm, data.args);
      }
    };
    this.worker.postMessage({type: "construct", "buf": new Uint8Array(options["buf"])});

    this["startCount"] = function() {
      var args = Array["from"](arguments);
      this.worker.postMessage({type: "startCount", args: args});
    }

    this["startPercent"] = function() {
      var args = Array["from"](arguments);
      this.worker.postMessage({type: "startPercent", args: args});
    }

    this["prepareCanvas"] = function() {
      var args = Array["from"](arguments);
      this.worker.postMessage({type: "prepareCanvas", args: args});
    }

    Object.assign(this, pfBase);
  }

  // Object.assign(pfProxyModule, pfBase);

  var pfModule = Module["PolyFlif"].extend("PolyFlif", Object.assign({
    "__construct": function(options) {
      this.options = options;
      this.buf = options["buf"];
      this["__parent"]["__construct"].call(this);
      this.cm = options["canvasMgr"] ? options["canvasMgr"] : new DirectCanvasManager(options["canvas"]);
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
    },

    "initAnimImage": function (n, aw, ah) {
      this.cm.initAnimImage(n, aw, ah);
    },

    "putRow": function (n, row, dataPtr) {
      var data = Module["HEAPU8"].subarray(dataPtr, dataPtr + (this.cm.animWidth * 4));
      this.cm.animPutRow(n, row, data);
    },

    "finishAnimTx": function() {
      this.cm.animStartPlay();
    },

    "finishLoading": function() {
      if (this.buf) {
        this.buf = undefined;
      }
      this.loaded(this.options);
    }
  }, pfBase));

  if (isAWorker) {
    var pf = null;
    function loadProxy() {
      self.postMessage({type:"onload"});
      self.close();
    }
    self.onmessage = function(m) {
      var data = m.data;
      if (data.type == "construct") {
        pf = new pfModule({"buf": data["buf"], "canvasMgr": new WorkerCanvasManager(), "onload": loadProxy});
      } else {
        pf[data.type].apply(pf, data.args);
      }
    }

  } else {
    // window["PolyFlif"] = pfModule;
    window["PolyFlif"] = pfProxyModule;

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
          function loaded() {
            cElem["toBlob"](function(blob) {
               elem.style.backgroundImage = "url('" + URL.createObjectURL(blob) + "')";
            }, 'image/png');
          }
          setTimeout(function() {
            var pf = new PolyFlif({"buf": content, "canvas": cElem, onload: loaded});
            pf["beginCount"](-1, 0, 0);
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
  }

})();

