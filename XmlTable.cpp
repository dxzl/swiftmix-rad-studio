//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "XmlTable.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// Note: for XML just 5: &lt; (<), &amp; (&), &gt; (>), &quot; ("), and &apos; (')
const WideChar* XMLCODES[XMLCODESLEN] =
{
  L"&quot;",
  L"&apos;",
  L"&gt;",
  L"&lt;",
  L"&amp;", // place this last to make a serch/replace faster!
};
//---------------------------------------------------------------------------
// The index of the char in this string looks up its HTML replacement
// string in the table above.
const WideChar XMLCHARS[XMLCODESLEN] = L"\"\'><&";
//---------------------------------------------------------------------------
// Windows URI percent-encoding
// For the UNC Windows file path: \\laptop\My Documents\FileSchemeURIs.doc
// The corresponding valid file URI in Windows is the following: file://laptop/My%20Documents/FileSchemeURIs.doc
// For the local Windows file path: C:\Documents and Settings\davris\FileSchemeURIs.doc
// The corresponding valid file URI in Windows is: file:///C:/Documents%20and%20Settings/davris/FileSchemeURIs.doc
// The important factors here are the use of percent-encoding and the number of slashes following the ‘file:’ scheme name.
const WideChar WINDOWSPERCENTCHARS[] = L" #%{}`^"; // plus all control chars
//---------------------------------------------------------------------------
// .XSPF Playlist Percent Encoding
// Chars below can be use UNENCODED in a URL
// Can use these unencoded    A-Z, a-z, 0-9, and
// Must %hex encode these (including SPACE) :/?#[]@ sub-delims !$&'()*+,;=
// include SPACE, don't include -._~'/
const WideChar PERCENTCHARS[] = L" !#$&'()*+,;=?@[]";
//---------------------------------------------------------------------------


