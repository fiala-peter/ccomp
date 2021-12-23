#ifndef STORAGE_H_INCLUDED
#define STORAGE_H_INCLUDED

enum class Storage
{
	NO_STORAGE,
	EXTERN,
	STATIC,
	AUTO,
	REGISTER
};

enum class Linkage
{
	UNDEFINED_LINKAGE,
	EXTERNAL_LINKAGE,
	INTERNAL_LINKAGE,
	NO_LINKAGE
};

enum class Scope
{
	FILE_SCOPE,
	BLOCK_SCOPE
};

#endif
