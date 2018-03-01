module Tabs = {
  type tab = {
    .
    "id":
      Js.Nullable.t(
        int
      ), /*
The ID of the tab. Tab IDs are unique within a browser session. Under some circumstances a Tab may not be assigned an ID, for example when querying foreign tabs using the sessions API, in which case a session ID may be present. Tab ID can also be set to chrome.tabs.TAB_ID_NONE for apps and devtools windows.

     */
    "index": int, /*
The zero-based index of the tab within its window.

     */
    "windowId": int, /*
The ID of the window the tab is contained within.

     */
    "openerTabId":
      option(
        int
      ), /*
The ID of the tab that opened this tab, if any. This property is only present if the opener tab still exists.

     */
    "selected": bool, /*
Deprecated since Chrome 33. Please use tabs.Tab.highlighted.


Whether the tab is selected.
     */
    "highlighted": bool, /*
Whether the tab is highlighted.

     */
    "active": bool, /*
Whether the tab is active in its window. (Does not necessarily mean the window is focused.)

     */
    "pinned": bool, /*
Whether the tab is pinned.

     */
    "audible":
      option(
        bool
      ), /*
Since Chrome 45.


Whether the tab has produced sound over the past couple of seconds (but it might not be heard if also muted). Equivalent to whether the speaker audio indicator is showing.
     */
    "discarded":
      option(
        bool
      ), /*
Since Chrome 54.


Whether the tab is discarded. A discarded tab is one whose content has been unloaded from memory, but is still visible in the tab strip. Its content gets reloaded the next time it's activated.
     */
    "autoDiscardable":
      option(
        bool
      ), /*
Since Chrome 54.


Whether the tab can be discarded automatically by the browser when resources are low.
     */
    "url":
      Js.Nullable.t(
        string
      ), /*
The URL the tab is displaying. This property is only present if the extension's manifest includes the "tabs" permission.

     */
    "title":
      option(
        string
      ), /*
The title of the tab. This property is only present if the extension's manifest includes the "tabs" permission.

     */
    "favIconUrl":
      option(
        string
      ), /*
The URL of the tab's favicon. This property is only present if the extension's manifest includes the "tabs" permission. It may also be an empty string if the tab is loading.

     */
    "status": option(string), /*
Either loading or complete.

     */
    "incognito":
      option(bool), /*
Whether the tab is in an incognito window.

     */
    "width":
      option(
        int
      ), /*
Since Chrome 31.


The width of the tab in pixels.
     */
    "height":
      option(
        int
      ), /*
Since Chrome 31.


The height of the tab in pixels.
     */
    "sessionId":
      option(string) /*
Since Chrome 31.


The session ID used to uniquely identify a Tab obtained from the sessions API.
     */
  };
  type queryInfo = {
    .
    "active": Js.boolean
    /*Whether the tabs are active in their windows. */
    /* "pinned": option(bool), */
    /* /\*Whether the tabs are pinned. *\/ */
    /* "audible": option(bool), */
    /* /\*Since Chrome 45. Whether the tabs are audible. *\/ */
    /* "muted": option(bool), */
    /* /\*Since Chrome 45. Whether the tabs are muted. *\/ */
    /* "highlighted": option(bool), */
    /* /\*Whether the tabs are highlighted. *\/ */
    /* "discarded": option(bool), */
    /* /\*Since Chrome 54. Whether the tabs are discarded. A discarded tab is one whose content has been unloaded from memory, but is still visible in the tab strip. Its content gets reloaded the next time it's activated. *\/ */
    /* "autoDiscardable": option(bool), */
    /* /\*Since Chrome 54. */
    /*   Whether the tabs can be discarded automatically by the browser when resources are low. *\/ */
    /* "currentWindow": option(bool), */
    /* /\*Whether the tabs are in the current window. *\/ */
    /* "lastFocusedWindow": option(bool), */
    /* /\*Whether the tabs are in the last focused window. *\/ */
    /* /\* "status": option(tabStatus), *\/ */
    /* /\*Whether the tabs have completed loading. *\/ */
    /* "title": option(string), */
    /* /\*Match page titles against a pattern. Note that this property is ignored if the extension doesn't have the "tabs" permission. *\/ */
    /* "index": option(int) */
    /* /\*The position of the tabs within their windows. *\/ */
  };
  [@bs.val]
  external query : (queryInfo, array(tab) => unit) => unit =
    "chrome.tabs.query";
  [@bs.val];
  let getActive = (callback: tab => unit) =>
    query(
      {"active": Js.true_},
      tabs => {
        Js.log2("tabs: ", tabs);
        callback(tabs[0]);
      }
    );
};
