// see http://www.codeproject.com/KB/cpp/sendkeys_cpp_Article.aspx

#ifdef __WINDOWS_MM__

#include "SendKeys.h"

#include <tchar.h>

const WORD CSendKeys::VKKEYSCANSHIFTON = 0x01;
const WORD CSendKeys::VKKEYSCANCTRLON  = 0x02;
const WORD CSendKeys::VKKEYSCANALTON   = 0x04;
const WORD CSendKeys::INVALIDKEY       = 0xFFFF;

const BYTE CSendKeys::ExtendedVKeys[MaxExtendedVKeys] =
{
    VK_UP, 
    VK_DOWN,
    VK_LEFT,
    VK_RIGHT,
    VK_HOME,
    VK_END,
    VK_PRIOR, // PgUp
    VK_NEXT,  //  PgDn
    VK_INSERT,
    VK_DELETE
};

CSendKeys::CSendKeys()
{
  m_nDelayNow = m_nDelayAlways = 0;
}

// Delphi port regexps:
// ---------------------
// search: .+Name:'([^']+)'.+vkey:([^\)]+)\)
// replace: {"\1", \2}
//
// **If you add to this list, you must be sure to keep it sorted alphabetically
// by Name because a binary search routine is used to scan it.**
//
CSendKeys::key_desc_t CSendKeys::KeyNames[CSendKeys::MaxSendKeysRecs] = 
{
  {("ADD"), VK_ADD},
  {("APPS"), VK_APPS},
  {("AT"), '@', true},
  {("BACKSPACE"), VK_BACK},
  {("BKSP"), VK_BACK},
  {("BREAK"), VK_CANCEL},
  {("BS"), VK_BACK},
  {("CAPSLOCK"), VK_CAPITAL},
  {("CARET"), '^', true},
  {("CLEAR"), VK_CLEAR},
  {("DECIMAL"), VK_DECIMAL},
  {("DEL"), VK_DELETE},
  {("DELETE"), VK_DELETE},
  {("DIVIDE"), VK_DIVIDE},
  {("DOWN"), VK_DOWN},
  {("END"), VK_END},
  {("ENTER"), VK_RETURN},
  {("ESC"), VK_ESCAPE},
  {("ESCAPE"), VK_ESCAPE},
  {("F1"), VK_F1},
  {("F10"), VK_F10},
  {("F11"), VK_F11},
  {("F12"), VK_F12},
  {("F13"), VK_F13},
  {("F14"), VK_F14},
  {("F15"), VK_F15},
  {("F16"), VK_F16},
  {("F2"), VK_F2},
  {("F3"), VK_F3},
  {("F4"), VK_F4},
  {("F5"), VK_F5},
  {("F6"), VK_F6},
  {("F7"), VK_F7},
  {("F8"), VK_F8},
  {("F9"), VK_F9},
  {("HELP"), VK_HELP},
  {("HOME"), VK_HOME},
  {("INS"), VK_INSERT},
  {("LEFT"), VK_LEFT},
  {("LEFTBRACE"), '{', true},
  {("LEFTPAREN"), '(', true},
  {("LWIN"), VK_LWIN},
  {("MULTIPLY"), VK_MULTIPLY},
  {("NUMLOCK"), VK_NUMLOCK},
  {("NUMPAD0"), VK_NUMPAD0},
  {("NUMPAD1"), VK_NUMPAD1},
  {("NUMPAD2"), VK_NUMPAD2},
  {("NUMPAD3"), VK_NUMPAD3},
  {("NUMPAD4"), VK_NUMPAD4},
  {("NUMPAD5"), VK_NUMPAD5},
  {("NUMPAD6"), VK_NUMPAD6},
  {("NUMPAD7"), VK_NUMPAD7},
  {("NUMPAD8"), VK_NUMPAD8},
  {("NUMPAD9"), VK_NUMPAD9},
  {("PERCENT"), '%', true},
  {("PGDN"), VK_NEXT},
  {("PGUP"), VK_PRIOR},
  {("PLUS"), '+', true},
  {("PRTSC"), VK_PRINT},
  {("RIGHT"), VK_RIGHT},
  {("RIGHTBRACE"), '}', true},
  {("RIGHTPAREN"), ')', true},
  {("RWIN"), VK_RWIN},
  {("SCROLL"), VK_SCROLL},
  {("SEPARATOR"), VK_SEPARATOR},
  {("SNAPSHOT"), VK_SNAPSHOT},
  {("SUBTRACT"), VK_SUBTRACT},
  {("TAB"), VK_TAB},
  {("TILDE"), '~', true},
  {("UP"), VK_UP},
  {("WIN"), VK_LWIN}
};


// calls keybd_event() and waits, if needed, till the sent input is processed
void CSendKeys::KeyboardEvent(BYTE VKey, BYTE ScanCode, LONG Flags)
{
  MSG KeyboardMsg;

  keybd_event(VKey, ScanCode, Flags, 0);

  if (m_bWait)
  {
    while (::PeekMessage(&KeyboardMsg, 0, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
    {
      ::TranslateMessage(&KeyboardMsg);
      ::DispatchMessage(&KeyboardMsg);
    }
  }
}

// Checks whether the specified VKey is an extended key or not
bool CSendKeys::IsVkExtended(BYTE VKey)
{
  for (int i=0;i<MaxExtendedVKeys;i++)
  {
    if (ExtendedVKeys[i] == VKey)
      return true;
  }
  return false;
}

// Generates KEYUP
void CSendKeys::SendKeyUp(BYTE VKey)
{
  BYTE ScanCode = LOBYTE(::MapVirtualKey(VKey, 0));

  KeyboardEvent(VKey, 
                ScanCode, 
                KEYEVENTF_KEYUP | (IsVkExtended(VKey) ? KEYEVENTF_EXTENDEDKEY : 0));
}

void CSendKeys::SendKeyDown(BYTE VKey, WORD NumTimes, bool GenUpMsg, bool bDelay)
{
  WORD Cnt = 0;
  BYTE ScanCode = 0;
  bool NumState = false;

  if (VKey == VK_NUMLOCK)
  {
    DWORD dwVersion = ::GetVersion();

    for (Cnt=1; Cnt<=NumTimes; Cnt++)
    {
      if (bDelay)
        CarryDelay();
      // snippet based on:
      // http://www.codeproject.com/cpp/togglekeys.asp
      if (dwVersion < 0x80000000)
      {
        ::keybd_event(VKey, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
        ::keybd_event(VKey, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      }
      else
      {
        {
          KEYBOARDSTATE_t KeyboardState;
          NumState = GetKeyState(VK_NUMLOCK) & 1 ? true : false;
          GetKeyboardState(&KeyboardState[0]);
          if (NumState)
            KeyboardState[VK_NUMLOCK] &= ~1;
          else
            KeyboardState[VK_NUMLOCK] |= 1;

          SetKeyboardState(&KeyboardState[0]);
        }
      }
    }
    return;
  }

  // Get scancode
  ScanCode = LOBYTE(::MapVirtualKey(VKey, 0));

  // Send keys
  for (Cnt=1; Cnt<=NumTimes; Cnt++)
  {
    // Carry needed delay ?
    if (bDelay)
      CarryDelay();

    KeyboardEvent(VKey, ScanCode, IsVkExtended(VKey) ? KEYEVENTF_EXTENDEDKEY : 0);
    if (GenUpMsg)
      KeyboardEvent(VKey, ScanCode, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP);
  }
}

// Checks whether a bit is set
bool CSendKeys::BitSet(BYTE BitTable, UINT BitMask)
{
  return BitTable & BitMask ? true : false;
}

// Sends a single key
void CSendKeys::SendKey(WORD MKey, WORD NumTimes, bool GenDownMsg)
{
  // Send appropriate shift keys associated with the given VKey
  if (BitSet(HIBYTE(MKey), VKKEYSCANSHIFTON))
    SendKeyDown(VK_SHIFT, 1, false);

  if (BitSet(HIBYTE(MKey), VKKEYSCANCTRLON))
    SendKeyDown(VK_CONTROL, 1, false);

  if (BitSet(HIBYTE(MKey), VKKEYSCANALTON))
    SendKeyDown(VK_MENU, 1, false);

  // Send the actual VKey
  SendKeyDown(LOBYTE(MKey), NumTimes, GenDownMsg, true);

  // toggle up shift keys
  if (BitSet(HIBYTE(MKey), VKKEYSCANSHIFTON))
    SendKeyUp(VK_SHIFT);

  if (BitSet(HIBYTE(MKey), VKKEYSCANCTRLON))
    SendKeyUp(VK_CONTROL);

  if (BitSet(HIBYTE(MKey), VKKEYSCANALTON))
    SendKeyUp(VK_MENU);
}

// Implements a simple binary search to locate special key name strings
WORD CSendKeys::StringToVKey(char* KeyString, int &idx)
{
  bool Found = false, Collided;
  int  Bottom = 0, 
       Top = MaxSendKeysRecs,
       Middle = (Bottom + Top) / 2;
  WORD retval = INVALIDKEY;

  idx    = -1;

  do
  {
    Collided = (Bottom == Middle) || (Top == Middle);
    int cmp = _tcsnicmp(KeyNames[Middle].keyName, KeyString, _tcslen(KeyString));
    if (cmp == 0)
    {
      Found = true;
      retval = KeyNames[Middle].VKey;
      idx    = Middle;
      break;
    }
    else
    {
      if (cmp < 0)
        Bottom = Middle;
      else
        Top = Middle;
      Middle = (Bottom + Top) / 2;
    }
  } while (!(Found || Collided));

  return retval;
}

// Releases all shift keys (keys that can be depressed while other keys are being pressed
// If we are in a modifier group this function does nothing
void CSendKeys::PopUpShiftKeys()
{
  if (!m_bUsingParens)
  {
    if (m_bShiftDown)
      SendKeyUp(VK_SHIFT);
    if (m_bControlDown)
      SendKeyUp(VK_CONTROL);
    if (m_bAltDown)
      SendKeyUp(VK_MENU);
    if (m_bWinDown)
      SendKeyUp(VK_LWIN);
    m_bWinDown = m_bShiftDown = m_bControlDown = m_bAltDown = false;
  }
}

// Sends a key string
bool CSendKeys::SendKeys(char* KeysString, bool Wait)
{
  WORD MKey, NumTimes = 0;
  char KeyString[300] = {0};
  int  keyIdx;

  char* pKey = (char*) KeysString;
  char  ch;

  m_bWait = Wait;

  m_bWinDown = m_bShiftDown = m_bControlDown = m_bAltDown = m_bUsingParens = false;

  while (ch = *pKey)
  {
    switch (ch)
    {
    // begin modifier group
    case '(':
      m_bUsingParens = true;
      break;

    // end modifier group
    case ')':
      m_bUsingParens = false;
      PopUpShiftKeys(); // pop all shift keys when we finish a modifier group close
      break;

    // ALT key
    case '%':
      m_bAltDown = true;
      SendKeyDown(VK_MENU, 1, false);
      break;

    // SHIFT key
    case '+':
      m_bShiftDown = true;
      SendKeyDown(VK_SHIFT, 1, false);
      break;

    // CTRL key
    case '^':
      m_bControlDown = true;
      SendKeyDown(VK_CONTROL, 1, false);
      break;

    // WINKEY (Left-WinKey)
    case '@':
      m_bWinDown = true;
      SendKeyDown(VK_LWIN, 1, false);
      break;

    // enter
    case '~':
      SendKeyDown(VK_RETURN, 1, true);
      PopUpShiftKeys();
      break;

    // begin special keys
    case '{':
      {
        char* p = pKey+1; // skip past the beginning '{'
        size_t t;

        // find end of close
        while (*p && *p != '}')
          p++;

        t = p - pKey;
        // special key definition too big?
        if (t > sizeof(KeyString))
          return false;

        // Take this KeyString into local buffer
        strncpy(KeyString, pKey+1, t);

        KeyString[t-1] = '\0';
        keyIdx = -1;

        pKey += t; // skip to next keystring

        // Invalidate key
        MKey = INVALIDKEY;

        // sending arbitrary vkeys?
        if (_strnicmp(KeyString, _T("VKEY"), 4) == 0)
        {
          p = KeyString + 4;
          MKey = _ttoi(p);
        }
        else if (_tcsnicmp(KeyString, _T("BEEP"), 4) == 0)
        {
          p = KeyString + 4 + 1;
          char* p1 = p;
          DWORD frequency, delay;

          if ((p1 = _tcsstr(p, _T(" "))) != NULL)
          {
            *p1++ = '\0';
            frequency = _ttoi(p);
            delay = _ttoi(p1);
            ::Beep(frequency, delay);
          }
        }
        // want to send/set delay?
        else if (_tcsnicmp(KeyString, _T("DELAY"), 5) == 0)
        {
          // Advance to parameters
          p = KeyString + 5;
          // set "sleep factor"
          if (*p == '=')
            m_nDelayAlways = _ttoi(p + 1); // Take number after the '=' character
          else
            // set "sleep now"
            m_nDelayNow = _ttoi(p);
        }
        // not command special keys, then process as keystring to VKey
        else
        {
          MKey = StringToVKey(KeyString, keyIdx);
          // Key found in table
          if (keyIdx != -1)
          {
            NumTimes = 1;

            // Does the key string have also count specifier?
            t = _tcslen(KeyNames[keyIdx].keyName);
            if (_tcslen(KeyString) > t)
            {
              p = KeyString + t;
              // Take the specified number of times
              NumTimes = _ttoi(p);
            }

            if (KeyNames[keyIdx].normalkey)
              MKey = ::VkKeyScan(KeyNames[keyIdx].VKey);
          }
        }

        // A valid key to send?
        if (MKey != INVALIDKEY)
        {
          SendKey(MKey, NumTimes, true);
          PopUpShiftKeys();
        }
      }
      break;

      // a normal key was pressed
    default:
      // Get the VKey from the key
      MKey = ::VkKeyScan(ch);
      SendKey(MKey, 1, true);
      PopUpShiftKeys();
    }
    pKey++;
  }

  m_bUsingParens = false;
  PopUpShiftKeys();
  return true;
}

// Carries the required delay and clears the m_nDelaynow value
void CSendKeys::CarryDelay()
{
  // Should we delay once?
  if (!m_nDelayNow)
    // should we delay always?
    m_nDelayNow = m_nDelayAlways;

  // No delay specified?
  if (m_nDelayNow)
    ::Sleep(m_nDelayNow); //::Beep(100, m_nDelayNow);

  // clear SleepNow
  m_nDelayNow = 0;
}

#endif

