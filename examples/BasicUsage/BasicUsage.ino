#include <Arduino.h>
#include "DevIQ_FileSystem.h"

using namespace DeviceIQ_FileSystem;

FileSystem fs;

void setup() {
    Serial.begin(115200);
    while(!Serial){}

    if (!fs.Initialized()) fs.Begin(true);

    fs.Mkdir("/logs");
    fs.WriteFile("/config.txt", "hello\n", false);
    fs.AppendFile("/config.txt", "more\n");

    String txt;
    if (fs.ReadFile("/config.txt", txt)) Serial.println(txt);

    fs.ListDir("/", Serial);

    fs.CopyFile("/config.txt", "/copy.txt");
    fs.MoveFile("/copy.txt", "/moved.txt");

    const uint8_t data[] = {0x41,0x42,0x43,0x0A};
    fs.SafeSave("/safe.bin", data, sizeof(data));

    uint8_t buf[16];
    size_t outRead = 0;
    if (fs.ReadBinary("/safe.bin", buf, sizeof(buf), outRead)) {
        Serial.print("safe.bin bytes: ");
        Serial.println(outRead);
        for (size_t i=0;i<outRead;i++) { Serial.print((char)buf[i]); }
    }

    fs.Truncate("/config.txt", 5);

    fs.ListDir("/", Serial);
}

void loop() {
    delay(2000);
}