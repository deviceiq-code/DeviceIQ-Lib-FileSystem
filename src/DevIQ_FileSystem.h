#ifndef DevIQ_FileSystem_h
#define DevIQ_FileSystem_h

#pragma once

#include <Arduino.h>
#include <LittleFS.h>

namespace DeviceIQ_FileSystem {
    class FileSystem {
        private:
            bool mInitialized = false;
            
        public:
            inline FileSystem() { mInitialized = LittleFS.begin(); }
            virtual ~FileSystem() {}

            inline bool Initialized() { return mInitialized; }
            inline size_t TotalSpace() { return (mInitialized ? LittleFS.totalBytes() : 0); }
            inline size_t UsedSpace() { return (mInitialized ? LittleFS.usedBytes() : 0); }
            inline size_t FreeSpace() { return (mInitialized ? TotalSpace() - UsedSpace() : 0); }
            inline float PercentUsed() { return (mInitialized && TotalSpace() ? (float)UsedSpace() * 100.0f / (float)TotalSpace() : 0.0f); }
            inline float PercentFree() { return 100.0f - PercentUsed(); }

            inline fs::File OpenFile(const String& path, const char* mode) { if (!mInitialized) return fs::File(); return LittleFS.open(path, mode); }
            inline bool DeleteFile(const String& path) { return (mInitialized ? LittleFS.remove(path) : false); }
            inline bool RemoveFile(const String& path) { return DeleteFile(path); }
            inline bool RenameFile(const String& from, const String& to) { return (mInitialized ? LittleFS.rename(from, to) : false); }
            inline bool Exists(const String& path) { return (mInitialized ? LittleFS.exists(path) : false); }

            bool CopyFile(const String& origin, const String& destination);

            bool Begin(bool autoFormat = false);
            void End();
            bool Format();

            bool ReadFile(const String& path, String& out);
            bool WriteFile(const String& path, const String& data, bool append = false);
            inline bool AppendFile(const String& path, const String& data) { return WriteFile(path, data, true); }

            bool ReadBinary(const String& path, uint8_t* buf, size_t len, size_t& outRead);
            bool WriteBinary(const String& path, const uint8_t* buf, size_t len, bool append = false);

            size_t FileSize(const String& path);
            bool IsDirectory(const String& path);

            bool Mkdir(const String& path);
            bool Rmdir(const String& path);
            void ListDir(const String& path, Print& out);
            void ListDirRecursive(const String& path, Print& out, uint8_t levels = 5);

            bool SafeSave(const String& path, const uint8_t* data, size_t len);
            bool MoveFile(const String& from, const String& to);
            bool Touch(const String& path);
            bool Truncate(const String& path, size_t newSize);
            String TempName(const String& prefix = "/tmp_");
    };
}

#endif