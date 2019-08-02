// This header file exists to provide multilanguage functionality
// for the RPC API.  An RPCapi object (from RPCapi.h) can be
// dynamically cast to an RPCapiML object in order to
// access this functionality.

#ifndef RPC_API_ML_H
#define RPC_API_ML_H

#include "RPCapi.h"

#ifdef RPC_API_ML


class RPCapiML {
public:
	// This class represents a collection of translations
	// for a single dialog.
	class Translator {
	public:
		// Destructor.
		virtual ~Translator(void) {}

		// This routine gets the indicated translator or translation.
		// Parameters:
		//		name:	The name of the control to translate.
		//		res:	The newly-allocated translation, or NULL
		//				if no matching translation exists.
		// Returns:
		//		true iff res is not NULL.
		virtual bool translationGet(RPCapi::TStringArg name, RPCapi::TString &res) const = 0;

	};	// class Translator


	// This class represents all of the currently-loaded Translators.
	class TranslatorTable {
	public:
		// Destructor.
		virtual ~TranslatorTable(void) {}

		// These routines get / set the language-code.
		virtual const RPCapi::TString &languageCodeGet(void) const = 0;
		virtual void languageCodeSet(RPCapi::TStringArg lc) = 0;

		// This routine clears all data cached in this table.
		virtual void reset(void) = 0;

		// These routines get or free a translator
		// for a particular RPC ID.
		// Parameters:
		//		id:		The ID to match.
		//		lc:		The language code to match.
		//		fn:		The filename to match.
		//		dlg:	The name of the dialog to match.
		//		trans:	The translator to free.
		// Returns:
		//		The matching translator, or NULL if
		//		there is no such translator.
		virtual const Translator *translatorGet(
			RPCapi::TStringArg fn, RPCapi::TStringArg dlg) = 0;
		virtual const Translator *translatorGetByID(
			const RPCapi::ID *id, RPCapi::TStringArg lc, RPCapi::TStringArg dlg) = 0;
		virtual const Translator *translatorGet(
			RPCapi::TStringArg id, RPCapi::TStringArg lc, RPCapi::TStringArg dlg) = 0;
		virtual void translatorFree(const Translator *trans) = 0;

	};	// class TranslatorTable


	// This routine returns the plugin-style ID of the API.
	virtual const RPCapi::TString &apiID(void) const = 0;

	// This routine gets the table of Translators.
	virtual TranslatorTable *translatorTableGet(void) = 0;

};	// class RPCapiML

#endif	// ifdef RPC_API_ML

#endif	// ifndef RPC_API_ML_H
