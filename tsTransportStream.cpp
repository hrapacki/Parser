#include "tsTransportStream.h"
#include "tsCommon.h"
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
int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
  uint16_t zero=8191;
  uint32_t* HP = (uint32_t*)Input;
  uint32_t Head = xSwapBytes32(*HP);
  m_CC = m_CC|Head;
  Head>>4;
  m_AFC = m_AFC|Head;
  Head>>2;
  m_TSC = m_TSC|Head;
  Head>>2;
  
  return NOT_VALID;
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
  //print sth
}

//=============================================================================================================================================================================
