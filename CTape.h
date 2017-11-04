#ifndef CTAPE_H
#define CTAPE_H

#include "stdafx.h"

class CTape
{
 protected:

  unsigned long TapeLength;
  unsigned char *TapeData;
  long double Speed;
  long double HeadPosition;
  int Channels;
  bool UseLeftChannel;
  bool UseRightChannel;

 public:

  CTape();
  ~CTape();

  bool LoadTape(char *FileName);
  bool SetSpeed(long double speed);
  bool ReadTape(unsigned char &volume);
  bool MoveHead(void);
  bool ResetHead(void);
  unsigned long GetTapeLength(void);
  void Eject(void);
  bool ApplyFilter(void);
  long double GetSpeed(void);
  bool EndOfTape(void);
  unsigned char GetLevel(int Step);
  bool SetUseChannel(bool use_left_channel,bool use_right_channel);
};

#endif