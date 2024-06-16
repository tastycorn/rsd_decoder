#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cstring>

char byte;
std::ifstream file;

struct RsdRecord
{
    int state {};
    int sequenceCount {};
    int dataSize {};
    int dataFields {};
    int channelId {};
    int bottomDepth {};
    int drawnBottomDepth {};
    int firstSampleDepth {};
    int lastSampleDepth {};
    int gain {};
    int sampleStatus {};
    int sampleCount {};
    bool shade {};
    double latitude {};
    double longitude {};
    float waterTemp {};
    int beam {};
};

int calculateTwosComplement(unsigned int num) {
    // Calculate the two's complement by subtracting the number from 2^n where n is the number of bits in an unsigned int
    // This effectively flips all the bits and adds 1 to the result
    int twosComplement = static_cast<int>(num) - (1 << (sizeof(unsigned int) * 8));

    return twosComplement;
}

float fourByteArrayToFloat(const char byteArray[4]) {
    static_assert(sizeof(float) == 4, "Size of float must be 4 bytes");

    float result;
    std::memcpy(&result, byteArray, sizeof(float));
    return result;
}

int twoByteVarInt32ToInt(char * input)
{
    char byte1 = input[1];
    char byte2 = input[0];
    
    unsigned char mask = 1; // Bit mask
    unsigned char bits1[8];
    unsigned char bits2[8];
    unsigned char theBits[14];

    // Extract the bits from byte1
    for (int i=0; i<8; i++)
    {
        // Mask each bit in the byte and store it
        bits1[i] = (byte1 & (mask << i)) != 0;
    }

    // Extract the bits from byte2
    for (int i=0; i<8; i++)
    {
        // Mask each bit in the byte and store it
        bits2[i] = (byte2 & (mask << i)) != 0;
    }

    // Combine the two binary strings into one, dropping the first bit for protobuf
    for (int i=0; i<7; i++)
    {
        theBits[i] = bits2[i];
        theBits[i+7] = bits1[i];
    }

    // Convert the binary to an integer
    int theInteger = 0;
    for (int i=0; i<14; i++)
    {
        if (theBits[i])
        {
            theInteger += pow(2,i);
        }
    }

    return theInteger;
}

int threeByteVarInt32ToInt(char * input)
{
    char byte1 = input[2];
    char byte2 = input[1];
    char byte3 = input[0];
    
    unsigned char mask = 1; // Bit mask
    unsigned char bits1[8];
    unsigned char bits2[8];
    unsigned char bits3[8];
    unsigned char theBits[21];

    // Extract the bits from the bytes
    for (int i=0; i<8; i++)
    {
        // Mask each bit in the byte and store it
        bits1[i] = (byte1 & (mask << i)) != 0;
        bits2[i] = (byte2 & (mask << i)) != 0;
        bits3[i] = (byte3 & (mask << i)) != 0;
    }

    // Combine the three binary strings into one, dropping the first bit for protobuf
    for (int i=0; i<7; i++)
    {
        theBits[i] = bits3[i];
        theBits[i+7] = bits2[i];
        theBits[i+14] = bits1[i];
    }

    // Convert the binary to an integer
    int theInteger = 0;
    for (int i=0; i<21; i++)
    {
        if (theBits[i])
        {
            theInteger += pow(2,i);
        }
    }
    
    return theInteger;
}

int fiveByteVarInt32ToInt(char * input)
{
    char byte1 = input[4];
    char byte2 = input[3];
    char byte3 = input[2];
    char byte4 = input[1];
    char byte5 = input[0];
    
    unsigned char mask = 1; // Bit mask
    unsigned char bits1[8];
    unsigned char bits2[8];
    unsigned char bits3[8];
    unsigned char bits4[8];
    unsigned char bits5[8];
    unsigned char theBits[35];

    // Extract the bits from the bytes
    for (int i=0; i<8; i++)
    {
        // Mask each bit in the byte and store it
        bits1[i] = (byte1 & (mask << i)) != 0;
        bits2[i] = (byte2 & (mask << i)) != 0;
        bits3[i] = (byte3 & (mask << i)) != 0;
        bits4[i] = (byte4 & (mask << i)) != 0;
        bits5[i] = (byte5 & (mask << i)) != 0;
    }

    // Combine the five binary strings into one, dropping the first bit for protobuf
    for (int i=0; i<7; i++)
    {
        theBits[i] = bits5[i];
        theBits[i+7] = bits4[i];
        theBits[i+14] = bits3[i];
        theBits[i+21] = bits2[i];
        theBits[i+28] = bits1[i];
    }

    // Convert the binary to an integer
    int theInteger = 0;
    for (int i=0; i<35; i++)
    {
        if (theBits[i])
        {
            theInteger += pow(2,i);
        }
    }
    
    return theInteger;
}

unsigned int fourBytesToUInt(char * input)
{
    char byte1 = input[3];
    char byte2 = input[2];
    char byte3 = input[1];
    char byte4 = input[0];
    
    unsigned char mask = 1; // Bit mask
    unsigned char bits1[8];
    unsigned char bits2[8];
    unsigned char bits3[8];
    unsigned char bits4[8];
    unsigned char theBits[32];

    // Extract the bits from the bytes
    for (int i=0; i<8; i++)
    {
        // Mask each bit in the byte and store it
        bits1[i] = (byte1 & (mask << i)) != 0;
        bits2[i] = (byte2 & (mask << i)) != 0;
        bits3[i] = (byte3 & (mask << i)) != 0;
        bits4[i] = (byte4 & (mask << i)) != 0;
    }

    // Combine the four binary strings into one
    for (int i=0; i<8; i++)
    {
        theBits[i] = bits4[i];
        theBits[i+8] = bits3[i];
        theBits[i+16] = bits2[i];
        theBits[i+24] = bits1[i];
    }

    // Convert the binary to an integer
    int theInteger = 0;
    for (int i=0; i<32; i++)
    {
        if (theBits[i])
        {
            theInteger += pow(2,i);
        }
    }

    return theInteger;
}

int twoBytesToInt(char * input)
{
    char byte1 = input[1];
    char byte2 = input[0];
    
    unsigned char mask = 1; // Bit mask
    unsigned char bits1[8];
    unsigned char bits2[8];
    unsigned char theBits[16];

    // Extract the bits from the bytes
    for (int i=0; i<8; i++)
    {
        // Mask each bit in the byte and store it
        bits1[i] = (byte1 & (mask << i)) != 0;
        bits2[i] = (byte2 & (mask << i)) != 0;
    }

    // Combine the two binary strings into one
    for (int i=0; i<8; i++)
    {
        theBits[i] = bits2[i];
        theBits[i+8] = bits1[i];
    }

    // Convert the binary to an integer
    unsigned int theInteger = 0;
    for (int i=0; i<16; i++)
    {
        if (theBits[i])
        {
            theInteger += pow(2,i);
        }
    }

    int finalInteger = calculateTwosComplement(theInteger);

    return finalInteger;
}

int fourBytesToInt(char * input)
{
    char byte1 = input[3];
    char byte2 = input[2];
    char byte3 = input[1];
    char byte4 = input[0];
    
    unsigned char mask = 1; // Bit mask
    unsigned char bits1[8];
    unsigned char bits2[8];
    unsigned char bits3[8];
    unsigned char bits4[8];
    unsigned char theBits[32];

    // Extract the bits from the bytes
    for (int i=0; i<8; i++)
    {
        // Mask each bit in the byte and store it
        bits1[i] = (byte1 & (mask << i)) != 0;
        bits2[i] = (byte2 & (mask << i)) != 0;
        bits3[i] = (byte3 & (mask << i)) != 0;
        bits4[i] = (byte4 & (mask << i)) != 0;
    }

    // Combine the four binary strings into one
    for (int i=0; i<8; i++)
    {
        theBits[i] = bits4[i];
        theBits[i+8] = bits3[i];
        theBits[i+16] = bits2[i];
        theBits[i+24] = bits1[i];
    }

    // Convert the binary to an integer
    unsigned int theInteger = 0;
    for (int i=0; i<32; i++)
    {
        if (theBits[i])
        {
            theInteger += pow(2,i);
        }
    }

    int finalInteger = calculateTwosComplement(theInteger);

    return finalInteger;
}

double convertGarminMapUnit(int input)
{
    double garminConstant = (360/pow(2,32));
    double returnedValue = input * garminConstant;
    return returnedValue;
}

int readNextField(RsdRecord * thePassedRecord)
{
    file.read(&byte, 1);
    switch (byte)
    {
        case '\x01':
        {
            char channelId;
            file.read(&channelId, 1);
            int temp = static_cast<int>(channelId);
            thePassedRecord->channelId = temp;
            return 1;
        }
        case '\x0b':
        {
            char bottomDepth[3];
            file.read(bottomDepth, 3);
            int temp = threeByteVarInt32ToInt(bottomDepth);
            thePassedRecord->bottomDepth = temp;
            return 1;
        }
        case '\x13':
        {
            char drawnBottomDepth[3];
            file.read(drawnBottomDepth, 3);
            int temp = threeByteVarInt32ToInt(drawnBottomDepth);
            thePassedRecord->drawnBottomDepth = temp;
            return 1;
        }
        case '\x19':
        {
            char firstSampleDepth;
            file.read(&firstSampleDepth, 1);
            int temp = static_cast<int>(firstSampleDepth);
            thePassedRecord->firstSampleDepth = temp;
            return 1;
        }
        case '\x23':
        {
            char lastSampleDepth[3];
            file.read(lastSampleDepth, 3);
            int temp = threeByteVarInt32ToInt(lastSampleDepth);
            thePassedRecord->lastSampleDepth = temp;
            return 1;
        }
        case '\x29':
        {
            char gain;
            file.read(&gain, 1);
            int temp = static_cast<int>(gain);
            thePassedRecord->gain = temp;
            return 1;
        }
        case '\x31':
        {
            char sampleStatus;
            file.read(&sampleStatus, 1);
            int temp = static_cast<int>(sampleStatus);
            thePassedRecord->sampleStatus = temp;
            return 1;
        }
        case '\x3c':
        {
            char sampleCount[4];
            file.read(sampleCount, 4);
            unsigned int temp = fourBytesToUInt(sampleCount);
            thePassedRecord->sampleCount = temp;
            return 1;
        }
        case '\x41':
        {
            char shadeAvailable;
            file.read(&shadeAvailable, 1);
            bool shade = 0;
            if (shadeAvailable)
                shade = 1;
            thePassedRecord->shade = shade;
            return 1;
        }
        case '\x4c':
        {
            char latitude[4];
            file.read(latitude, 4);
            int temp = fourBytesToInt(latitude);
            double finalLatitude = convertGarminMapUnit(temp);
            thePassedRecord->latitude = finalLatitude;
            return 1;
        }
        case '\x54':
        {
            char longitude[4];
            file.read(longitude, 4);
            int temp = fourBytesToInt(longitude);
            double finalLongitude = convertGarminMapUnit(temp);
            thePassedRecord->longitude = finalLongitude;
            return 1;
        }
        case '\x5c':
        {
            char waterTemperature[4];
            file.read(waterTemperature, 4);
            float actualTemperature = fourByteArrayToFloat(waterTemperature);
            thePassedRecord->waterTemp = actualTemperature;
            return 1;
        }
        case '\x61':
        {
            char beam;
            file.read(&beam, 1);
            thePassedRecord->beam = static_cast<int>(beam);
            return 1;
        }
        case '\x0a':
        {
            char bottomDepth[2];
            file.read(bottomDepth, 2);
            int temp = twoByteVarInt32ToInt(bottomDepth);
            thePassedRecord->bottomDepth = temp;
            return 1;
        }
        case '\x12':
        {
            char drawnBottomDepth[2];
            file.read(drawnBottomDepth, 2);
            int temp = twoByteVarInt32ToInt(drawnBottomDepth);
            thePassedRecord->drawnBottomDepth = temp;
            return 1;
        }
        case '\x22':
        {
            char lastSampleDepth[2];
            file.read(lastSampleDepth, 2);
            int temp = twoByteVarInt32ToInt(lastSampleDepth);
            thePassedRecord->lastSampleDepth = temp;
            return 1;
        }
        case '\x0d':
        {
            char bottomDepth[5];
            file.read(bottomDepth, 5);
            int temp = fiveByteVarInt32ToInt(bottomDepth);
            thePassedRecord->bottomDepth = temp;
            return 1;
        }
        case '\x15':
        {
            char drawnBottomDepth[5];
            file.read(drawnBottomDepth, 5);
            int temp = fiveByteVarInt32ToInt(drawnBottomDepth);
            thePassedRecord->drawnBottomDepth = temp;
            return 1;
        }
        default:
        {
            std::cout << "Unknown field!" << std::endl;
            return 0;
        }
    }
}

int main() {
    std::string filename;
    std::cout << "Enter the name of the file: ";
    std::cin >> filename;

    file.open(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file.\n";
        return 1;
    }

    std::vector<char> buffer(6);
    std::vector<char> magicNumber = {'\x06', '\x04', '\x86', '\xda', '\xe9', '\xb7'};
    char firstRecord = '\x01';
    char validState = '\x02';
    char endOfRecords = '\x03';

    std::ofstream outf{"test.csv"};

    // If we couldn't open the output file stream for writing
    if (!outf)
    {
        // Print an error and exit
        std::cerr << "Uh oh, test.csv could not be opened for writing!\n";
        return 1;
    }
    outf << "SequenceCount,State,DataSize,DataFields,ChannelID,BottomDepth,DrawnBottomDepth,FirstSampleDepth,LastSampleDepth,Gain,SampleStatus,SampleCount,Shade,Latitude,Longitude,WaterTemp,Beam\n";
    while (file.read(&byte, 1)) {

        // Fill the buffer with the current byte plus the next 4
        for (int i=0; i<5; i++)
        {
            buffer.at(i) = byte;
            file.read(&byte, 1);
        }
        // Put the last read byte into the buffer
        buffer.at(5) = byte;
        
        if (buffer == magicNumber)
        {

            // Jump ahead 4 bytes to the 'record state' value
            file.seekg(4, std::ios::cur);

            // Read in the record state byte, then determine if it is the first record, final record, or a valid record that needs to be dealt with.
            char state;
            file.read(&state, 1);
            if (state == firstRecord)
            {
                std::cout << "Processing...." << std::endl;
            }
            if (state == endOfRecords)
            {
                std::cout << "End of records." << std::endl;
                // This is the last record, so we need to break the while loop and close the file.
                break;
            }
            if (state == validState)
            {                
                RsdRecord theRecord {};
                RsdRecord *recordPtr = &theRecord;
                theRecord.state = state;

                // Jump ahead 5 bytes to the sequence count.
                file.seekg(5, std::ios::cur);
                char sequenceCount[4];
                file.read(sequenceCount, 4);
                int theSequenceCount = fourBytesToInt(sequenceCount);
                theRecord.sequenceCount = theSequenceCount;

                // Jump ahead 6 bytes to the data size.
                file.seekg(6, std::ios::cur);
                char dataSize[2];
                file.read(dataSize, 2);
                int theDataSize = twoBytesToInt(dataSize);
                theRecord.dataSize = theDataSize;
                
                // Jump ahead 9 bytes to the first field number.
                file.seekg(9, std::ios::cur);
                file.read(&byte, 1);
                int numberOfFields = static_cast<int>(byte);
                theRecord.dataFields = numberOfFields;

                // Run the readNextField() function as many times as there are declared data fields stored in numberOfFields
                for(int i=0; i<numberOfFields; i++)
                {
                    int success = readNextField(recordPtr);
                    if (!success)
                    {
                        return 0;
                    }
                }

                // Write the values to the file
                outf << theRecord.sequenceCount << ",";
                outf << theRecord.state << ",";
                outf << theRecord.dataSize << ",";
                outf << theRecord.dataFields << ",";
                outf << theRecord.channelId << ",";
                outf << theRecord.bottomDepth << ",";
                outf << theRecord.drawnBottomDepth << ",";
                outf << theRecord.firstSampleDepth << ",";
                outf << theRecord.lastSampleDepth << ",";
                outf << theRecord.gain << ",";
                outf << theRecord.sampleStatus << ",";
                outf << theRecord.sampleCount << ",";
                outf << theRecord.shade << ",";
                outf << std::setprecision(15) << theRecord.latitude << ",";
                outf << theRecord.longitude << ",";
                outf << std::setprecision(5) << theRecord.waterTemp << ",";
                outf << theRecord.beam << "\n";
            }
        }

        // Need to reposition the read byte 5 bytes back because you read 6 into the buffer
        file.seekg(-5, std::ios::cur);

    }

    file.close();
    return 0;
}