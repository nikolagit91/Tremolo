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

    //generisanje sfinfo struct-a koji sadrzi sav info ulaznog fajla
    SF_INFO sfinfo;
    
    
    //ucitavanje argumenata
    const char* inputFilename = argv[1];
    const char* outputFilename = argv[2];
    float frequency = atof(argv[3]); 
    float tremolo_depth = atof(argv[4]); 	 // 0.0-.1.0
 
   
    //otvaranje ulaznog fajla
    SNDFILE* inputFile = sf_open(inputFilename, SFM_READ, &sfinfo);
    
   
    
    if (!inputFile) {
        fprintf(stderr, "Neuspesno otvaranje %s\n fajla\n", inputFilename);
        return 1;
    }
    
    
    printf("Input file: %s\n", inputFilename);
    SF_INFO inputFileInfo;
    sf_command(inputFile, SFC_GET_CURRENT_SF_INFO, &inputFileInfo, sizeof(inputFileInfo));
    
    

    if ( inputFileInfo.channels != 2 || inputFileInfo.samplerate != 44100) {
        fprintf(stderr, "Pogresan format audio fajla\n");
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
    float t = 0;
    float tremolo_rate = 2.0 * PI * frequency / inputFileInfo.samplerate;
    float* outputBuffer = malloc(numSamples * sizeof(float));
    
    for (int i = 0; i < numSamples; i += inputFileInfo.channels) {
        
        float trem_factor = 1.0 - (tremolo_depth *(0.5 * sinf(t) + 0.5));
        t += tremolo_rate;
        
        if (t > 2 * PI) t -= 2 * PI;
        
        outputBuffer[i] = inputBuffer[i] * trem_factor;
        outputBuffer[i + 1] = inputBuffer[i + 1] * trem_factor;
    }


    //generisanje otuputFileInfo struct-a sa istim parametrima kao inputFileInfo
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

