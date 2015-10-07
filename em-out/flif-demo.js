/*
    GNU LESSER GENERAL PUBLIC LICENSE
    Copyright (C) 2015 Uproot Labs India Pvt Ltd

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* Viewer
   TODO: Wrap as separate class { */
var splitPosition = 0.5;
var viewWidth = 0;
var viewHeight = 0;
var viewLocked = false;

function setElementDimension(query, w, h) {
  var e = document.querySelector(query);
  e.style.width = ""+w+"px";
  e.style.height = ""+h+"px";
}

function updateViewer(width, height) {
  setElementDimension("#viewer", width, height + 100);
  setElementDimension(".viewer-parent", width, height + 100);

  // This shouldn't be required, but the right-wrapper expands in height for some reason
  setElementDimension("#viewer-right .wrapper", width, height);

  viewWidth = width;
  viewHeight = height;

  updateSplit();
  setElementTop("#left-info", viewHeight);
  setElementTop("#right-info", viewHeight);
}

function updateSplit() {
  var leftWidth = Math.floor(viewWidth * splitPosition);
  var rightWidth = viewWidth - leftWidth;
  setElementDimension("#viewer-left", leftWidth, viewHeight + 100);
  setElementDimension("#viewer-right", rightWidth, viewHeight + 100);
}

function setElementTop(query, t) {
  var e = document.querySelector(query);
  e.style.top = ""+t+"px";
}

function moveSplit(event) {
  if (!viewLocked) {
  var view = document.getElementById("viewer");
  var offset = view.getBoundingClientRect();
  var x = Math.max(0, event.clientX - offset.left);
  var y = Math.max(0, event.clientY - offset.top);

  splitPosition = x / viewWidth;
  }

  updateSplit();

  setElementTop("#left-info", viewHeight);
  setElementTop("#right-info", viewHeight);
  return false;
}

function lockRightView() {
  splitPosition = 1;
  viewLocked = true;
  document.getElementById("viewer").classList.remove("unlocked");
}

function unLockRightView() {
  splitPosition = 0.5;
  viewLocked = false;
  document.getElementById("viewer").classList.add("unlocked");
}

/* } End of Viewer */

var imgInfoPatch = {
  "spinfox": {imgSize: {width:  148, height: 148 }},
  "tea": {imgSize: {width: 496, height: 320}},
  "train": {imgSize: {width: 494, height: 371}}
}

function init() {
  window.viewer.parentElement.hidden = true;
  for (ii in imgInfos) {
   var imgInfo = imgInfos[ii];
   if (imgInfo.name) {
     if (!imgInfo.imgSize) {
       imgInfo.imgSize = imgInfoPatch[imgInfo.name].imgSize;
       imgInfo.category = "anim";
     } else {
       imgInfo.category = "still";
     }
   }
  }
  imgInfos.sort(function (a, b) {
    if (!b.imgSize) {
      return 1;
    } else if (!a.imgSize) {
      return -1;
    } else {
      return a.imgSize.height - b.imgSize.height;
    }
  });
  var stillsGroupElem = document.querySelector('#imageSelect .stillsGroup');
  var animsGroupElem = document.querySelector('#imageSelect .animationsGroup');
  function addOption(idx, title, category) {
    var optElem = document.createElement("option");
    optElem.setAttribute("value", idx);
    optElem.textContent = title;
    var elem = category == "anim" ? animsGroupElem : stillsGroupElem;
    elem.appendChild(optElem);
  }
  for (var i = 1; i < imgInfos.length; i++) {
    var info = imgInfos[i];
    addOption(i, info.name + " " + info.imgSize.width + " x " + info.imgSize.height, info.category);
  }
  document.getElementById("viewer").addEventListener("mousemove", moveSplit, false);
  bgColorChanged();

  setLeftInfo("FLIF", 0, 0);
  setRightInfo("PNG", 0, 0);
}

// TODO: Call on window load, or some such
init();

function resetView() {
  var anims = window["flifAnims"];
  if (anims) {
    anims.play = false;
  }

  window.viewer.parentElement.hidden = false;
  document.getElementById('pngImg').src = ""
  var canvas = document.getElementById('canvas');
  var ctx = canvas.getContext("2d");
  ctx.clearRect(0, 0, canvas.width, canvas.height);
}

function showAbout() {
  document.getElementById('aboutModal').classList.add('visible');
}

function hideAbout() {
  document.getElementById('aboutModal').classList.remove('visible');
}

function truncateChanged() {
  decode();
}

function imageChanged() {
  decode();
}

function bgColorChanged() {
  var bgInput = document.getElementById('bgColor');
  console.log(bgInput.value);
  document.querySelector("#viewer-left .wrapper").style.background = bgInput.value;
  document.querySelector("#viewer-right .wrapper").style.background = bgInput.value;
}

function fileExists(path) {
  try {
    var stat = FS.stat(path)
    return stat != undefined;
  } catch(e) {
    return false;
  }
}

function getURLAsBytes(url, andThen) {
    var xhr = new XMLHttpRequest();
    xhr.responseType = "arraybuffer";
    xhr.open("GET", url);
    xhr.onload = function() {
      var content = new Uint8Array(this.response);
      andThen(content);
    };
    xhr.send();
}

function loadFile(path, url, andThen) {
  if (fileExists(path)) {
    andThen();
  } else {
    Module.setStatus("Loading...");
    getURLAsBytes(url, function(content) {
      FS.writeFile(path, content, {encoding:"binary"})
      andThen();
    });
  }
}

function bufferExists(id) {
  return jsBuffers.hasOwnProperty(id);
}

function addBuffer(id, content) {
  jsBuffers[id] = content;
}

// Separate array than jsBuffers, so that look-ups are fast.
var jsBufferPaths = [];
function reserveBuffer(path) {
  for (var i = 0; i < jsBufferPaths.length; i ++) {
    if (jsBufferPaths[i] == path) {
      return i;
    }
  }
  jsBufferPaths.push(path);
  return jsBufferPaths.length - 1;
}

function loadBytes(id, url, andThen) {
  if (bufferExists(id)) {
    andThen();
  } else {
    Module.setStatus("Loading...");
    getURLAsBytes(url, function(content) {
      addBuffer(id, content)
      andThen();
    });
  }
}

function ensureDir(dirPath) {
/*
  try {
    FS.mkdir(dirPath);
  } catch (e) {
    // Ignored
  }
*/
}

function setRightInfo(title, truncSize, fullSize) {
  var titleStr = '<p>'+title+'&nbsp;»</p>';
  setInfo('#right-info', titleStr, truncSize, fullSize);
}

function setLeftInfo(title, truncSize, fullSize) {
  var titleStr = '<p>«&nbsp;'+title+'</p>';
  setInfo('#left-info', titleStr, truncSize, fullSize);
}

function humanize(length) {
  return (length > (1024 * 1024)) ?
           ''+(Math.round((length / (1024*1024))*10)/10)+'MB' :
           (
             (length > 1024) ?  ''+(Math.round((length / 1024)*10)/10)+'KB' : ''+length+'B'
           );
  
}

function setInfo(query, titleStr, truncSize, fullSize) {
  var sizeStr = truncSize == fullSize ? humanize(fullSize) : humanize(truncSize) + "&nbsp;/&nbsp;" + humanize(fullSize);
  var sizeParaStr = '<p class="size">'+sizeStr+'</p>';
  var htmlStr = titleStr + sizeParaStr;
  document.querySelector(query).innerHTML = htmlStr;
}

function decodePng() {
  var imgIdx = getSelectedIdx();

  if (imgIdx > 0) {
    var truncateInput = document.getElementById('truncate');
    var truncateAmount = 100 - truncateInput.value;

    var imgInfo = imgInfos[imgIdx];
    var path = "assets/"+imgInfo.name+"-i.png";
    getURLAsBytes(path, function (content) {
      var blob = new Blob([content], {type: 'image/png'});
      if (truncateAmount != 100) {
        // var truncatedSize =  Math.floor(content.length * (truncateAmount / 100));
        var flifSize = imgInfo.fileSizes.flif;
        var truncatedSize =  Math.min(content.length, Math.floor(flifSize * (truncateAmount / 100)));
        setRightInfo("PNG", truncatedSize, content.length);
        blob = blob.slice(0, truncatedSize);
      } else {
        setRightInfo("PNG", content.length, content.length);
      }
      var burl = URL.createObjectURL(blob);
      document.getElementById('pngImg').src = burl;
    });
  }
}

function getSelectedIdx() {
  var imageSelect = document.getElementById('imageSelect');
  return parseInt(imageSelect.selectedOptions[0].getAttribute("value"));
}

function decode() {
  resetView();

  ensureDir("/assets");

  var imgIdx = getSelectedIdx();

  if (imgIdx > 0) {
    var imgInfo = imgInfos[imgIdx];
    if (imgInfo.category == "still") {
      decodePng();
      unLockRightView();
    } else {
      lockRightView();
    }
    updateViewer(imgInfo.imgSize.width, imgInfo.imgSize.height);
    var path = "assets/"+imgInfo.name+".flif";
    // loadFile("/" + path, path, function () {
    var bufId = reserveBuffer(path);
    loadBytes(bufId, path, function () {
      decodeSync(bufId, imgIdx);
    });
  }
}

var jsBuffers = [];

function decodeSync(bufId, imgIdx) {
  if (imgIdx > 0) {
    var info = imgInfos[imgIdx];
    var truncateInput = document.getElementById('truncate');
    var truncateAmount = 100 - truncateInput.value;
    if (info.credit) {
      document.getElementById("credits").innerHTML = "<a href='"+info.credit+"'>Image credit</a>"
    } else {
      document.getElementById("credits").innerHTML = "";
    }
    Module.setStatus("Decoding...");

    var flifSize = info.fileSizes.flif;
    var truncatedSize = Math.floor(flifSize * (truncateAmount/100));
    setLeftInfo("FLIF", truncatedSize, flifSize);

    setTimeout(function() {
      Module.ccall("mainy", 'number', ['number', 'number', 'string'], [truncateAmount, bufId, "/assets/"+info.name+".flif"]);
      // Module.setStatus("Decoded with " + truncateAmount + "% stream");
      if (info.category == "still") {
        if (truncateAmount == 100) {
          Module.setStatus("Comparing with original image");
        } else {
          Module.setStatus("Comparing with same sized PNG");
        }
      } else {
        Module.setStatus("Viewing FLIF animation");
      }
    }, 10);
  }
}
