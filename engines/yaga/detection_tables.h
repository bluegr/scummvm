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

namespace Yaga {

const PlainGameDescriptor yagaGames[] = {
	{"basketball2004", "Backyard Basketball 2004"},
	{"football2004", "Backyard Football 2004"},
	{"hockey", "Backyard Hockey"},
	{"pajama4", "Pajama Sam 4: Life is Rough When You Lose Your Stuff "},
	{"puttpepsbirthday", "Putt-Putt: Pep's Birthday Surprise"},
	{ 0, 0 }
};

const ADGameDescription gameDescriptions[] = {
	{
		"pajama4",
		nullptr,
		AD_ENTRY1s("rooms.he", "4ad491932a603d639bad658695d0827d", 249492758),
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},

	AD_TABLE_END_MARKER
};

} // End of namespace Yaga
