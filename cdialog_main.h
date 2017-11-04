#ifndef CDIALOG_MAIN_H
#define CDIALOG_MAIN_H

#include "stdafx.h"
#include "ctape.h"

class CDialog_Main:public CDialog
{
 protected:

  CEdit *cEdit_ZeroMin;
  CEdit *cEdit_ZeroMax;
  CEdit *cEdit_OneMin;
  CEdit *cEdit_OneMax;
  CEdit *cEdit_LeadMin;
  CEdit *cEdit_LeadMax;
  CEdit *cEdit_Level;
  CEdit *cEdit_LevelPoints;

  CButton *cButton_UseLeftChannel;
  CButton *cButton_UseRightChannel;

  CButton *cButton_LevelManual;
  CButton *cButton_LevelAuto;

  int Level;
  int LevelPoints;
  bool LevelAuto;

 public:

  CDialog_Main(LPCTSTR lpszTemplateName, CWnd* pParentWnd);

  ~CDialog_Main();

  afx_msg void OnOK(void);
  afx_msg BOOL OnInitDialog(void);

  DECLARE_MESSAGE_MAP()
  afx_msg void OnDestroy(void);
  afx_msg void OnCommand_Button_CreateTap(void);

  int GetSignalLength(CTape *cTape,unsigned char &CurrentSignal);
  bool CreateTapFile(char *FileTitle,CTape *cTape,float ZeroMin,float ZeroMax,float OneMin,float OneMax,float LeadMin,float LeadMax);
};

#endif