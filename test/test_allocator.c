// SPDX-FileCopyrightText: 2025 Erin Catto
// SPDX-License-Identifier: MIT

#include "block_allocator.h"
#include "test_macros.h"

typedef struct
{
	int value1;
	float value2;
} Foo;

_Static_assert( sizeof( Foo ) >= sizeof( void* ), "too small" );

static int TestBlockAllocate( void )
{
	b3BlockAllocator allocator = b3CreateBlockAllocator( (int)sizeof( Foo ), 2 );

	Foo* item1 = b3AllocateElement( &allocator );
	ENSURE( item1 != NULL );

	Foo* item2 = b3AllocateElement( &allocator );
	ENSURE( item2 != NULL );

	b3DestroyBlockAllocator( &allocator );
	return 0;
}

static int TestBlockClear( void )
{
	b3BlockAllocator allocator = b3CreateBlockAllocator( (int)sizeof( Foo ), 0 );

	Foo* item1 = b3AllocateElement( &allocator );
	Foo* item2 = b3AllocateElement( &allocator );

	b3FreeElement( &allocator, item1 );
	b3FreeElement( &allocator, item2 );

	b3DestroyBlockAllocator( &allocator );
	return 0;
}

/// This prevents the misaligned memory panic in b3FreeElement (issue #78)
static int TestBlockAllocatorAlignment( void )
{
	// Choose a size that is deliberately not a multiple of pointer size.
	const int elementSize = 12; // 12 % 8 == 4 on 64-bit

	b3BlockAllocator allocator = b3CreateBlockAllocator( elementSize, 0 );

	// Allocate enough elements to cross block boundaries
	const int numToAllocate = B3_BLOCK_SIZE + 8;

	for ( int i = 0; i < numToAllocate; ++i )
	{
		void* p = b3AllocateElement( &allocator );
		ENSURE( p != NULL );

		// Every element pointer must be aligned for void* free-list storage
		ENSURE( ((uintptr_t)p & (sizeof(void*) - 1)) == 0 );

		b3FreeElement( &allocator, p );
	}

	// Test free list reuse path
	void* a = b3AllocateElement( &allocator );
	void* b = b3AllocateElement( &allocator );
	ENSURE( ((uintptr_t)a & (sizeof(void*) - 1)) == 0 );
	ENSURE( ((uintptr_t)b & (sizeof(void*) - 1)) == 0 );

	b3FreeElement( &allocator, a );

	void* c = b3AllocateElement( &allocator );
	ENSURE( ((uintptr_t)c & (sizeof(void*) - 1)) == 0 );

	b3FreeElement( &allocator, b );
	b3FreeElement( &allocator, c );

	b3DestroyBlockAllocator( &allocator );
	return 0;
}

int AllocatorTest( void )
{
	RUN_SUBTEST( TestBlockAllocate );
	RUN_SUBTEST( TestBlockClear );
	RUN_SUBTEST( TestBlockAllocatorAlignment );

	return 0;
}
