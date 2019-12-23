const epd4in2 = require('./build/Release/epd2in7');
const gd = require('node-gd');

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
		epd4in2.displayFrame(
			buf,
			() => {
				resolve();
			}
		);
	})
}

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
		epd4in2.displayFrame(
			buf,
			() => {
				resolve();
			}
		);
	})
}

function displayPartialImageBuffer(img, x, y, w, h) {
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
		epd4in2.displayPartialFrame(
			buf,
			x, y, w, h,
			() => {
				resolve();
			}
		);
	})
}

exports.getImageBuffer = getImageBuffer;

exports.displayImageBuffer = displayImageBuffer;

exports.displayGrayImageBuffer = displayGrayImageBuffer;

exports.displayPartialImageBuffer = displayPartialImageBuffer;

exports.init = () => new Promise(resolve => {
	epd4in2.init(() => {
		resolve();
	});
})

exports.initGray = () => new Promise(resolve => {
	epd4in2.init_gray(() => {
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
