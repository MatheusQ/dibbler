/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 licence
 *
 * $Id: AddrClient.cpp,v 1.15 2007-12-09 04:08:35 thomson Exp $
 *
 */

#include <iostream>
#include <string>
#include <limits.h>
#include "AddrClient.h"
#include "Logger.h"

TAddrClient::TAddrClient(SmartPtr<TDUID> duid) {
    this->DUID=duid;
}

SmartPtr<TDUID> TAddrClient::getDUID() {
    return this->DUID;
}

// --- IA ------------------------------------------------------------

SmartPtr<TAddrIA> TAddrClient::getIA() {
    return IAsLst.get();
}

SmartPtr<TAddrIA> TAddrClient::getIA(unsigned long IAID) {
    SmartPtr<TAddrIA> ptr;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getIAID() == IAID) {
            return ptr;
        }
    }
    return SmartPtr<TAddrIA>();
}

void TAddrClient::firstIA() {
    IAsLst.first();
}

void TAddrClient::addIA(SmartPtr<TAddrIA> ia) {
    IAsLst.append(ia);
}

int TAddrClient::countIA() {
    return IAsLst.count();
}

bool TAddrClient::delIA(unsigned long IAID) {
    SmartPtr<TAddrIA> ptr;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getIAID() == IAID) {
            IAsLst.del();
            return true;
        }
    }
    return false;
}

// --- PD ------------------------------------------------------------

SmartPtr<TAddrIA> TAddrClient::getPD() {
    return PDLst.get();
}

SmartPtr<TAddrIA> TAddrClient::getPD(unsigned long IAID) {
    SmartPtr<TAddrIA> ptr;
    PDLst.first();

    while ( ptr = PDLst.get() ) {
        if (ptr->getIAID() == IAID) {
            return ptr;
        }
    }
    return 0;
}

void TAddrClient::firstPD() {
    PDLst.first();
}

void TAddrClient::addPD(SmartPtr<TAddrIA> pd) {
    PDLst.append(pd);
}

int TAddrClient::countPD() {
    return PDLst.count();
}

bool TAddrClient::delPD(unsigned long IAID) {
    SmartPtr<TAddrIA> ptr;
    PDLst.first();

    while ( ptr = PDLst.get() ) {
        if (ptr->getIAID() == IAID) {
            PDLst.del();
            return true;
        }
    }
    return false;
}

// --- TA ------------------------------------------------------------

SmartPtr<TAddrIA> TAddrClient::getTA() {
    return TALst.get();
}

SmartPtr<TAddrIA> TAddrClient::getTA(unsigned long IAID) {
    SmartPtr<TAddrIA> ptr;
    TALst.first();

    while ( ptr = TALst.get() ) {
        if (ptr->getIAID() == IAID) {
            return ptr;
        }
    }
    return 0;
}

void TAddrClient::firstTA() {
    TALst.first();
}

void TAddrClient::addTA(SmartPtr<TAddrIA> ia) {
    TALst.append(ia);
}

int TAddrClient::countTA() {
    return TALst.count();
}

bool TAddrClient::delTA(unsigned long iaid) {
    SmartPtr<TAddrIA> ptr;
    TALst.first();
    while ( ptr = TALst.get() ) {
        if (ptr->getIAID() == iaid) {
            TALst.del();
            return true;
        }
    }
    return false;
}

// --------------------------------------------------------------------
// --- time related methods -------------------------------------------
// --------------------------------------------------------------------

unsigned long TAddrClient::getT1Timeout() {
    SmartPtr<TAddrIA> ptr;
    unsigned long ts = ULONG_MAX;

    IAsLst.first();
    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=STATE_CONFIGURED)
            continue;
        if (ts > ptr->getT1Timeout())
            ts = ptr->getT1Timeout();
    }

    PDLst.first();
    while ( ptr = PDLst.get() ) {
	if (ptr->getState()!=STATE_CONFIGURED)
	    continue;
        if (ts > ptr->getT1Timeout())
            ts = ptr->getT1Timeout();
    }
    return ts;
}

unsigned long TAddrClient::getT2Timeout() {
    SmartPtr<TAddrIA> ptr;
    unsigned long ts = ULONG_MAX;
    IAsLst.first();

    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=STATE_CONFIGURED)
            continue;
        if (ts > ptr->getT2Timeout())
            ts = ptr->getT2Timeout();
    }

    PDLst.first();
    while ( ptr = PDLst.get() ) {
	if (ptr->getState()!=STATE_CONFIGURED)
	    continue;
        if (ts > ptr->getT2Timeout())
            ts = ptr->getT2Timeout();
    }

    return ts;
}

unsigned long TAddrClient::getPrefTimeout() {
    SmartPtr<TAddrIA> ptr;
    unsigned long ts = ULONG_MAX;

    IAsLst.first();
    while ( ptr = IAsLst.get() ) {
        if (ptr->getState()!=STATE_CONFIGURED)
            continue;
        if (ts > ptr->getPrefTimeout())
            ts = ptr->getPrefTimeout();
    }

    PDLst.first();
    while ( ptr = PDLst.get() ) {
	if (ptr->getState()!=STATE_CONFIGURED)
	    continue;
        if (ts > ptr->getPrefTimeout())
            ts = ptr->getPrefTimeout();
    }

    return ts;
}

unsigned long TAddrClient::getValidTimeout() {
    SmartPtr<TAddrIA> ptr;
    unsigned long ts = ULONG_MAX;

    IAsLst.first();
    while ( ptr = IAsLst.get() ) {
        if (ts > ptr->getValidTimeout())
            ts = ptr->getValidTimeout();
    }

    PDLst.first();
    while ( ptr = PDLst.get() ) {
        if (ts > ptr->getValidTimeout())
            ts = ptr->getValidTimeout();
    }

    return ts;
}

unsigned long TAddrClient::getLastTimestamp() {

    unsigned long ts = 0;
    SmartPtr<TAddrIA> ptr;

    IAsLst.first();
    while ( ptr = IAsLst.get() ) {
        if (ts < ptr->getTimestamp())
            ts = ptr->getTimestamp();
    }

    firstTA();
    while ( ptr = getTA() ) {
        if (ts < ptr->getTimestamp())
            ts = ptr->getTimestamp();
    }


    PDLst.first();
    while ( ptr = PDLst.get() ) {
        if (ts > ptr->getTimestamp())
            ts = ptr->getTimestamp();
    }
    
    return ts;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream & operator<<(ostream & strum,TAddrClient &x) 
{
    if (x.DUID->getLen()==1)
	strum << "  <!-- 1-byte length DUID. DECLINED-ADDRESSES -->" << endl;
    strum << "  <AddrClient>" << endl;
    if (x.DUID->getLen())
	strum << "    " << *x.DUID;

    strum << "    <!-- " << x.IAsLst.count() << " IA(s) -->" << endl;
    SmartPtr<TAddrIA> ptr;
    x.IAsLst.first();
    while (ptr = x.IAsLst.get() ) {
        strum << *ptr;
    }

    strum << "    <!-- " << x.TALst.count() << " TA(s) -->" << endl;
    x.TALst.first();
    while (ptr = x.TALst.get() ) {
        strum << *ptr;
    }

    strum << "    <!-- " << x.PDLst.count() << " PD(s) -->" << endl;
    x.PDLst.first();
    while (ptr = x.PDLst.get() ) {
        strum << *ptr;
    }

    strum << "  </AddrClient>" << endl;
    if (x.DUID->getLen()==1)
	strum << "  <!-- 1-byte length DUID. DECLINED-ADDRESSES -->" << endl;
    return strum;
}
