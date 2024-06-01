#pragma once
#include "tsCommon.h"
#include <string>

/*
*/


//=============================================================================================================================================================================

class xTS
{
public:
  static constexpr uint32_t TS_PacketLength  = 188;
  static constexpr uint32_t TS_HeaderLength  = 4;

  static constexpr uint32_t PES_HeaderLength = 6;

  static constexpr uint32_t BaseClockFrequency_Hz         =    90000; //Hz
  static constexpr uint32_t ExtendedClockFrequency_Hz     = 27000000; //Hz
  static constexpr uint32_t BaseClockFrequency_kHz        =       90; //kHz
  static constexpr uint32_t ExtendedClockFrequency_kHz    =    27000; //kHz
  static constexpr uint32_t BaseToExtendedClockMultiplier =      300;
};

//=============================================================================================================================================================================

class xTS_PacketHeader
{
public:
  enum class ePID : uint16_t
  {
    PAT  = 0x0000,
    CAT  = 0x0001,
    TSDT = 0x0002,
    IPMT = 0x0003,
    NIT  = 0x0010, //DVB specific PID
    SDT  = 0x0011, //DVB specific PID
    NuLL = 0x1FFF,
  };

protected:
  uint16_t  m_SB;
  uint16_t m_E;
  uint16_t  m_S;
  uint16_t  m_T;
  uint16_t m_PID;
  uint16_t  m_TSC;
  uint16_t  m_AFC;
  uint16_t  m_CC;

public:
  void     Reset();
  int32_t  Parse(const uint8_t* Input);
  void     Print() const;

public:
  uint16_t  getSyncByte() const { return m_SB; }
  uint16_t  get_E() const { return m_E; }
  uint16_t  get_S() const { return m_S; }
  uint16_t  get_T() const { return m_T; }
  uint16_t get_PID() const { return m_PID; }
  uint16_t  get_TSC() const { return m_TSC; }
  uint16_t  get_AFC() const { return m_AFC; }
  uint16_t  get_CC() const { return m_CC; }

public:
  //TODO - derrived informations
  bool     hasAdaptationField() const {
    return (m_AFC==2||m_AFC==3);
   }
  bool     hasPayload        () const {
    return !(m_AFC==2||m_AFC==0);
   }
};

class xTS_AdaptationField
{
protected:
//setup
uint16_t m_AdaptationFieldControl;
uint16_t DC;
uint16_t RA;
uint16_t SP;
uint16_t PR;
uint16_t OR;
uint16_t SF;
uint16_t TP;
uint16_t EX;
uint16_t m_AdaptationFieldLength;
//optional fields - PCR
public:
void Reset();
int32_t Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl);
void Print() const;
public:
//mandatory fields
uint16_t getAdaptationFieldLength () const { return m_AdaptationFieldLength ; }

uint8_t getDC () const { return DC ; }
uint8_t getRA () const { return RA ; }
uint8_t getSP () const { return SP ; }
uint8_t getPR () const { return PR ; }
uint8_t getOR () const { return OR ; }
uint8_t getSF () const { return SF ; }
uint8_t getTP () const { return TP ; }
uint8_t getEX () const { return EX ; }
//derived values
//uint32_t getNumBytes () const { }
};
//=============================================================================================================================================================================
class xPES_PacketHeader
{
public:
enum eStreamId : uint8_t
{
eStreamId_program_stream_map = 0xBC,
eStreamId_padding_stream = 0xBE,
eStreamId_private_stream_2 = 0xBF,
eStreamId_ECM = 0xF0,
eStreamId_EMM = 0xF1,
eStreamId_program_stream_directory = 0xFF,
eStreamId_DSMCC_stream = 0xF2,
eStreamId_ITUT_H222_1_type_E = 0xF8,
};
protected:
//PES packet header
uint32_t m_PacketStartCodePrefix;
uint8_t m_StreamId;
uint16_t m_PacketLength;
public:
void Reset();
int32_t Parse(const uint8_t* Input);
void Print() const;
public:
    uint16_t m_HeaderLength;

//PES packet header
uint16_t GetHeaderLength() const {return m_HeaderLength;}
uint32_t getPacketStartCodePrefix() const { return m_PacketStartCodePrefix; }
uint8_t getStreamId () const { return m_StreamId; }
uint16_t getPacketLength () const { return m_PacketLength; }
};

class xPES_Assembler
{
public:
enum class eResult : int32_t
{
UnexpectedPID = 1,
StreamPacketLost ,
AssemblingStarted ,
AssemblingContinue,
AssemblingFinished,
};
protected:
//setup
int16_t Datalen;
int32_t m_PID;
//buffer

uint32_t m_BufferSize;
uint32_t m_DataOffset;
//operation
int8_t m_LastContinuityCounter;
bool m_Started;

public:
    uint8_t* m_Buffer;
    xPES_PacketHeader m_PESH;
void Reset();
xPES_Assembler ();
~xPES_Assembler();
void Init (int32_t PID);

eResult AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField);
uint16_t CalcDataLen(){
Datalen= getNumPacketBytes()-m_PESH.GetHeaderLength();
return Datalen;
}
void PrintPESH ()  { m_PESH.Print(); }
uint8_t* getPacket () { return m_Buffer; }
int32_t getNumPacketBytes() const {
    if(m_DataOffset >= m_PESH.getPacketLength()- m_PESH.m_HeaderLength){
            return m_DataOffset+m_PESH.GetHeaderLength();}
    else{
       return m_DataOffset;
    }
     }
protected:
void xBufferReset ();
void xBufferAppend(const uint8_t* Data, int32_t Size);
};


