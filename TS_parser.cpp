#include "tsCommon.h"
#include "tsTransportStream.h"
#include <fstream>
#include <iostream>
using namespace std;
//=============================================================================================================================================================================

int main(int argc, char *argv[ ], char *envp[ ])
{
  // TODO - open file
  // TODO - check if file if opened
  cout << "siema";
  xTS_PacketHeader    TS_PacketHeader;

  int32_t TS_PacketId = 0;
  while(/*not eof*/ 0)
  {
    cout << "siema";
    // TODO - read from file
    ofstream myfile;
    myfile.open("example_new.ts");
    TS_PacketHeader.Reset();
    TS_PacketHeader.Parse(/*TS_PacketBuffer*/ nullptr);

    printf("%010d ", TS_PacketId);
    TS_PacketHeader.Print();
    printf("\n");

    TS_PacketId++;
  }
  
  // TODO - close file

  return EXIT_SUCCESS;
}

//=============================================================================================================================================================================
