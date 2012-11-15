/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

#include "stdafx.h"
#include "Resources/resource.h"
#include "FWIORemappingController.h"

#include "common/fastdelegate.h"
#include "ParamDesk/Params/IORemappingDlg.h"

using namespace fastdelegate;
typedef CFirmwareDataMediator FWDM;

//#define IOREMCNTR_DEBUG

CFWIORemappingController::CFWIORemappingController(IORVIEW* ip_view)
: mp_view(ip_view)
, mp_fwdm(NULL)
{
 mp_view->setOnItemSelected(MakeDelegate(this, &CFWIORemappingController::OnItemSelected));
}

CFWIORemappingController::~CFWIORemappingController()
{
 //empty
}

void CFWIORemappingController::AttachFWDM(FWDM* ip_fwdm)
{
 mp_fwdm = ip_fwdm;
 ASSERT(mp_fwdm);

 _PrepareLogic();

 _CheckErrors();

 _UpdateView();
}

void CFWIORemappingController::OnActivate(void)
{
 _PrepareLogic();
}

void CFWIORemappingController::OnDeactivate(void)
{
 mp_view->ResetContent();
}

void CFWIORemappingController::Enable(bool state)
{
 mp_view->Enable(state);
}

void CFWIORemappingController::EnableSECU3TFeatures(bool i_enable)
{
 mp_view->EnableSECU3TItems(i_enable);
}

void CFWIORemappingController::OnItemSelected(FWDM::IOSid iosId, FWDM::IOPid iopId)
{
 //Detach plugs from specified slot
 _DetachPlugsFromSpecifiedSlot(iosId);

 //Attach a selected plug to slot
 _AttachPlugToSpecifiedSlot(iopId, iosId);

 //Not connected plugs must be connected to default free slots
 _AttachFreeSlotsToDefaultPlugs();

 //For debug purposes
 _DisplayPlugs();

 //Update UI
 _UpdateView();
}

void CFWIORemappingController::_PrepareLogic(void)
{
 if (!mp_fwdm)
  return; //Can't prepare when FWDM is not attached

 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {
  m_defValMap.insert(std::make_pair(FWDM::IOS_ECF, FWDM::IOP_ECFv0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ST_BLOCK, FWDM::IOP_ST_BLOCKv0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_IGN_OUT3, FWDM::IOP_IGN_OUT3v0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_IGN_OUT4, FWDM::IOP_IGN_OUT4v0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_IO1, FWDM::IOP_ADD_IO1v0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_IO2, FWDM::IOP_ADD_IO2v0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_IE, FWDM::IOP_IEv0));
  m_defValMap.insert(std::make_pair(FWDM::IOS_FE, FWDM::IOP_FEv0));

  //fill view with values
  mp_view->ResetContent();
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_ECFv0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ECF, true);

  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_ST_BLOCKv0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ST_BLOCK, true);

  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_IGN_OUT3v0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IGN_OUT3, true);

  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_IGN_OUT4v0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IGN_OUT4, true);

  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_ADD_IO1v0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ADD_IO1, true);

  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_ADD_IO2v0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ADD_IO2, true);

  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_IEv0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IE, true);

  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FL_PUMPv0, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_HALL_OUTv0, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_STROBEv0, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_PWRRELAYv0, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FEv0, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_FE, true);
 }
 else
 { //v1.0+
  m_defValMap.clear();
  m_defValMap.insert(std::make_pair(FWDM::IOS_ECF, FWDM::IOP_ECF));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ST_BLOCK, FWDM::IOP_ST_BLOCK));
  m_defValMap.insert(std::make_pair(FWDM::IOS_IGN_OUT3, FWDM::IOP_IGN_OUT3));
  m_defValMap.insert(std::make_pair(FWDM::IOS_IGN_OUT4, FWDM::IOP_IGN_OUT4));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_IO1, FWDM::IOP_ADD_IO1));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_IO2, FWDM::IOP_ADD_IO2));
  m_defValMap.insert(std::make_pair(FWDM::IOS_IE, FWDM::IOP_IE));
  m_defValMap.insert(std::make_pair(FWDM::IOS_FE, FWDM::IOP_FE));
  m_defValMap.insert(std::make_pair(FWDM::IOS_PS, FWDM::IOP_PS));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_I1, FWDM::IOP_ADD_I1));
  m_defValMap.insert(std::make_pair(FWDM::IOS_ADD_I2, FWDM::IOP_ADD_I2));

  //Fill view with values
  mp_view->ResetContent();
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_ECF, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ECF, true);

  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_ST_BLOCK, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ST_BLOCK, true);

  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_IGN_OUT3, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IGN_OUT3, true);

  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_IGN_OUT4, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IGN_OUT4, true);

  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_IGN, _T("IGN"));
  mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_ADD_IO1, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ADD_IO1, true);

  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_IGN, _T("IGN"));
  mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_ADD_IO2, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_ADD_IO2, true);

  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_IE, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_IE, true);

  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_STROBE, _T("STROBE"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_PWRRELAY, _T("PWRRELAY"));
  mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FE, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_FE, true); 

  mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_IGN, _T("IGN"));
  mp_view->AddItem(FWDM::IOS_PS, FWDM::IOP_PS, _T("NONE"));
  mp_view->EnableItem(FWDM::IOS_PS, true); 
 }
}

void CFWIORemappingController::_AttachFreeSlotsToDefaultPlugs(void)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {//V0.0
  for(int s = FWDM::IOS_ECF; s < FWDM::IOS_COUNTv0; ++s)
  {
   if (!_IsSlotFree((FWDM::IOSid)s))
    continue;
   std::map<FWDM::IOSid, FWDM::IOPid>::const_iterator it = m_defValMap.find((FWDM::IOSid)s);
   if (it != m_defValMap.end())
    _AttachPlug(it->second, (FWDM::IOSid)s); //default
  }
 }
 else
 {//V1.0+  
  std::map<FWDM::IOPid, FWDM::IOSid> attachList;
  //build list of free slots which have to be attached to default plugs
  for(int s = FWDM::IOS_ECF; s < FWDM::IOS_COUNT; ++s)
  {
   if (!_IsSlotFree((FWDM::IOSid)s))
    continue;
   std::map<FWDM::IOSid, FWDM::IOPid>::const_iterator it = m_defValMap.find((FWDM::IOSid)s);
   if (it != m_defValMap.end())
    attachList.insert(std::make_pair(it->second, (FWDM::IOSid)s));
  }
  //attach
  std::map<FWDM::IOPid, FWDM::IOSid>::const_iterator it = attachList.begin();
  for(; it != attachList.end(); ++it)
   _AttachPlug(it->first, it->second); //default
 } 
}

bool CFWIORemappingController::_IsSlotFree(FWDM::IOSid iosId)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {//V0.0
  DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, iosId);
  for(int p = FWDM::IOP_ECFv0; p < FWDM::IOP_COUNTv0; ++p)
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if (plug == slot)
    return false; //already connected to one of plugs
  }
 }
 else
 {//V1.0+
  DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, iosId);
  for(int p = FWDM::IOP_ECF; p < FWDM::IOP_COUNT; ++p)
  {  
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if (plug == 0)
    continue; //skip reserved plugs
   if (plug == slot)
    return false; //already connected to one of plugs
   //���� ����� ADD_Ix �� ��������, �� ������ ADD_IOx ���� �� ������� ���������� (� ��������), � ��������� ������
   //����� �������� ����� �� ��������� � �� ����� ������ ������ �����
   if ((iosId == FWDM::IOS_ADD_IO1 && plug == mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_I1)) ||
      (iosId == FWDM::IOS_ADD_I1 && plug == mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_IO1)))    
    return false; //already connected to one of plugs
   if ((iosId == FWDM::IOS_ADD_IO2 && plug == mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_I2)) ||
      (iosId == FWDM::IOS_ADD_I2 && plug == mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_IO2)))
    return false; //already connected to one of plugs
  } 
 }
 return true; //free!
}

void CFWIORemappingController::_DetachPlugsFromSpecifiedSlot(FWDM::IOSid iosId)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {//V0.0
  DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, iosId);
  for(int p = FWDM::IOP_ECFv0; p < FWDM::IOP_COUNTv0; ++p)
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if(plug == slot)
     _AttachPlug((FWDM::IOPid)p); //stub
  }
 }
 else
 {//V1.0+
  DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, iosId);
  DWORD slot_add_i1 = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_I1);
  DWORD slot_add_i2 = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_I2);
  for(int p = FWDM::IOP_ECF; p < FWDM::IOP_COUNT; ++p)
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if (plug == 0)
    continue; //skip reserved plugs
   //���� ���������� ���������� ADD_IO1, �� ����� ����� ��������� ADD_I1;
   //���� ���������� ���������� ADD_IO2, �� ����� ����� ��������� ADD_I2;
   if((plug == slot) || (iosId == FWDM::IOS_ADD_IO1 && plug == slot_add_i1) || (iosId == FWDM::IOS_ADD_IO2 && plug == slot_add_i2))
    _AttachPlug((FWDM::IOPid)p); //stub
  }
 }
}

void CFWIORemappingController::_UpdateView(void)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {//V0.0
  for(int s = FWDM::IOS_ECF; s < FWDM::IOS_COUNTv0; ++s)
  {  
   DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, (FWDM::IOSid)s);
   for(int p = FWDM::IOP_ECFv0; p < FWDM::IOP_COUNTv0; ++p)
   {
    DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
    //If slot == stub, then it is not allowed in this firmware
    if (slot == plug && slot != mp_fwdm->GetSStub())
     mp_view->SelectItem((FWDM::IOSid)s, (FWDM::IOPid)p);
   }
  }
 }
 else
 {//V1.0+
  for(int s = FWDM::IOS_ECF; s < FWDM::IOS_COUNT; ++s)
  {  
   DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, (FWDM::IOSid)s);
   DWORD slot_add_i1 = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_I1);
   DWORD slot_add_i2 = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, FWDM::IOS_ADD_I2);
   for(int p = FWDM::IOP_ECF; p < FWDM::IOP_COUNT; ++p)
   {
    DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
    //If slot == stub, then it is not allowed in this firmware
    if ((slot == plug && slot != mp_fwdm->GetSStub()) || 
       (s == FWDM::IOS_ADD_IO1 && plug == slot_add_i1) || (s == FWDM::IOS_ADD_IO2 && plug == slot_add_i2))
     mp_view->SelectItem((FWDM::IOSid)s, (FWDM::IOPid)p);
   }
  }
 }
}

void CFWIORemappingController::_AttachPlug(FWDM::IOPid iopId)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
 {//V0.0
  mp_fwdm->SetIOPlug(FWDM::IOX_INIT, iopId, mp_fwdm->GetSStub());
  mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetSStub());
 }
 else
 {//V1.0+
  mp_fwdm->SetIOPlug(FWDM::IOX_INIT, iopId, mp_fwdm->GetSStub());
  if (_IsIOPInput(iopId))
   mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetGStub()); //��� ������ ���� ��������, 
  else
   mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetSStub()); //��� ������� ����
 }
}

void CFWIORemappingController::_AttachPlugToSpecifiedSlot(FWDM::IOPid iopId, FWDM::IOSid iosId)
{
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)//V0.0
 _AttachPlug(iopId, iosId);
 else
 {//V1.0+
  if (_IsIOPInput(iopId) && iosId == FWDM::IOS_ADD_IO1)
   _AttachPlug(iopId, FWDM::IOS_ADD_I1); //substitute IOS_ADD_IO1 to IOS_ADD_I1
  else if (_IsIOPInput(iopId) && iosId == FWDM::IOS_ADD_IO2)
   _AttachPlug(iopId, FWDM::IOS_ADD_I2); //substitute IOS_ADD_IO2 to IOS_ADD_I2
  else if (iopId == FWDM::IOP_ADD_IO1)   //when connecting ADD_IO1 plug back to ADD_IO1 slot we also have to connect ADD_I1
   _AttachPlug(FWDM::IOP_ADD_IO1, iosId), _AttachPlug(FWDM::IOP_ADD_I1, FWDM::IOS_ADD_I1);
  else if (iopId == FWDM::IOP_ADD_IO2)   //when connecting ADD_IO1 plug back to ADD_IO1 slot we also have to connect ADD_I1
   _AttachPlug(FWDM::IOP_ADD_IO2, iosId), _AttachPlug(FWDM::IOP_ADD_I2, FWDM::IOS_ADD_I2);
  else
   _AttachPlug(iopId, iosId);
 }
}

void CFWIORemappingController::_AttachPlug(FWDM::IOPid iopId, FWDM::IOSid iosId)
{
 mp_fwdm->SetIOPlug(FWDM::IOX_INIT, iopId, mp_fwdm->GetIOSlot(FWDM::IOX_INIT, iosId));
 mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetIOSlot(FWDM::IOX_DATA, iosId));
}

bool CFWIORemappingController::_IsIOPInput(FWDM::IOPid iopId) const
{
 return (iopId == FWDM::IOP_PS || iopId == FWDM::IOP_ADD_I1 || iopId == FWDM::IOP_ADD_I2 || iopId == FWDM::IOP_IGN);
}

bool CFWIORemappingController::_IsIOSInput(FWDM::IOSid iosId) const
{
 return (iosId == FWDM::IOS_PS || iosId == FWDM::IOS_ADD_I1 || iosId == FWDM::IOS_ADD_I2);
}

bool CFWIORemappingController::_FixRedundantConnections(void)
{
 bool result = true;
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 int beginPlug, endPlug;
 if (iov == FWDM::IOV_V00) {//V0.0
  beginPlug = FWDM::IOP_ECFv0;
  endPlug = FWDM::IOP_COUNTv0;
 }
 else {//V1.0+
  beginPlug = FWDM::IOP_ECF;
  endPlug = FWDM::IOP_COUNT; 
 }

 for(int p = beginPlug; p < endPlug; ++p)
 {
  DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
  if (plug == 0)
   continue;
  bool find = false;
  for(int pp = beginPlug; pp < endPlug; ++pp)
  {
   DWORD value = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)pp);
   if (value == 0 || value == mp_fwdm->GetSStub() || value == mp_fwdm->GetGStub())
    continue; //skip reserved plugs and stubs
   if (plug == value)
   {
    if (find)//redundant?
    {
     _AttachPlug((FWDM::IOPid)pp); //remove redundant connection
     result = false;
    }
    else
     find = true;
   }
  }
 }
 return result;
}

bool CFWIORemappingController::_FixInputsVSOutputs(void)
{
 bool result = true;
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 if (iov == FWDM::IOV_V00)
  return result; //V0.0 had no input remapping capabilities!

 //V1.0+
 for(int p = FWDM::IOP_ECF; p < FWDM::IOP_COUNT; ++p)
 {
  if (mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p) == 0)
   continue; //skip reserved plugs
  
  FWDM::IOSid sId = _GetConnectedSlot((FWDM::IOPid)p, true); //init
  if ((sId != FWDM::IOS_NA) && ((_IsIOPInput((FWDM::IOPid)p) && !_IsIOSInput((FWDM::IOSid)sId)) ||
      (!_IsIOPInput((FWDM::IOPid)p) &&  _IsIOSInput((FWDM::IOSid)sId))))
  {
    _AttachPlug((FWDM::IOPid)p); //fix using a stub
    result = false;
  }
  sId = _GetConnectedSlot((FWDM::IOPid)p, true); //data
  if ((sId != FWDM::IOS_NA) && ((_IsIOPInput((FWDM::IOPid)p) && !_IsIOSInput((FWDM::IOSid)sId)) ||
      (!_IsIOPInput((FWDM::IOPid)p) &&  _IsIOSInput((FWDM::IOSid)sId))))
  {
    _AttachPlug((FWDM::IOPid)p); //fix using a stub
    result = false;
  }
 }
 return result;
}

bool CFWIORemappingController::_CheckErrors(void)
{
 //Check and disconnect plugs if they are redundant
 bool result = _FixRedundantConnections();
 ASSERT(result);

 //Inputs must not be connected to outputs and vice versa
 result = _FixInputsVSOutputs();
 ASSERT(result);

 //Attach free slots to default plugs
 _AttachFreeSlotsToDefaultPlugs();

 _DisplayPlugs();
 return true;
}

FWDM::IOSid CFWIORemappingController::_GetConnectedSlot(FWDM::IOPid iopId, bool init /*=true*/)
{
 DWORD plug = mp_fwdm->GetIOPlug(init ? FWDM::IOX_INIT : FWDM::IOX_DATA, iopId);
 int slotCount = (FWDM::IOV_V00 == mp_fwdm->GetIORemVersion()) ? FWDM::IOS_COUNTv0 : FWDM::IOS_COUNT;
 for(int s = FWDM::IOS_ECF; s < slotCount; ++s)
 {
  DWORD slot = mp_fwdm->GetIOSlot(init ? FWDM::IOX_INIT : FWDM::IOX_DATA, (FWDM::IOSid)s);
  if (plug == slot)
   return (FWDM::IOSid)s;
 }
 return FWDM::IOS_NA; //there is no slot connected to specified plug
}

void CFWIORemappingController::_DisplayPlugs(void)
{
#ifdef IOREMCNTR_DEBUG
 FWDM::IORemVer iov = mp_fwdm->GetIORemVersion();
 int beginPlug, endPlug;
 if (iov == FWDM::IOV_V00) {//V0.0
  beginPlug = FWDM::IOP_ECFv0;
  endPlug = FWDM::IOP_COUNTv0;
 }
 else {//V1.0+
  beginPlug = FWDM::IOP_ECF;
  endPlug = FWDM::IOP_COUNT; 
 }
 std::map<FWDM::IOSid, _TSTRING> names;
 std::map<FWDM::IOSid, _TSTRING>::iterator it;
 names.insert(std::make_pair(FWDM::IOS_ECF, _T("ECF")));
 names.insert(std::make_pair(FWDM::IOS_ST_BLOCK, _T("ST_BLOCK")));
 names.insert(std::make_pair(FWDM::IOS_IGN_OUT3, _T("IGN_OUT3")));
 names.insert(std::make_pair(FWDM::IOS_IGN_OUT4, _T("IGN_OUT4")));
 names.insert(std::make_pair(FWDM::IOS_ADD_IO1, _T("ADD_IO1")));
 names.insert(std::make_pair(FWDM::IOS_ADD_IO2, _T("ADD_IO2")));
 names.insert(std::make_pair(FWDM::IOS_IE, _T("IE")));
 names.insert(std::make_pair(FWDM::IOS_FE, _T("FE")));
 if (iov > FWDM::IOV_V00) {//V1.0+
  names.insert(std::make_pair(FWDM::IOS_PS, _T("PS")));
  names.insert(std::make_pair(FWDM::IOS_ADD_I1, _T("ADD_I1")));
  names.insert(std::make_pair(FWDM::IOS_ADD_I2, _T("ADD_I2")));
 }
 CString out(_T("      [INIT]            [DATA]\n"));
 for(int p = beginPlug; p < endPlug; ++p)
 {
  FWDM::IOSid sId;
  CString str_i, str_d;
  sId = _GetConnectedSlot((FWDM::IOPid)p, true);
  it = names.find(sId);
  if (sId != FWDM::IOS_NA && it != names.end())
  {
   str_i = it->second.c_str();
  }
  else
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if (plug == mp_fwdm->GetSStub())
    str_i = _T("SSTUB");
   else if (plug == mp_fwdm->GetGStub())
    str_i = _T("GSTUB");
   else
    str_i = _T("?");
  }
  sId = _GetConnectedSlot((FWDM::IOPid)p, false);
  it = names.find(sId);
  if (sId != FWDM::IOS_NA && it != names.end())
   str_d = it->second.c_str();
  else
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_DATA, (FWDM::IOPid)p);
   if (plug == mp_fwdm->GetSStub())
    str_d = _T("SSTUB");
   else if (plug == mp_fwdm->GetGStub())
    str_d = _T("GSTUB");
   else
    str_d = _T("?");
  }
  while(str_i.GetLength() < 20)
   str_i+=_T(" ");

  CString n;
  n.Format("%02d   ", p);
  out+=n+str_i+str_d+_T("\n");
 }
 AfxMessageBox(out);
#endif //IOREMCNTR_DEBUG
}
