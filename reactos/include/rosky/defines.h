#ifndef __RSK_DEFINES_H
#define __RSK_DEFINES_H

/* Messages */
#define MSG_MOUSE_BUT1_PRESSED		162
#define MSG_MOUSE_BUT2_PRESSED		163
#define MSG_MOUSE_BUT1_RELEASED		164
#define MSG_MOUSE_BUT2_RELEASED		165
#define MSG_GUI_REDRAW			170
#define MSG_COMMAND			2505
#define MSG_QUIT			2600
#define MSG_DESTROY			2700

/* Window Styles */
#define WF_DONT_EREASE_BACKGROUND	0x00000010
#define WF_NO_FRAME			0x00000020
#define WF_MODAL			0x00000100
#define WF_HAS_MENU			0x00001000
#define WF_HAS_STATUSBAR		0x00002000
#define WF_NO_INITIAL_DRAW		0x00008000
#define WF_FOCUSABLE			0x00010000
#define WF_NOT_SIZEABLE			0x00040000
#define WF_SMALL_TITLE			0x00080000
#define WF_NOT_MOVEABLE			0x00100000
#define WF_NO_TITLE			0x00200000
#define WF_POPUP			0x00400000
#define WF_NO_BUTTONS			0x00800000
#define WF_FREEFORM			0x08000000
#define WF_USE_BACKGROUND		0x20000000
#define WF_USER				0x20000000
#define WF_HIDE				0x40000000
#define WF_DESKTOP			0x80000000
#define WF_TRANSPARENT			0x01000000

/* GC types */
#define GC_TYPE_WINDOW			0x00000002
#define GC_TYPE_DIB			0x00000004

/* Menu flags */
#define MENU_SEPERATOR			0x00000001

/* Message box flags */
#define WGF_MB_YESNO			0x00000001
#define WGF_MB_OK			0x00000010
#define WGF_MB_CANCEL			0x00000100
#define WGF_MB_ICON_INFO		0x00100000
#define WGF_MB_ICON_ASK			0x01000000
#define WGF_MB_ICON_STOP		0x10000000

/* Message box return values */
#define ID_OK				1
#define ID_YES				2
#define ID_NO				3
#define ID_CANCEL			4

#endif /* __RSK_DEFINES_H */
