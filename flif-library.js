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
  }
});
