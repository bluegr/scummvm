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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "sherlock/tattoo/tattoo_fixed_text.h"
#include "sherlock/sherlock.h"

namespace Sherlock {

namespace Tattoo {

static const char *const fixedTextEN[] = {
	"Money",
	"Money",
	"Card",
	"Card",
	"Tobacco",
	"Tobacco",
	"Timetable",
	"Timetable",
	"Summons",
	"Summons",
	"Foolscap",
	"Foolscap",
	"Damp Paper",
	"Foolscap",
	"Bull's Eye",
	"Bull's Eye Lantern",

	"Open",
	"Look",
	"Talk",
	"Use",
	"Journal",
	"Inventory",
	"Options",
	"Solve",
	"with",
	"No effect...",
	"This person has nothing to say at the moment",
	"Picked up",

	"Page %d",
	"Close Journal", 
	"Search Journal",
	"Save Journal",
	"Abort Search",
	"Search Backwards",
	"Search Forwards",
	"Text Not Found !",

	"Holmes",
	"Jock",
	"Bull",
	"Round",
	"Turn Total",
	"Dart",
	"to start",
	"Hit a key",
	"Press a key",
	"bullseye", // ??
	"GAME OVER",
	"BUSTED",
	"Wins",
	"Scored %d points",
	"points", // ??
	"Hit %s %d",
	"Double",
	"Triple",

	"Apply",
	"Water",
	"Heat",
	"Load Game",
	"Save Game",
	"Music",
	"Sound Effects",
	"Voices",
	"Text Windows",
	"Transparent Menus",
	"Change Font Style",
	"Off",
	"On",
	"Quit",
	"Are you sure you",
	"wish to Quit ?",
	"Yes",
	"No",
	"Enter Password",
	"Going East", // correct password, was not and should not to be translated
	// SH2: People names
	"Sherlock Holmes",
	"Dr. Watson",
	"Mrs. Hudson",
	"Stanley Forbes",
	"Mycroft Holmes",
	"Wiggins",
	"Police Constable Burns",
	"Augustus Trimble",
	"Police Constable Daley",
	"Matron",
	"Sister Grace",
	"Preston McCabe",
	"Bob Colleran",
	"Jonas Rigby",
	"Police Constable Roach",
	"James Dewar",
	"Sergeant Jeremy Duncan",
	"Inspector Gregson",
	"Inspector Lestrade",
	"Jesse Needhem",
	"Arthur Fleming",
	"Mr. Thomas Pratt",
	"Mathilda (Tillie) Mason",
	"Adrian Russell",
	"Eldridge Whitney",
	"Hepplethwaite",
	"Horace Silverbridge",
	"Old Sherman",
	"Maxwell Verner",
	"Millicent Redding",
	"Virgil Silverbridge",
	"George O'Keeffe",
	"Lord Denys Lawton",
	"Jenkins",
	"Jock Mahoney",
	"Bartender",
	"Lady Cordelia Lockridge",
	"Pettigrew",
	"Sir Avery Fanshawe",
	"Hodgkins",
	"Wilbur \"Birdy\" Heywood",
	"Jacob Farthington",
	"Philip Bledsoe",
	"Sidney Fowler",
	"Professor Theodore Totman",
	"Rose Hinchem",
	"Tallboy",
	"Ethlebert \"Stitch\" Rumsey",
	"Charles Freedman",
	"Nigel Hemmings",
	"Fairfax Carter",
	"Wilhelm II",
	"Wachthund",
	"Jonathan Wilson",
	"David Lloyd-Jones",
	"Edward Hargrove",
	"Misteray",
	"The Lascar",
	"Parrot",
	"Vincent Scarrett",
	"Alexandra",
	"Queen Victoria",
	"John Brown",
	"A Patient",
	"A Patient",
	"Patron",
	"Queen Victoria",
	"Patient in white",
	"Lush",
	"Drunk",
	"Prostitute",
	"Mudlark",
	"Grinder",
	"Bouncer",
	"Agnes Ratchet",
	"Aloysius Ratchet",
	"Real Estate Agent",
	"Candy Clerk",
	"Beadle",
	"Prussian",
	"Mrs. Rowbottom",
	"Miss Lloyd-Jones",
	"Tavern patron",
	"User",
	"Toby",
	"Stationer",
	"Law Clerk",
	"Ministry Clerk",
	"Bather",
	"Maid",
	"Lady Fanshawe",
	"Sidney Ratchet",
	"Boy",
	"Patron",
	"Constable Brit",
	"Wagon Driver"
};

// sharp-s       : 0xE1 / octal 341
// small a-umlaut: 0x84 / octal 204
// small o-umlaut: 0x94 / octal 224
// small u-umlaut: 0x81 / octal 201
// large O-umlaut: 0x99 / octal 231
static const char *const fixedTextDE[] = {
	"Geld",
	"Geld",
	"S. Holmes",
	"S. Holmes",
	"Tabak",
	"Tabak",
	"Plan",
	"Plan",
	"Aufforderg.",
	"Aufforderg.",
	"Blatt pap.",
	"Pergament",
	"Dunstig pap",
	"Dunstig pap",
	"Handlampe",
	"Handlampe",

	"\231ffne",
	"Schau",
	"Rede",
	"Benutze",
	"Tagebuch",
	"Tasche",
	"Optionen",
	"L\224osen",
	"mit",
	"Keine Wirkung...",
	"Diese Person wei\341 im Augenblick nichts zu berichten.",
	"Picked up", // <-- ??

	"Seite %d",
	"Schlie\341en",
	"Lessen", // <--
	"In Datei sichern",
	"Suche abbrechen",
	"R\201ckw\204rts suchen ",
	"Vorwarts suchen ",
	"Text nicht gefunden",

	"Holmes",
	"Jock",
	"Bull",
	"Runde: %d",
	"Gesamt: %d",
	"Pfeil # %d",
	"zum Starten",
	"Taste dr\201cken",
	"Taste dr\201cken",
	"Bullseye", // ??
	"SPIEL BEENDET!",
	"VERLOREN!",
	"Gewinnt!", // "Holmes Gewinnt!", "%s Gewinnt!"
	"Erzielte %d Punkte",
	"Punkte", // ??
	"Treffer %s %d",
	"Doppel",
	"Dreifach",

	"Benutze",
	"Wasser",
	"Erhitze",
	"Spiel laden",
	"Spiel sichern",
	"Musik",
	"Soundeffekte",
	"Voices",
	"Textfenster",
	"Transparente Men\201s",
	"Schriftart andern",
	"Aus",
	"An",
	"Ende",
	"Spiel beenden? ",
	"Sind Sie sicher ?",
	"Ja",
	"Nein",
	"Pa\341wort eingeben",
	"Going East", // correct password, was not and should not to be translated
	// SH2: People names
	"Sherlock Holmes", // note: People names were not translated in the German interpreter
	"Dr. Watson",
	"Mrs. Hudson",
	"Stanley Forbes",
	"Mycroft Holmes",
	"Wiggins",
	"Police Constable Burns",
	"Augustus Trimble",
	"Police Constable Daley",
	"Matron",
	"Sister Grace",
	"Preston McCabe",
	"Bob Colleran",
	"Jonas Rigby",
	"Police Constable Roach",
	"James Dewar",
	"Sergeant Jeremy Duncan",
	"Inspector Gregson",
	"Inspector Lestrade",
	"Jesse Needhem",
	"Arthur Fleming",
	"Mr. Thomas Pratt",
	"Mathilda (Tillie) Mason",
	"Adrian Russell",
	"Eldridge Whitney",
	"Hepplethwaite",
	"Horace Silverbridge",
	"Old Sherman",
	"Maxwell Verner",
	"Millicent Redding",
	"Virgil Silverbridge",
	"George O'Keeffe",
	"Lord Denys Lawton",
	"Jenkins",
	"Jock Mahoney",
	"Bartender",
	"Lady Cordelia Lockridge",
	"Pettigrew",
	"Sir Avery Fanshawe",
	"Hodgkins",
	"Wilbur \"Birdy\" Heywood",
	"Jacob Farthington",
	"Philip Bledsoe",
	"Sidney Fowler",
	"Professor Theodore Totman",
	"Rose Hinchem",
	"Tallboy",
	"Ethlebert \"Stitch\" Rumsey",
	"Charles Freedman",
	"Nigel Hemmings",
	"Fairfax Carter",
	"Wilhelm II",
	"Wachthund",
	"Jonathan Wilson",
	"David Lloyd-Jones",
	"Edward Hargrove",
	"Misteray",
	"The Lascar",
	"Parrot",
	"Vincent Scarrett",
	"Alexandra",
	"Queen Victoria",
	"John Brown",
	"A Patient",
	"A Patient",
	"Patron",
	"Queen Victoria",
	"Patient in white",
	"Lush",
	"Drunk",
	"Prostitute",
	"Mudlark",
	"Grinder",
	"Bouncer",
	"Agnes Ratchet",
	"Aloysius Ratchet",
	"Real Estate Agent",
	"Candy Clerk",
	"Beadle",
	"Prussian",
	"Mrs. Rowbottom",
	"Miss Lloyd-Jones",
	"Tavern patron",
	"User",
	"Toby",
	"Stationer",
	"Law Clerk",
	"Ministry Clerk",
	"Bather",
	"Maid",
	"Lady Fanshawe",
	"Sidney Ratchet",
	"Boy",
	"Patron",
	"Constable Brit",
	"Wagon Driver"
};

// small a w/ accent grave: 0x85 / octal 205
// small e w/ accent acute: 0x82 / octal 202
// small e w/ accent grave: 0x8A / octal 212
// small e w/ circonflexe:  0x88 / octal 210
// small cedilla:           0x87 / octal 207
static const char *const fixedTextFR[] = {
	"Argent",
	"Argent",
	"S. Holmes",
	"S. Holmes",
	"Tabac",
	"Tabac",
	"Horaire",
	"Horaire",
	"Convocation",
	"Convocation",
	"Feuille",
	"Feuille",
	"F. humide",
	"Feuille",
	"Lanterne",
	"Lanterne",

	"Ouvrir",
	"Regarder",
	"Parler",
	"Utiliser",
	"Journal", // <--
	"Inventaire",
	"Options",
	"R\202soudre",
	"avec",
	"Sans effet...",
	"Cette personne n'a rien \205 ajouter pour le moment.",
	"Picked up", // <-- ??

	"Page %d",
	"Fermer",
	"Lessen", // <--
	"Sauvegarder",
	"Annuler ",
	"Chercher avant",
	"Chercher apr\212s",
	"Texte introuvable !",

	"Holmes",
	"Jock",
	"Bull",
	"Tour: %d",
	"Total: %d",
	"Fl\202chette # %d",
	"pour commencer",
	"Appuyez sur C",
	"Appuyez sur C",
	"Bullseye", // ??
	"FIN DE LA PARTIE!", // original: "Fin de la partie!"
	"FIASCO!",
	"Gagnant!", // "Holmes Gagnant!", "%s Gagnant!"
	"Total des points: %d",
	"Punkte", // ??
	"Treffer %s %d",
	"double",
	"triple",

	"Mouillez",
	"Puis",
	"Chauffez",
	"Charger",
	"Sauvegarder",
	"Musique",
	"Sons",
	"Voix",
	"Fen\210tres de texte", // 0x88
	"Menu Transparent",
	"Changer la fonte",
	"Aus", // ???
	"An", // ???
	"Quitter",
	"Voulez-vous quitter?",
	"Sind Sie sicher ?", // ???
	"Oui",
	"Non",
	"Entrez le mot de passe",
	"Going East", // correct password, was not and should not to be translated
	// SH2: People names
	"Sherlock Holmes",
	"Dr. Watson",
	"Mme. Hudson",
	"Stanley Forbes",
	"Mycroft Holmes",
	"Wiggins",
	"Sergent Burns",
	"Augustus Trimble",
	"Sergent Daley",
	"Infirmi\212re chef",
	"Mme. Grace",
	"Preston McCabe",
	"Bob Colleran",
	"Jonas Rigby",
	"Sergent Roach",
	"James Dewar",
	"Sergent Jeremy Duncan",
	"Inspecteur Gregson",
	"Inspecteur Lestrade",
	"Jesse Needhem",
	"Arthur Fleming",
	"M. Thomas Pratt",
	"Mathilda (Tillie) Mason",
	"Adrian Russell",
	"Eldridge Whitney",
	"Hepplethwaite",
	"Horace Silverbridge",
	"Sherman",
	"Maxwell Verner",
	"Millicent Redding",
	"Virgil Silverbridge",
	"George O'Keeffe",
	"Lord Denys Lawton",
	"Jenkins",
	"Jock Mahoney",
	"Serveur",
	"Lady Cordelia Lockridge",
	"Pettigrew",
	"Sir Avery Fanshawe",
	"Hodgkins",
	"Wilbur \"Birdy\" Heywood",
	"Jacob Farthington",
	"Philip Bledsoe",
	"Sidney Fowler",
	"Professeur Theodore Totman",
	"Rose Hinchem",
	"Tallboy",
	"Ethlebert \"Stitch\" Rumsey",
	"Charles Freedman",
	"Nigel Hemmings",
	"Fairfax Carter",
	"Wilhelm II",
	"Wachthund",
	"Jonathan Wilson",
	"David Lloyd-Jones",
	"Edward Hargrove",
	"Misteray",
	"Le Lascar",
	"Oiseau",
	"Vincent Scarrett",
	"Alexandra",
	"Queen Victoria",
	"John Brown",
	"Patient",
	"Patient",
	"Client",
	"Queen Victoria",
	"Patient en blanc",
	"Ivrogne",
	"Ivrogne",
	"Belle femme",
	"Mudlark",
	"Broyeur",
	"Videur",
	"Agnes Ratchet",
	"Aloysius Ratchet",
	"Immobilier",
	"Gar\207on",
	"Beadle",
	"Prussian",
	"Mme. Rowbottom",
	"Mme Lloyd-Jones",
	"Tavern Client",
	"User",
	"Toby",
	"Papeterie",
	"Law Clerc",
	"Ministry Employ\202",
	"Clint du thermes",
	"Bonne",
	"Lady Fanshawe",
	"Sidney Ratchet",
	"Gar\207on",
	"Client",
	"Sergent Brit",
	"Wagon Driver"
};

// TODO: There also was a Spanish version of Sherlock Holmes 2
static const FixedTextLanguageEntry fixedTextLanguages[] = {
	{ Common::DE_DEU,   fixedTextDE },
	// { Common::ES_ESP,   fixedTextES },
	{ Common::EN_ANY,   fixedTextEN },
	{ Common::FR_FRA,   fixedTextFR },
	{ Common::UNK_LANG, fixedTextEN }
};

TattooFixedText::TattooFixedText(SherlockEngine *vm) : FixedText(vm) {
	// Figure out which fixed texts to use
	Common::Language curLanguage = _vm->getLanguage();

	const FixedTextLanguageEntry *curLanguageEntry = fixedTextLanguages;

	while (curLanguageEntry->language != Common::UNK_LANG) {
		if (curLanguageEntry->language == curLanguage)
			break; // found current language
		curLanguageEntry++;
	}
	_curLanguageEntry = curLanguageEntry;
}

const char *TattooFixedText::getText(int fixedTextId) {
	return _curLanguageEntry->fixedTextArray[fixedTextId];
}

const Common::String TattooFixedText::getActionMessage(FixedTextActionId actionId, int messageIndex) {
	return Common::String();
}


} // End of namespace Tattoo

} // End of namespace Sherlock
