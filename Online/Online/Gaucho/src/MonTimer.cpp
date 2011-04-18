#include "Gaucho/MonTimer.h"
#include "Gaucho/MonSubSys.h"

MonTimer::MonTimer(MonSubSys *HSys, int period) : GenTimer((void*)HSys,period*1000)
{
  m_Hsys = HSys;
  m_dueTime = 0;
}

MonTimer::~MonTimer( )
{
}

void MonTimer::timerHandler ( void )
{
  m_Hsys->Lock();
  m_Hsys->m_genSrv->setRunNo(m_Hsys->m_runno);
  m_Hsys->m_genSrv->setTime(m_dueTime);
  m_Hsys->makeRates();
  m_Hsys->m_genSrv->Serialize();
  m_Hsys->unLock();

  m_Hsys->m_genSrv->Update();
}

void MonTimer::Stop()
{
  m_Hsys->Lock();
  GenTimer::Stop();
  m_Hsys->unLock();
}
