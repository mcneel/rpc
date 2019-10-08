#ifndef _RPC_CONTROL_H_
#define _RPC_CONTROL_H_

#include "RPCapi.h"


class RPCcon {
public:
	virtual ~RPCcon(void) {}

class BaseControl;

// Implement this Callback so that you can be notified of control
// value changes while you are displaying a control in a UI.
// Note that some controls may cause other controls' values to change
// (and this callback is how you'll be notified of those changes).
class Callback {
public:
	virtual ~Callback(void) {}

	// This routine is used by a control to notify registered
	// callbacks that it has changed.
	// Parameters:
	//		con:	The control making the callback.
	//		action:	The action that prompted the callback.
	typedef enum ACTION {
		VALUE_CHANGE = 1,
		ENABLED_CHANGE = 2,
		CAPTION_CHANGE = 3,
		INSTANCE_ID_PRE_CHANGE = 4,
		INSTANCE_ID_POST_CHANGE = 5
	} ACTION_T;	// typedef enum ACTION
	virtual void RPCconCallback(BaseControl *con, int action) = 0;
};	// class Callback


// dynamic_cast an RPCapi::Instance to this class in order to get
// that Instance's controls.
class Instance {
public:
	virtual ~Instance(void) {}

	// This routine gets this Instance's controls.
	// Delete the returned array, but not its contents.
	virtual BaseControl **RPCconControlsGet(int &num) = 0;

	// This routine gets a pointer to a single control in this Instance.
	// Parameters:
	//		name:	The name of the control to get.
	virtual BaseControl *RPCconControlGet(TStringArg name) = 0;

	// (Un)register a callback w/ this Control.
	// Instance will send back an INSTANCE_ID_PRE/POST_CHANGE
	// when the RPC ID of the Instance changes (like in switch content).
	// When this happens, all of the current RPCcon::BaseControls
	// will be deleted, and new ones will populate the instance.
	virtual void rpcConCallbackRegister(Callback *cb, bool reg) = 0;

};	// class Instance


class BaseControl {
public:
	virtual ~BaseControl(void) {}

	// Gets the caption of this control.
	virtual const TString &captionGet(void) const = 0;

	// Get the type of this control.
	// See TYPE_CODE in control classes.
	virtual int controlTypeGet(void) const = 0;

	// Gets the enabled state of this control.
	virtual bool enabledGet(void) const = 0;

	// Get / set a handle / identifier for this control.
	virtual int handIdGet(void) const = 0;
	virtual void handIdSet(int i) = 0;

	// Get the name of this control.
	// This corresponds to the names used w/ Instance::getTransform().
	virtual const TString &nameGet(void) const = 0;

	// Get the visible state of this control.
	virtual bool visibleGet(void) const = 0;

    // Reset control to the default value
    virtual void reset(){}

	// (Un)register a callback w/ this Control.
	virtual void rpcConCallbackRegister(Callback *cb, bool reg) = 0;
};	// class BaseControl


// A button control is just a push button.
class Button : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 36
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~Button(void) {}

	// Call this routine when the button is pressed.
	virtual void buttonPush(void) = 0;
};	// class Button


class CheckBox : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 37
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~CheckBox(void) {}

	// These routines get/set the value of this control.
	virtual int valueGet(void) const = 0;
	virtual void valueSet(int v) = 0;
};	// class CheckBox


// A ComboBox contains different single-selectable text values.
// We recommend a drop-down size of 81.
class ComboBox : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 35
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~ComboBox(void) {}

	// Get the value to put in the combo's drop down.
	virtual TString **listItemsGet(int &num) const = 0;

	// These routines get/set the value of this control.
	virtual const TString &valueGet(void) const = 0;
	virtual void valueSet(TStringArg v) = 0;
	virtual int indexGet(void) = 0;
	virtual void indexSet(int i) = 0;
};	// class ComboBox


// An integer control is a text edit box that displays
// a number (possibly w/ units) followed by a spinner control.
// The spinner's min/max range should be set by
// this object's valueMinGet and valueMaxGet routine results.
// We recommend displaying a static text control to the right
// of this control containing its caption.
class EditInteger : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 39
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~EditInteger(void) {}

	// This routine gets the unit type of this control.
	// See RPCapi::Units for possible unit types.
	// (A control w/ units should display a meaningful
	// unit suffix after its scaled numeric value,
	// ie:  "1m" vs "100cm" vs "1000mm").
	virtual int unitTypeGet(void) const = 0;

	// These routines get/set the value of this control.
	virtual int valueGet(void) const = 0;
	virtual void valueSet(int v) = 0;

	// These routines get the min/max values for this control.
	virtual int valueMaxGet(void) const = 0;
	virtual int valueMinGet(void) const = 0;
};	// class EditInteger


// An real control is a text edit box that displays
// a real number (possibly w/ units) followed by a spinner control.
// The spinner's min/max range should be set by
// this object's valueMinGet and valueMaxGet routine results.
// We recommend displaying a static text control to the right
// of this control containing its caption.
class EditReal : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 40
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~EditReal(void) {}

	// This routine gets the unit type of this control.
	// See RPCapi::Units for possible unit types.
	// (A control w/ units should display a meaningful
	// unit suffix after its scaled numeric value,
	// ie:  "1m" vs "100cm" vs "1000mm").
	virtual int unitTypeGet(void) const = 0;

	// These routines get/set the value of this control.
	virtual double valueGet(void) const = 0;
	virtual void valueSet(double v) = 0;

	// These routines get the min/max values for this control.
	virtual double valueMaxGet(void) const = 0;
	virtual double valueMinGet(void) const = 0;
};	// class EditReal


// This is a box in which text can be editted.
// Note that some EditText controls may be read-only.
// For these, "enabledGet" will return false.
class EditText : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 38
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~EditText(void) {}

	// These routines get / set the value of this control.
	virtual const TString &valueGet(void) const = 0;
	virtual void valueSet(TStringArg v) = 0;
};	// class EditText


// A control to choose a file with.
class FileChooser : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 49
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~FileChooser(void) {}

	// This routine gets the filters used in this chooser.
	// NOTE:  The caller must delete the returned arrays
	//        and their contents.
	// Parameters:
	//		num:	The number of filters.
	//		names:	The names.
	//		suffs:	The suffixes.  One entry per suffix.  An entry
	//				contains 1/more suffixes, separated by semicolons.
	//				Example:
	//				names = "Images"
	//				suffs = "bmp;jpg"
	virtual void filtersGet(int &num, TString **&names, TString **&suffs) = 0;

	// Call this routine to launch an RPCapi provided dialog
	// to select a file with.
	virtual void launchDlg(void) = 0;

	// These routines get / set the value of this control.
	virtual const TString &valueGet(void) const = 0;
	virtual void valueSet(TStringArg v) = 0;
};	// class FileChooser


// An Image control contains a single image.  This image may change
// while the image control is being displayed.  Handle VALUE_CHANGED
// and use hbitmapGet to display this change.
class Image : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 54
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~Image(void) {}

	// Get the recommended width / height for this control.
	// If you have a desired width,
	// scale height proportionatley for best results:
	// height = (int)ceil(((double)height / (double)width) * desired_width);
	virtual void dimensionsGet(int &width, int &height) const = 0;

	// Get the image to display.
	// Be sure to clean up this bitmap after you are done using
	// it w/ DeleteObject().
	// Parameters:
	//		hwndDlg:  If not NULL, this dialog is used to scale
	//		          the resulting image according to its dialog units.
	//		iwidth:	  If hwndDlg is NULL, and this is positive,
	//		          then this is the desired image width.
	//		iheight:  If hwndDlg is NULL, and this is positive,
	//		          then this is the desired image height.
	virtual HBITMAP hbitmapGet(HWND hwndDlg,
		int iwidth = -1, int iheight = -1) = 0;

	// Get the image to display.  It is in RGB format.
	// Parameters:
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
	//		absSize:	If true, the width/height represent the absolute
	//					size (accept no substitutes).  If false, the
	//					returned image may have lower res than that
	//					specified.
	// Returns:
	//		The number of bytes used or required to store the requested,
	//		image, or < 0 on an error.
	virtual int imageGet(unsigned char *&buffer, bool alloc,
		int &width, int &height, bool absSize = false) = 0;

	// These routines get / set the path to the image.
	// This path can be either a file, or a path
	// internal to the RPC (denoted w/ a prefix of "__RPC:".
	virtual const TString &valueGet(void) const = 0;
	virtual void valueSet(TStringArg v) = 0;

};	// class Image


// A ListBox contains different single-selectable text values.
// It is identical to a ComboBox, except in desired appearance
// (ie, a combo-box uses the drop-down list for selection).
class ListBox : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 34
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~ListBox(void) {}

	// Get the value to put in the combo's drop down.
	virtual TString **listItemsGet(int &num) const = 0;

	// These routines get/set the value of this control.
	virtual const TString &valueGet(void) const = 0;
	virtual void valueSet(TStringArg v) = 0;
	virtual int indexGet(void) = 0;
	virtual void indexSet(int i) = 0;
};	// class ListBox


// A rollout control contains other controls in a single
// control that can optionally be opened / closed.
// This can also be implemented as a static frame.
class Rollout : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 42
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~Rollout(void) {}

	// This routine gets the child controls of this rollout.
	// Delete the returned array, but not its contents.
	virtual BaseControl **controlsGet(int &num) = 0;
};	// class Rollout


// An slider control is a slider that selects amonger integer values.
// We recommend displaying a static text control above
// this control containing its caption.
class Slider : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 41
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~Slider(void) {}

	// These routines get/set the value of this control.
	virtual int valueGet(void) const = 0;
	virtual void valueSet(int v) = 0;

	// These routines get the min/max values for this control.
	virtual int valueMaxGet(void) const = 0;
	virtual int valueMinGet(void) const = 0;
};	// class Slider


// This is just a piece of static text (a label).
// The caption is the text to be displayed; this control has no "value".
class StaticText : public BaseControl {
public:
	typedef enum TYPE_CODE {
		TYPE_CODE = 33
	} TYPE_CODES_T;	// typedef enum TYPE_CODES
	virtual ~StaticText(void) {}
};	// class StaticText


// A control to select a color.
class ColorPicker : public BaseControl
{
public:
    typedef enum TYPE_CODE {
        TYPE_CODE = 92
    } TYPE_CODES_T;

    // Call this routine to launch an RPCapi provided dialog to select a color.
    virtual void launchDlg() = 0;

    // These routines get / set the value of this control.
    virtual const RPCapi::Color& valueGet() const = 0;
    virtual void valueSet(const RPCapi::Color& value) = 0;
};

class ImageSlider : public BaseControl
{
public:
    typedef enum TYPE_CODE {
        TYPE_CODE = 93
    } TYPE_CODES_T;

    // Get number of images in the collection
    virtual int count() const = 0;

    // These routines switch to the previous / next image in the collection
    virtual void previous() = 0;
    virtual void next() = 0;

    // These routines get / set the index of selected image
    virtual int  getIndex() const = 0;
    virtual void setIndex(int index) = 0;

    // Get the recommended width / height for the image control
    virtual void getImageDimensions(int& width, int& height) const = 0;

    // Get the image to display.
    // Parameters:
    //      hwndDlg:  If not NULL, this dialog is used to scale
    //                the resulting image according to its dialog units.
    //      width:    If hwndDlg is NULL, and value is positive,
    //                then this is the desired image width.
    //      height:   If hwndDlg is NULL, and value is positive,
    //                then this is the desired image height.
    virtual HBITMAP getBitmap(HWND hwndDlg, int width = -1, int height = -1) = 0;

    // Get the path to the image.
    // Path can be a filename, or a reference to internal RPC image
    // (path should start with "__RPC:" prefix).
    virtual const TString& getImagePath() const = 0;
};

};	// class RPCcon
#endif	// ifndef _RPC_CONTROL_H_
