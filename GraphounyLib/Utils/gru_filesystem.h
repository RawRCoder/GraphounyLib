#pragma once
#include "..\gr_shared.h"
#include "gru_array.h"

// Does the specified path exists? (uses boost)
extern bool PathExists(const std::wstring& path);
extern u64 GetFileSize(const std::wstring& path);
extern bool IsPathADirectory(const std::wstring& path);
extern bool IsPathAFile(const std::wstring& path);
extern std::wstring RelativizePath(const std::wstring& fullPath, const std::wstring& basePath);

extern void IterateFilesInDirectory(const std::wstring& directory, Array<std::wstring>::DConstIterateT func, pointer obj = nullptr);
extern void IterateFilesAndSubdirsInDirectory(const std::wstring& directory, Array<std::wstring>::DConstIterateT func, pointer obj = nullptr);

template <class T>
extern T ReadGeneric(std::istream* src, bool srcIsBE);

#define __DEF_STREAM_READER(T) inline static T Read_##T(std::istream* src, bool srcIsBE = false) { return ReadGeneric<T>(src, srcIsBE); }
__DEF_STREAM_READER(i16);
__DEF_STREAM_READER(u16);
__DEF_STREAM_READER(i32);
__DEF_STREAM_READER(u32);
__DEF_STREAM_READER(i64);
__DEF_STREAM_READER(u64);
__DEF_STREAM_READER(f32);
__DEF_STREAM_READER(f64);

template <class T>
extern void WriteGeneric(std::ostream* t, T value, bool BE = false);
#define __DEF_STREAM_WRITER(T) inline static void Write_##T(std::ostream* t, T value, bool BE = false) { WriteGeneric<T>(t, value, BE); }
__DEF_STREAM_WRITER(i16);
__DEF_STREAM_WRITER(u16);
__DEF_STREAM_WRITER(i32);
__DEF_STREAM_WRITER(u32);
__DEF_STREAM_WRITER(i64);
__DEF_STREAM_WRITER(u64);
__DEF_STREAM_WRITER(f32);
__DEF_STREAM_WRITER(f64);

extern void WriteFileToStream(std::ostream* sOut, std::wstring wsName, u32 bufferSize = 0x10000);