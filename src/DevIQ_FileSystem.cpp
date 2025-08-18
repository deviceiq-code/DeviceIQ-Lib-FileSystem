#include "DevIQ_FileSystem.h"

using namespace DeviceIQ_FileSystem;

bool FileSystem::CopyFile(const String& origin, const String& destination) {
    if (!mInitialized) return false;
    if (origin == destination) return true;

    fs::File src = OpenFile(origin, "r");

    if (!src) return false;
    if (src.isDirectory()) { src.close(); return false; }
    
    fs::File dst = OpenFile(destination, "w");
    if (!dst) { src.close(); return false; }
    
    uint8_t buf[1024];
    while (true) {
        size_t n = src.read(buf, sizeof(buf));
        if (n == 0) break;
        size_t written = 0;
        while (written < n) {
            size_t w = dst.write(buf + written, n - written);
            if (w == 0) { src.close(); dst.close(); return false; }
            written += w;
        }
    }

    dst.flush();
    dst.close();
    src.close();
    return true;
}

bool FileSystem::Begin(bool autoFormat) {
    if (mInitialized) return true;

    mInitialized = LittleFS.begin();
    
    if (!mInitialized && autoFormat) { LittleFS.format(); mInitialized = LittleFS.begin(); }
    return mInitialized;
}

void FileSystem::End() {
    if (mInitialized) { LittleFS.end(); mInitialized = false; }
}

bool FileSystem::Format() {
    if (!mInitialized) return false;

    LittleFS.end();
    bool ok = LittleFS.format();
    mInitialized = LittleFS.begin();
    return ok && mInitialized;
}

bool FileSystem::ReadFile(const String& path, String& out) {
    if (!mInitialized) return false;

    fs::File f = OpenFile(path, "r");
    if (!f) return false;

    if (f.isDirectory()) { f.close(); return false; }
    
    out = "";
    
    if (f.size() > 0) out.reserve((size_t)f.size());
    
    uint8_t buf[512];
    while (true) {
        size_t n = f.read(buf, sizeof(buf));
        if (n == 0) break;
        out.concat((const char*)buf, n);
    }
    
    f.close();
    return true;
}

bool FileSystem::WriteFile(const String& path, const String& data, bool append) {
    if (!mInitialized) return false;
    
    const char* mode = append ? "a" : "w";

    fs::File f = OpenFile(path, mode);
    if (!f) return false;

    const uint8_t* p = (const uint8_t*)data.c_str();
    size_t len = data.length();
    size_t written = 0;
    while (written < len) {
        size_t w = f.write(p + written, len - written);
        if (w == 0) { f.close(); return false; }
        written += w;
    }
    
    f.flush();
    f.close();
    return true;
}

bool FileSystem::ReadBinary(const String& path, uint8_t* buf, size_t len, size_t& outRead) {
    if (!mInitialized) return false;

    outRead = 0;
    if (!buf || len == 0) return false;
    
    fs::File f = OpenFile(path, "r");
    if (!f) return false;
    
    if (f.isDirectory()) { f.close(); return false; }
    
    while (outRead < len) {
        size_t n = f.read(buf + outRead, len - outRead);
        if (n == 0) break;
        outRead += n;
    }
    
    f.close();
    return true;
}

bool FileSystem::WriteBinary(const String& path, const uint8_t* buf, size_t len, bool append) {
    if (!mInitialized) return false;

    const char* mode = append ? "a" : "w";
    
    if (!buf && len > 0) return false;

    fs::File f = OpenFile(path, mode);
    if (!f) return false;

    size_t written = 0;
    while (written < len) {
        size_t w = f.write(buf + written, len - written);
        if (w == 0) { f.close(); return false; }
        written += w;
    }

    f.flush();
    f.close();
    return true;
}

size_t FileSystem::FileSize(const String& path) {
    if (!mInitialized) return 0;

    fs::File f = OpenFile(path, "r");
    if (!f) return 0;

    if (f.isDirectory()) { f.close(); return 0; }
    size_t s = f.size();
    
    f.close();
    return s;
}

bool FileSystem::IsDirectory(const String& path) {
    if (!mInitialized) return false;

    fs::File f = OpenFile(path, "r");
    if (!f) return false;
    
    bool d = f.isDirectory();
    f.close();
    return d;
}

bool FileSystem::Mkdir(const String& path) {
    if (!mInitialized) return false;
    return LittleFS.mkdir(path);
}

bool FileSystem::Rmdir(const String& path) {
    if (!mInitialized) return false;
    return LittleFS.rmdir(path);
}

void FileSystem::ListDir(const String& path, Print& out) {
    if (!mInitialized) { out.println(F("FS not initialized")); return; }

    fs::File dir = OpenFile(path, "r");
    if (!dir || !dir.isDirectory()) { out.println(F("Not a directory")); if (dir) dir.close(); return; }

    fs::File entry = dir.openNextFile();
    while (entry) {
        out.print(entry.name());
        
        if (entry.isDirectory()) {
            out.println("/");
        } else {
            out.print(" "); out.println(entry.size());
        }

        entry.close();
        entry = dir.openNextFile();
    }
    
    dir.close();
}

void FileSystem::ListDirRecursive(const String& path, Print& out, uint8_t levels) {
    if (!mInitialized) { out.println(F("FS not initialized")); return; }

    if (levels == 0) return;

    fs::File dir = OpenFile(path, "r");
    if (!dir || !dir.isDirectory()) { out.println(F("Not a directory")); if (dir) dir.close(); return; }

    fs::File entry = dir.openNextFile();
    while (entry) {
        String name = String(path) + (path.endsWith("/") ? "" : "/") + entry.name();
        out.println(name);

        if (entry.isDirectory()) {
            entry.close();
            ListDirRecursive(name, out, levels - 1);
        } else {
            entry.close();
        }

        entry = dir.openNextFile();
    }

    dir.close();
}

bool FileSystem::SafeSave(const String& path, const uint8_t* data, size_t len) {
    if (!mInitialized) return false;

    String tmp = path + ".tmp";

    fs::File f = OpenFile(tmp, "w");
    if (!f) return false;

    size_t written = 0;
    while (written < len) {
        size_t w = f.write(data + written, len - written);
        
        if (w == 0) {
            f.close();
            DeleteFile(tmp);
            return false;
        }

        written += w;
    }

    f.flush();
    f.close();

    if (Exists(path)) {
        if (!DeleteFile(path)) {
            DeleteFile(tmp);
            return false;
        }
    }

    if (!RenameFile(tmp, path)) { DeleteFile(tmp); return false; }
    return true;
}

bool FileSystem::MoveFile(const String& from, const String& to) {
    if (!mInitialized) return false;
    
    if (from == to) return true;
    if (Exists(to)) { if (!DeleteFile(to)) return false; }
    return RenameFile(from, to);
}

bool FileSystem::Touch(const String& path) {
  if (!mInitialized) return false;

  fs::File f = OpenFile(path, "a");
  if (!f) return false;

  f.flush();
  f.close();
  return true;
}

bool FileSystem::Truncate(const String& path, size_t newSize) {
    if (!mInitialized) return false;

    if (!Exists(path)) return false;
    if (newSize == 0) return WriteBinary(path, nullptr, 0, false);
    String tmp = path + ".trunc";

    fs::File src = OpenFile(path, "r");
    if (!src) return false;

    if (src.isDirectory()) { src.close(); return false; }

    fs::File dst = OpenFile(tmp, "w");
    if (!dst) { src.close(); return false; }

    size_t remaining = newSize;
    uint8_t buf[512];
    while (remaining > 0) {
        size_t n = src.read(buf, remaining > sizeof(buf) ? sizeof(buf) : remaining);
        if (n == 0) break;
        size_t written = 0;
        while (written < n) {
            size_t w = dst.write(buf + written, n - written);
            if (w == 0) { src.close(); dst.close(); DeleteFile(tmp); return false; }
            written += w;
        }
        remaining -= n;
    }

    dst.flush();
    dst.close();
    src.close();

    if (Exists(path)) { if (!DeleteFile(path)) { DeleteFile(tmp); return false; } }
    if (!RenameFile(tmp, path)) { DeleteFile(tmp); return false; }
    return true;
}

String FileSystem::TempName(const String& prefix) {
    String s = prefix;
    s += String((uint32_t)millis());
    s += ".tmp";
    return s;
}
