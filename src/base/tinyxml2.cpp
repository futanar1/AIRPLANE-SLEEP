
/*
Original code by Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "tinyxml2.h"

#include <new>		// yes, this one new style header, is in the Android SDK.
#if defined(ANDROID_NDK) || defined(__BORLANDC__) || defined(__QNXNTO__)
#   include <stddef.h>
#   include <stdarg.h>
#else
#   include <cstddef>
#   include <cstdarg>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (!defined WINCE)
// Microsoft Visual Studio, version 2005 and higher. Not WinCE.
	/*int _snprintf_s(
	   char *buffer,
	   size_t sizeOfBuffer,
	   size_t count,
	   const char *format [,
		  argument] ...
	);*/
	static inline int TIXML_SNPRINTF( char* buffer, size_t size, const char* format, ... )
	{
		va_list va;
		va_start( va, format );
		const int result = vsnprintf_s( buffer, size, _TRUNCATE, format, va );
		va_end( va );
		return result;
	}

	static inline int TIXML_VSNPRINTF( char* buffer, size_t size, const char* format, va_list va )
	{
		const int result = vsnprintf_s( buffer, size, _TRUNCATE, format, va );
		return result;
	}

	#define TIXML_VSCPRINTF	_vscprintf
	#define TIXML_SSCANF	sscanf_s
#elif defined _MSC_VER
// Microsoft Visual Studio 2003 and earlier or WinCE
	#define TIXML_SNPRINTF	_snprintf
	#define TIXML_VSNPRINTF _vsnprintf
	#define TIXML_SSCANF	sscanf
	#if (_MSC_VER < 1400 ) && (!defined WINCE)
		// Microsoft Visual Studio 2003 and not WinCE.
		#define TIXML_VSCPRINTF   _vscprintf // VS2003's C runtime has this, but VC6 C runtime or WinCE SDK doesn't have.
	#else
		// Microsoft Visual Studio 2003 and earlier or WinCE.
		static inline int TIXML_VSCPRINTF( const char* format, va_list va )
		{
			int len = 512;
			for (;;) {
				len = len*2;
				char* str = new char[len]();
				const int required = _vsnprintf(str, len, format, va);
				delete[] str;
				if ( required != -1 ) {
					TIXMLASSERT( required >= 0 );
					len = required;
					break;
				}
			}
			TIXMLASSERT( len >= 0 );
			return len;
		}
	#endif
#else
// GCC version 3 and higher
//#warning( "Using sn* functions." )
#define TIXML_SNPRINTF	snprintf
#define TIXML_VSNPRINTF	vsnprintf
static inline int TIXML_VSCPRINTF( const char* format, va_list va )
{
    int len = vsnprintf( 0, 0, format, va );
    TIXMLASSERT( len >= 0 );
    return len;
}
#define TIXML_SSCANF   sscanf
#endif

#if defined(_WIN64)
#define TIXML_FSEEK _fseeki64
	#define TIXML_FTELL _ftelli64
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) \
	|| defined(__NetBSD__) || defined(__DragonFly__) || defined(__ANDROID__)
#define TIXML_FSEEK fseeko
#define TIXML_FTELL ftello
#elif defined(__unix__) && defined(__x86_64__)
#define TIXML_FSEEK fseeko64
	#define TIXML_FTELL ftello64
#else
	#define TIXML_FSEEK fseek
	#define TIXML_FTELL ftell
#endif


static const char LINE_FEED				= static_cast<char>(0x0a);			// all line endings are normalized to LF
static const char LF = LINE_FEED;
static const char CARRIAGE_RETURN		= static_cast<char>(0x0d);			// CR gets filtered out
static const char CR = CARRIAGE_RETURN;
static const char SINGLE_QUOTE			= '\'';
static const char DOUBLE_QUOTE			= '\"';

// Bunch of unicode info at:
//		http://www.unicode.org/faq/utf_bom.html
//	ef bb bf (Microsoft "lead bytes") - designates UTF-8

static const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
static const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
static const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

namespace tinyxml2
{

    struct Entity {
        const char* pattern;
        int length;
        char value;
    };

    static const int NUM_ENTITIES = 5;
    static const Entity entities[NUM_ENTITIES] = {
            { "quot", 4,	DOUBLE_QUOTE },
            { "amp", 3,		'&'  },
            { "apos", 4,	SINGLE_QUOTE },
            { "lt",	2, 		'<'	 },
            { "gt",	2,		'>'	 }
    };


    StrPair::~StrPair()
    {
        Reset();
    }


    void StrPair::TransferTo( StrPair* other )
    {
        if ( this == other ) {
            return;
        }
        // This in effect implements the assignment operator by "moving"
        // ownership (as in auto_ptr).

        TIXMLASSERT( other != 0 );
        TIXMLASSERT( other->_flags == 0 );
        TIXMLASSERT( other->_start == 0 );
        TIXMLASSERT( other->_end == 0 );

        other->Reset();

        other->_flags = _flags;
        other->_start = _start;
        other->_end = _end;

        _flags = 0;
        _start = 0;
        _end = 0;
    }


    void StrPair::Reset()
    {
        if ( _flags & NEEDS_DELETE ) {
            delete [] _start;
        }
        _flags = 0;
        _start = 0;
        _end = 0;
    }


    void StrPair::SetStr( const char* str, int flags )
    {
        TIXMLASSERT( str );
        Reset();
        size_t len = strlen( str );
        TIXMLASSERT( _start == 0 );
        _start = new char[ len+1 ];
        memcpy( _start, str, len+1 );
        _end = _start + len;
        _flags = flags | NEEDS_DELETE;
    }


    char* StrPair::ParseText( char* p, const char* endTag, int strFlags, int* curLineNumPtr )
    {
        TIXMLASSERT( p );
        TIXMLASSERT( endTag && *endTag );
        TIXMLASSERT(curLineNumPtr);

        char* start = p;
        const char  endChar = *endTag;
        size_t length = strlen( endTag );

        // Inner loop of text parsing.
        while ( *p ) {
            if ( *p == endChar && strncmp( p, endTag, length ) == 0 ) {
                Set( start, p, strFlags );
                return p + length;
            } else if (*p == '\n') {
                ++(*curLineNumPtr);
            }
            ++p;
            TIXMLASSERT( p );
        }
        return 0;
    }


    char* StrPair::ParseName( char* p )
    {
        if ( !p || !(*p) ) {
            return 0;
        }
        if ( !XMLUtil::IsNameStartChar( (unsigned char) *p ) ) {
            return 0;
        }

        char* const start = p;
        ++p;
        while ( *p && XMLUtil::IsNameChar( (unsigned char) *p ) ) {
            ++p;
        }

        Set( start, p, 0 );
        return p;
    }


    void StrPair::CollapseWhitespace()
    {
        // Adjusting _start would cause undefined behavior on delete[]
        TIXMLASSERT( ( _flags & NEEDS_DELETE ) == 0 );
        // Trim leading space.
        _start = XMLUtil::SkipWhiteSpace( _start, 0 );

        if ( *_start ) {
            const char* p = _start;	// the read pointer
            char* q = _start;	// the write pointer

            while( *p ) {
                if ( XMLUtil::IsWhiteSpace( *p )) {
                    p = XMLUtil::SkipWhiteSpace( p, 0 );
                    if ( *p == 0 ) {
                        break;    // don't write to q; this trims the trailing space.
                    }
                    *q = ' ';
                    ++q;
                }
                *q = *p;
                ++q;
                ++p;
            }
            *q = 0;
        }