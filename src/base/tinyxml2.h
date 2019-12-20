
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

#ifndef TINYXML2_INCLUDED
#define TINYXML2_INCLUDED

#if defined(ANDROID_NDK) || defined(__BORLANDC__) || defined(__QNXNTO__)
#   include <ctype.h>
#   include <limits.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#	if defined(__PS3__)
#		include <stddef.h>
#	endif
#else
#   include <cctype>
#   include <climits>
#   include <cstdio>
#   include <cstdlib>
#   include <cstring>
#endif
#include <stdint.h>

/*
   TODO: intern strings instead of allocation.
*/
/*
	gcc:
        g++ -Wall -DTINYXML2_DEBUG tinyxml2.cpp xmltest.cpp -o gccxmltest.exe

    Formatting, Artistic Style:
        AStyle.exe --style=1tbs --indent-switches --break-closing-brackets --indent-preprocessor tinyxml2.cpp tinyxml2.h
*/

#if defined( _DEBUG ) || defined (__DEBUG__)
#   ifndef TINYXML2_DEBUG
#       define TINYXML2_DEBUG
#   endif
#endif

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

// We do not need dllexport or visibility("default")
#define TINYXML2_LIB

#if !defined(TIXMLASSERT)
#if defined(TINYXML2_DEBUG)
#   if defined(_MSC_VER)
#       // "(void)0," is for suppressing C4127 warning in "assert(false)", "assert(true)" and the like
#       define TIXMLASSERT( x )           if ( !((void)0,(x))) { __debugbreak(); }
#   elif defined (ANDROID_NDK)
#       include <android/log.h>
#       define TIXMLASSERT( x )           if ( !(x)) { __android_log_assert( "assert", "grinliz", "ASSERT in '%s' at %d.", __FILE__, __LINE__ ); }
#   else
#       include <assert.h>
#       define TIXMLASSERT                assert
#   endif
#else
#   define TIXMLASSERT( x )               {}
#endif
#endif

/* Versioning, past 1.0.14:
	http://semver.org/
*/
static const int TIXML2_MAJOR_VERSION = 9;
static const int TIXML2_MINOR_VERSION = 0;
static const int TIXML2_PATCH_VERSION = 0;

#define TINYXML2_MAJOR_VERSION 9
#define TINYXML2_MINOR_VERSION 0
#define TINYXML2_PATCH_VERSION 0

// A fixed element depth limit is problematic. There needs to be a
// limit to avoid a stack overflow. However, that limit varies per
// system, and the capacity of the stack. On the other hand, it's a trivial
// attack that can result from ill, malicious, or even correctly formed XML,
// so there needs to be a limit in place.
static const int TINYXML2_MAX_ELEMENT_DEPTH = 100;

namespace tinyxml2
{
    class XMLDocument;
    class XMLElement;
    class XMLAttribute;
    class XMLComment;
    class XMLText;
    class XMLDeclaration;
    class XMLUnknown;
    class XMLPrinter;

/*
	A class that wraps strings. Normally stores the start and end
	pointers into the XML file itself, and will apply normalization
	and entity translation if actually read. Can also store (and memory
	manage) a traditional char[]

    Isn't clear why TINYXML2_LIB is needed; but seems to fix #719
*/
    class TINYXML2_LIB StrPair
    {
    public:
        enum Mode {
            NEEDS_ENTITY_PROCESSING			= 0x01,
            NEEDS_NEWLINE_NORMALIZATION		= 0x02,
            NEEDS_WHITESPACE_COLLAPSING     = 0x04,

            TEXT_ELEMENT		            = NEEDS_ENTITY_PROCESSING | NEEDS_NEWLINE_NORMALIZATION,
            TEXT_ELEMENT_LEAVE_ENTITIES		= NEEDS_NEWLINE_NORMALIZATION,
            ATTRIBUTE_NAME		            = 0,
            ATTRIBUTE_VALUE		            = NEEDS_ENTITY_PROCESSING | NEEDS_NEWLINE_NORMALIZATION,
            ATTRIBUTE_VALUE_LEAVE_ENTITIES  = NEEDS_NEWLINE_NORMALIZATION,
            COMMENT							= NEEDS_NEWLINE_NORMALIZATION
        };

        StrPair() : _flags( 0 ), _start( 0 ), _end( 0 ) {}
        ~StrPair();

        void Set( char* start, char* end, int flags ) {
            TIXMLASSERT( start );
            TIXMLASSERT( end );
            Reset();
            _start  = start;
            _end    = end;
            _flags  = flags | NEEDS_FLUSH;
        }

        const char* GetStr();

        bool Empty() const {
            return _start == _end;
        }

        void SetInternedStr( const char* str ) {
            Reset();
            _start = const_cast<char*>(str);
        }

        void SetStr( const char* str, int flags=0 );

        char* ParseText( char* in, const char* endTag, int strFlags, int* curLineNumPtr );
        char* ParseName( char* in );

        void TransferTo( StrPair* other );
        void Reset();

    private:
        void CollapseWhitespace();

        enum {
            NEEDS_FLUSH = 0x100,
            NEEDS_DELETE = 0x200
        };

        int     _flags;
        char*   _start;
        char*   _end;

        StrPair( const StrPair& other );	// not supported
        void operator=( const StrPair& other );	// not supported, use TransferTo()
    };


/*
	A dynamic array of Plain Old Data. Doesn't support constructors, etc.
	Has a small initial memory pool, so that low or no usage will not
	cause a call to new/delete
*/
    template <class T, int INITIAL_SIZE>
    class DynArray
    {
    public:
        DynArray() :
                _mem( _pool ),
                _allocated( INITIAL_SIZE ),
                _size( 0 )
        {
        }

        ~DynArray() {
            if ( _mem != _pool ) {
                delete [] _mem;
            }
        }

        void Clear() {
            _size = 0;
        }

        void Push( T t ) {
            TIXMLASSERT( _size < INT_MAX );
            EnsureCapacity( _size+1 );
            _mem[_size] = t;
            ++_size;
        }

        T* PushArr( int count ) {
            TIXMLASSERT( count >= 0 );
            TIXMLASSERT( _size <= INT_MAX - count );
            EnsureCapacity( _size+count );
            T* ret = &_mem[_size];
            _size += count;
            return ret;
        }

        T Pop() {
            TIXMLASSERT( _size > 0 );
            --_size;
            return _mem[_size];
        }

        void PopArr( int count ) {
            TIXMLASSERT( _size >= count );
            _size -= count;
        }

        bool Empty() const					{
            return _size == 0;
        }

        T& operator[](int i)				{
            TIXMLASSERT( i>= 0 && i < _size );
            return _mem[i];
        }

        const T& operator[](int i) const	{
            TIXMLASSERT( i>= 0 && i < _size );
            return _mem[i];
        }

        const T& PeekTop() const            {
            TIXMLASSERT( _size > 0 );
            return _mem[ _size - 1];
        }

        int Size() const					{
            TIXMLASSERT( _size >= 0 );
            return _size;
        }

        int Capacity() const				{
            TIXMLASSERT( _allocated >= INITIAL_SIZE );
            return _allocated;
        }

        void SwapRemove(int i) {
            TIXMLASSERT(i >= 0 && i < _size);
            TIXMLASSERT(_size > 0);
            _mem[i] = _mem[_size - 1];
            --_size;
        }

        const T* Mem() const				{
            TIXMLASSERT( _mem );
            return _mem;
        }

        T* Mem() {
            TIXMLASSERT( _mem );
            return _mem;
        }

    private:
        DynArray( const DynArray& ); // not supported
        void operator=( const DynArray& ); // not supported

        void EnsureCapacity( int cap ) {
            TIXMLASSERT( cap > 0 );
            if ( cap > _allocated ) {
                TIXMLASSERT( cap <= INT_MAX / 2 );
                const int newAllocated = cap * 2;
                T* newMem = new T[newAllocated];
                TIXMLASSERT( newAllocated >= _size );
                memcpy( newMem, _mem, sizeof(T)*_size );	// warning: not using constructors, only works for PODs
                if ( _mem != _pool ) {
                    delete [] _mem;
                }
                _mem = newMem;
                _allocated = newAllocated;
            }
        }

        T*  _mem;
        T   _pool[INITIAL_SIZE];
        int _allocated;		// objects allocated
        int _size;			// number objects in use
    };


/*
	Parent virtual class of a pool for fast allocation
	and deallocation of objects.
*/
    class MemPool
    {
    public:
        MemPool() {}
        virtual ~MemPool() {}

        virtual int ItemSize() const = 0;