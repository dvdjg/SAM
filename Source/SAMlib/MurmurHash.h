/**
 * Funciones de hash rápido de 64 bits para evitar tener que usar las de criptografía.
 */
#ifndef MURMURHASH_H
#define MURMURHASH_H

typedef unsigned long long uint64_t;

#if defined(_M_X64) || defined(__amd64__) || defined(_LP64) || defined(__LP64__)
    // || (__SIZEOF_POINTER__ == 8) || UINTPTR_MAX == 0xffffffffffffffff
#define MurmurHash64 MurmurHash64A
#else
#define MurmurHash64 MurmurHash64B
#endif

// 64-bit hash for 64-bit platforms
uint64_t MurmurHash64A ( const void * key, int len, unsigned int seed = 104711);

// 64-bit hash for 32-bit platforms
uint64_t MurmurHash64B ( const void * key, int len, unsigned int seed = 104711);

#endif // MURMURHASH_H
