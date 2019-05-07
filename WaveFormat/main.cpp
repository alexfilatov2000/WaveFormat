#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

typedef struct {
    int32_t chunkId;   // Завжди містить значення 0x52494646 (літери "RIFF")
    int32_t chunkSize; // 36 + розмір другого підрозділу в байтах
    // Іншими словами 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
    // Це розмір всього файла мінус 8 байтів які займають поля chunkId та chunkSize
    int32_t format;    // Для wav-файла це завжди 0x57415645 (літери "WAVE")
} RIFFHEADER;
RIFFHEADER getRH(char* fileName);

typedef struct {
    int32_t subchunk1Id;   // Завжди 0x666d7420 – літери "fmt "
    int32_t subchunk1Size; // Завжди 16 для аудіо PCM. Це розмір частини підрозділу, що слідує після цього числа
    int16_t audioFormat;   // PCM = 1
    int16_t numChannels;   // Mono = 1, Stereo = 2
    int32_t sampleRate;    // Наприклад 44100
    int32_t byteRate;      // == SampleRate  NumChannels  BitsPerSample/8
    int16_t blockAlign;    // == NumChannels * BitsPerSample/8
    int16_t bitsPerSample; // 8 bits = 8, 16 bits = 16, etc.
} SUBCHUNK1;
SUBCHUNK1 getSUBCHUNK1(char* fileName);

typedef struct {
    int32_t subchunk2Id;   // 0x64617461 – літери "data"
    int32_t subchunk2Size; // == NumSamples * NumChannels * BitsPerSample/8, кількість байтів аудіоданих
    //int8_t[] data;         // семпли
} SUBCHUNK2;
SUBCHUNK2 getSUBCHUNK2(char* fileName);



int main(int argc, const char * argv[]) {
    char InFileName[250];
    char OutFileName[250];
    
    strcpy(InFileName, argv[1]);
    strcpy(OutFileName, argv[2]);
    
    cout<<"InFileName = "<<InFileName<<endl;
    cout<<"OutFileName = "<<OutFileName<<endl;
    
    RIFFHEADER RH = getRH(InFileName);
    
    SUBCHUNK1 SC1 = getSUBCHUNK1(InFileName);
    
    SUBCHUNK2 SC2 = getSUBCHUNK2(InFileName);
    
    int n;
    cout<<"n = "; cin>>n;
    
    int newChunkSize = sizeof(RIFFHEADER) + sizeof(SUBCHUNK1) + sizeof(SUBCHUNK2) + SC2.subchunk2Size * n - 8;
    RH.chunkSize = newChunkSize;
    SC2.subchunk2Size *= n;
    
    ifstream fin(InFileName, ios::binary);
    fin.seekg(sizeof(RIFFHEADER) + sizeof(SUBCHUNK1) + sizeof(SUBCHUNK2));
    
    ofstream fout(OutFileName, ios::binary);
    fout.write((char*) &RH, sizeof(RH));
    fout.write((char*) &SC1, sizeof(SC1));
    fout.write((char*) &SC2, sizeof(SC2));
    
    char* buff = new char[SC1.blockAlign];
    
    while(fin.read(buff, sizeof(buff))){
        for (int i = 0; i<n; i++) fout.write(buff, sizeof(buff));
    }
    delete [] buff;
    fin.close();
    fout.close();
    
    return 0;
}


SUBCHUNK2 getSUBCHUNK2(char* fileName){
    ifstream fin(fileName, ios::binary);
    fin.seekg(sizeof(RIFFHEADER) + sizeof(SUBCHUNK1));
    
    SUBCHUNK2 SC2;
    fin.read((char*) &SC2, sizeof(SUBCHUNK2));
    
    cout<<hex<<"subchunk2Id: "<<SC2.subchunk2Id<<endl;
    cout<<dec<<"subchunk2Size: "<<SC2.subchunk2Size<<endl;
    
    
    fin.close();
    return SC2;
}

SUBCHUNK1 getSUBCHUNK1(char* fileName){
    ifstream fin(fileName, ios::binary);
    fin.seekg(sizeof(RIFFHEADER));
    
    
    
    SUBCHUNK1 SC1;
    fin.read((char*) &SC1, sizeof(SUBCHUNK1));
    
    cout<<hex<<"subchunk1Id: "<<SC1.subchunk1Id<<endl;
    cout<<dec<<"subchunk1Size: "<<SC1.subchunk1Size<<endl;
    cout<<"audioFormat: "<<SC1.audioFormat<<endl;
    cout<<"numChannels: "<<SC1.numChannels<<endl;
    cout<<"sampleRate: "<<SC1.sampleRate<<endl;
    cout<<"byteRate: "<<SC1.byteRate<<endl;
    cout<<"blockAlign: "<<SC1.blockAlign<<endl;
    cout<<"bitsPerSample: "<<SC1.bitsPerSample<<endl;
    
    fin.close();
    return SC1;
}

RIFFHEADER getRH(char* fileName){
    ifstream fin(fileName, ios::binary );
    RIFFHEADER RH;
    fin.read((char*) &RH, sizeof(RH));
    
    //    char RIFF[5] = {0};
    //    strncpy(RIFF, (char*) &RH.chunkId, 4);
    //    cout<<"chunkId: "<< RIFF<<endl;
    //    cout<<"chunkSize: "<< RH.chunkSize<<endl;
    //
    //    char format[5] = {0};
    //    strncpy(format, (char*) &RH.format, 4);
    //    cout<<"format: "<<format<<endl;
    
    fin.close();
    return RH;
}
