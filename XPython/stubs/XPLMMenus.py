"""
Theory of Operation

Plug-ins can create menus in the menu bar of X-Plane.  This is done by
creating a menu and then creating items.  Menus are referred to by an
opaque ID.  Items are referred to by (zero-based) index number.

Menus are "sandboxed" between plugins---no plugin can access the menus of
any other plugin. Furthermore, all menu indices are relative to your
plugin's menus only; if your plugin creates two sub-menus in the Plugins
menu at different times, it doesn't matter how many other plugins also
create sub-menus of Plugins in the intervening time: your sub-menus will be
given menu indices 0 and 1. (The SDK does some work in the back-end to
filter out menus that are irrelevant to your plugin in order to deliver
this consistency for each plugin.)

When you create a menu item, you specify how we should handle clicks on
that menu item. You can either have the XPLM trigger a callback (the
XPLMMenuHandler_f associated with the menu that contains the item), or you
can simply have a command be triggered (with no associated call to your
menu handler). The advantage of the latter method is that X-Plane will
display any keyboard shortcuts associated with the command. (In contrast,
there are no keyboard shortcuts associated with menu handler callbacks with
specific parameters.)


Enumerations and constants
**************************
XPLMMenuCheck

These enumerations define the various 'check' states for an X-Plane menu.
'checking' in X-Plane actually appears as a light which may or may not be
lit.  So there are three possible states.
"""


# XPLMMenuCheck
# there is no symbol to the left of the menu item.
xplm_Menu_NoCheck   = 0
# the menu has a mark next to it that is unmarked (not lit).
xplm_Menu_Unchecked = 1
# the menu has a mark next to it that is checked (lit).
xplm_Menu_Checked   = 2



def XPLMMenuHandler_f(inMenuRef, inItemRef):
    """
    A menu handler function takes two reference pointers, one for the menu
    (specified when the menu was created) and one for the item (specified when
    the item was created).
    """
    pass


def XPLMFindPluginsMenu():
    """
    This function returns the ID of the plug-ins menu, which is created for you
    at startup.
    """
    pass


def XPLMFindAircraftMenu():
    """
    This function returns the ID of the menu for the currently-loaded aircraft,
    used for showing aircraft-specific commands.

    The aircraft menu is created by X-Plane at startup, but it remains hidden
    until it is populated via XPLMAppendMenuItem() or
    XPLMAppendMenuItemWithCommand().

    Only plugins loaded with the user's current aircraft are allowed to access
    the aircraft menu. For all other plugins, this will return None, and any
    attempts to add menu items to it will fail.
    """
    pass


def XPLMCreateMenu(self,
                   inName,
                   inParentMenu,
                   inParentItem,
                   inHandler,
                   inMenuRef):
    """
    This function creates a new menu and returns its ID.  It returns None if
    the menu cannot be created.  Pass in a parent menu ID and an item index to
    create a submenu, or None for the parent menu to put the menu in the menu
    bar.  The menu's name is only used if the menu is in the menubar.  You also
    pass a handler function and a menu reference value. Pass None for the
    handler if you do not need callbacks from the menu (for example, if it only
    contains submenus).

    Important: you must pass a valid, non-empty menu title even if the menu is
    a submenu where the title is not visible.

    inName : menu title (string)
    inParentMenu : parent menu handle
    inParentItem : index (integer)
    inHandler : callback or None (XPLMMenuHandler_f)
    inMenuRef : object passed to the callback
    """
    pass


def XPLMDestroyMenu(inMenuID):
    """
    This function destroys a menu that you have created.  Use this to remove a
    submenu if necessary.  (Normally this function will not be necessary.)
    """
    pass


def XPLMClearAllMenuItems(inMenuID):
    """
    This function removes all menu items from a menu, allowing you to rebuild
    it.  Use this function if you need to change the number of items on a menu.
    """
    pass


def XPLMAppendMenuItem(inMenu,
                       inItemName,
                       inItemRef,
                       inDeprecatedAndIgnored):
    """
    This routine appends a new menu item to the bottom of a menu and returns
    its index. Pass in the menu to add the item to, the items name, and a
    inItemRef for this item.

    Returns a negative index if the append failed (due to an invalid parent
    menu argument).

    Note that all menu indices returned are relative to your plugin's menus
    only; if your plugin creates two sub-menus in the Plugins menu at different
    times, it doesn't matter how many other plugins also create sub-menus of
    Plugins in the intervening time: your sub-menus will be given menu indices
    0 and 1. (The SDK does some work in the back-end to filter out menus that
    are irrelevant to your plugin in order to deliver this consistency for each
    plugin.)
    """
    pass


def XPLMAppendMenuItemWithCommand(inMenu, inItemName, inCommandToExecute):
    """
    Like XPLMAppendMenuItem(), but instead of the new menu item triggering the
    XPLMMenuHandler_f of the containiner menu, it will simply execute the
    command you pass in. Using a command for your menu item allows the user to
    bind a keyboard shortcut to the command and see that shortcut represented
    in the menu.

    Returns a negative index if the append failed (due to an invalid parent
    menu argument).

    Like XPLMAppendMenuItem(), all menu indices are relative to your plugin's
    menus only.
    """
    pass


def XPLMAppendMenuSeparator(inMenu):
    """
    This routine adds a separator to the end of a menu.

    Returns a negative index if the append failed (due to an invalid parent
    menu argument).
    """
    pass


def XPLMSetMenuItemName(inMenu, inIndex, inItemName, inForceEnglish):
    """
    This routine changes the name of an existing menu item.  Pass in the menu
    ID and the index of the menu item.
    """
    pass


def XPLMCheckMenuItem(inMenu, index, inCheck):
    """
    Set whether a menu item is checked.  Pass in the menu ID and item index.
    """
    pass


def XPLMCheckMenuItemState(inMenu, index, outCheck):
    """
    This routine returns whether a menu item is checked or not. A menu item's
    check mark may be on or off, or a menu may not have an icon at all.
    """
    pass


def XPLMEnableMenuItem(inMenu, index, enabled):
    """
    Sets whether this menu item is enabled.  Items start out enabled.
    """
    pass


def XPLMRemoveMenuItem(inMenu, inIndex):
    """
    Removes one item from a menu.  Note that all menu items below are moved up
    one; your plugin must track the change in index numbers.
    """
    pass

