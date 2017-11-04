#include "stdafx.h"
#include "cdialog_main.h"

class CWinApp_Main:public CWinApp
{
 protected:

 public:

  CWinApp_Main(void);
 
  ~CWinApp_Main();
 
  BOOL InitInstance(void);
  
};

CWinApp_Main::CWinApp_Main(void)
{
}

CWinApp_Main::~CWinApp_Main()
{
}

BOOL CWinApp_Main::InitInstance(void)
{
 CDialog_Main cDialog_Main((LPSTR)IDD_DIALOG_MAIN,NULL);
 cDialog_Main.DoModal();
 return(TRUE);
}

CWinApp_Main cWinAppMain;


