#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sndfile.h>

#define PI 3.14159265358979323846

int main(int argc, char *argv[]) {

    if (argc != 5) {
        fprintf(stderr, "\nNetacan broj argumenata\n\n");
        return 1;
    }

    //generisanje sfinfo struct fajla koji sadrzi sav info ulaznog fajla
    SF_INFO sfinfo;
    
    
    //ucitavanje argumenata
    const char* inputFilename = argv[1];
    int depth = atoi(argv[2]);
    float rate = atof(argv[3]);
    const char* outputFilename = argv[4];
 
    
    //otvaranje ulaznog fajla
    SNDFILE* inputFile = sf_open(inputFilename, SFM_READ, &sfinfo);
    
   
    
    if (!inputFile) {
        fprintf(stderr, "Error: failed to open input file %s\n", inputFilename);
        return 1;
    }
    
    
    printf("Input file: %s\n", inputFilename);
    SF_INFO inputFileInfo;
    sf_command(inputFile, SFC_GET_CURRENT_SF_INFO, &inputFileInfo, sizeof(inputFileInfo));
    
    


    if (/*inputFileInfo.format != SF_FORMAT_WAV|| */ inputFileInfo.channels != 2 || inputFileInfo.samplerate != 44100) {
        fprintf(stderr, "Error: unsupported audio format\n");
        sf_close(inputFile);
        return 1;
    }

    //generisanje ulaznog bafera 
    int numFrames = inputFileInfo.frames;
    int numSamples = numFrames * inputFileInfo.channels;
    float* inputBuffer = malloc(numSamples * sizeof(float));

    //citanje frejmova/semplova
    int framesRead = sf_readf_float(inputFile, inputBuffer, numFrames);

    if (framesRead != numFrames) {
        fprintf(stderr, "Error: could not read all frames from input file\n");
        sf_close(inputFile);
        free(inputBuffer);
        return 1;
    }

    //TREMOLO efekat
    const float maxDepth = 32767.0;
    const float delta = 2.0 * PI * rate / inputFileInfo.samplerate;
    float depthFactor;
    float* outputBuffer = malloc(numSamples * sizeof(float));

    for (int i = 0; i < numSamples; i += 2) {
        float phase = sin(delta * i);
        depthFactor = 1.0 - (depth / maxDepth) * phase;
        outputBuffer[i] = inputBuffer[i] * depthFactor;
        outputBuffer[i + 1] = inputBuffer[i + 1] * depthFactor;
    }

    //generisanje otuputFileInfo struct fajla sa jednakim parametrima kao ulazni fajl
    SF_INFO outputFileInfo = inputFileInfo;
   
   //generisanje izlaznog fajla
    SNDFILE* outputFile = sf_open(outputFilename, SFM_WRITE, &outputFileInfo);

    if (!outputFile) {
        fprintf(stderr, "Error: failed to open output file %s\n", outputFilename);
        sf_close(inputFile);
        free(inputBuffer);
        free(outputBuffer);
        return 1;
    }

    //kopiranje modifikovanog ulaznog fajla u izlazni fajl
    int framesWritten = sf_writef_float(outputFile, outputBuffer, numFrames);


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

