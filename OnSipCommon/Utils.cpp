#include "stdafx.h"

#include "Utils.h"
#include <time.h>
#include <sys\timeb.h>
#include <time.h>
#include <windows.h>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include "logger.h"
#include "atlenc.h"
#include <assert.h>

using namespace std;

// Format the current time using the specified strftime format values
//static
// Added special non-strftime flag to output milliseconds,  %t
tstring DateTimeOperations::FormatNow(const TCHAR* format)
{
	// Get current time (with milliseconds)
	struct _timeb timebuffer;
	_ftime_s(&timebuffer);

	// Convert time to 'tm' type for string formatting
	struct tm tmx;
	localtime_s(&tmx,&timebuffer.time);

	// Get # of milliseconds as string
	tstring szms = Strings::stringFormat(_T("%04d"),timebuffer.millitm);
	// Create new copy of format string to be updated with milliseconds
	tstring szFormat = format;
	szFormat = Strings::replace(szFormat,_T("%t"),szms);

	// Format the time using strftime format types
	TCHAR str[200];
	_tcsftime( str, sizeof(str)/sizeof(TCHAR), szFormat.c_str(), &tmx );

	// Return back as tstring
	return tstring(str);
}

// Return the formatted string for the specified time
//static 
tstring DateTimeOperations::getTimeString(time_t& t)
{
	TCHAR buf[256];
	_tctime_s( buf, sizeof(buf)/sizeof(TCHAR), &t);
	return tstring(buf);
}

// Convert time_t to system time in struct tm
struct tm DateTimeOperations::GetUTCTime(time_t& t)
{
	// Convert to system time
	struct tm tm;
	gmtime_s(&tm,&t);
	return tm;
}

tstring DateTimeOperations::getUTCTimeString(time_t& t)
{
	// Convert to UTC time
	struct tm tm;
	tm = GetUTCTime(t);

	// Convert to string
	return Strings::stringFormat("%i-%02i-%02iT%02i:%02i:%02iZ", tm.tm_year + 1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

//*************************************************************
//*************************************************************

//static the sip address (e.g.  "sip:name@domain.com" into the 
//  name and domain parts.  The prefix "sip:" is optional.
//  Returns false if sipaddress is not in correct format.
bool Utils::ParseSIP(const tstring& sipAddress,tstring* userName,tstring* domain)
{
	tstring sipaddr = Strings::trim(sipAddress);
	if ( sipaddr.empty() )
		return false;
	// Remove "sip:" prefix
	if ( Strings::startsWith( Strings::tolower(sipaddr), _T("sip:") ) )
		sipaddr = sipaddr.substr(4);
	// Ensure has "@"
	if ( !Strings::contains(sipaddr,_T("@")) )
		return false;
	// Break up values by "@"
	tstring_vector vals = Strings::split(sipaddr,"@");
	if ( vals.size() != 2 )
		return false;
	*userName = vals[0];
	*domain = vals[1];
	// If domain does not contain a ".", then an error
	if ( !Strings::contains(*domain,_T(".")) )
		return false;
	return true;
}

//*************************************************************
//*************************************************************

tstring_vector Strings::split(const tstring& str, const tstring& delimiters)
{
	tstring_vector tokens;

	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
	return tokens;
}

tstring Strings::trim_right (const tstring & s, const tstring & t)
{ 
	tstring d (s); 
	tstring::size_type i (d.find_last_not_of (t));
	if (i == tstring::npos)
		return _T("");
	else
		return d.erase (d.find_last_not_of (t) + 1) ; 
}

tstring Strings::trim_left (const tstring & s, const tstring & t) 
{ 
	tstring d (s); 
	return d.erase (0, s.find_first_not_of (t)) ; 
}

tstring Strings::trim (const tstring & s, const tstring & t)
{ 
	tstring d (s); 
	return trim_left (trim_right (d, t), t) ; 
}

// returns a lower case version of the string 
tstring Strings::tolower (const tstring & s)
{
	tstring d (s);
	transform (d.begin (), d.end (), d.begin (), (int(*)(int)) ::tolower);
	return d;
}

// returns an upper case version of the string 
tstring Strings::toupper (const tstring & s)
{
	tstring d (s);
	transform (d.begin (), d.end (), d.begin (), (int(*)(int)) ::toupper);
	return d;
}

// string find-and-replace
//  source = string to have replaced text
//  target = string to be searched within source and replaced
//  replacement = new string to be replaced where target exists
// returns new modified string
tstring Strings::replace(const tstring& source, const tstring& target, const tstring& replacement)
{
	tstring str = source;
	tstring::size_type pos = 0,   // where we are now
		found;     // where the found data is

	if (target.size () > 0)   // searching for nothing will cause a loop
	{
		while ((found = str.find (target, pos)) != tstring::npos)
		{
			str.replace (found, target.size (), replacement);
			pos = found + replacement.size ();
		}
	}

	return str;
};

// Strip all non-numeric values from string
tstring Strings::stripNonNumeric(const tstring& str)
{
	const TCHAR* pstr = str.data();
	tstring strReturn;
    while ( *pstr )
    {   
        if (is_tdigit(*pstr) )
            strReturn += *pstr;
        pstr++;
    }
	return strReturn;
}
// case insensitive compare of strings
bool Strings::stringsSame(const tstring& str1,const tstring& str2)
{	return _tcsicmp(str1.c_str(), str2.c_str()) == 0; }

// Returns true if 'str' starts with 'chars'
bool Strings::startsWith(const tstring& str,const tstring& chars)
{	return str.find(chars) == 0;	}

// Returns true if 'str' ends with 'chars'
bool Strings::endsWith(const tstring& str,const tstring& chars)
{
	size_t i = str.rfind(chars);
	return (i != tstring::npos) && (i == (str.length() - chars.length()));
}

// Returns true if 'str' contains 'chars'
bool Strings::contains(const tstring& str,const tstring& chars)
{	return str.find(chars) != tstring::npos;	}

tstring Strings::stringFormat(const TCHAR* format, ...)
{
	va_list v;
	va_start(v,format);
	// Get required length of characters, add 1 for NULL
	int len = _vsctprintf(format,v) + 1;
	// Allocate the string buffer
	TCHAR* str = new TCHAR[len];
	_vstprintf_s(str,len,format,v);
	va_end(v);
	// Convert to string
	tstring ret(str);
	// Free memory and return formatted tstring
	delete[] str;
	return ret;
}

//static
// string converts from widechar to multibyte
string Strings::convert(const wstring& src)
{
	string ret;
	ret.resize(src.size());
	size_t i;
	wcstombs_s(&i, (char *) ret.data(), ret.size()+1, src.data(), ret.size() );
	return ret;
}

//static
// string converts from mulibyte to widechar
wstring Strings::convert(const string& src)
{
	size_t i;
	wstring ret;
	ret.resize(src.size());
	mbstowcs_s(&i,(wchar_t *)ret.data(),ret.size()+1, src.data(), ret.size());
	return ret;
}

const TCHAR * toStrip = _T(" \n\r\t");

tstring Strings::lstrip(const tstring& s) {
	tstring::size_type n = s.find_first_not_of(toStrip);
	return n == tstring::npos ? tstring() : s.substr(n);
}

tstring Strings::rstrip(const tstring& s) {
	tstring::size_type n = s.find_last_not_of(toStrip);
	return n == tstring::npos ? tstring() : s.substr(0, n+1);
}

tstring Strings::strip(const tstring& s) {
	return lstrip(rstrip(s));
}

tstring Strings::repr(const tstring& _s) {
	string s = Strings::T_TO_S(_s);
	string r;
	for (unsigned i = 0; i < s.size(); i++) {
		if (s[i] == '\n')
			r += "\\n";
		else if (s[i] == '\r')
			r += "\\r";
		else if (s[i] == '\t')
			r += "\\t";
		else
			r += s[i];
	}
	return Strings::S_TO_T(r);
}

// Do an XOR of each character in strInput with the charXor.
// Returns back the XOR string
tstring Strings::_xorWithChar(const tstring& strInput,TCHAR charXor)
{
	// Create the return string
	tstring strOutput;
	strOutput.resize(strInput.size());

	// Do XOR on each of the chars
	unsigned count = 0;
	tstring::const_iterator iter = strInput.begin();
	while ( iter != strInput.end() )
		strOutput[count++] = (*iter++) ^ charXor;
	return strOutput;
}

// Do an XOR of each char in strInput with each char in stringXor
// Returns back the new XOR string
tstring Strings::_xor(const tstring& strInput,const tstring& stringXor)
{
	tstring retString = strInput;
	for ( tstring::const_iterator iter = stringXor.begin(); iter != stringXor.end(); ++iter )
		retString = _xorWithChar(retString,*iter);
	return retString;
}

// Simple decrypt on 'instr' using the key
string Strings::decryptString(const string& strInput,const string& decryptKey)
{
	// Convert the base64 into bytes
	char output[1024];
	int len=(int)sizeof(output);
	BOOL ret = Base64Decode( strInput.data(), strInput.size(), (BYTE *)output, &len );
	_ASSERT(ret);

	// Put bytes back into a string
	string retString;
	retString.resize(len);
	for ( int i=0; i<len; ++i )
		retString[i] = output[i];

	// decrypt the string using the xor chars
	return _xor(retString,reverseString(decryptKey));
}

// Do a simple encrypt on strInput with each character in the encryptKey
string Strings::encryptString(const string& strInput,const string& encryptKey)
{
	// Encrypt the string
	string strRet = _xor(strInput,encryptKey);

	// Convert the string to base64
	char output[1024];
	int len=(int)sizeof(output);
	BOOL ret = Base64Encode( (const BYTE*) strRet.data(), strRet.size(), output, &len, ATL_BASE64_FLAG_NOPAD | ATL_BASE64_FLAG_NOCRLF );
	_ASSERT(ret);

	// Put the base64 bytes into the return value
	strRet.resize(len);
	for ( int i=0; i<len; ++i )
		strRet[i] = output[i];

	return strRet;
}

// Returns a reverse copy of 'str'
tstring Strings::reverseString(const tstring& strInput)
{
	// Create return string of same size
	tstring retString;
	retString.resize(strInput.size());

	// Reverse the string
	unsigned count=0;
	tstring::const_reverse_iterator iter = strInput.rbegin();
	while ( iter != strInput.rend() )
		retString[count++] = *iter++;
	return retString;
}



