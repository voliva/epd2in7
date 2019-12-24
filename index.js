const epd4in2 = require('./build/Release/epd2in7');
const gd = require('node-gd');

const width = exports.width = epd4in2.width();
const height = exports.height = epd4in2.height();
const colors = exports.colors = {
	white: 0xFF,
	gray: 0xC0,
	darkgray: 0x80,
	black: 0x00
}
const speed = exports.speed = {
	normal: 0,
	quick: 1,
	quickest: 2
};

exports.getImageBuffer = async (orientation = 'portrait') => {
	const img = await new Promise(resolve =>
		(orientation === 'portrait')
			? gd.create(width, height, (error, img) => resolve(img))
			: gd.create(height, width, (error, img) => resolve(img))
	);

	for (let i = 0; i < 256; i++) {
		img.colorAllocate(i, i, i);
	}

	return img;
};

const createBuffer = (img, colorMap) => {
	const buf = new Buffer.alloc(width * height, 0);
	for (let y = 0; y < height; y++) {
		for (let x = 0; x < width; x++) {
			const color = img.height == height
				? img.getPixel(x, y)
				: img.getPixel(img.width - y, x);
			buf[x + y * width] = colorMap(color);
		}
	}
	return buf;
}

exports.init = (sp = speed.normal) => new Promise(resolve =>
	epd4in2.init(sp, resolve)
);

exports.initGray = (sp = speed.normal) => new Promise(resolve =>
	epd4in2.initGray(sp, resolve)
);

exports.displayImageBuffer = (img, bounds) => new Promise(resolve => {
	const buf = createBuffer(img, color => color < 128 ? colors.black : colors.white);
	if (!bounds) {
		return epd4in2.display(
			buf,
			resolve
		);
	}
	return epd4in2.display(
		buf,
		bounds.x,
		bounds.y,
		bounds.w,
		bounds.h,
		resolve
	);
});

exports.displayGrayImageBuffer = (img, bounds) => new Promise(resolve => {
	const buf = createBuffer(img, color => color);
	if (!bounds) {
		return epd4in2.displayGray(
			buf,
			resolve
		);
	}
	return epd4in2.displayGray(
		buf,
		bounds.x,
		bounds.y,
		bounds.w,
		bounds.h,
		resolve
	);
});

exports.clear = () => new Promise(resolve =>
	epd4in2.clear(resolve)
);

exports.sleep = () => new Promise(resolve =>
	epd4in2.sleep(resolve)
);
