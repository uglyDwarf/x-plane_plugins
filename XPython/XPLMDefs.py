###############################################################################
# Based on CHeaders/XPLM/XPLMDefs.h
###############################################################################


###############################################################################
# GLOBAL DEFINITIONS
###############################################################################
# These definitions are used in all parts of the SDK.


###############################################################################
# XPLMPluginID
#
# Each plug-in is identified by a unique integer ID.  This ID can be used to
# disable or enable a plug-in, or discover what plug-in is 'running' at the
# time.  A plug-in ID is unique within the currently running instance of
# X-Plane unless plug-ins are reloaded.  Plug-ins may receive a different
# unique ID each time they are loaded.
#
# For persistent identification of plug-ins, use XPLMFindPluginBySignature in
# XPLMUtiltiies.h
#
# -1 indicates no plug-in.

# No plugin
XPLM_NO_PLUGIN_ID = -1

# X-Plane itself
XPLM_PLUGIN_XPLANE = 0

# The current XPLM revision is 2.10 (210)
kXPLM_Version = 210

###############################################################################
# XPLMKeyFlags
#
# These bitfields define modifier keys in a platform independent way. When a
# key is pressed, a series of messages are sent to your plugin.  The down
# flag is set in the first of these messages, and the up flag in the last.
# While the key is held down, messages are sent with neither to indicate that
# the key is being held down as a repeated character.
#
# The control flag is mapped to the control flag on Macintosh and PC.
# Generally X-Plane uses the control key and not the command key on
# Macintosh, providing a consistent interface across platforms that does not
# necessarily match the Macintosh user interface guidelines.  There is not
# yet a way for plugins to access the Macintosh control keys without using
# #ifdefed code.

# The shift key is down
xplm_ShiftFlag     = 1

# The option or alt key is down
xplm_OptionAltFlag = 2

# The control key is down
xplm_ControlFlag   = 4

# The key is being pressed down
xplm_DownFlag      = 8

# The key is being released
xplm_UpFlag        = 16


###############################################################################
# ASCII CONTROL KEY CODES
###############################################################################
# These definitions define how various control keys are mapped to ASCII key
# codes. Not all key presses generate an ASCII value, so plugin code should
# be prepared to see null characters come from the keyboard...this usually
# represents a key stroke that has no equivalent ASCII, like a page-down
# press.  Use virtual key codes to find these key strokes. ASCII key codes
# take into account modifier keys; shift keys will affect capitals and
# punctuation; control key combinations may have no vaild ASCII and produce
# NULL.  To detect control-key combinations, use virtual key codes, not ASCII
# keys.

XPLM_KEY_RETURN      = 13

XPLM_KEY_ESCAPE      = 27

XPLM_KEY_TAB         = 9

XPLM_KEY_DELETE      = 8

XPLM_KEY_LEFT        = 28

XPLM_KEY_RIGHT       = 29

XPLM_KEY_UP          = 30

XPLM_KEY_DOWN        = 31

XPLM_KEY_0           = 48

XPLM_KEY_1           = 49

XPLM_KEY_2           = 50

XPLM_KEY_3           = 51

XPLM_KEY_4           = 52

XPLM_KEY_5           = 53

XPLM_KEY_6           = 54

XPLM_KEY_7           = 55

XPLM_KEY_8           = 56

XPLM_KEY_9           = 57

XPLM_KEY_DECIMAL     = 46


###############################################################################
# VIRTUAL KEY CODES
###############################################################################
# These are cross-platform defines for every distinct keyboard press on the
# computer. Every physical key on the keyboard has a virtual key code.  So
# the "two" key on the  top row of the main keyboard has a different code
# from the "two" key on the numeric key pad.  But the 'w' and 'W' character
# are indistinguishable by virtual key code  because they are the same
# physical key (one with and one without the shift key).
#
# Use virtual key codes to detect keystrokes that do not have ASCII
# equivalents, allow the user to map the numeric keypad separately from the
# main keyboard, and detect control key and other modifier-key combinations
# that generate ASCII control key sequences (many of which are not available
# directly via character keys in the SDK).
#
# To assign virtual key codes we started with the Microsoft set but made some
# additions and changes.  A few differences:
#
# 1. Modifier keys are not available as virtual key codes.  You cannot get
# distinct modifier press and release messages.  Please do not try to use
# modifier keys as regular keys; doing so will almost certainly interfere
# with users' abilities to use the native x-plane key bindings.
#
# 2. Some keys that do not exist on both Mac and PC keyboards are removed.
#
# 3. Do not assume that the values of these keystrokes are interchangeable
# with MS v-keys.

XPLM_VK_BACK         = 0x08

XPLM_VK_TAB          = 0x09

XPLM_VK_CLEAR        = 0x0C

XPLM_VK_RETURN       = 0x0D

XPLM_VK_ESCAPE       = 0x1B

XPLM_VK_SPACE        = 0x20

XPLM_VK_PRIOR        = 0x21

XPLM_VK_NEXT         = 0x22

XPLM_VK_END          = 0x23

XPLM_VK_HOME         = 0x24

XPLM_VK_LEFT         = 0x25

XPLM_VK_UP           = 0x26

XPLM_VK_RIGHT        = 0x27

XPLM_VK_DOWN         = 0x28

XPLM_VK_SELECT       = 0x29

XPLM_VK_PRINT        = 0x2A

XPLM_VK_EXECUTE      = 0x2B

XPLM_VK_SNAPSHOT     = 0x2C

XPLM_VK_INSERT       = 0x2D

XPLM_VK_DELETE       = 0x2E

XPLM_VK_HELP         = 0x2F

# XPLM_VK_0 thru XPLM_VK_9 are the same as ASCII '0' thru '9' (= 0x30 - 0x39)
XPLM_VK_0            = 0x30

XPLM_VK_1            = 0x31

XPLM_VK_2            = 0x32

XPLM_VK_3            = 0x33

XPLM_VK_4            = 0x34

XPLM_VK_5            = 0x35

XPLM_VK_6            = 0x36

XPLM_VK_7            = 0x37

XPLM_VK_8            = 0x38

XPLM_VK_9            = 0x39

# XPLM_VK_A thru XPLM_VK_Z are the same as ASCII 'A' thru 'Z' (= 0x41 - 0x5A)
XPLM_VK_A            = 0x41

XPLM_VK_B            = 0x42

XPLM_VK_C            = 0x43

XPLM_VK_D            = 0x44

XPLM_VK_E            = 0x45

XPLM_VK_F            = 0x46

XPLM_VK_G            = 0x47

XPLM_VK_H            = 0x48

XPLM_VK_I            = 0x49

XPLM_VK_J            = 0x4A

XPLM_VK_K            = 0x4B

XPLM_VK_L            = 0x4C

XPLM_VK_M            = 0x4D

XPLM_VK_N            = 0x4E

XPLM_VK_O            = 0x4F

XPLM_VK_P            = 0x50

XPLM_VK_Q            = 0x51

XPLM_VK_R            = 0x52

XPLM_VK_S            = 0x53

XPLM_VK_T            = 0x54

XPLM_VK_U            = 0x55

XPLM_VK_V            = 0x56

XPLM_VK_W            = 0x57

XPLM_VK_X            = 0x58

XPLM_VK_Y            = 0x59

XPLM_VK_Z            = 0x5A

XPLM_VK_NUMPAD0      = 0x60

XPLM_VK_NUMPAD1      = 0x61

XPLM_VK_NUMPAD2      = 0x62

XPLM_VK_NUMPAD3      = 0x63

XPLM_VK_NUMPAD4      = 0x64

XPLM_VK_NUMPAD5      = 0x65

XPLM_VK_NUMPAD6      = 0x66

XPLM_VK_NUMPAD7      = 0x67

XPLM_VK_NUMPAD8      = 0x68

XPLM_VK_NUMPAD9      = 0x69

XPLM_VK_MULTIPLY     = 0x6A

XPLM_VK_ADD          = 0x6B

XPLM_VK_SEPARATOR    = 0x6C

XPLM_VK_SUBTRACT     = 0x6D

XPLM_VK_DECIMAL      = 0x6E

XPLM_VK_DIVIDE       = 0x6F

XPLM_VK_F1           = 0x70

XPLM_VK_F2           = 0x71

XPLM_VK_F3           = 0x72

XPLM_VK_F4           = 0x73

XPLM_VK_F5           = 0x74

XPLM_VK_F6           = 0x75

XPLM_VK_F7           = 0x76

XPLM_VK_F8           = 0x77

XPLM_VK_F9           = 0x78

XPLM_VK_F10          = 0x79

XPLM_VK_F11          = 0x7A

XPLM_VK_F12          = 0x7B

XPLM_VK_F13          = 0x7C

XPLM_VK_F14          = 0x7D

XPLM_VK_F15          = 0x7E

XPLM_VK_F16          = 0x7F

XPLM_VK_F17          = 0x80

XPLM_VK_F18          = 0x81

XPLM_VK_F19          = 0x82

XPLM_VK_F20          = 0x83

XPLM_VK_F21          = 0x84

XPLM_VK_F22          = 0x85

XPLM_VK_F23          = 0x86

XPLM_VK_F24          = 0x87

# The following definitions are extended and are not based on the Microsoft
# key set.
XPLM_VK_EQUAL        = 0xB0

XPLM_VK_MINUS        = 0xB1

XPLM_VK_RBRACE       = 0xB2

XPLM_VK_LBRACE       = 0xB3

XPLM_VK_QUOTE        = 0xB4

XPLM_VK_SEMICOLON    = 0xB5

XPLM_VK_BACKSLASH    = 0xB6

XPLM_VK_COMMA        = 0xB7

XPLM_VK_SLASH        = 0xB8

XPLM_VK_PERIOD       = 0xB9

XPLM_VK_BACKQUOTE    = 0xBA

XPLM_VK_ENTER        = 0xBB

XPLM_VK_NUMPAD_ENT   = 0xBC

XPLM_VK_NUMPAD_EQ    = 0xBD

