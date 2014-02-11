// Tft Touch Shield V1.0 demo - Display BMP file

#include <SD.h>
#include <TFT.h>

// In the SD card, place 24 bit color BMP files (be sure they are 24-bit!)
// the file itself
File bmpFile;

// information we extract about the bitmap file
int bmpWidth, bmpHeight;
uint8_t bmpDepth, bmpImageoffset;

void setup()
{
    Serial.begin(9600);

    Tft.init();

    Serial.print("Initializing SD card...");
    pinMode(53, OUTPUT); //53 is used as chip select pin

    if (!SD.begin(53)) { //53 is used as chip select pin
        Serial.println("failed!");
        return;
    }
    Serial.println("SD OK!");

}

void loop()
{
    Tft.setOrientation(0);

    //Image 1
    bmpFile = SD.open("flower.bmp");
    if (! bmpFile) {
        Serial.println("didnt find image");
        while (1);
    }

    if (! bmpReadHeader(bmpFile)) {
        Serial.println("bad bmp");
        return;
    }

    Serial.print("image size ");
    Serial.print(bmpWidth, DEC);
    Serial.print(", ");
    Serial.println(bmpHeight, DEC);
    bmpdraw(bmpFile, 0, 0);
    delay(3000);
    bmpFile.close();

    //Image 2
    bmpFile = SD.open("hibiscus.bmp");
    if (! bmpFile) {
        Serial.println("didnt find image");
        while (1);
    }

    if (! bmpReadHeader(bmpFile)) {
        Serial.println("bad bmp");
        return;
    }

    Serial.print("image size ");
    Serial.print(bmpWidth, DEC);
    Serial.print(", ");
    Serial.println(bmpHeight, DEC);
    bmpdraw(bmpFile, 0, 0);
    delay(3000);
}


/*********************************************/
// This procedure reads a bitmap and draws it to the screen
// its sped up by reading many pixels worth of data at a time
// instead of just one pixel at a time. increading the buffer takes
// more RAM but makes the drawing a little faster. 20 pixels' worth
// is probably a good place

#define BUFFPIXEL 20

void bmpdraw(File f, int x, int y) {
    bmpFile.seek(bmpImageoffset);

    uint32_t time = millis();
    uint16_t p;
    uint8_t g, b;
    int i, j;

    uint8_t sdbuffer[3 * BUFFPIXEL];  // 3 * pixels to buffer
    uint8_t buffidx = 3*BUFFPIXEL;


    for (i=0; i< bmpHeight; i++) {

        Tft.setXY(x, y+bmpHeight-i);


        for (j=0; j<bmpWidth; j++) {
            // read more pixels
            if (buffidx >= 3*BUFFPIXEL) {
                bmpFile.read(sdbuffer, 3*BUFFPIXEL);
                buffidx = 0;
            }

            // convert pixel from 888 to 565
            b = sdbuffer[buffidx++];     // blue
            g = sdbuffer[buffidx++];     // green
            p = sdbuffer[buffidx++];     // red

            p >>= 3;
            p <<= 6;

            g >>= 2;
            p |= g;
            p <<= 5;

            b >>= 3;
            p |= b;

            // write out the 16 bits of color
            Tft.sendData(p);
        }
    }
    Serial.print(millis() - time, DEC);
    Serial.println(" ms");
}

boolean bmpReadHeader(File f) {
    // read header
    uint32_t tmp;

    if (read16(f) != 0x4D42) {
        // magic bytes missing
        return false;
    }

    // read file size
    tmp = read32(f);
    Serial.print("size 0x"); Serial.println(tmp, HEX);

    // read and ignore creator bytes
    read32(f);

    bmpImageoffset = read32(f);
    Serial.print("offset "); Serial.println(bmpImageoffset, DEC);

    // read DIB header
    tmp = read32(f);
    Serial.print("header size "); Serial.println(tmp, DEC);
    bmpWidth = read32(f);
    bmpHeight = read32(f);


    if (read16(f) != 1)
    return false;

    bmpDepth = read16(f);
    Serial.print("bitdepth "); Serial.println(bmpDepth, DEC);

    if (read32(f) != 0) {
        // compression not supported!
        return false;
    }

    Serial.print("compression "); Serial.println(tmp, DEC);

    return true;
}

/*********************************************/
// These read data from the SD card file and convert them to big endian
// (the data is stored in little endian format!)

// LITTLE ENDIAN!
uint16_t read16(File f) {
    uint16_t d;
    uint8_t b;
    b = f.read();
    d = f.read();
    d <<= 8;
    d |= b;
    return d;
}

// LITTLE ENDIAN!
uint32_t read32(File f) {
    uint32_t d;
    uint16_t b;

    b = read16(f);
    d = read16(f);
    d <<= 16;
    d |= b;
    return d;
}


