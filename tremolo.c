#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define PI 3.14159265358979323846

typedef struct {
    char riff[4];
    int32_t fileSize;
    char wave[4];
    char fmt[4];
    int32_t fmtSize;
    int16_t audioFormat;
    int16_t numChannels;
    int32_t sampleRate;
    int32_t byteRate;
    int16_t blockAlign;
    int16_t bitsPerSample;
    char data[4];
    int32_t dataSize;
} WAVHeader;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s input.wav depth rate output.wav\n", argv[0]);
        return 1;
    }

    const char* inputFilename = argv[1];
    int depth = atoi(argv[2]);
    double rate = atof(argv[3]);
    const char* outputFilename = argv[4];

    FILE* inputFile = fopen(inputFilename, "rb");

    if (!inputFile) {
        fprintf(stderr, "Error: failed to open input file %s\n", inputFilename);
        return 1;
    }

    // Read WAV header
    WAVHeader header;
    fread(&header, sizeof(WAVHeader), 1, inputFile);

    if (header.audioFormat != 1) {
        fprintf(stderr, "Error: unsupported audio format %d\n", header.audioFormat);
        return 1;
    }

    if (header.numChannels != 2) {
        fprintf(stderr, "Error: unsupported number of channels %d\n", header.numChannels);
        return 1;
    }

    if (header.bitsPerSample != 16) {
        fprintf(stderr, "Error: unsupported number of bits per sample %d\n", header.bitsPerSample);
        return 1;
    }

    // Compute tremolo effect
    const int bufferSize = 1024;
    const double maxDepth = 32767.0;
    const double delta = 2.0 * PI * rate / header.sampleRate;
    short buffer[bufferSize];
    short outBuffer[bufferSize];

    FILE* outputFile = fopen(outputFilename, "wb");

    if (!outputFile) {
        fprintf(stderr, "Error: failed to open output file %s\n", outputFilename);
        return 1;
    }

    // Write WAV header
    fwrite(&header, sizeof(WAVHeader), 1, outputFile);

    while (fread(buffer, sizeof(int16_t), bufferSize, inputFile) == bufferSize) {
        for (int i = 0; i < bufferSize; i += 2) {
            double phase = sin(delta * i);
            double depthFactor = 1.0 - (depth / maxDepth) * phase;
            outBuffer[i] = (int16_t)(buffer[i] * depthFactor);
            outBuffer[i + 1] = (int16_t)(buffer[i + 1] * depthFactor);
        }
        fwrite(outBuffer, sizeof(int16_t), bufferSize, outputFile);
    }

    // Update WAV header with data size
    long fileSize = ftell(outputFile);
    fseek(outputFile, sizeof(int32_t), SEEK_SET);
    int32_t dataSize = fileSize - sizeof(WAVHeader);
    fwrite(&dataSize, sizeof(int32_t), 1, outputFile);

    fclose(outputFile);
    fclose(inputFile);

    printf("Done.\n");

    return 0;
}

