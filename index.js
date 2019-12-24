const epd4in2 = require('./build/Release/epd2in7');
const gd = require('node-gd');

// NODE_SET_METHOD(exports, "width", width);
// NODE_SET_METHOD(exports, "height", height);
// NODE_SET_METHOD(exports, "init", init);
// NODE_SET_METHOD(exports, "initGray", initGray);
// NODE_SET_METHOD(exports, "clear", clear);
// NODE_SET_METHOD(exports, "display", display);
// NODE_SET_METHOD(exports, "displayGray", displayGray);
// NODE_SET_METHOD(exports, "sleep", sleep);

const width = epd4in2.width();
const height = epd4in2.height();

function getImageBuffer(orientation) {
	return new Promise(resolve => {
		let img
		if (orientation === undefined || orientation === 'portrait') {
			img = gd.createSync(height, width);
		} else {
			img = gd.createSync(width, height);
		}

		for (let i = 0; i < 256; i++) {
			img.colorAllocate(i, i, i);
		}

		return resolve(img);
	})
}
exports.getImageBuffer = getImageBuffer;

function displayImageBuffer(img) {
	return new Promise(resolve => {
		let buf = new Buffer.alloc(width * height, 0);
		for (let y = 0; y < height; y++) {
			for (let x = 0; x < width; x++) {
				let color = img.height == height
					? img.getPixel(x, y)
					: img.getPixel(img.width - y, x);
				if (color < 128) { // black
					buf[x + y * width] = 0x00;
				} else { // white
					buf[x + y * width] = 0xff;
				}
			}
		}
		epd4in2.display(
			buf,
			() => {
				resolve();
			}
		);
	})
}
exports.displayImageBuffer = displayImageBuffer;

function displayGrayImageBuffer(img) {
	return new Promise(resolve => {
		let buf = new Buffer.alloc(width * height, 0);
		for (let y = 0; y < height; y++) {
			for (let x = 0; x < width; x++) {
				let color = img.height == height
					? img.getPixel(x, y)
					: img.getPixel(img.width - y, x);
				buf[x + y * width] = color;
			}
		}
		epd4in2.displayGray(
			buf,
			() => {
				resolve();
			}
		);
	})
}
exports.displayGrayImageBuffer = displayGrayImageBuffer;

// function displayPartialImageBuffer(img, x, y, w, h) {
// 	return new Promise(resolve => {
// 		let buf = new Buffer.alloc(width * height, 0);
// 		for (let y = 0; y < height; y++) {
// 			for (let x = 0; x < width; x++) {
// 				let color = img.height == height
// 					? img.getPixel(x, y)
// 					: img.getPixel(img.width - y, x);
// 				if (color < 128) { // black
// 					buf[x + y * width] = 0x00;
// 				} else { // white
// 					buf[x + y * width] = 0xff;
// 				}
// 			}
// 		}
// 		epd4in2.displayPartialFrame(
// 			buf,
// 			x, y, w, h,
// 			() => {
// 				resolve();
// 			}
// 		);
// 	})
// }
// exports.displayPartialImageBuffer = displayPartialImageBuffer; 

exports.init = () => new Promise(resolve => {
	epd4in2.init(() => {
		resolve();
	});
})

exports.initGray = () => new Promise(resolve => {
	epd4in2.initGray(() => {
		resolve();
	});
})

exports.clear = () => new Promise(resolve => {
	epd4in2.clear(() => {
		resolve();
	});
})

exports.sleep = () => new Promise(resolve => {
	epd4in2.sleep(() => {
		resolve();
	});
})

exports.colors = {
	white: 0xFF,
	gray: 0xC0,
	darkgray: 0x80,
	black: 0x00
}

exports.width = width;
exports.height = height;
exports.gd = gd;
