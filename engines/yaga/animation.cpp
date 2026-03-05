/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/path.h"
#include "common/compression/unzip.h"
#include "common/stream.h"
#include "common/memstream.h"
#include "yaga/animation.h"

#include "common/substream.h"
#include "common/compression/deflate.h"

namespace Yaga {

enum MngTags {
	kMngChunkMHDR = 0x4d484452,
	kMngChunkTIME = 0x74494d45,
	kMngChunkBACK = 0x4241434b,
	kMngChunkSBIT = 0x73424954,
	kMngChunkPLTE = 0x504c5445,
	kMngChunkTRNS = 0x74524e53,
	kMngChunkFRAM = 0x4652414d,
	kMngChunkFLAG = 0x666c4147,
	kMngChunkDEFI = 0x44454649,
	kMngChunkTEXT = 0x74455874,
	kMngChunkIHDR = 0x49484452,
	kMngChunkIDAT = 0x49444154,
	kMngChunkIEND = 0x49454e44,
	kMngChunkMEND = 0x4d454e44
};

enum PNGColorType {
	kPngGrayScale = 0,          // bit depths: 1, 2, 4, 8, 16
	kPngTrueColor = 2,          // bit depths: 8, 16
	kPngIndexed = 3,            // bit depths: 1, 2, 4, 8
	kPngGrayScaleWithAlpha = 4, // bit depths: 8, 16
	kPngTrueColorWithAlpha = 6  // bit depths: 8, 16
};

Animation::Animation(const Common::String &filename) {
	_archive = Common::makeZipArchive(Common::Path(filename));
}

Animation::~Animation() {
	for (auto &animation : _animations) {
		for (auto &frame : animation.frames) {
			for (auto &layer : frame.layers) {
				layer.surface.free();
			}
			frame.layers.clear();
		}
		animation.frames.clear();
	}
	_animations.clear();

	delete _archive;
}

YagaLayer *Animation::getLayer(uint animationIndex, uint frameIndex, uint layerIndex) {
	if (animationIndex >= _animations.size())
		return nullptr;
	YagaAnimation &animation = _animations[animationIndex];
	if (frameIndex >= animation.frames.size())
		return nullptr;
	YagaFrame &frame = animation.frames[frameIndex];
	if (layerIndex >= frame.layers.size())
		return nullptr;
	return &frame.layers[layerIndex];
}

Graphics::Surface *Animation::getImage() {
	if (_animations.empty() || _animations[0].frames.empty() || _animations[0].frames[0].layers.empty())
		return new Graphics::Surface();
	return &_animations[0].frames[0].layers[0].surface;
}

void Animation::load(const Common::String &filePath) {
	if (filePath.hasSuffixIgnoreCase(".mng"))
		loadMng(filePath);
	else if (filePath.hasSuffixIgnoreCase(".rle"))
		loadRle(filePath);
	else
		error("Unsupported anim format: %s", filePath.c_str());
}

Graphics::PixelFormat Animation::getPixelFormatFromColorType(byte colorType) {
	switch (colorType) {
	case kPngIndexed:
	//	return Graphics::PixelFormat::createFormatCLUT8();
	case kPngTrueColor:
	//	return Graphics::PixelFormat::createFormatRGB24();
	case kPngTrueColorWithAlpha:
		return Graphics::PixelFormat::createFormatRGBA32();
	default:
		error("Unsupported PNG color type: %d", colorType);
	}
}

void Animation::loadMng(const Common::String &filePath) {
	static const byte MNG_SIG[] = { 0x8a, 0x4d, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
	byte sig[8];
	Common::SeekableReadStream *animStream = _archive->createReadStreamForMember(Common::Path(filePath));

	// Create a new animation and place it in the list of animations.
	// We will fill it as we read the MNG stream.
	_animations.emplace_back();
	YagaAnimation *animation = &_animations.back();
	YagaFrame *currentFrame = nullptr;
	YagaLayer *currentLayer = nullptr;
	byte tmp;
	uint16 width, height;
	byte r, g, b, a;

	animStream->read(sig, 8);
	if (memcmp(sig, MNG_SIG, 8) != 0) {
		error("Invalid MNG signature: %s", filePath.c_str());
	}

	for (int i = 0; i < 256; i++) {
		_palette[i * 4 + 0] = 0; // r
		_palette[i * 4 + 1] = 0; // g
		_palette[i * 4 + 2] = 0; // b
		_palette[i * 4 + 3] = 0xff; // a
	}

	while (!animStream->eos()) {
		uint32 size = animStream->readUint32BE();
		uint32 tag = animStream->readUint32BE();
		switch (tag) {
		case kMngChunkMHDR:
			assert(size == 28);
			// we read the layer size from the IHDR chunk
			animStream->skip(size);
			break;
		case kMngChunkTIME:
			assert(size == 7);
			animStream->skip(size);
			break;
		case kMngChunkBACK:	// Background color
			assert(size == 7);
			// r, g, b: 3 * 2 bytes
			// mandatory flag: 1 byte
			animStream->skip(size);
			break;
		case kMngChunkSBIT:	// Number of significant bits
			assert(size <= 4);
			animStream->skip(size);
			break;
		case kMngChunkTRNS:	// Transparency
			assert(size == 0 || size == 256);
			if (size == 256) {
				for (int i = 0; i < 256; i++) {
					_palette[i * 4 + 3] = animStream->readByte(); // a
				}
			}
			break;
		case kMngChunkPLTE:	// Palette
			assert(size == 0 || size == 256 * 3);
			if (size == 256 * 3) {
				for (int i = 0; i < 256; i++) {
					_palette[i * 4 + 0] = animStream->readByte(); // r
					_palette[i * 4 + 1] = animStream->readByte(); // g
					_palette[i * 4 + 2] = animStream->readByte(); // b
				}
			}
			break;
		case kMngChunkFRAM:	// Add frame to animation
			if (size == 10) {	// first frame
				animStream->skip(size);
				assert(!currentFrame);
			} else {
				assert(size == 0);
				if (currentFrame && currentFrame->layers.empty())	// empty frame
					break;
			}

			animation->frames.emplace_back();
			currentFrame = &animation->frames.back();
			break;
		case kMngChunkDEFI:	// Add layer to frame
			assert(size == 12);
			assert(currentFrame);

			currentFrame->layers.emplace_back();
			currentLayer = &currentFrame->layers.back();

			animStream->skip(4);
			currentLayer->pos.x = (uint16)animStream->readUint32BE();
			currentLayer->pos.y = (uint16)animStream->readUint32BE();
			//TODO: state = 1
			break;
		case kMngChunkTEXT:
			assert(currentLayer);
			//currentLayer->name = animStream->readString('\0', size);
			animStream->skip(size);
			break;
		case kMngChunkFLAG:
			assert(size == 4);
			assert(currentLayer);
			currentLayer->mask = animStream->readUint32LE();
			break;
		case kMngChunkIHDR:
			assert(size == 13);
			assert(currentLayer);
			width = (uint16)animStream->readUint32BE();
			height = (uint16)animStream->readUint32BE();
			tmp = animStream->readByte();	// depth
			assert(tmp == 8);
			_colorType = animStream->readByte();
			tmp = animStream->readByte(); // compression
			assert(tmp == 0);
			tmp = animStream->readByte(); // filter
			assert(tmp == 0);
			tmp = animStream->readByte(); // interlace
			assert(tmp == 0);

			currentLayer->surface.create(width, height, Graphics::PixelFormat::createFormatRGBA32());
			break;
		case kMngChunkIDAT:
			// Note: there can be multiple IDAT chunks for a single layer, but we assume that there
			// is always one chunk per layer here.
			// This is the case for all MNG files we have seen, but it is not guaranteed by the specification.
			assert(currentLayer);
			{
				Common::SeekableSubReadStream subStream(animStream, animStream->pos(), animStream->pos() + size);
				Common::SeekableReadStream *idatStream = Common::wrapCompressedReadStream(&subStream, DisposeAfterUse::NO);
				width = currentLayer->surface.w;
				height = currentLayer->surface.h;
				Graphics::PixelFormat format = currentLayer->surface.format;

				for (int16 y = 0; y < height; ++y) {
					byte filterType = idatStream->readByte();
					assert(filterType == 0); // no filter

					for (int16 x = 0; x < width; ++x) {
						if (_colorType == kPngTrueColor || _colorType == kPngTrueColorWithAlpha) {
							r = idatStream->readByte();
							g = idatStream->readByte();
							b = idatStream->readByte();
							a = _colorType == kPngTrueColorWithAlpha ? idatStream->readByte() : 0xff;
							currentLayer->surface.setPixel(x, y, format.ARGBToColor(a, r, g, b));
						} else if (_colorType == kPngIndexed) {
							byte color = idatStream->readByte();
							r = _palette[color * 4 + 0];
							g = _palette[color * 4 + 1];
							b = _palette[color * 4 + 2];
							a = _palette[color * 4 + 3];
							currentLayer->surface.setPixel(x, y, format.ARGBToColor(a, r, g, b));
						}
					}
				}
			}
			break;
		case kMngChunkIEND:
			assert(size == 0);
			break;
		case kMngChunkMEND:
			// End of MNG stream, should be last chunk
			break;
		default:
			warning("Unknown MNG chunk %08x, size %d", tag, size);
			animStream->skip(size);
			break;
		}

		animStream->skip(4); // skip CRC
		if (tag == kMngChunkMEND)
			break;
	}

	delete animStream;
}

void Animation::loadRle(const Common::String &filePath) {
	static const byte RLE_SIG[] = { 0xF2, 0x65, 0x6C, 0x72, 0x00, 0x00, 0x20, 0x4D };
	byte sig[8];
	uint32 frameCount, flags, layerCount;
	YagaAnimation *animation = nullptr;
	YagaFrame *currentFrame = nullptr;
	YagaLayer *currentLayer = nullptr;
	Common::SeekableReadStream *animStream = _archive->createReadStreamForMember(Common::Path(filePath));

	animStream->read(sig, 8);
	if (memcmp(sig, RLE_SIG, 8) != 0) {
		error("Invalid RLE signature: %s", filePath.c_str());
	}

	frameCount = animStream->readUint32LE();
	flags = animStream->readUint32LE();
	if (flags & 1) {
		animStream->read(_palette, 256 * 4);
	}

	// Create a new animation and place it in the list of animations.
	// We will fill it as we read the RLE stream.
	_animations.emplace_back();
	animation = &_animations.back();

	for (uint32 i = 0; i < frameCount; i++) {
		animation->frames.emplace_back();
		currentFrame = &animation->frames.back();

		animStream->skip(12); // skip frame header
		layerCount = animStream->readUint32LE();

		for (uint32 j = 0; j < layerCount; j++) {
			currentFrame->layers.emplace_back();
			currentLayer = &currentFrame->layers.back();

			decodeRleLayer(animStream, currentLayer);
		}
	}

	delete animStream;
}

void Animation::decodeRleLayer(Common::SeekableReadStream *animStream, YagaLayer *layer) {
	uint32 layerFormat, imageSize, flags, tmp;
	uint16 width, height;
	byte sig[4];
	byte layerPalette[256 * 4];
	byte *palette = _palette;
	uint32 *pixels;
	int offset = 0;
	byte r, g, b, a;
	Graphics::PixelFormat format = Graphics::PixelFormat::createFormatRGBA32();
	
	layer->pos.x = (int16)animStream->readUint32LE();
	layer->pos.y = (int16)animStream->readUint32LE();
	animStream->skip(4);
	layer->mask = animStream->readUint32LE();

	// currentLayer->name = animStream->readString('\0', size);
	animStream->skip(0x39); // skip name
	animStream->read(sig, 4);
	assert(memcmp(sig, "rle\00", 4) == 0);
	layerFormat = animStream->readUint32LE();
	animStream->skip(3); // skip zeroes
	width = (uint16)animStream->readUint32LE();
	height = (uint16)animStream->readUint32LE();
	flags = animStream->readUint32LE();
	animStream->skip(4);
	tmp = animStream->readUint32LE();
	assert(tmp == 1);
	imageSize = animStream->readUint32LE();

	if (flags & 1) {
		animStream->read(layerPalette, 256 * 4);
		palette = layerPalette;
	}

	layer->surface.create(width, height, format);
	pixels = (uint32 *)layer->surface.getPixels();

	switch (layerFormat) {
	case 0x40012F9:
	case 0x40012FB: // paletted
		while (imageSize > 0) {
			const byte code = animStream->readByte();
			const int count = (code & 0x3F) + 1;
			if ((code & 0xC0) == 0xC0) {
				// transparent
			} else if ((code & 0x80) == 0x80) {
				const byte color = animStream->readByte();

				r = palette[color * 4 + 0];
				g = palette[color * 4 + 1];
				b = palette[color * 4 + 2];
				a = palette[color * 4 + 3];
				uint32 value = format.ARGBToColor(a, r, g, b);

				for (int i = 0; i < count; ++i) {
					pixels[offset + i] = value;
				}
				--imageSize;
			} else {
				for (int i = 0; i < count; ++i) {
					const byte color = animStream->readByte();

					r = palette[color * 4 + 0];
					g = palette[color * 4 + 1];
					b = palette[color * 4 + 2];
					a = palette[color * 4 + 3];
					uint32 value = format.ARGBToColor(a, r, g, b);

					pixels[offset + i] = value;
				}
				imageSize -= count;
			}
			--imageSize;
			offset += count;
		}
		break;
	case 0xC0012F9: // RGBA
		while (imageSize > 0) {
			const byte code = animStream->readByte();
			const int count = (code & 0x3F) + 1;
			if ((code & 0xC0) == 0xC0) {
				// transparent
			} else if ((code & 0x80) == 0x80) {
				const uint32 color = animStream->readUint32LE();
				for (int i = 0; i < count; ++i) {
					pixels[offset + i] = color;
				}
				imageSize -= 4;
			} else {
				for (int i = 0; i < count; ++i) {
					const uint32 color = animStream->readUint32LE();
					pixels[offset + i] = color;
				}
				imageSize -= count * 4;
			}
			--imageSize;
			offset += count;
		}
		break;
	default:
		error("Unsupported RLE format 0x%x\n", layerFormat);
	}

	assert(imageSize == 0);
}

} // End of namespace Yaga
