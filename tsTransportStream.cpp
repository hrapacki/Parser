#include "tsTransportStream.h"
#include "tsCommon.h"
#include <iostream>
#include <cstdint>
using namespace std;
//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================


/// @brief Reset - reset all TS packet header fields
void xTS_PacketHeader::Reset()
{
  m_SB=0;
  m_E=0;
  m_S=0;
  m_T=0;
  m_PID=0;
  m_TSC=0;
  m_AFC=0;
  m_CC=0;
}


/**
  @brief Parse all TS packet header field
  @param Input is pointer to buffer containing TS packet
  @return Number of parsed bytes (4 on success, -1 on failure)
 */
int32_t xTS_PacketHeader::Parse(const uint8_t* Input) {
  uint32_t* HP = (uint32_t*)Input;
  uint32_t Head = xSwapBytes32(*HP);
    m_SB = (Head >> 24) & 0xFF;
    m_E = (Head >> 23) & 0x01;
    m_S = (Head >> 22) & 0x01;
    m_T = (Head >> 21) & 0x01;
    m_PID = (Head >> 8) & 0x1FFF;
    m_TSC = (Head >> 6) & 0x03;
    m_AFC = (Head >> 4) & 0x03;
    m_CC = Head & 0x0F;

    return 4;
}


/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
 cout << "SB: " << m_SB << " E: " << m_E << " S: " << m_S << " T: " << m_T;
 cout << " PID: " << m_PID << " TSC: " << m_TSC << " AF: " << m_AFC << " CC: " << m_CC << " ";
 if(m_AFC!=2&&m_AFC!=3&&m_PID!=136){
    cout << endl;
 }
}

/// @brief Reset - reset all TS packet header fields
void xTS_AdaptationField::Reset()
{
m_AdaptationFieldControl=0;
DC=0;
RA=0;
SP=0;
PR=0;
OR=0;
SF=0;
TP=0;
EX=0;
m_AdaptationFieldLength=0;
}
/**
@brief Parse adaptation field
@param PacketBuffer is pointer to buffer containing TS packet
@param AdaptationFieldControl is value of Adaptation Field Control field of
corresponding TS packet header
@return Number of parsed bytes (length of AF or -1 on failure)
*/
int32_t xTS_AdaptationField::Parse(const uint8_t* PacketBuffer, uint8_t
AdaptationFieldControl)
{
if(AdaptationFieldControl==2||AdaptationFieldControl==3){
m_AdaptationFieldLength=(PacketBuffer[4])&0xFF;
DC=(PacketBuffer[5]>>7)  & 0x01;
RA=(PacketBuffer[5]>>6)  & 0x01;
SP=(PacketBuffer[5]>>5)  & 0x01;
PR=(PacketBuffer[5]>>4)  & 0x01;
OR=(PacketBuffer[5]>>3)  & 0x01;
SF=(PacketBuffer[5]>>2)  & 0x01;
TP=(PacketBuffer[5]>>1)  & 0x01;
EX=PacketBuffer[5]  & 0x01;
xTS_AdaptationField::Print();
}
return 2;
}
/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const
{
//cout << " L: " << m_AdaptationFieldLength << " DC: " << DC << " RA: " << RA << " SP: " << SP << " PR: " << PR << " OR: " << OR << " SF: " << SF
//<< " TP: " << TP << " EX: " << EX << endl;
}
//=============================================================================================================================================================================
int32_t xPES_PacketHeader::Parse(const uint8_t* Input)
{
    m_PacketStartCodePrefix = (static_cast<uint32_t>(Input[0]) << 16) | (static_cast<uint32_t>(Input[1]) << 8) | Input[2];
    m_StreamId = Input[3];
    m_PacketLength = ((Input[4] << 8) | Input[5])+6;

    if (m_StreamId != eStreamId_program_stream_map &&
        m_StreamId != eStreamId_padding_stream &&
        m_StreamId != eStreamId_private_stream_2 &&
        m_StreamId != eStreamId_ECM &&
        m_StreamId != eStreamId_EMM &&
        m_StreamId != eStreamId_program_stream_directory &&
        m_StreamId != eStreamId_DSMCC_stream &&
        m_StreamId != eStreamId_ITUT_H222_1_type_E) {
        m_HeaderLength = Input[8] + 9;
    }
    return 6;
}

void xPES_PacketHeader::Reset()
{
    m_PacketStartCodePrefix = 0;
    m_StreamId = 0;
    m_PacketLength = 0;
    m_HeaderLength = 0;
}
void xPES_Assembler::Reset() {
    if (m_Buffer) {
        delete[] m_Buffer;
        m_Buffer = nullptr;
    }
    m_LastContinuityCounter = -1;
    m_Started = false;
    m_PESH.Reset();
    m_DataOffset = 0;
}

xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField) {
    if (PacketHeader->get_PID() != 136) {
        return eResult::UnexpectedPID;
    }

    if (m_Started && (PacketHeader->get_CC() != ((m_LastContinuityCounter + 1) & 0x0F))) {
        Reset();
        return eResult::StreamPacketLost;
    }
    m_LastContinuityCounter = PacketHeader->get_CC();

    const uint8_t* Payload = TransportStreamPacket + xTS::TS_HeaderLength;
    int32_t PayloadSize = xTS::TS_PacketLength - xTS::TS_HeaderLength;

    if (PacketHeader->hasAdaptationField()) {
        Payload += 1 + AdaptationField->getAdaptationFieldLength();
        PayloadSize -= 1 + AdaptationField->getAdaptationFieldLength();
    }
    if (PacketHeader->get_S() == 1 && !m_Started) {
        m_PESH.Reset();
        m_Started = true;
        m_PESH.Parse(Payload);
        int32_t packetLength = m_PESH.getPacketLength();
        m_Buffer = new uint8_t[packetLength];
        memcpy(m_Buffer, Payload + m_PESH.m_HeaderLength, PayloadSize - m_PESH.m_HeaderLength);
        m_DataOffset = PayloadSize - m_PESH.m_HeaderLength;
        return eResult::AssemblingStarted;
    } else if (m_Started) {

        memcpy(m_Buffer + m_DataOffset, Payload, PayloadSize);
        m_DataOffset += PayloadSize;

        if (
            m_DataOffset >= m_PESH.getPacketLength()- m_PESH.m_HeaderLength
            ) {

            m_Started = false;
            return eResult::AssemblingFinished;
        } else {
            return eResult::AssemblingContinue;
        }
    }

    return eResult::UnexpectedPID;
}

void xPES_PacketHeader::Print() const
{
    //cout << " PSCP: " << m_PacketStartCodePrefix << " SID: " << static_cast<int>(m_StreamId) << " L:" << m_PacketLength << " ";
}

xPES_Assembler::xPES_Assembler() : m_Buffer(nullptr), m_LastContinuityCounter(-1), m_Started(false), m_DataOffset(0) {
}

xPES_Assembler::~xPES_Assembler() {
    if (m_Buffer) {
        delete[] m_Buffer;
    }
}
