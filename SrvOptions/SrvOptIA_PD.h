/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * 
 * released under GNU GPL v2 or later licence
 *
 *
 */

class TSrvOptIA_PD;

#ifndef SRVOPTIA_PD_H
#define SRVOPTIA_PD_H

#include "OptIA_PD.h"
#include "SrvOptIAPrefix.h" 
#include "SmartPtr.h"
#include "DUID.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h" 
#include "Container.h"
#include "IPv6Addr.h"

class TSrvOptIA_PD : public TOptIA_PD
{
  public:
    
    TSrvOptIA_PD(SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvOptIA_PD> queryOpt,
		 SmartPtr<TIPv6Addr> clntAddr, SmartPtr<TDUID> duid,
		 int iface, unsigned long &addrCount, int msgType , TMsg* parent);

    TSrvOptIA_PD(char * buf, int bufsize, TMsg* parent);    
    TSrvOptIA_PD(long IAID, long T1, long T2, TMsg* parent);    
    TSrvOptIA_PD(long IAID, long T1, long T2, int Code, string Msg, TMsg* parent);

/* Constructor used in answers to:
 * - SOLICIT 
 * - SOLICIT (with RAPID_COMMIT)
 * - REQUEST */
    TSrvOptIA_PD(SmartPtr<TSrvAddrMgr> addrMgr,  SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvOptIA_PD> queryOpt,
		 SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr, 
		 int iface, int msgType, TMsg* parent);
    
    void releaseAllPrefixes(bool quiet);

    void solicit(SmartPtr<TSrvOptIA_PD> queryOpt);
    void request(SmartPtr<TSrvOptIA_PD> queryOpt);
    void renew(SmartPtr<TSrvOptIA_PD> queryOpt);
    void rebind(SmartPtr<TSrvOptIA_PD> queryOpt);
    void release(SmartPtr<TSrvOptIA_PD> queryOpt);
    void confirm(SmartPtr<TSrvOptIA_PD> queryOpt);
    void decline(SmartPtr<TSrvOptIA_PD> queryOpt);
    bool doDuties();
 private:
    SmartPtr<TSrvAddrMgr> AddrMgr;
    SmartPtr<TSrvCfgMgr>  CfgMgr;
    SmartPtr<TIPv6Addr>   ClntAddr;
    SmartPtr<TDUID>       ClntDuid;
    int                   Iface;
    
    int assignPrefix(SmartPtr<TIPv6Addr> hint, bool quiet);
    List(TIPv6Addr) getFreePrefixes(SmartPtr<TIPv6Addr> hint);

    unsigned long Prefered;
    unsigned long Valid;
    unsigned long PDLength;

};

#endif
