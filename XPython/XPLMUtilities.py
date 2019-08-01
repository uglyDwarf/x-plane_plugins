"""
X-PLANE USER INTERACTION
************************
The user interaction APIs let you simulate commands the user can do with a
joystick, keyboard etc. Note that it is generally safer for future
compatibility to use one of these commands than to manipulate the
underlying sim data.

xplm_key_* enums represent all the keystrokes available within X-Plane. They can
be sent to X-Plane directly. For example, you can reverse thrust using
these enumerations.

xplm_joy_*  are enumerations for all of the things you can do with a joystick
button in X-Plane. They currently match the buttons menu in the equipment
setup dialog, but these enums will be stable even if they change in
X-Plane.

The plug-in system is based on Austin's cross-platform OpenGL framework and
could theoretically be adapted to run in other apps like WorldMaker. The
plug-in system also runs against a test harness for internal development
and could be adapted to another flight sim (in theory at least). So the xplm_Host_*
enum is provided to allow plug-ins to indentify what app they are running
under.

xplm_Language_* enums define what language the sim is running in. These enumerations
do not imply that the sim can or does run in all of these languages; they
simply provide a known encoding in the event that a given sim version is
localized to a certain language.

xplm_DataFile_* enums define types of data files you can load or unload using the
SDK.


X-PLANE COMMAND MANAGEMENT
**************************
The command management APIs let plugins interact with the command-system in
X-Plane, the abstraction behind keyboard presses and joystick buttons. This
API lets you create new commands and modify the behavior (or get
notification) of existing ones.

An X-Plane command consists of three phases: a beginning, continuous
repetition, and an ending. The command may be repeated zero times in the
event that the user presses a button only momentarily.

A command ref is an opaque identifier for an X-Plane command. Command
references stay the same for the life of your plugin but not between
executions of X-Plane. Command refs are used to execute commands, create
commands, and create callbacks for particular commands.

Note that a command is not "owned" by a particular plugin. Since many
plugins may participate in a command's execution, the command does not go
away if the plugin that created it is unloaded.

"""

# XPLMCommandKeyID
xplm_key_pause      =  0
xplm_key_revthrust  =  1
xplm_key_jettison   =  2
xplm_key_brakesreg  =  3
xplm_key_brakesmax  =  4
xplm_key_gear       =  5
xplm_key_timedn     =  6
xplm_key_timeup     =  7
xplm_key_fadec      =  8
xplm_key_otto_dis   =  9
xplm_key_otto_atr   = 10
xplm_key_otto_asi   = 11
xplm_key_otto_hdg   = 12
xplm_key_otto_gps   = 13
xplm_key_otto_lev   = 14
xplm_key_otto_hnav  = 15
xplm_key_otto_alt   = 16
xplm_key_otto_vvi   = 17
xplm_key_otto_vnav  = 18
xplm_key_otto_nav1  = 19
xplm_key_otto_nav2  = 20
xplm_key_targ_dn    = 21
xplm_key_targ_up    = 22
xplm_key_hdgdn      = 23
xplm_key_hdgup      = 24
xplm_key_barodn     = 25
xplm_key_baroup     = 26
xplm_key_obs1dn     = 27
xplm_key_obs1up     = 28
xplm_key_obs2dn     = 29
xplm_key_obs2up     = 30
xplm_key_com1_1     = 31
xplm_key_com1_2     = 32
xplm_key_com1_3     = 33
xplm_key_com1_4     = 34
xplm_key_nav1_1     = 35
xplm_key_nav1_2     = 36
xplm_key_nav1_3     = 37
xplm_key_nav1_4     = 38
xplm_key_com2_1     = 39
xplm_key_com2_2     = 40
xplm_key_com2_3     = 41
xplm_key_com2_4     = 42
xplm_key_nav2_1     = 43
xplm_key_nav2_2     = 44
xplm_key_nav2_3     = 45
xplm_key_nav2_4     = 46
xplm_key_adf_1      = 47
xplm_key_adf_2      = 48
xplm_key_adf_3      = 49
xplm_key_adf_4      = 50
xplm_key_adf_5      = 51
xplm_key_adf_6      = 52
xplm_key_transpon_1 = 53
xplm_key_transpon_2 = 54
xplm_key_transpon_3 = 55
xplm_key_transpon_4 = 56
xplm_key_transpon_5 = 57
xplm_key_transpon_6 = 58
xplm_key_transpon_7 = 59
xplm_key_transpon_8 = 60
xplm_key_flapsup    = 61
xplm_key_flapsdn    = 62
xplm_key_cheatoff   = 63
xplm_key_cheaton    = 64
xplm_key_sbrkoff    = 65
xplm_key_sbrkon     = 66
xplm_key_ailtrimL   = 67
xplm_key_ailtrimR   = 68
xplm_key_rudtrimL   = 69
xplm_key_rudtrimR   = 70
xplm_key_elvtrimD   = 71
xplm_key_elvtrimU   = 72
xplm_key_forward    = 73
xplm_key_down       = 74
xplm_key_left       = 75
xplm_key_right      = 76
xplm_key_back       = 77
xplm_key_tower      = 78
xplm_key_runway     = 79
xplm_key_chase      = 80
xplm_key_free1      = 81
xplm_key_free2      = 82
xplm_key_spot       = 83
xplm_key_fullscrn1  = 84
xplm_key_fullscrn2  = 85
xplm_key_tanspan    = 86
xplm_key_smoke      = 87
xplm_key_map        = 88
xplm_key_zoomin     = 89
xplm_key_zoomout    = 90
xplm_key_cycledump  = 91
xplm_key_replay     = 92
xplm_key_tranID     = 93
xplm_key_max        = 94


# XPLMCommandButtonID
xplm_joy_nothing    = 0
xplm_joy_start_all  = 1
xplm_joy_start_0    = 2
xplm_joy_start_1    = 3
xplm_joy_start_2    = 4
xplm_joy_start_3    = 5
xplm_joy_start_4    = 6
xplm_joy_start_5    = 7
xplm_joy_start_6    = 8
xplm_joy_start_7    = 9
xplm_joy_throt_up   = 10
xplm_joy_throt_dn   = 11
xplm_joy_prop_up    = 12
xplm_joy_prop_dn    = 13
xplm_joy_mixt_up    = 14
xplm_joy_mixt_dn    = 15
xplm_joy_carb_tog   = 16
xplm_joy_carb_on    = 17
xplm_joy_carb_off   = 18
xplm_joy_trev       = 19
xplm_joy_trm_up     = 20
xplm_joy_trm_dn     = 21
xplm_joy_rot_trm_up = 22
xplm_joy_rot_trm_dn = 23
xplm_joy_rud_lft    = 24
xplm_joy_rud_cntr   = 25
xplm_joy_rud_rgt    = 26
xplm_joy_ail_lft    = 27
xplm_joy_ail_cntr   = 28
xplm_joy_ail_rgt    = 29
xplm_joy_B_rud_lft  = 30
xplm_joy_B_rud_rgt  = 31
xplm_joy_look_up    = 32
xplm_joy_look_dn    = 33
xplm_joy_look_lft   = 34
xplm_joy_look_rgt   = 35
xplm_joy_glance_l   = 36
xplm_joy_glance_r   = 37
xplm_joy_v_fnh      = 38
xplm_joy_v_fwh      = 39
xplm_joy_v_tra      = 40
xplm_joy_v_twr      = 41
xplm_joy_v_run      = 42
xplm_joy_v_cha      = 43
xplm_joy_v_fr1      = 44
xplm_joy_v_fr2      = 45
xplm_joy_v_spo      = 46
xplm_joy_flapsup    = 47
xplm_joy_flapsdn    = 48
xplm_joy_vctswpfwd  = 49
xplm_joy_vctswpaft  = 50
xplm_joy_gear_tog   = 51
xplm_joy_gear_up    = 52
xplm_joy_gear_down  = 53
xplm_joy_lft_brake  = 54
xplm_joy_rgt_brake  = 55
xplm_joy_brakesREG  = 56
xplm_joy_brakesMAX  = 57
xplm_joy_speedbrake = 58
xplm_joy_ott_dis    = 59
xplm_joy_ott_atr    = 60
xplm_joy_ott_asi    = 61
xplm_joy_ott_hdg    = 62
xplm_joy_ott_alt    = 63
xplm_joy_ott_vvi    = 64
xplm_joy_tim_start  = 65
xplm_joy_tim_reset  = 66
xplm_joy_ecam_up    = 67
xplm_joy_ecam_dn    = 68
xplm_joy_fadec      = 69
xplm_joy_yaw_damp   = 70
xplm_joy_art_stab   = 71
xplm_joy_chute      = 72
xplm_joy_JATO       = 73
xplm_joy_arrest     = 74
xplm_joy_jettison   = 75
xplm_joy_fuel_dump  = 76
xplm_joy_puffsmoke  = 77
xplm_joy_prerotate  = 78
xplm_joy_UL_prerot  = 79
xplm_joy_UL_collec  = 80
xplm_joy_TOGA       = 81
xplm_joy_shutdown   = 82
xplm_joy_con_atc    = 83
xplm_joy_fail_now   = 84
xplm_joy_pause      = 85
xplm_joy_rock_up    = 86
xplm_joy_rock_dn    = 87
xplm_joy_rock_lft   = 88
xplm_joy_rock_rgt   = 89
xplm_joy_rock_for   = 90
xplm_joy_rock_aft   = 91
xplm_joy_idle_hilo  = 92
xplm_joy_lanlights  = 93
xplm_joy_max        = 94


# XPLMHostApplicationID
xplm_Host_Unknown    = 0
xplm_Host_XPlane     = 1
xplm_Host_PlaneMaker = 2
xplm_Host_WorldMaker = 3
xplm_Host_Briefer    = 4
xplm_Host_PartMaker  = 5
xplm_Host_YoungsMod  = 6
xplm_Host_XAuto      = 7


# XPLMLanguageCode
xplm_Language_Unknown  =  0
xplm_Language_English  =  1
xplm_Language_French   =  2
xplm_Language_German   =  3
xplm_Language_Italian  =  4
xplm_Language_Spanish  =  5
xplm_Language_Korean   =  6
xplm_Language_Russian  =  7
xplm_Language_Greek    =  8
xplm_Language_Japanese =  9
xplm_Language_Chinese  = 10


# XPLMDataFileType
xplm_DataFile_Situation   = 1
xplm_DataFile_ReplayMovie = 2



def XPLMError_f(self, inMessage):
    """
    An XPLM error callback is a function that you provide to receive debugging
    information from the plugin SDK. See XPLMSetErrorCallback for more
    information. NOTE: for the sake of debugging, your error callback will be
    called even if your plugin is not enabled, allowing you to receive debug
    info in your XPluginStart and XPluginStop callbacks. To avoid causing logic
    errors in the management code, do not call any other plugin routines from
    your error callback - it is only meant for logging!
    """
    pass

def XPLMSimulateKeyPress(inKeyType, inKey):
    """
    This function simulates a key being pressed for X-Plane. The keystroke goes
    directly to X-Plane; it is never sent to any plug-ins. However, since this
    is a raw key stroke it may be mapped by the keys file or enter text into a
    field.

    WARNING: This function will be deprecated; do not use it. Instead use
    XPLMCommandKeyStroke.
    """
    pass

def XPLMXPLMSpeakString(inString):
    """
    This function displays the string in a translucent overlay over the current
    display and also speaks the string if text-to-speech is enabled. The string
    is spoken asynchronously, this function returns immediately.
    """
    pass

def XPLMCommandKeyStroke(inKey):
    """
    This routine simulates a command-key stroke. However, the keys are done by
    function, not by actual letter, so this function works even if the user has
    remapped their keyboard. Examples of things you might do with this include
    pausing the simulator.
    """
    pass

def XPLMCommandButtonPress(inButton):
    """
    This function simulates any of the actions that might be taken by pressing
    a joystick button. However, this lets you call the command directly rather
    than have to know which button is mapped where. Important: you must release
    each button you press. The APIs are separate so that you can 'hold down' a
    button for a fixed amount of time.
    """
    pass

def XPLMCommandButtonRelease(inButton):
    """
    This function simulates any of the actions that might be taken by pressing
    a joystick button. See XPLMCommandButtonPress.
    """
    pass

def XPLMGetVirtualKeyDescription(inVirtualKey):
    """
    Given a virtual key code (as defined in XPLMDefs.h) this routine returns a
    human-readable string describing the character. This routine is provided
    for showing users what keyboard mappings they have set up. The string may
    read 'unknown' or be a blank or None string if the virtual key is unknown.
    """
    pass

def XPLMReloadScenery():
    """
    XPLMReloadScenery reloads the current set of scenery. You can use this
    function in two typical ways: simply call it to reload the scenery, picking
    up any new installed scenery, .env files, etc. from disk. Or, change the
    lat/ref and lon/ref data refs and then call this function to shift the
    scenery environment.
    """
    pass

def XPLMGetSystemPath():
    """
    This function returns the full path to the X-System folder. Note that this
    is a directory path, so it ends in a trailing : or /.
    """
    pass

def XPLMGetPrefsPath():
    """
    This routine returns a full path to a file that is within X-Plane's
    preferences directory. (You should remove the file name back to the last
    directory separator to get the preferences directory.
    """
    pass

def XPLMGetDirectorySeparator():
    """
    This routine returns a string with one char and a null terminator that is
    the directory separator for the current platform. This allows you to write
    code that concatinates directory paths without having to #ifdef for

    platform.
    """
    pass

def XPLMExtractFileAndPath(inFullPath):
    """
    Given a full path to a file, this routine separates the path from the file.
    If the path is a partial directory (e.g. ends in : or \) the trailing
    directory separator is removed. This routine works in-place; a pointer to
    the file part of the buffer is returned; the original buffer still starts
    with the path.
    """
    pass

def XPLMGetDirectoryContents(inDirectoryPath, inFirstReturn, inFileNameBufSize, inIndexCount):
    """
    This routine returns a list of files in a directory (specified by a full
    path, no trailing : or \). The output is returned as a list of strings.
    terminated strings. An index array (if specified) is filled with pointers
    into the strings. This routine will return 1 if you had
    capacity for all files or 0 if you did not. You can also skip a given
    number of files.

    inDirectoryPath - a string containing the full path to
    the directory with no trailing directory char.

    inFirstReturn - the zero-based index of the first file in the directory to
    return. (Usually zero to fetch all in one pass.)

    inFileNameBufSize - the size of the file name buffer in bytes.

    inIndexCount - the max size of the index in entries.

    Returns a tuple:
      res = 1 if all info could be returned, 0 if there was a buffer overrun.
      namesList = list of files in the directory.
      outTotalFiles =  number of files in the directory.

    WARNING: Before X-Plane 7 this routine did not properly iterate through
    directories. If X-Plane 6 compatibility is needed, use your own code to
    iterate directories.
    """
    pass

def XPLMInitialized():
    """
    This function returns 1 if X-Plane has properly initialized the plug-in
    system. If this routine returns 0, many XPLM functions will not work.

    NOTE: Under normal circumstances a plug-in should never be running while
    the plug-in manager is not initialized.

    WARNING: This function is generally not needed and may be deprecated in the
    future.
    """
    pass

def XPLMGetVersions():
    """
    This routine returns the revision of both X-Plane and the XPLM DLL. All
    versions are three-digit decimal numbers (e.g. 606 for version 6.06 of
    X-Plane); the current revision of the XPLM is 200 (2.00). This routine also
    returns the host ID of the app running us.

    The most common use of this routine is to special-case around X-Plane
    version-specific behavior.

    Returns tuple:
      outXPlaneVersion
      outXPLMVersion
      outHostID
    """
    pass

def XPLMGetLanguage():
    """
    This routine returns the langauge the sim is running in.
    """
    pass

def XPLMDebugString(inString):
    """
    This routine outputs a string to the Log.txt file. The file is
    immediately flushed so you will not lose data. (This does cause a
    performance penalty.)
    """
    pass

def XPLMSetErrorCallback(self, inCallback):
    """
    XPLMSetErrorCallback installs an error-reporting callback for your plugin.
    Normally the plugin system performs minimum diagnostics to maximize
    performance. When you install an error callback, you will receive calls due
    to certain plugin errors, such as passing bad parameters or incorrect data.

    The intention is for you to install the error callback during debug
    sections and put a break-point inside your callback. This will cause you to
    break into the debugger from within the SDK at the point in your plugin
    where you made an illegal call.

    Installing an error callback may activate error checking code that would
    not normally run, and this may adversely affect performance, so do not
    leave error callbacks installed in shipping plugins.
    """
    pass

def XPLMFindSymbol(inString):
    """
    This routine will attempt to find the symbol passed in the inString
    parameter. If the symbol is found a pointer the function is returned,
    in the form of a integer constructed from the pointer.
    """
    pass

def XPLMLoadDataFile(inFileType, inFilePath):
    """
    Loads a data file of a given type. Paths must be relative to the X-System
    folder. To clear the replay, pass a None file name (this is only valid with
    replay movies, not sit files).
    """
    pass

def XPLMSaveDataFile(inFileType, inFilePath):
    """
    Saves the current situation or replay; paths are relative to the X-System
    folder.
    """
    pass


# XPLMCommandPhase
xplm_CommandBegin    = 0 # The command is being started.
xplm_CommandContinue = 1 # The command is continuing to execute.
xplm_CommandEnd      = 2 # The command has ended.


def XPLMCommandCallback_f(inCommand, inPhase, inRefcon):
    """
    A command callback is a function in your plugin that is called when a
    command is pressed. Your callback receives the command reference for the
    particular command, the phase of the command that is executing, and a
    reference pointer that you specify when registering the callback.

    Your command handler should return 1 to let processing of the command
    continue to other plugins and X-Plane, or 0 to halt processing, potentially
    bypassing X-Plane code.
    """
    pass

def XPLMFindCommand(inName):
    """
    XPLMFindCommand looks up a command by name, and returns its command
    reference or None if the command does not exist.
    """
    pass

def XPLMCommandBegin(inCommand):
    """
    XPLMCommandBegin starts the execution of a command, specified by its
    command reference. The command is "held down" until XPLMCommandEnd is
    called.
    """
    pass

def XPLMCommandEnd(inCommand):
    """
    XPLMCommandEnd ends the execution of a given command that was started with
    XPLMCommandBegin.
    """
    pass

def XPLMCommandOnce(inCommand):
    """
    This executes a given command momentarily, that is, the command begins and
    ends immediately.
    """
    pass

def XPLMCreateCommand(inName, inDescription):
    """
    XPLMCreateCommand creates a new command for a given string. If the command
    already exists, the existing command reference is returned. The description
    may appear in user interface contexts, such as the joystick configuration
    screen.
    """
    pass

def XPLMRegisterCommandHandler(inComand, inHandler, inBefore, inRefcon):
    """
    XPLMRegisterCommandHandler registers a callback to be called when a command
    is executed. You provide a callback with a reference pointer.

    If inBefore is true, your command handler callback will be executed before
    X-Plane executes the command, and returning 0 from your callback will
    disable X-Plane's processing of the command. If inBefore is false, your
    callback will run after X-Plane. (You can register a single callback both
    before and after a command.)
    """
    pass

def XPLMUnregisterCommandHandler(inComand, inHandler, inBefore, inRefcon):
    """
    XPLMUnregisterCommandHandler removes a command callback registered with
    XPLMRegisterCommandHandler.
    """
    pass


