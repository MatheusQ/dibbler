/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: DHCPClient.cpp,v 1.16 2004-12-03 20:51:42 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.15  2004/11/02 02:13:49  thomson
 * no message
 *
 * Revision 1.14  2004/11/01 23:31:24  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.13  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.12  2004/10/25 20:45:52  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.11  2004/09/07 15:37:44  thomson
 * Socket handling changes.
 *
 * Revision 1.10  2004/07/05 00:53:03  thomson
 * Various changes.
 *
 * Revision 1.7  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *                                                                           
 */

#include <iostream>
#include <stdlib.h>
#include "SmartPtr.h"
#include "DHCPClient.h"
#include "ClntTransMgr.h"
#include "IfaceMgr.h"
#include "ClntIfaceMgr.h"
#include "Logger.h"
#include "Portable.h"

volatile int serviceShutdown;

//static void foo() {
//    SmartPtr<TClntParsGlobalOpt> opt = new TClntParsGlobalOpt();
//}

TDHCPClient::TDHCPClient(string config)
{
    //foo();
    string oldconf = config+"-old";

    serviceShutdown = 0;
    srand(now());
    this->IsDone = false;

    IfaceMgr = new TClntIfaceMgr();
    if ( IfaceMgr->isDone() ) {
 	  Log(Crit) << "Fatal error during IfaceMgr init. Aborting." << LogEnd;
	  this->IsDone = true;
	  return;
    }

    IfaceMgr->dump(CLNTIFACEMGR_FILE);

    TransMgr = new TClntTransMgr(IfaceMgr, config);
    TransMgr->setThat(TransMgr);
}

void TDHCPClient::stop() {
    serviceShutdown = 1;

#ifdef WIN32
    // just to break select() in WIN32 systems
    SmartPtr<TIfaceIface> iface = IfaceMgr->getIfaceByID(TransMgr->getCtrlIface());
    Log(Warning) << "Sending SHUTDOWN packet on the " << iface->getName()
        << "/" << iface->getID() << " (addr=" << TransMgr->getCtrlAddr() << ")." << LogEnd;
    int fd = sock_add("", TransMgr->getCtrlIface(),"::",0,true, false); 
    char buf = CONTROL_MSG;
    int cnt=sock_send(fd,TransMgr->getCtrlAddr(),&buf,1,DHCPCLIENT_PORT,TransMgr->getCtrlIface());
    sock_del(fd);
#endif
}


void TDHCPClient::run()
{
    SmartPtr<TMsg> msg;
    while ( !TransMgr->isDone() && (!this->isDone()) )
    {
	if (serviceShutdown)
	    TransMgr->shutdown();
	
	TransMgr->doDuties();
	
	unsigned int timeout = TransMgr->getTimeout();
	if (timeout == 0)
	    timeout = 1;
	
        Log(Notice) << "Sleeping for " << timeout << " second(s)." << LogEnd;
        SmartPtr<TMsg> msg=IfaceMgr->select(timeout);
	
        if (msg) {
	    int iface = msg->getIface();
	    SmartPtr<TIfaceIface> ptrIface;
	    ptrIface = IfaceMgr->getIfaceByID(iface);
            Log(Notice) << "Received " << msg->getName() << " on " << ptrIface->getName() 
			<< "/" << iface	<< hex << ",TransID=0x" << msg->getTransID() 
			<< dec << ", addr=" << msg->countOption() << " opts:";
            SmartPtr<TOpt> ptrOpt;
            msg->firstOption();
            while (ptrOpt = msg->getOption() )
                Log(Cont) << " " << ptrOpt->getOptType(); 
            Log(Cont) << LogEnd;
	    
            TransMgr->relayMsg(msg);
        }
    }
    Log(Notice) << "Bye bye." << LogEnd;
}

bool TDHCPClient::isDone() {
    return IsDone;
}

bool TDHCPClient::checkPrivileges() {
    // FIXME: check privileges
    return true;
}

TDHCPClient::~TDHCPClient()
{
}
