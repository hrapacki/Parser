#include "tsTransportStream.h"
#include "tsCommon.h"
#include <iostream>
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
int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
  uint32_t* HP = (uint32_t*)Input;
  uint32_t Head = xSwapBytes32(*HP);
  m_CC = m_CC|Head;
  Head>>4;
  m_CC&15;
  m_AFC = m_AFC|Head;
  Head>>2;
  m_AFC&3;
  m_TSC = m_TSC|Head;
  Head>>2;
  m_TSC&3;
  m_PID = m_PID|Head;
  Head>>13;
  m_PID|8191;
  m_T = m_T|Head;
  Head>>1;
  m_T&1;
  m_S = m_S|Head;
  Head>>1;
  m_S&1;
  m_E = m_E|Head;
  Head>>1;
  m_E&1;
  m_SB = m_SB|Head;
  Head>>8;
  return NOT_VALID;
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
 cout << "SB: " << m_SB << " E: " << m_E << " S: " << m_S << " T: " << m_T;
 cout << " PID: " << m_PID << " TSC: " << m_TSC << " AFC: " << m_AFC << " CC: " << m_CC << endl;
} 

//=============================================================================================================================================================================
