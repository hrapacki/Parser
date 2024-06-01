#include "tsCommon.h"
#include "tsTransportStream.h"
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char *argv[], char *envp[]) {
    xTS_PacketHeader TS_PacketHeader;
    xTS_AdaptationField TS_AdaptationField;
    xPES_Assembler PES_Assembler;
    ifstream file("example_new.ts", ios::binary);
    ofstream outputFile("PID136.mp2", ios::binary);

    int32_t TS_PacketId = 0;
    while (!file.eof()) {
        uint8_t TS_PacketBuffer[188];
        file.read(reinterpret_cast<char*>(TS_PacketBuffer), 188);

        TS_PacketHeader.Reset();
        TS_AdaptationField.Reset();
        TS_PacketHeader.Parse(TS_PacketBuffer);

       //printf("%010d ", TS_PacketId);
        //TS_PacketHeader.Print();
        TS_AdaptationField.Parse(TS_PacketBuffer, TS_PacketHeader.get_AFC());
        TS_PacketId++;

        if (TS_PacketHeader.get_PID() == 136) {
            xPES_Assembler::eResult Result = PES_Assembler.AbsorbPacket(TS_PacketBuffer, &TS_PacketHeader, &TS_AdaptationField);
            switch(Result) {
                case xPES_Assembler::eResult::StreamPacketLost:
                   //printf("PcktLost ");
                    break;
                case xPES_Assembler::eResult::AssemblingStarted:
                    //printf("Started ");
                    PES_Assembler.PrintPESH();
                    break;
                case xPES_Assembler::eResult::AssemblingContinue:
                    //printf("Continue ");
                    break;
                case xPES_Assembler::eResult::AssemblingFinished:
                    ("Finished ");
                   //printf("PES: Len=%d", PES_Assembler.getNumPacketBytes());
                    //printf(" HeadLen=%d", PES_Assembler.m_PESH.GetHeaderLength());
                    //printf(" DataLen=%d", PES_Assembler.CalcDataLen());
                    outputFile.write(reinterpret_cast<char*>(PES_Assembler.m_Buffer), PES_Assembler.getNumPacketBytes()-PES_Assembler.m_PESH.GetHeaderLength());
                    PES_Assembler.Reset();
                    break;
                default:
                    break;
            }
            //printf("\n");
        }
    }

    file.close();
    outputFile.close();
    return EXIT_SUCCESS;
}
