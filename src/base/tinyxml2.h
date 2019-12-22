
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
        virtual void* Alloc() = 0;
        virtual void Free( void* ) = 0;
        virtual void SetTracked() = 0;
    };


/*
	Template child class to create pools of the correct type.
*/
    template< int ITEM_SIZE >
    class MemPoolT : public MemPool
    {
    public:
        MemPoolT() : _blockPtrs(), _root(0), _currentAllocs(0), _nAllocs(0), _maxAllocs(0), _nUntracked(0)	{}
        ~MemPoolT() {
            MemPoolT< ITEM_SIZE >::Clear();
        }

        void Clear() {
            // Delete the blocks.
            while( !_blockPtrs.Empty()) {
                Block* lastBlock = _blockPtrs.Pop();
                delete lastBlock;
            }
            _root = 0;
            _currentAllocs = 0;
            _nAllocs = 0;
            _maxAllocs = 0;
            _nUntracked = 0;
        }

        virtual int ItemSize() const	{
            return ITEM_SIZE;
        }
        int CurrentAllocs() const		{
            return _currentAllocs;
        }

        virtual void* Alloc() {
            if ( !_root ) {
                // Need a new block.
                Block* block = new Block();
                _blockPtrs.Push( block );

                Item* blockItems = block->items;
                for( int i = 0; i < ITEMS_PER_BLOCK - 1; ++i ) {
                    blockItems[i].next = &(blockItems[i + 1]);
                }
                blockItems[ITEMS_PER_BLOCK - 1].next = 0;
                _root = blockItems;
            }
            Item* const result = _root;
            TIXMLASSERT( result != 0 );
            _root = _root->next;

            ++_currentAllocs;
            if ( _currentAllocs > _maxAllocs ) {
                _maxAllocs = _currentAllocs;
            }
            ++_nAllocs;
            ++_nUntracked;
            return result;
        }

        virtual void Free( void* mem ) {
            if ( !mem ) {
                return;
            }
            --_currentAllocs;
            Item* item = static_cast<Item*>( mem );
#ifdef TINYXML2_DEBUG
            memset( item, 0xfe, sizeof( *item ) );
#endif
            item->next = _root;
            _root = item;
        }
        void Trace( const char* name ) {
            printf( "Mempool %s watermark=%d [%dk] current=%d size=%d nAlloc=%d blocks=%d\n",
                    name, _maxAllocs, _maxAllocs * ITEM_SIZE / 1024, _currentAllocs,
                    ITEM_SIZE, _nAllocs, _blockPtrs.Size() );
        }

        void SetTracked() {
            --_nUntracked;
        }

        int Untracked() const {
            return _nUntracked;
        }

        // This number is perf sensitive. 4k seems like a good tradeoff on my machine.
        // The test file is large, 170k.
        // Release:		VS2010 gcc(no opt)
        //		1k:		4000
        //		2k:		4000
        //		4k:		3900	21000
        //		16k:	5200
        //		32k:	4300
        //		64k:	4000	21000
        // Declared public because some compilers do not accept to use ITEMS_PER_BLOCK
        // in private part if ITEMS_PER_BLOCK is private
        enum { ITEMS_PER_BLOCK = (4 * 1024) / ITEM_SIZE };

    private:
        MemPoolT( const MemPoolT& ); // not supported
        void operator=( const MemPoolT& ); // not supported

        union Item {
            Item*   next;
            char    itemData[ITEM_SIZE];
        };
        struct Block {
            Item items[ITEMS_PER_BLOCK];
        };
        DynArray< Block*, 10 > _blockPtrs;
        Item* _root;

        int _currentAllocs;
        int _nAllocs;
        int _maxAllocs;
        int _nUntracked;
    };



/**
	Implements the interface to the "Visitor pattern" (see the Accept() method.)
	If you call the Accept() method, it requires being passed a XMLVisitor
	class to handle callbacks. For nodes that contain other nodes (Document, Element)
	you will get called with a VisitEnter/VisitExit pair. Nodes that are always leafs
	are simply called with Visit().

	If you return 'true' from a Visit method, recursive parsing will continue. If you return
	false, <b>no children of this node or its siblings</b> will be visited.

	All flavors of Visit methods have a default implementation that returns 'true' (continue
	visiting). You need to only override methods that are interesting to you.

	Generally Accept() is called on the XMLDocument, although all nodes support visiting.

	You should never change the document from a callback.

	@sa XMLNode::Accept()
*/
    class TINYXML2_LIB XMLVisitor
    {
    public:
        virtual ~XMLVisitor() {}

        /// Visit a document.
        virtual bool VisitEnter( const XMLDocument& /*doc*/ )			{
            return true;
        }
        /// Visit a document.
        virtual bool VisitExit( const XMLDocument& /*doc*/ )			{
            return true;
        }

        /// Visit an element.
        virtual bool VisitEnter( const XMLElement& /*element*/, const XMLAttribute* /*firstAttribute*/ )	{
            return true;
        }
        /// Visit an element.
        virtual bool VisitExit( const XMLElement& /*element*/ )			{
            return true;
        }

        /// Visit a declaration.
        virtual bool Visit( const XMLDeclaration& /*declaration*/ )		{
            return true;
        }
        /// Visit a text node.
        virtual bool Visit( const XMLText& /*text*/ )					{
            return true;
        }
        /// Visit a comment node.
        virtual bool Visit( const XMLComment& /*comment*/ )				{
            return true;
        }
        /// Visit an unknown node.
        virtual bool Visit( const XMLUnknown& /*unknown*/ )				{
            return true;
        }
    };

// WARNING: must match XMLDocument::_errorNames[]
    enum XMLError {
        XML_SUCCESS = 0,
        XML_NO_ATTRIBUTE,
        XML_WRONG_ATTRIBUTE_TYPE,
        XML_ERROR_FILE_NOT_FOUND,
        XML_ERROR_FILE_COULD_NOT_BE_OPENED,
        XML_ERROR_FILE_READ_ERROR,
        XML_ERROR_PARSING_ELEMENT,
        XML_ERROR_PARSING_ATTRIBUTE,
        XML_ERROR_PARSING_TEXT,
        XML_ERROR_PARSING_CDATA,
        XML_ERROR_PARSING_COMMENT,
        XML_ERROR_PARSING_DECLARATION,
        XML_ERROR_PARSING_UNKNOWN,
        XML_ERROR_EMPTY_DOCUMENT,
        XML_ERROR_MISMATCHED_ELEMENT,
        XML_ERROR_PARSING,
        XML_CAN_NOT_CONVERT_TEXT,
        XML_NO_TEXT_NODE,
        XML_ELEMENT_DEPTH_EXCEEDED,

        XML_ERROR_COUNT
    };


/*
	Utility functionality.
*/
    class TINYXML2_LIB XMLUtil
    {
    public:
        static const char* SkipWhiteSpace( const char* p, int* curLineNumPtr )	{
            TIXMLASSERT( p );

            while( IsWhiteSpace(*p) ) {
                if (curLineNumPtr && *p == '\n') {
                    ++(*curLineNumPtr);
                }
                ++p;
            }
            TIXMLASSERT( p );
            return p;
        }
        static char* SkipWhiteSpace( char* const p, int* curLineNumPtr ) {
            return const_cast<char*>( SkipWhiteSpace( const_cast<const char*>(p), curLineNumPtr ) );
        }

        // Anything in the high order range of UTF-8 is assumed to not be whitespace. This isn't
        // correct, but simple, and usually works.
        static bool IsWhiteSpace( char p )					{
            return !IsUTF8Continuation(p) && isspace( static_cast<unsigned char>(p) );
        }

        inline static bool IsNameStartChar( unsigned char ch ) {
            if ( ch >= 128 ) {
                // This is a heuristic guess in attempt to not implement Unicode-aware isalpha()
                return true;
            }
            if ( isalpha( ch ) ) {
                return true;
            }
            return ch == ':' || ch == '_';
        }

        inline static bool IsNameChar( unsigned char ch ) {
            return IsNameStartChar( ch )
                   || isdigit( ch )
                   || ch == '.'
                   || ch == '-';
        }

        inline static bool IsPrefixHex( const char* p) {
            p = SkipWhiteSpace(p, 0);
            return p && *p == '0' && ( *(p + 1) == 'x' || *(p + 1) == 'X');
        }

        inline static bool StringEqual( const char* p, const char* q, int nChar=INT_MAX )  {
            if ( p == q ) {
                return true;
            }
            TIXMLASSERT( p );
            TIXMLASSERT( q );
            TIXMLASSERT( nChar >= 0 );
            return strncmp( p, q, nChar ) == 0;
        }

        inline static bool IsUTF8Continuation( const char p ) {
            return ( p & 0x80 ) != 0;
        }

        static const char* ReadBOM( const char* p, bool* hasBOM );
        // p is the starting location,
        // the UTF-8 value of the entity will be placed in value, and length filled in.
        static const char* GetCharacterRef( const char* p, char* value, int* length );
        static void ConvertUTF32ToUTF8( unsigned long input, char* output, int* length );

        // converts primitive types to strings
        static void ToStr( int v, char* buffer, int bufferSize );
        static void ToStr( unsigned v, char* buffer, int bufferSize );
        static void ToStr( bool v, char* buffer, int bufferSize );
        static void ToStr( float v, char* buffer, int bufferSize );
        static void ToStr( double v, char* buffer, int bufferSize );