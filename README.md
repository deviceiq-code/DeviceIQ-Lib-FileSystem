DeviceIQ FileSystem Library
===========================
`DeviceIQ FileSystem Library` is a lightweight file system utility library for Arduino/ESP32 projects using LittleFS. It provides file management, directory operations, and safe write utilities with a compact, easy-to-use API.

Features
--------
- Initialization
  - Mount (`Begin`), unmount (`End`), and format (`Format`) LittleFS
- File information
  - `TotalSpace`, `UsedSpace`, `FreeSpace`, `PercentUsed`, `PercentFree`
  - `FileSize`, `IsDirectory`
- Basic file operations
  - `OpenFile`, `Exists`, `DeleteFile`/`RemoveFile`, `RenameFile`, `MoveFile`, `Touch`, `Truncate`
- Data read/write
  - Text: `ReadFile`, `WriteFile`, `AppendFile`
  - Binary: `ReadBinary`, `WriteBinary`
  - Safe write: `SafeSave` (temp file + atomic rename)
- Directories
  - `Mkdir`, `Rmdir`, `ListDir`, `ListDirRecursive`
- Utilities
  - `CopyFile`, `TempName`

Installation
------------
1. Copy `DevIQ_FileSystem.h` and `DevIQ_FileSystem.cpp` into your project (`src/` for PlatformIO).
2. Ensure your ESP32/ESP8266 core includes LittleFS (`<LittleFS.h>`).
3. Optional: add an example under `examples/` (see `FileOpsDemo` below).

Usage Example
-------------

```cpp
#include <Arduino.h>
#include "DevIQ_FileSystem.h"
using namespace DeviceIQ_FileSystem;

FileSystem fs;

void setup() {
  Serial.begin(115200);
  if (!fs.Initialized()) fs.Begin(true);

  fs.WriteFile("/test.txt", "Hello\n");
  fs.AppendFile("/test.txt", "World\n");

  String txt;
  if (fs.ReadFile("/test.txt", txt)) Serial.println(txt);

  fs.ListDir("/", Serial);
}

void loop() {}
```

API Summary
-----------

### Initialization
| Method | Description |
|---|---|
| `bool Begin(bool autoFormat=false)` | Mounts LittleFS; formats first if `autoFormat` is true and mount fails. |
| `void End()` | Unmounts LittleFS. |
| `bool Format()` | Formats LittleFS and remounts. |

### File Information
| Method | Description |
|---|---|
| `size_t TotalSpace()` | Total bytes in partition. |
| `size_t UsedSpace()` | Used bytes. |
| `size_t FreeSpace()` | Remaining bytes. |
| `float PercentUsed()` | Usage percentage. |
| `float PercentFree()` | 100 − `PercentUsed`. |
| `size_t FileSize(const String& path)` | Size in bytes (0 if not a file). |
| `bool IsDirectory(const String& path)` | True if path is a directory. |

### Basic Operations
| Method | Description |
|---|---|
| `fs::File OpenFile(const String& path, const char* mode)` | Opens a file (`"r"`, `"w"`, `"a"`, etc.). |
| `bool Exists(const String& path)` | Checks if path exists. |
| `bool DeleteFile(const String& path)` | Deletes a file. |
| `bool RemoveFile(const String& path)` | Alias of `DeleteFile`. |
| `bool RenameFile(const String& from, const String& to)` | Renames a file. |
| `bool MoveFile(const String& from, const String& to)` | Rename with overwrite semantics. |
| `bool Touch(const String& path)` | Creates an empty file or updates it. |
| `bool Truncate(const String& path, size_t newSize)` | Shrinks or recreates file to `newSize`. |

### Data Read/Write
| Method | Description |
|---|---|
| `bool ReadFile(const String& path, String& out)` | Reads text to `out`. |
| `bool WriteFile(const String& path, const String& data, bool append=false)` | Writes/appends text. |
| `bool AppendFile(const String& path, const String& data)` | Appends text (shortcut). |
| `bool ReadBinary(const String& path, uint8_t* buf, size_t len, size_t& outRead)` | Reads up to `len` bytes. |
| `bool WriteBinary(const String& path, const uint8_t* buf, size_t len, bool append=false)` | Writes binary data. |
| `bool SafeSave(const String& path, const uint8_t* data, size_t len)` | Atomic write via `path.tmp` + rename. |

### Directories
| Method | Description |
|---|---|
| `bool Mkdir(const String& path)` | Creates directory. |
| `bool Rmdir(const String& path)` | Removes directory. |
| `void ListDir(const String& path, Print& out)` | Lists a directory. |
| `void ListDirRecursive(const String& path, Print& out, uint8_t levels=5)` | Recursive listing. |

### Utilities
| Method | Description |
|---|---|
| `bool CopyFile(const String& origin, const String& destination)` | Copies file contents. |
| `String TempName(const String& prefix="/tmp_")` | Returns a temp filename. |

Notes
-----
- Designed for LittleFS on ESP32/ESP8266.
- `Begin(true)` is handy during development to auto-format on first mount failure.
- `SafeSave` helps prevent corrupted files on reboot/power loss.

License
-------
This library is released under the MIT License.
