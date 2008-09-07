#include "innocent/graphics.h"

#include "common/system.h"
#include "graphics/surface.h"

#include "innocent/innocent.h"
#include "innocent/resources.h"

namespace Innocent {

Graphics::Graphics(Engine *engine)
	 : _engine(engine) {
}

void Graphics::init() {
	 _resources = _engine->resources();
	 _system = _engine->_system;
	 loadInterface();
}

void Graphics::loadInterface() {
	byte palette[0x400];

	_resources->loadInterfaceImage(_interface, palette);
	debug(kAck, "loaded interface image");

	_engine->_system->setPalette(palette + 160 * 4, 160, 96);
	debug(kAck, "set interface palette");
}

void Graphics::paintInterface() {
	debug(kAck, "painting interface");
	_engine->_system->copyRectToScreen(_interface, 320, 0, 152, 320, 48);
	debug(kAck, "painted interface");
}

void Graphics::setBackdrop(uint16 id) {
	debug(2, "setting backdrop image id to 0x%04x", id);

	byte palette[0x400];
	_backdrop.reset(_resources->loadBackdrop(id, palette));
	debug(3, "backdrop loaded");
	_system->setPalette(palette, 0, 256);
}

void Graphics::paintBackdrop() {
	// TODO cropping
	_system->copyRectToScreen(reinterpret_cast<byte *>(_backdrop->pixels), 320, 0, 0, 320, 200);
}

void Graphics::paintText(uint16 left, uint16 top, byte colour, byte *string) {
	byte ch;
	uint16 current_left = left;
	byte current_colour = colour;

	while ((ch = *(string++))) {
		switch(ch) {
		case '\r':
			current_left = left;
			top += kLineHeight;
			break;
		case kStringDefaultColour:
			current_colour = colour;
			break;
		case kStringSetColour:
			current_colour = *(string++);
			break;
		default:
			current_left += paintChar(current_left, top, current_colour, ch);
		}
	}
}

byte Graphics::clampChar(byte ch) {
	if (ch == '#')
		return '!';
	if (ch < ' ' || ch > '~')
		return '?';
	return ch;
}

/**
 * @returns char width
 */
uint16 Graphics::paintChar(uint16 left, uint16 top, byte colour, byte ch) const {
	// TODO perhaps cache or sth
	ch = clampChar(ch);
	if (ch == ' ')
		return 4; // space has no glyph, just width 4
	Sprite *glyph = _resources->getGlyph(ch);
	glyph->recolour(colour);
	_system->copyRectToScreen(reinterpret_cast<byte *>(glyph->pixels), glyph->pitch/8,
							   left, top, glyph->w, glyph->h);
	uint16 w = glyph->w;
	delete glyph;
	return w;
}

} // End of namespace Innocent
