var ctx, img;
var width = 320, height = 200;
function mydraw() {
	if (!ctx) {
		var canvas = document.querySelector("canvas")
		canvas.width = width;
		canvas.height = height;
		ctx = canvas.getContext("2d");
		//data = new Uint8ClampedArray(Module.HEAP, pointer, width * height * 4);
		var pointer = Module._get_screen();
		//data = Module.HEAPU8.subarray(pointer, width * height * 4);
		var data = new Uint8ClampedArray(Module.HEAPU8.buffer, pointer, width * height * 4);
		img = new ImageData(data, width, height);
	}
	ctx.putImageData(img, 0, 0);
}

function fixAudio() {
	  if (!audioContext)
	    nextBuf();
	  else if (audioContext.state == 'suspended')
	    audioContext.resume().then(() => {
	      console.log('Playback resumed successfully');
	      nextBuf();
	    });
}

function libinit() {
	document.addEventListener('keydown', function(ev) {
		if (!audioContext)
			fixAudio();
		Module._on_key(ev.keyCode, 1);
		//console.log('down ', ev.keyCode);
		ev.preventDefault();
		return false;
	});
	document.addEventListener('keyup', function(ev) {
		Module._on_key(ev.keyCode, 0);
		//console.log('up ', ev.keyCode);
		ev.preventDefault();
		return false;
	});

	document.body.addEventListener('click', function() {
		document.body.requestPointerLock && document.body.requestPointerLock();
		fixAudio();
	});
	document.body.addEventListener('mousedown', function(ev) {
		Module._on_mousebutton(ev.button, 1);
	});
	document.body.addEventListener('mouseup', function(ev) {
		Module._on_mousebutton(ev.button, 0);
	});
	document.body.addEventListener('mousemove', function(ev) {
		Module._on_mousemove(ev.movementX, ev.movementY);
	});
}


var musicVolume = 128 / 2, musicGain;
var audioContext, rate, samps, time;
var audioStarting;
function audioInit() {
  if (audioContext)
    audioContext.close();
  musicGain = null;
  audioContext = new AudioContext();
  rate = 48000; //audioContext.sampleRate;
  samps = 48000 >> 3;
  time = 0;
  freeSources = [];
  audioStarting = true;
  if (audioContext.state == 'suspended') {
    fixAudio();
    return;
  }
  //playerweb._playerweb_init(rate);
  console.log('playerweb init done');
  musicVolumeSet(musicVolume);
}

function fillBuf(myBuffer) {
  let ch0 = myBuffer.getChannelData(0);
  let ch1 = myBuffer.getChannelData(1);
  let ofs = Module._playerweb_gen(samps * 2) >> 1;
  if (!ofs)
    throw new Error("playerweb: " + Module._playerweb_get_last_rc());
  //ofs >>= 1;
  let sndBuf = Module.HEAP16; //playerwebMem16s;
  //let sndBuf = playerwebMem16s.subarray(ofs, ofs + samps * 2);
  //console.log(sndBuf);
  for (var i = 0, p = ofs; i < samps; i++, p += 2) {
    ch0[i] = sndBuf[p] / 32768;
    ch1[i] = sndBuf[p + 1] / 32768;
  }
  //onsole.log(ofs);
  /*
  for (let sampleNumber = 0 ; sampleNumber < samps ; sampleNumber++) {
    ch0[sampleNumber] = generateSample(sampleNumber);
    ch1[sampleNumber] = generateSample(sampleNumber);
  }
  */
}
    
let freeBufs = [];
let bufCount = 0;
function nextBuf() {
  if (!audioContext)
    audioInit();
  if (!musicGain) {
    audioStarting && musicVolumeSet(musicVolume);
    return;
  }
  let myBuffer = freeBufs.length ? freeBufs.pop() : audioContext.createBuffer(2, samps, rate);
  fillBuf(myBuffer);
  let src = audioContext.createBufferSource();
  src.buffer = myBuffer;
  src.connect(musicGain);
  let startTime = time, lenTime = samps / rate;
  src.onended = function() { bufCount--; freeBufs.push(this.buffer); nextBuf(); if (audioContext.currentTime >= startTime + lenTime * 1.5) nextBuf(); }
  src.start(startTime);
  time += lenTime;
  bufCount++;
}

function playStart() {
	!window.audioContenxt && audioInit();
	if (!window.audioContext)
		return;
	if (musicGain) {
		musicGain.disconnect();
		musicGain = null;
	}
	console.log('playStart vol=', musicVolume);
	musicVolumeSet(musicVolume);
	//nextBuf();
	//nextBuf();
	//nextBuf();
	//nextBuf();
	//nextBuf();
}

var samples = new Array(256);
function getSampleBuf(num) {
	let buf = samples[num];
	if (buf)
		return buf;
	let len = Module._get_sample_size(num)
	buf = audioContext.createBuffer(1, len, 11025);
	let p = Module._get_sample_data(num);
	let heap = Module.HEAPU8;
	let data = buf.getChannelData(0);
	for (let i = 0; i < len; i++)
		data[i] = (heap[p + i] - 128) / 128;
	samples[num] = buf;
	return buf;
}
var freeSources = [];
var freePans = [];
var repeatSamples = [];
var repeatFree = [];
function playSample(num, pan, vol, repeat) {
	var srcNodes = freeSources.pop();
	var srcNode, gainNode, panNode;
	if (srcNodes) {
		[srcNode, gainNode, panNode] = srcNodes;
	} else {
		srcNode = audioContext.createBufferSource();
		gainNode = audioContext.createGain();
		panNode = audioContext.createStereoPanner();
		//srcNodes = [srcNode, gainNode, panNode];
		srcNode.connect(gainNode);
		gainNode.connect(panNode);
		panNode.connect(audioContext.destination);
		//srcNode.onended = function() { freeSources.push(srcNodes); }
	}
	gainNode.gain.value = vol / (65536 * 2);
	panNode.pan.value = (pan - 32768) / 32768;
	srcNode.buffer = getSampleBuf(num);
	srcNode.loop = repeat;
	srcNode.start();
	if (!repeat)
		return -1;
	var idx = repeatFree.length ? repeatFree.pop() : repeatSamples.length;
	repeatSamples[idx] = [gainNode, panNode, srcNode];
	return idx;
}

function updateSample(idx, pan, vol) {
	var info = repeatSamples[idx];
	var gainNode = info[0], panNode = info[1], srcNode = info[2];
	gainNode.gain.value = vol / (65536 * 2);
	panNode.pan.value = (pan - 32768) / 32768;
}

function stopSample(idx) {
	var info = repeatSamples[idx];
	var gainNode = info[0], panNode = info[1], srcNode = info[2];
	srcNode.disconnect();
	gainNode.disconnect();
	panNode.disconnect();
	repeatSamples[idx] = null;
	repeatFree.push(idx);
}

function videoSetRes(w, h) {
	width = w;
	height = h;
	ctx = null;
}

function musicVolumeSet(n) {
	console.log('musicVolumeSet ', n);
	musicVolume = n;
	var vol = n / 128;
	if (!n) {
		musicGain && musicGain.disconnect();
		musicGain = null;
	} else {
		if (!musicGain) {
			if (!audioContext || audioContext.state == 'suspended')
				return;
			audioStarting = false;
			musicGain = audioContext.createGain();
			musicGain.gain.value = vol;
			musicGain.connect(audioContext.destination);
			time = audioContext.currentTime;
			nextBuf();
			nextBuf();
			console.log('started');
		} else
			musicGain.gain.setTargetAtTime(vol, audioContext.currentTime, 0.1);
	}
}

function playAd(wakeUp) {
	var div = document.createElement('div');
	var allow2;
	function done1() { allow2 = true; }
	function done2() { if (allow2) done(); }
	function done() {
		if (!wakeUp)
			return;
		window.removeEventListener('keydown', done1, true);
		window.removeEventListener('keyup', done2, true);
		div.remove();
		console.log('waking up');
		wakeUp();
		wakeUp = null;
	}
	console.log('playAd');
	div.innerHTML = '<div tabindex="0" style="background:#000;position:absolute;top:0;left:0;right:0;bottom:0;display:flex;justify-content:center"><video autoplay style="flex:1;object-fit:contain"><source src="descent.mp4"></video></div>';
	div.firstChild.onkeypress = div.firstChild.onclick = div.querySelector('video').onended = done;
	document.body.appendChild(div);
	window.addEventListener('keydown', done1, true);
	window.addEventListener('keyup', done2, true);
}
function playStop() {
	if (musicGain) {
		musicGain.disconnect();
		musicGain = null;
	}
}

function saveFile(fn, a) {
	var rawLength = a.length;
	var b = new Array(rawLength);
	for (var i = 0; i < rawLength; i++)
		b[i] = String.fromCharCode(a[i]);
	localStorage.setItem(fn, window.btoa(b.join("")));
}

function loadFile(fn) {
	var val = localStorage.getItem(fn);
	if (!val)
		return;
	var raw = window.atob(val);
	var rawLength = raw.length;
	var array = new Uint8Array(new ArrayBuffer(rawLength));
	for (var i = 0; i < rawLength; i++)
		array[i] = raw.charCodeAt(i);
	return array;
}

function listFiles() {
	return Object.keys(localStorage);
}

function loadFilenames(buf, fnsize, fnmax, filespec, stripext) {
	var mem = Module.HEAPU8, p = filespec, a = [], c;
	while ((c = mem[p++]))
		a.push(String.fromCharCode(c));
	var filespecstr = a.join("");
	console.log('loadFilenames ' + filespecstr);
	var re = new RegExp(filespecstr.substr(1).replace(new RegExp("\\W", "g"), "\\$&") + "$", "i");
	var num = 0;
	p = buf;
	for (let fn of listFiles()) {
		console.log(fn + ' ' + re.test(fn));
		if (re.test(fn) && fn.length < fnsize) {
			if (stripext)
				fn = fn.substr(0, fn.lastIndexOf('.'));
			for (var l = fn.length, i = 0; i < l; i++)
				mem[p + i] = fn.charCodeAt(i);
			mem[p + l] = 0;
			p += fnsize;
			num++;
		}
	}
	console.log(num);
	return num;
}
