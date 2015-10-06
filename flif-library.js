mergeInto(LibraryManager.library, {
  jsBufGetSize: function (id) {
    var buf = jsBuffers[id];
    return buf.length;
  },
  jsBufGetC: function (id, idx) {
    var buf = jsBuffers[id];
    return buf[idx];
  }
});
