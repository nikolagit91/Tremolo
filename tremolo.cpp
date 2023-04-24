#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

const double PI = 3.14159265358979323846;

int main(int argc, char *argv[]) {

    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " input.wav depth rate output.wav" << endl;
        return 1;
    }

    const char* inputFilename = argv[1];
    int depth = stoi(argv[2]);
    double rate = stod(argv[3]);
    const char* outputFilename = argv[4];

    ifstream inputFile(inputFilename, ios::binary);

    if (!inputFile) {
        cerr << "Error: failed to open input file " << inputFilename << endl;
        return 1;
    }

    // Read WAV header
    char riff[4], wave[4], fmt[4], data[4];
    int32_t fileSize, fmtSize, audioFormat, numChannels, sampleRate, byteRate, blockAlign, bitsPerSample, dataSize;

    inputFile.read(riff, 4);
    inputFile.read(reinterpret_cast<char*>(&fileSize), 4);
    inputFile.read(wave, 4);
    inputFile.read(fmt, 4);
    inputFile.read(reinterpret_cast<char*>(&fmtSize), 4);
    inputFile.read(reinterpret_cast<char*>(&audioFormat), 2);
    inputFile.read(reinterpret_cast<char*>(&numChannels), 2);
    inputFile.read(reinterpret_cast<char*>(&sampleRate), 4);
    inputFile.read(reinterpret_cast<char*>(&byteRate), 4);
    inputFile.read(reinterpret_cast<char*>(&blockAlign), 2);
    inputFile.read(reinterpret_cast<char*>(&bitsPerSample), 2);
    inputFile.read(data, 4);
    inputFile.read(reinterpret_cast<char*>(&dataSize), 4);

    if (audioFormat != 1) {
        cerr << "Error: unsupported audio format " << audioFormat << endl;
        return 1;
    }

    if (numChannels != 2) {
        cerr << "Error: unsupported number of channels " << numChannels << endl;
        return 1;
    }

    if (bitsPerSample != 16) {
        cerr << "Error: unsupported number of bits per sample " << bitsPerSample << endl;
        return 1;
    }

    // Compute tremolo effect
    const int bufferSize = 1024;
    const double maxDepth = 32767.0;
    const double delta = 2.0 * PI * rate / sampleRate;
    int16_t buffer[bufferSize];
    int16_t outBuffer[bufferSize];

    ofstream outputFile(outputFilename, ios::binary);

    if (!outputFile) {
        cerr << "Error: failed to open output file " << outputFilename << endl;
        return 1;
    }

    outputFile.write(riff, 4);
    outputFile.write(reinterpret_cast<const char*>(&fileSize), 4);
    outputFile.write(wave, 4);
    outputFile.write(fmt, 4);
    outputFile.write(reinterpret_cast<const char*>(&fmtSize), 4);
    outputFile.write(reinterpret_cast<const char*>(&audioFormat), 2);
    outputFile.write(reinterpret_cast<const char*>(&numChannels), 2);
    outputFile.write(reinterpret_cast<const char*>(&sampleRate), 4);
    outputFile.write(reinterpret_cast<const char*>(&byteRate), 4);
    outputFile.write(reinterpret_cast<const char*>(&blockAlign), 2);
    outputFile.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
    outputFile.write(data, 4);
    outputFile.write(reinterpret_cast<const char*>(&dataSize),4);
    
    
    while (inputFile.read(reinterpret_cast<char*>(buffer), sizeof(buffer))) {
    for (int i = 0; i < bufferSize; i += 2) {
        double phase = sin(delta * i);
        double depthFactor = 1.0 - (depth / maxDepth) * phase;
        outBuffer[i] = static_cast<int16_t>(buffer[i] * depthFactor);
        outBuffer[i + 1] = static_cast<int16_t>(buffer[i + 1] * depthFactor);
    }
    outputFile.write(reinterpret_cast<const char*>(outBuffer), sizeof(outBuffer));
}

outputFile.close();
inputFile.close();

cout << "Done." << endl;

return 0;

  }  

