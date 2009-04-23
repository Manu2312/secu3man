#pragma once

#include "ITabController.h"
#include "CommunicationManager.h"
#include "StatusBarManager.h"
#include "io-core/ControlApp.h"
#include "io-core/ControlAppAdapter.h"
#include "io-core/ufcodes.h"
#include "common/ObjectTimer.h"
#include "common/unicodesupport.h"


class CCheckEngineTabDlg;

class CCheckEngineTabController : public ITabController, private IAPPEventHandler 
{
  public:
	CCheckEngineTabController(CCheckEngineTabDlg* i_view, CCommunicationManager* i_comm, CStatusBarManager* i_sbar);
	virtual ~CCheckEngineTabController();

  private:
	typedef std::map<size_t, _TSTRING> ErrorsIDContainer;

    CCheckEngineTabDlg*  m_view;
	CCommunicationManager* m_comm;
	CStatusBarManager*  m_sbar;
    CControlAppAdapter* m_pAdapter;

	bool m_real_time_errors_mode;
	ErrorsIDContainer m_errors_ids;

	void _SetErrorsToList(const SECU3IO::CEErrors* ip_errors);
	void _GetErrorsFromList(SECU3IO::CEErrors* op_errors);
	
	//from IAPPEventHandler:
    virtual void OnPacketReceived(const BYTE i_descriptor, SECU3IO::SECU3Packet* ip_packet);           
    virtual void OnConnection(const bool i_online);


    //���������/�������� ������� Check Engine
    virtual void OnActivate(void);
	virtual void OnDeactivate(void);
	virtual bool OnClose(void);

    void OnRealTimeErrors(void); 
    void OnReadSavedErrors(void); 
    void OnWriteSavedErrors(void); 
	void OnListSetAllErrors(void);
	void OnListClearAllErrors(void);

	void OnSettingsChanged(void);
};
