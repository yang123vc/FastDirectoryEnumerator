/* FastDirectoryEnumerator
Enumerates very, very large directories quickly by directly using kernel syscalls. For POSIX and Windows.
(C) 2013 Niall Douglas http://www.nedprod.com/
File created: Aug 2013
*/

#ifndef FASTDIRECTORYENUMERATOR_H
#define FASTDIRECTORYENUMERATOR_H

#ifndef FASTDIRECTORYENUMERATOR_API
#define FASTDIRECTORYENUMERATOR_API
#endif

#include "std_filesystem.hpp"
#include <sys/types.h>

namespace FastDirectoryEnumerator
{
	struct timespec
	{
		time_t tv_sec;
		long tv_nsec;
	};
	typedef unsigned long long off_t;

	/*! \brief Bitflags for availability of metadata

	These replicate the `stat` structure. `st_birthtime` is a BSD extension, but
	also is available on Windows.
	*/
	union have_metadata_flags
	{
		struct
		{
			unsigned int have_dev:1, have_ino:1, have_mode:1, have_nlink:1, have_uid:1, have_gid:1,
				have_rdev:1, have_atimespec:1, have_mtimespec:1, have_ctimespec:1, have_size:1, 
				have_allocated:1, have_flags:1, have_gen:1, have_birthtimespec:1;
		};
		unsigned int value;
	};
	//! An entry in a directory
	class FASTDIRECTORYENUMERATOR_API directory_entry
	{
		friend std::unique_ptr<std::vector<directory_entry>> enumerate_directory(void *h, size_t maxitems, std::filesystem::path glob=std::filesystem::path(), bool namesonly=false);

		std::filesystem::path leafname;
		have_metadata_flags have_metadata;
		struct stat_t // Derived from BSD
		{
			uint64_t        st_dev;           /* inode of device containing file */
			uint64_t        st_ino;           /* inode of file */
			uint16_t        st_mode;          /* perms of file */
			int16_t         st_nlink;         /* number of hard links */
			int16_t         st_uid;           /* user ID of the file */
			int16_t         st_gid;           /* group ID of the file */
			dev_t           st_rdev;          /* type of file */
			struct timespec st_atimespec;     /* time of last access */
			struct timespec st_mtimespec;     /* time of last data modification */
			struct timespec st_ctimespec;     /* time of last status change */
			off_t           st_size;          /* file size, in bytes */
			off_t           st_allocated;     /* bytes allocated for file */
			uint32_t        st_flags;         /* user defined flags for file */
			uint32_t        st_gen;           /* file generation number */
			int32_t         st_lspare;
			struct timespec st_birthtimespec; /* time of file creation (birth) */
		} stat;
		void _int_fetch(have_metadata_flags wanted);
	public:
		//! Constructs an instance
		directory_entry()
		{
			have_metadata.value=0;
			memset(&stat, 0, sizeof(stat));
		}
		bool operator==(const directory_entry& rhs) const BOOST_NOEXCEPT_OR_NOTHROW { return leafname == rhs.leafname; }
		bool operator!=(const directory_entry& rhs) const BOOST_NOEXCEPT_OR_NOTHROW { return leafname != rhs.leafname; }
		bool operator< (const directory_entry& rhs) const BOOST_NOEXCEPT_OR_NOTHROW { return leafname < rhs.leafname; }
		bool operator<=(const directory_entry& rhs) const BOOST_NOEXCEPT_OR_NOTHROW { return leafname <= rhs.leafname; }
		bool operator> (const directory_entry& rhs) const BOOST_NOEXCEPT_OR_NOTHROW { return leafname > rhs.leafname; }
		bool operator>=(const directory_entry& rhs) const BOOST_NOEXCEPT_OR_NOTHROW { return leafname >= rhs.leafname; }
		//! The name of the directory entry
		std::filesystem::path name() const BOOST_NOEXCEPT_OR_NOTHROW { return leafname; }
		//! A bitfield of what metadata is ready right now
		have_metadata_flags metadata_ready() const BOOST_NOEXCEPT_OR_NOTHROW { return have_metadata; }
		//! Fetches the specified metadata, returning that newly available. This is a blocking call.
		have_metadata_flags fetch_metadata(have_metadata_flags wanted)
		{
			have_metadata_flags tofetch;
			wanted.value&=metadata_supported().value;
			tofetch.value=wanted.value&~have_metadata.value;
			if(tofetch.value) _int_fetch(tofetch);
			return have_metadata;
		}
		//! Returns st_dev
		uint64_t st_dev() { if(!have_metadata.have_dev) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_dev=1; _int_fetch(tofetch); } return stat.st_dev; }
		//! Returns st_ino
		uint64_t st_ino() { if(!have_metadata.have_ino) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_ino=1; _int_fetch(tofetch); } return stat.st_ino; }
		//! Returns st_mode
		uint16_t st_mode() { if(!have_metadata.have_mode) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_mode=1; _int_fetch(tofetch); } return stat.st_mode; }
		//! Returns st_nlink
		int16_t st_nlink() { if(!have_metadata.have_nlink) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_nlink=1; _int_fetch(tofetch); } return stat.st_nlink; }
		//! Returns st_uid
		int16_t st_uid() { if(!have_metadata.have_uid) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_uid=1; _int_fetch(tofetch); } return stat.st_uid; }
		//! Returns st_gid
		int16_t st_gid() { if(!have_metadata.have_gid) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_gid=1; _int_fetch(tofetch); } return stat.st_gid; }
		//! Returns st_rdev
		dev_t st_rdev() { if(!have_metadata.have_rdev) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_rdev=1; _int_fetch(tofetch); } return stat.st_rdev; }
		//! Returns st_atimespec
		struct timespec st_atimespec() { if(!have_metadata.have_atimespec) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_atimespec=1; _int_fetch(tofetch); } return stat.st_atimespec; }
		//! Returns st_mtimespec
		struct timespec st_mtimespec() { if(!have_metadata.have_mtimespec) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_mtimespec=1; _int_fetch(tofetch); } return stat.st_mtimespec; }
		//! Returns st_ctimespec
		struct timespec st_ctimespec() { if(!have_metadata.have_ctimespec) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_ctimespec=1; _int_fetch(tofetch); } return stat.st_ctimespec; }
		//! Returns st_size
		off_t st_size() { if(!have_metadata.have_size) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_size=1; _int_fetch(tofetch); } return stat.st_size; }
		//! Returns st_allocated
		off_t st_allocated() { if(!have_metadata.have_allocated) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_allocated=1; _int_fetch(tofetch); } return stat.st_allocated; }
		//! Returns st_flags
		uint32_t st_flags() { if(!have_metadata.have_flags) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_flags=1; _int_fetch(tofetch); } return stat.st_flags; }
		//! Returns st_gen
		uint32_t st_gen() { if(!have_metadata.have_gen) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_gen=1; _int_fetch(tofetch); } return stat.st_gen; }
		//! Returns st_birthtimespec
		struct timespec st_birthtimespec() { if(!have_metadata.have_birthtimespec) { have_metadata_flags tofetch; tofetch.value=0; tofetch.have_birthtimespec=1; _int_fetch(tofetch); } return stat.st_birthtimespec; }

		//! A bitfield of what metadata is available on this platform. This doesn't mean all is available for every filing system.
		static have_metadata_flags metadata_supported() BOOST_NOEXCEPT_OR_NOTHROW;
	};

	//! Starts the enumeration of a directory. This actually simply opens the directory and returns the fd or `HANDLE`.
	extern FASTDIRECTORYENUMERATOR_API void *begin_enumerate_directory(std::filesystem::path path);
	//! Ends the enumeration of a directory. This simply closes the fd or `HANDLE`.
	extern FASTDIRECTORYENUMERATOR_API void end_enumerate_directory(void *h);
	/*! \brief Enumerates a directory as quickly as possible, retrieving all zero-cost metadata.

	Note that maxitems items may not be retreived for various reasons, including that glob filtered them out.
	A zero item vector return is entirely possible, but this does not mean end of enumeration: only a null
	shared_ptr means that.

	Windows returns the common stat items, Linux usually returns only `st_rdev`, other POSIX just the leafname.
	Setting namesonly to true returns as little information as possible.

	Suggested code for merging chunks of enumeration into a single vector:
	\code
	void *h=begin_enumerate_directory(_L("testdir"));
	std::unique_ptr<std::vector<directory_entry>> enumeration, chunk;
	while((chunk=enumerate_directory(h, NUMBER_OF_FILES)))
		if(!enumeration)
			enumeration=std::move(chunk);
		else
			enumeration->insert(enumeration->end(), std::make_move_iterator(chunk->begin()), std::make_move_iterator(chunk->end()));
	end_enumerate_directory(h);
	\endcode
	*/
	extern FASTDIRECTORYENUMERATOR_API std::unique_ptr<std::vector<directory_entry>> enumerate_directory(void *h, size_t maxitems, std::filesystem::path glob, bool namesonly);
} // namespace

namespace std
{
	template<> struct hash<FastDirectoryEnumerator::directory_entry>
	{
	public:
		size_t operator()(const FastDirectoryEnumerator::directory_entry& p) const
		{
			return boost::filesystem::hash_value(p.name());
		}
	};
}

#endif
