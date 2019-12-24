
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
        static void ToStr(int64_t v, char* buffer, int bufferSize);
        static void ToStr(uint64_t v, char* buffer, int bufferSize);

        // converts strings to primitive types
        static bool	ToInt( const char* str, int* value );
        static bool ToUnsigned( const char* str, unsigned* value );
        static bool	ToBool( const char* str, bool* value );
        static bool	ToFloat( const char* str, float* value );
        static bool ToDouble( const char* str, double* value );
        static bool ToInt64(const char* str, int64_t* value);
        static bool ToUnsigned64(const char* str, uint64_t* value);
        // Changes what is serialized for a boolean value.
        // Default to "true" and "false". Shouldn't be changed
        // unless you have a special testing or compatibility need.
        // Be careful: static, global, & not thread safe.
        // Be sure to set static const memory as parameters.
        static void SetBoolSerialization(const char* writeTrue, const char* writeFalse);

    private:
        static const char* writeBoolTrue;
        static const char* writeBoolFalse;
    };


/** XMLNode is a base class for every object that is in the
	XML Document Object Model (DOM), except XMLAttributes.
	Nodes have siblings, a parent, and children which can
	be navigated. A node is always in a XMLDocument.
	The type of a XMLNode can be queried, and it can
	be cast to its more defined type.

	A XMLDocument allocates memory for all its Nodes.
	When the XMLDocument gets deleted, all its Nodes
	will also be deleted.

	@verbatim
	A Document can contain:	Element	(container or leaf)
							Comment (leaf)
							Unknown (leaf)
							Declaration( leaf )

	An Element can contain:	Element (container or leaf)
							Text	(leaf)
							Attributes (not on tree)
							Comment (leaf)
							Unknown (leaf)

	@endverbatim
*/
    class TINYXML2_LIB XMLNode
    {
        friend class XMLDocument;
        friend class XMLElement;
    public:

        /// Get the XMLDocument that owns this XMLNode.
        const XMLDocument* GetDocument() const	{
            TIXMLASSERT( _document );
            return _document;
        }
        /// Get the XMLDocument that owns this XMLNode.
        XMLDocument* GetDocument()				{
            TIXMLASSERT( _document );
            return _document;
        }

        /// Safely cast to an Element, or null.
        virtual XMLElement*		ToElement()		{
            return 0;
        }
        /// Safely cast to Text, or null.
        virtual XMLText*		ToText()		{
            return 0;
        }
        /// Safely cast to a Comment, or null.
        virtual XMLComment*		ToComment()		{
            return 0;
        }
        /// Safely cast to a Document, or null.
        virtual XMLDocument*	ToDocument()	{
            return 0;
        }
        /// Safely cast to a Declaration, or null.
        virtual XMLDeclaration*	ToDeclaration()	{
            return 0;
        }
        /// Safely cast to an Unknown, or null.
        virtual XMLUnknown*		ToUnknown()		{
            return 0;
        }

        virtual const XMLElement*		ToElement() const		{
            return 0;
        }
        virtual const XMLText*			ToText() const			{
            return 0;
        }
        virtual const XMLComment*		ToComment() const		{
            return 0;
        }
        virtual const XMLDocument*		ToDocument() const		{
            return 0;
        }
        virtual const XMLDeclaration*	ToDeclaration() const	{
            return 0;
        }
        virtual const XMLUnknown*		ToUnknown() const		{
            return 0;
        }

        /** The meaning of 'value' changes for the specific type.
            @verbatim
            Document:	empty (NULL is returned, not an empty string)
            Element:	name of the element
            Comment:	the comment text
            Unknown:	the tag contents
            Text:		the text string
            @endverbatim
        */
        const char* Value() const;

        /** Set the Value of an XML node.
            @sa Value()
        */
        void SetValue( const char* val, bool staticMem=false );

        /// Gets the line number the node is in, if the document was parsed from a file.
        int GetLineNum() const { return _parseLineNum; }

        /// Get the parent of this node on the DOM.
        const XMLNode*	Parent() const			{
            return _parent;
        }

        XMLNode* Parent()						{
            return _parent;
        }

        /// Returns true if this node has no children.
        bool NoChildren() const					{
            return !_firstChild;
        }

        /// Get the first child node, or null if none exists.
        const XMLNode*  FirstChild() const		{
            return _firstChild;
        }

        XMLNode*		FirstChild()			{
            return _firstChild;
        }

        /** Get the first child element, or optionally the first child
            element with the specified name.
        */
        const XMLElement* FirstChildElement( const char* name = 0 ) const;

        XMLElement* FirstChildElement( const char* name = 0 )	{
            return const_cast<XMLElement*>(const_cast<const XMLNode*>(this)->FirstChildElement( name ));
        }

        /// Get the last child node, or null if none exists.
        const XMLNode*	LastChild() const						{
            return _lastChild;
        }

        XMLNode*		LastChild()								{
            return _lastChild;
        }

        /** Get the last child element or optionally the last child
            element with the specified name.
        */
        const XMLElement* LastChildElement( const char* name = 0 ) const;

        XMLElement* LastChildElement( const char* name = 0 )	{
            return const_cast<XMLElement*>(const_cast<const XMLNode*>(this)->LastChildElement(name) );
        }

        /// Get the previous (left) sibling node of this node.
        const XMLNode*	PreviousSibling() const					{
            return _prev;
        }

        XMLNode*	PreviousSibling()							{
            return _prev;
        }

        /// Get the previous (left) sibling element of this node, with an optionally supplied name.
        const XMLElement*	PreviousSiblingElement( const char* name = 0 ) const ;

        XMLElement*	PreviousSiblingElement( const char* name = 0 ) {
            return const_cast<XMLElement*>(const_cast<const XMLNode*>(this)->PreviousSiblingElement( name ) );
        }

        /// Get the next (right) sibling node of this node.
        const XMLNode*	NextSibling() const						{
            return _next;
        }

        XMLNode*	NextSibling()								{
            return _next;
        }

        /// Get the next (right) sibling element of this node, with an optionally supplied name.
        const XMLElement*	NextSiblingElement( const char* name = 0 ) const;

        XMLElement*	NextSiblingElement( const char* name = 0 )	{
            return const_cast<XMLElement*>(const_cast<const XMLNode*>(this)->NextSiblingElement( name ) );
        }

        /**
            Add a child node as the last (right) child.
            If the child node is already part of the document,
            it is moved from its old location to the new location.
            Returns the addThis argument or 0 if the node does not
            belong to the same document.
        */
        XMLNode* InsertEndChild( XMLNode* addThis );

        XMLNode* LinkEndChild( XMLNode* addThis )	{
            return InsertEndChild( addThis );
        }
        /**
            Add a child node as the first (left) child.
            If the child node is already part of the document,
            it is moved from its old location to the new location.
            Returns the addThis argument or 0 if the node does not
            belong to the same document.
        */
        XMLNode* InsertFirstChild( XMLNode* addThis );
        /**
            Add a node after the specified child node.
            If the child node is already part of the document,
            it is moved from its old location to the new location.
            Returns the addThis argument or 0 if the afterThis node
            is not a child of this node, or if the node does not
            belong to the same document.
        */
        XMLNode* InsertAfterChild( XMLNode* afterThis, XMLNode* addThis );

        /**
            Delete all the children of this node.
        */
        void DeleteChildren();

        /**
            Delete a child of this node.
        */
        void DeleteChild( XMLNode* node );

        /**
            Make a copy of this node, but not its children.
            You may pass in a Document pointer that will be
            the owner of the new Node. If the 'document' is
            null, then the node returned will be allocated
            from the current Document. (this->GetDocument())

            Note: if called on a XMLDocument, this will return null.
        */
        virtual XMLNode* ShallowClone( XMLDocument* document ) const = 0;

        /**
            Make a copy of this node and all its children.

            If the 'target' is null, then the nodes will
            be allocated in the current document. If 'target'
            is specified, the memory will be allocated is the
            specified XMLDocument.

            NOTE: This is probably not the correct tool to
            copy a document, since XMLDocuments can have multiple
            top level XMLNodes. You probably want to use
            XMLDocument::DeepCopy()
        */
        XMLNode* DeepClone( XMLDocument* target ) const;

        /**
            Test if 2 nodes are the same, but don't test children.
            The 2 nodes do not need to be in the same Document.

            Note: if called on a XMLDocument, this will return false.
        */
        virtual bool ShallowEqual( const XMLNode* compare ) const = 0;

        /** Accept a hierarchical visit of the nodes in the TinyXML-2 DOM. Every node in the
            XML tree will be conditionally visited and the host will be called back
            via the XMLVisitor interface.

            This is essentially a SAX interface for TinyXML-2. (Note however it doesn't re-parse
            the XML for the callbacks, so the performance of TinyXML-2 is unchanged by using this
            interface versus any other.)

            The interface has been based on ideas from:

            - http://www.saxproject.org/
            - http://c2.com/cgi/wiki?HierarchicalVisitorPattern

            Which are both good references for "visiting".

            An example of using Accept():
            @verbatim
            XMLPrinter printer;
            tinyxmlDoc.Accept( &printer );
            const char* xmlcstr = printer.CStr();
            @endverbatim
        */
        virtual bool Accept( XMLVisitor* visitor ) const = 0;

        /**
            Set user data into the XMLNode. TinyXML-2 in
            no way processes or interprets user data.
            It is initially 0.
        */
        void SetUserData(void* userData)	{ _userData = userData; }

        /**
            Get user data set into the XMLNode. TinyXML-2 in
            no way processes or interprets user data.
            It is initially 0.
        */
        void* GetUserData() const			{ return _userData; }

    protected:
        explicit XMLNode( XMLDocument* );
        virtual ~XMLNode();

        virtual char* ParseDeep( char* p, StrPair* parentEndTag, int* curLineNumPtr);

        XMLDocument*	_document;
        XMLNode*		_parent;
        mutable StrPair	_value;
        int             _parseLineNum;

        XMLNode*		_firstChild;
        XMLNode*		_lastChild;

        XMLNode*		_prev;
        XMLNode*		_next;

        void*			_userData;

    private:
        MemPool*		_memPool;
        void Unlink( XMLNode* child );
        static void DeleteNode( XMLNode* node );
        void InsertChildPreamble( XMLNode* insertThis ) const;
        const XMLElement* ToElementWithName( const char* name ) const;

        XMLNode( const XMLNode& );	// not supported
        XMLNode& operator=( const XMLNode& );	// not supported
    };


/** XML text.

	Note that a text node can have child element nodes, for example:
	@verbatim
	<root>This is <b>bold</b></root>
	@endverbatim

	A text node can have 2 ways to output the next. "normal" output
	and CDATA. It will default to the mode it was parsed from the XML file and
	you generally want to leave it alone, but you can change the output mode with
	SetCData() and query it with CData().
*/
    class TINYXML2_LIB XMLText : public XMLNode
    {
        friend class XMLDocument;
    public:
        virtual bool Accept( XMLVisitor* visitor ) const;

        virtual XMLText* ToText()			{
            return this;
        }
        virtual const XMLText* ToText() const	{
            return this;
        }

        /// Declare whether this should be CDATA or standard text.
        void SetCData( bool isCData )			{
            _isCData = isCData;
        }
        /// Returns true if this is a CDATA text element.
        bool CData() const						{
            return _isCData;
        }

        virtual XMLNode* ShallowClone( XMLDocument* document ) const;
        virtual bool ShallowEqual( const XMLNode* compare ) const;

    protected:
        explicit XMLText( XMLDocument* doc )	: XMLNode( doc ), _isCData( false )	{}
        virtual ~XMLText()												{}

        char* ParseDeep( char* p, StrPair* parentEndTag, int* curLineNumPtr );

    private:
        bool _isCData;

        XMLText( const XMLText& );	// not supported
        XMLText& operator=( const XMLText& );	// not supported
    };


/** An XML Comment. */
    class TINYXML2_LIB XMLComment : public XMLNode
    {
        friend class XMLDocument;
    public:
        virtual XMLComment*	ToComment()					{
            return this;
        }
        virtual const XMLComment* ToComment() const		{
            return this;
        }

        virtual bool Accept( XMLVisitor* visitor ) const;

        virtual XMLNode* ShallowClone( XMLDocument* document ) const;
        virtual bool ShallowEqual( const XMLNode* compare ) const;

    protected:
        explicit XMLComment( XMLDocument* doc );
        virtual ~XMLComment();

        char* ParseDeep( char* p, StrPair* parentEndTag, int* curLineNumPtr);

    private:
        XMLComment( const XMLComment& );	// not supported
        XMLComment& operator=( const XMLComment& );	// not supported
    };


/** In correct XML the declaration is the first entry in the file.
	@verbatim
		<?xml version="1.0" standalone="yes"?>
	@endverbatim

	TinyXML-2 will happily read or write files without a declaration,
	however.

	The text of the declaration isn't interpreted. It is parsed
	and written as a string.
*/
    class TINYXML2_LIB XMLDeclaration : public XMLNode
    {
        friend class XMLDocument;
    public:
        virtual XMLDeclaration*	ToDeclaration()					{
            return this;
        }
        virtual const XMLDeclaration* ToDeclaration() const		{
            return this;
        }

        virtual bool Accept( XMLVisitor* visitor ) const;

        virtual XMLNode* ShallowClone( XMLDocument* document ) const;
        virtual bool ShallowEqual( const XMLNode* compare ) const;

    protected:
        explicit XMLDeclaration( XMLDocument* doc );
        virtual ~XMLDeclaration();

        char* ParseDeep( char* p, StrPair* parentEndTag, int* curLineNumPtr );

    private:
        XMLDeclaration( const XMLDeclaration& );	// not supported
        XMLDeclaration& operator=( const XMLDeclaration& );	// not supported
    };


/** Any tag that TinyXML-2 doesn't recognize is saved as an
	unknown. It is a tag of text, but should not be modified.
	It will be written back to the XML, unchanged, when the file
	is saved.

	DTD tags get thrown into XMLUnknowns.
*/
    class TINYXML2_LIB XMLUnknown : public XMLNode
    {
        friend class XMLDocument;
    public:
        virtual XMLUnknown*	ToUnknown()					{
            return this;
        }
        virtual const XMLUnknown* ToUnknown() const		{
            return this;
        }

        virtual bool Accept( XMLVisitor* visitor ) const;

        virtual XMLNode* ShallowClone( XMLDocument* document ) const;
        virtual bool ShallowEqual( const XMLNode* compare ) const;

    protected:
        explicit XMLUnknown( XMLDocument* doc );
        virtual ~XMLUnknown();

        char* ParseDeep( char* p, StrPair* parentEndTag, int* curLineNumPtr );

    private:
        XMLUnknown( const XMLUnknown& );	// not supported
        XMLUnknown& operator=( const XMLUnknown& );	// not supported
    };



/** An attribute is a name-value pair. Elements have an arbitrary
	number of attributes, each with a unique name.

	@note The attributes are not XMLNodes. You may only query the
	Next() attribute in a list.
*/
    class TINYXML2_LIB XMLAttribute
    {
        friend class XMLElement;
    public:
        /// The name of the attribute.
        const char* Name() const;

        /// The value of the attribute.
        const char* Value() const;

        /// Gets the line number the attribute is in, if the document was parsed from a file.
        int GetLineNum() const { return _parseLineNum; }

        /// The next attribute in the list.
        const XMLAttribute* Next() const {
            return _next;
        }

        /** IntValue interprets the attribute as an integer, and returns the value.
            If the value isn't an integer, 0 will be returned. There is no error checking;
            use QueryIntValue() if you need error checking.
        */
        int	IntValue() const {
            int i = 0;
            QueryIntValue(&i);
            return i;
        }

        int64_t Int64Value() const {
            int64_t i = 0;
            QueryInt64Value(&i);
            return i;
        }

        uint64_t Unsigned64Value() const {
            uint64_t i = 0;
            QueryUnsigned64Value(&i);
            return i;
        }

        /// Query as an unsigned integer. See IntValue()
        unsigned UnsignedValue() const			{
            unsigned i=0;
            QueryUnsignedValue( &i );
            return i;
        }
        /// Query as a boolean. See IntValue()
        bool	 BoolValue() const				{
            bool b=false;
            QueryBoolValue( &b );
            return b;
        }
        /// Query as a double. See IntValue()
        double 	 DoubleValue() const			{
            double d=0;
            QueryDoubleValue( &d );
            return d;
        }
        /// Query as a float. See IntValue()
        float	 FloatValue() const				{
            float f=0;
            QueryFloatValue( &f );
            return f;
        }

        /** QueryIntValue interprets the attribute as an integer, and returns the value
            in the provided parameter. The function will return XML_SUCCESS on success,
            and XML_WRONG_ATTRIBUTE_TYPE if the conversion is not successful.
        */
        XMLError QueryIntValue( int* value ) const;
        /// See QueryIntValue
        XMLError QueryUnsignedValue( unsigned int* value ) const;
        /// See QueryIntValue
        XMLError QueryInt64Value(int64_t* value) const;
        /// See QueryIntValue
        XMLError QueryUnsigned64Value(uint64_t* value) const;
        /// See QueryIntValue
        XMLError QueryBoolValue( bool* value ) const;