/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/Sound.cpp $
 * $Id: Sound.cpp 26 2010-02-02 04:01:05Z alexbevi $
 *
 */

#include "dgds/resources/Sound.h"

namespace Dgds {

Sound::Sound(Resource *res):
	_res(res) {
	init();
}

Sound::~Sound() {
	//_tags.clear();
	delete _res;
}

bool Sound::init() {
	bool ret = false;

	Resource *snd = _res->getSubResource("SND:");

	if (snd == NULL)
		return ret;

	Resource *inf = snd->getSubResource("INF:");
	Resource *tag = snd->getSubResource("TAG:");
	Resource *fnm = snd->getSubResource("FNM:");

	if (inf == NULL || tag == NULL || fnm == NULL)
		return ret;

	tag->seek(0);
	uint16 tagcount = tag->readUint16LE();

	Common::Array<Resource *> dat = snd->getSubResourceCollection("DAT:");
	//debug ("Found %c Data Entries", dat.size());

	for (uint16 i = 0; i < tagcount; i++) {
		uint16 tagid = tag->readUint16LE();
		//Common::String tagval = tag->to_s(false);

		Resource *entryData = 0;
		for (int32 j = 0; j < dat.size(); j++) {
			uint16 entryId = dat[j]->readUint16LE();
			if (entryId == tagid) {
				entryData = dat.remove_at(j);
				break;
			}
		}

		assert(entryData == 0);

#if 0
		ResourceEntry entry;
		entry.id   = tagid;
		entry.name = tagval;

		_tags.setVal(tagid, entry);
#endif
	}
	delete tag;

	// clean any additional entries left
	// NOTE this technically shouldn't be necessary as all
	// elements should be accounted for ... but for now ...
	for (uint32 i = 0; i < dat.size(); i++)
		delete dat[i];

	dat.clear();

	// XXX there's a VERY good chance that the FNM tag processing
	// can be skipped entirely, as it looks like (A) the filenames
	// are mapped in the same order they appear in the TAG list, and
	// (B) each sound resource is essentially the tag name + ".SND"
	fnm->seek(0);
	uint16 fnmcount = fnm->readUint16LE();
	for (uint16 j = 0; j < fnmcount; j++) {
		uint16 fnmid = fnm->readUint16LE();
		//_tags[fnmid].filename = fnm->to_s(false);
	}
	delete fnm;

	/* XXX print list of resources
	ResourceTags::const_iterator iter  = _tags.begin();
	while (iter != _tags.end()) {
		debug("%d:%s:%s", iter->_value.id, iter->_value.name.c_str(), iter->_value.filename.c_str());
		++iter;
	}
	// */

	// XXX skipping TAG_INF for the sound resource as it appears
	// to contain nothing more than a list of the sound resource
	// id values, which we've already parsed when we processed
	// the TAG_TAG data
	delete inf;

	return true; //ret;
}

} // end of namespace Dgds
