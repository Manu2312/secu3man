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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CFWIORemappingController::CFWIORemappingController(IORVIEW* ip_view)
: mp_view(ip_view)
{
 mp_view->setOnItemSelected(MakeDelegate(this, &CFWIORemappingController::OnItemSelected));

 m_defValMap.insert(std::make_pair(FWDM::IOP_ECF, FWDM::IOS_ECF));
 m_defValMap.insert(std::make_pair(FWDM::IOP_ST_BLOCK, FWDM::IOS_ST_BLOCK));
 m_defValMap.insert(std::make_pair(FWDM::IOP_IGN_OUT3, FWDM::IOS_IGN_OUT3));
 m_defValMap.insert(std::make_pair(FWDM::IOP_IGN_OUT4, FWDM::IOS_IGN_OUT4));
 m_defValMap.insert(std::make_pair(FWDM::IOP_ADD_IO1, FWDM::IOS_ADD_IO1));
 m_defValMap.insert(std::make_pair(FWDM::IOP_ADD_IO2, FWDM::IOS_ADD_IO2));
 m_defValMap.insert(std::make_pair(FWDM::IOP_IE, FWDM::IOS_IE));
 m_defValMap.insert(std::make_pair(FWDM::IOP_FE, FWDM::IOS_FE));
}

CFWIORemappingController::~CFWIORemappingController()
{
 //empty
}

void CFWIORemappingController::AttachFWDM(FWDM* ip_fwdm)
{
 mp_fwdm = ip_fwdm;
 ASSERT(mp_fwdm);

 _CheckErrors();

 _UpdateView();
}
void CFWIORemappingController::OnActivate(void)
{
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_ECF, FWDM::IOP_ECF, _T("NONE"));

 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_ST_BLOCK, FWDM::IOP_ST_BLOCK, _T("NONE"));

 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT3, FWDM::IOP_IGN_OUT3, _T("NONE"));

 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_IGN_OUT4, FWDM::IOP_IGN_OUT4, _T("NONE"));

 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_ADD_IO1, FWDM::IOP_ADD_IO1, _T("NONE"));

 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_ADD_IO2, FWDM::IOP_ADD_IO2, _T("NONE"));

 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_IE, FWDM::IOP_IE, _T("NONE"));

 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FL_PUMP, _T("FL_PUMP"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_HALL_OUT, _T("HALL_OUT"));
 mp_view->AddItem(FWDM::IOS_FE, FWDM::IOP_FE, _T("NONE"));
}

void CFWIORemappingController::OnDeactivate(void)
{
 //empty
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
 //Detach redundant plugs
 _DetachPlugsFromSlot(iosId);

 //Attach selected plug to slot
 _AttachPlug(iopId, iosId);

 //Update UI
 _UpdateView();
}

void CFWIORemappingController::_DetachPlugsFromSlot(FWDM::IOSid iosId)
{
 DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, iosId);
 for(int p = FWDM::IOP_ECF; p <= FWDM::IOP_COUNT; ++p)
 {
  DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
  if(plug == slot)
  {
   std::map<FWDM::IOPid, FWDM::IOSid>::const_iterator it = m_defValMap.find((FWDM::IOPid)p);
   if (it == m_defValMap.end())
    _AttachPlug((FWDM::IOPid)p); //stub
   else
    _AttachPlug((FWDM::IOPid)p, it->second); //default
  }
 }
}

void CFWIORemappingController::_UpdateView(void)
{
 for(int s = FWDM::IOS_ECF; s <= FWDM::IOS_COUNT; ++s)
 {
  DWORD slot = mp_fwdm->GetIOSlot(FWDM::IOX_INIT, (FWDM::IOSid)s);
  for(int p = FWDM::IOP_ECF; p <= FWDM::IOP_COUNT; ++p)
  {
   DWORD plug = mp_fwdm->GetIOPlug(FWDM::IOX_INIT, (FWDM::IOPid)p);
   if (slot == plug)
    mp_view->SelectItem((FWDM::IOSid)s, (FWDM::IOPid)p);
  }
 }
}

void CFWIORemappingController::_AttachPlug(FWDM::IOPid iopId)
{
 mp_fwdm->SetIOPlug(FWDM::IOX_INIT, iopId, mp_fwdm->GetSStub());
 mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetSStub());
}

void CFWIORemappingController::_AttachPlug(FWDM::IOPid iopId, FWDM::IOSid iosId)
{
 mp_fwdm->SetIOPlug(FWDM::IOX_INIT, iopId, mp_fwdm->GetIOSlot(FWDM::IOX_INIT, iosId));
 mp_fwdm->SetIOPlug(FWDM::IOX_DATA, iopId, mp_fwdm->GetIOSlot(FWDM::IOX_DATA, iosId));
}

bool CFWIORemappingController::_CheckErrors(void)
{
 //todo
 return false;
}