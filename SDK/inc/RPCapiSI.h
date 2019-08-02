// This file exists to provide an RPC Instance -agnostic
// interface to the SelectionWindow suite.

#ifndef _RPC_API_SI_H_
#define _RPC_API_SI_H_

#define RPC_SEL_INT
#ifdef RPC_SEL_INT
#define RPC_SEL_INT_KEYWORDS

#include "RPCapiEx.h"


class RPCapiSI {
public:

	// This extends the basic RPCapi::Client interface.
	class ClientSI {
	public:
		virtual ~ClientSI(void) {}

		// This routine retrieves the filetypes this client supports.
		// Only files of these types will be retrieved from the ACM.
		// File types should be integers whose low-order bytes
		// match the upper case letters in the file type extension.
		// Example:  RPC files have fileType == 0x00525043
		// Example:  SKP files have fileType == 0x00534b50
		// NOTE:  In the absence of this function,
		//        only RPC files are supported.
		// Parameters:
		//		num:	The number of filetypes, returned by referenc.
		// Returns:
		//		An array containing the supported filetypes.
		virtual const int *RPCfiletypes(int &num) = 0;
	};	// class ClientSI


	// This class displays a selection window of the indicated type.
	class SelectionInterface : public RPCapi::BaseObject {
	public:
		typedef enum OBJECT_CODE_TYPE {
			OBJECT_CODE = 80	// pass to RPCapi::newObject().
		} OBJECT_CODE_T;

		typedef enum WINDOW_CODE {
			NONE = 0,
			CLASSIC = 1,		// Proj / Cat combo and content list
			CLASSIC_PLUS = 2,	// adds Message Center, Configure,
								// About, and Help buttons.
			TREE = 3,			// Proj combo and cat / content treeview
			THUMBS = 4,			// thumbnail browser
			THUMBS_PLUS = 5		// adds Message Center, Configure,
								// About, and Help buttons
		} WINDOW_CODE_T;	// end typedef enum WINDOW_CODE

		// OR these w/ the window code:
		typedef enum WINDOW_OPTIONS {
										// By default, set initial selection
										// from the RPC Instance.
			NO_SELECTION = 0x00010000,	// Do not use global selection or
										// instance to set initial selection.
			GLOBAL_SELECTION = 0x00020000	// Use globally stored
											// last selection to set
											// initial selection
		} WINDOW_OPTIONS_T;	// end typedef enum WINDOW_OPTIONS

		typedef enum MODE_CODE {
			MODELESS = 1,		// Shows window inside parent
			FLOATING = 2,		// Shows window in its own border
			MODAL = 3			// Modal dialog
		} MODE_CODE_T;


		// A caller can register an object of this class with
		// a SelectionInterface so that it can be notified
		// when the SI's selection has changed.
		class SIcallback {
		public:
			// These are the possible actions that can be
			// passed to a SIcallback from a SelectionInterface.
			typedef enum ACTION_CODE_TYPE {
				NONE = 0,				 // No action.
				SEL_CHANGE = 2,			 // Selection has changed.
				WINDOW_CLOSING = 3		 // args: Type: int,  Values: IDOK(1), IDCANCEL(2)
			} ACTION_CODE_T; // end typedef enum ACTION_CODE_TYPE

			// Destructor
			virtual ~SIcallback(void) {}

			// This routine is called by a SelectionWindow2 on a change.
			// Parameters:
			//		sw:			The SelectionWindow2 making the callback.
			//		action:		The action taken.  See
			//					SelectionWindow2::Callback::ACTION_CODE.
			//		args:		Additional arguments.
			virtual void RPCselectionInterfaceCallbackProc(
				const SelectionInterface *sw, int action, va_list args) = 0;

		};	// class SIcallback


		virtual ~SelectionInterface(void) {}

		// For internal use only.
		virtual const void *baseSelTableGet(void) const = 0;
		virtual void baseSelTableSet(const void *st) = 0;

		// For internal use only.
		virtual void contentDatabase(void *cdb, int cdbPortions = 0) = 0;

		// This routine hides this window (destroys the HWND).
		virtual void hide(void) = 0;

		// These routines get / set the currently-selected ID
		// of this window.
		virtual void idSet(const RPCapi::ID *id) = 0;
		virtual const RPCapi::ID *idGet(void) const = 0;

		// For internal use only.
		virtual void *projTableGet(void) const = 0;
		virtual void projTableSet(void *pt) = 0;

		// This routine registers a callback with this window.
		// Parameters:
		//		callback:	The callback to register.
		//		reg:		If true, register.  If false, unregister.
		virtual void registerCallback(SIcallback *callback, bool reg) = 0;

		// This routine shows this window.
		// Parameters:
		//		phwnd:	The handle to the parent window.
		//		win:	Which window to show (see WINDOW_CODE, above).
		//		m:		The mode to display (see MODE_CODE, above).
		//		x:		The initial x position in the parent.
		//		y:		The initial y position in the parent.
		//		w:		If not 0, the initial width.
		//		h:		If not 0, the initial height.
		//		z:		If not NULL, the window to place this
		//				after in the z-order.
		// Returns:
		//		A handle to the new window
		virtual HWND show(HWND phwnd, int win, int m,
			int x = 0, int y = 0, int w = 0, int h = 0, HWND z = NULL) = 0;

	};	// class SelectionInterface


	// This routine gets all category/content/ID tuples
	// All returned arrays are newly-allocated.
	// All returned entries are constant pointers.
	virtual int allSelectionEntries(const RPCapiEx::IDex **&ids,
			const TString **&cats, const TString **&cons) = 0;

	// These routines get/set the location of the ACM that
	// this API will connect to.
	// NOTE:  The returned strings should be deallocated by the caller.
	// Parameters:
	//		acmType:	Type of the ACM.
	//					0 == unspecified.
	//					1 == locally started by client.
	//					2 == network.
	//					3 == none.
	//					4 == locally started by client if not already specified.
	//					5 == network if not already specified.
	//					6 == none if not already specified.
	//		acmExe:		If not NULL, exe of ACM to launch locally.
	//		acmDNS:		If not NULL, the network location of the ACM.
	//		acmPort:	If not 0, the port of the ACM.
	virtual void acmGet(int &acmType, char *acmExe,
		char *acmDNS, short &acmPort) = 0;
	virtual void acmSet(int acmType, TStringArg acmExe,
		TStringArg acmDNS, short acmPort) = 0;

	// Add a file to the global SelectionTable.
	// Parameters:
	//		rpcFile:	File to add.
	//		rpcId:		If not NULL, receives the RPC's ID.
	//		cat:		If not NULL, receives the RPC's category.
	//		con:		If not NULL, receives the RPC's content.
	// Returns true iff successful.
	virtual bool idAddFile(TStringArg rpcFile,
		RPCapiEx::IDex **rpcId, TString **cat, TString **con) = 0;

	// This routine retrieves all IDs currently in the
	// global SelectionTable.
	// It returns a newly-allocated array of newly-allocated IDs.
	virtual RPCapiEx::IDex **idAll(int &num) = 0;

	// Tell the RPCapi you want keywords w/ RPC IDs before
	// calling updateACM for the first time.
	#ifdef RPC_SEL_INT_KEYWORDS
	virtual void idKeywordSupport(bool s) = 0;
	virtual bool idKeywordSupport(void) = 0;
	virtual void idKeywords(const RPCapi::ID *id,
		TString **&idKeywords, int &num) = 0;
	#endif	// ifdef RPC_SEL_INT_KEYWORDS

	// This routine gets the filename matching
	// the input ID.  This routine uses the global SelectionTable
	// in RPCcls to accomplish this mapping.
	// The return is a newly-allocated TString,
	// or NULL if there is no match.
	virtual TString *idToFilename(const RPCapi::ID *id) = 0;

	// This routine gets the type of the file associated
	// with the input ID.  File type means RPC, SKP, etc.
	// This routine uses the global SelectionTable in RPCcls
	// to accomplish this mapping.
	// File types should be integers whose low-order bytes
	// match the upper case letters in the file type extension.
	// Example:  RPC files have fileType == 0x00525043
	// Example:  SKP files have fileType == 0x00534b50
	virtual int idToFiletype(const RPCapi::ID *id) = 0;

	// This routine gets the category and content for the input ID.
	// This routine uses the global SelectionTable
	// in RPCcls to accomplish this mapping.
	virtual void idToCatCon(const RPCapi::ID *id,
		TString *&cat, TString *&con) = 0;

	// This routine extracts the preview for the indicated RPC.
	// The preview is in RGB format, and normally 124x98 (36456 bytes).
	// Parameters:
	//		id:		The RPC's id.
	//		buffer:	buffer in which the image data is placed.
	//				This buffer may be allocated inside this
	//				function, and a pointer to it is returned by
	//				reference.
	//		alloc:	if true, this routine allocates buffer.
	//				if false, this routine fills in the buffer passed
	//				to it, or returns the size needed if that buffer
	//				is NULL.
	//		width:	Width, returned by reference.
	//		height:	Height, returned by reference.
	// Returns:
	//		The number of bytes used or required to store the requested,
	//		image, or < 0 on an error.
	virtual int idToPreview(const RPCapi::ID *id,
		unsigned char *&buffer, bool alloc, int &width, int &height) = 0;

};	// class RPCapiSI

#endif	// ifdef RPC_SEL_INT
#endif	// ifndef _RPC_API_SI_H_
