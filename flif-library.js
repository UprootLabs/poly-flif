mergeInto(LibraryManager.library, {
  jsBufGetSize: function (id) {
    var buf = jsBuffers[id];
    return buf.length;
  },
  jsBufGetC: function (id, idx) {
    var buf = jsBuffers[id];
    return buf[idx];
  },

  initCanvasDraw: function() {
    var canvas = document.getElementById('canvas');
    window["ctx"] = canvas.getContext('2d');
    window["imgData"] = ctx.getImageData(0,0,canvas.width,1);
    window["imgDataData"] = imgData.data;
  },

  putPixel: function(indx, r, g, b, a) {
    var idd = window["imgDataData"];
    idd[indx    ] = r;
    idd[indx + 1] = g;
    idd[indx + 2] = b;
    idd[indx + 3] = a;
  },

  showRow: function(i) {
    window["ctx"].putImageData(window["imgData"], 0, i);
  },

  finishCanvasDraw: function() {
    window["ctx"] = undefined;
    window["imgData"] = undefined;
    window["imgDataData"] = undefined;
  },

  initAnimImage: function(n, width, height) {
    if (n == 0) {
      var nAnims = [];
      nAnims.width = width;
      nAnims.height = height;
      window["flifAnims"] = nAnims;
      var canvas = document.getElementById('canvas');
      canvas.width = width;
      canvas.height = height;
    }
    var anims = window["flifAnims"];
    anims[n] = new ImageData(width, height);
  },
  putRow: function(n, i, data) {
    var anims = window["flifAnims"];
    var heap = Module["HEAPU8"].subarray(data, data + (anims.width * 4));
    anims[n].data.set(heap, i * anims.width * 4);
  },
  finishAnimTx: function() {
    var anims = window["flifAnims"];
    anims.play = true;
    var canvas = document.getElementById('canvas');
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
