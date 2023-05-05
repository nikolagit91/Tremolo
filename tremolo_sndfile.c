#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sndfile.h>

#define PI 3.14159265358979323846

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s input.wav depth rate output.wav\n", argv[0]);
        return 1;
    }

    SF_INFO sfinfo;
    const char* inputFilename = argv[1];
    int depth = atoi(argv[2]);
    double rate = atof(argv[3]);
    const char* outputFilename = argv[4];

    SNDFILE* inputFile = sf_open(inputFilename, SFM_READ, &sfinfo);
    
   
    
    if (!inputFile) {
        fprintf(stderr, "Error: failed to open input file %s\n", inputFilename);
        return 1;
    }
    
    
    printf("Input file: %s\n", inputFilename);
    
    SF_INFO inputFileInfo;
    sf_command(inputFile, SFC_GET_CURRENT_SF_INFO, &inputFileInfo, sizeof(inputFileInfo));
    
    
	printf("Format code is: %d\n", inputFileInfo.format);

    if (/*inputFileInfo.format != SF_FORMAT_WAV|| */ inputFileInfo.channels != 2 || inputFileInfo.samplerate != 44100) {
        fprintf(stderr, "Error: unsupported audio format\n");
        sf_close(inputFile);
        return 1;
    }

    int numFrames = inputFileInfo.frames;
    int numSamples = numFrames * inputFileInfo.channels;
    double* inputBuffer = malloc(numSamples * sizeof(double));

    int framesRead = sf_readf_double(inputFile, inputBuffer, numFrames);

    if (framesRead != numFrames) {
        fprintf(stderr, "Error: could not read all frames from input file\n");
        sf_close(inputFile);
        free(inputBuffer);
        return 1;
    }

    // Compute tremolo effect
    const double maxDepth = 32767.0;
    const double delta = 2.0 * PI * rate / inputFileInfo.samplerate;
    double depthFactor;
    double* outputBuffer = malloc(numSamples * sizeof(double));

    for (int i = 0; i < numSamples; i += 2) {
        double phase = sin(delta * i);
        depthFactor = 1.0 - (depth / maxDepth) * phase;
        outputBuffer[i] = inputBuffer[i] * depthFactor;
        outputBuffer[i + 1] = inputBuffer[i + 1] * depthFactor;
    }

    SF_INFO outputFileInfo = inputFileInfo;
    SNDFILE* outputFile = sf_open(outputFilename, SFM_WRITE, &outputFileInfo);

    if (!outputFile) {
        fprintf(stderr, "Error: failed to open output file %s\n", outputFilename);
        sf_close(inputFile);
        free(inputBuffer);
        free(outputBuffer);
        return 1;
    }

    int framesWritten = sf_writef_double(outputFile, outputBuffer, numFrames);

    if (framesWritten != numFrames) {
        fprintf(stderr, "Error: could not write all frames to output file\n");
        sf_close(outputFile);
        sf_close(inputFile);
        free(inputBuffer);
        free(outputBuffer);
        return 1;
    }

    	
    sf_close(outputFile);
    sf_close(inputFile);

    printf("Done.\n");

    free(inputBuffer);
    free(outputBuffer);

    return 0;
}

