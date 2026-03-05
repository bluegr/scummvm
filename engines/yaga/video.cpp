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

#include "common/events.h"
#include "engines/engine.h"
#include "graphics/framelimiter.h"
#include "video/bink_decoder.h"
#include "yaga/video.h"

namespace Yaga {

bool VideoFile::play(const Common::String &filename) {
	if (!_videoDecoder.loadFile(Common::Path(filename))) {
		warning("Video::play: could not open '%s'", filename.c_str());
		return false;
	}

	bool skipVideo = false;
	Common::Event e;
	Graphics::FrameLimiter limiter(g_system, 60);

	//_events->clearEvents();
	_videoDecoder.start();

	while (!g_engine->shouldQuit() && !_videoDecoder.endOfVideo() && !skipVideo) {
		while (g_system->getEventManager()->pollEvent(e)) {
			if (e.type == Common::EVENT_KEYUP) {
				if (e.kbd.keycode == Common::KEYCODE_ESCAPE)
					skipVideo = true;
			}
		}

		if (_videoDecoder.needsUpdate()) {
			const Graphics::Surface *frame = _videoDecoder.decodeNextFrame();
			if (frame) {
				g_system->copyRectToScreen(frame->getPixels(), frame->pitch, 0, 0, frame->w, frame->h);
			}

			g_system->updateScreen();
		}

		g_system->delayMillis(10);

		//Common::CustomEventType action;
		//if (_events->getAction(action)) {
		//	if (action == kActionSkip)
		//		skipVideo = true;
		//}
	}

	_videoDecoder.close();

	return !skipVideo;
}


} // End of namespace Yaga
