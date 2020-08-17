/*
 * Manager.h
 *
 *  Created on: Feb 10, 2020
 *      Author: pony
 */

#ifndef HEADER_MANAGER_H_
#define HEADER_MANAGER_H_

#include "ThostFtdcMdApi.h"
#include "CustomMdSpi.h"
#include "ThostFtdcTraderApi.h"
#include "CustomTraderSpi.h"
#include "CustomUserStruct.h"
#include <queue>

class Manager{

private:
	//account info
	std::string m_brokerID;
	std::string m_userID;
	std::string m_password;
	std::string m_appID;
	std::string m_authCode;
	char m_marketFrontAddr[30];
	char m_traderFrontAddr[30];

	//instruments info
	char * *m_instrumentIDs;
	int m_nInstruments;

	//api and spi
	CThostFtdcMdApi *m_pMdApi;
	CustomMdSpi *m_pMdSpi;
	CThostFtdcTraderApi *m_pTraderApi;
	CustomTraderSpi *m_pTraderSpi;
	int m_nRequestID;
	TThostFtdcOrderRefType m_orderRef;
	int m_count;

	std::queue<Message> m_msgQueue;
	std::ofstream m_logFile;
	std::ofstream m_signalFile;

	double m_ask[2];
	double m_bid[2];
	bool m_bReady[2];

public:
	void setAccountInfo(std::string brokerID, std::string userID, std::string password, std::string appID, std::string authCode);
	void setMarketFrontAddr(std::string marketFrontAddr);
	void setTraderFrontAddr(std::string traderFrontAddr);
	void setInstruments(char * *instrumentIDs, int nInstruments);
	void init();
	void run();
	~Manager()
	{
		this->m_signalFile.close();
	}

};


#endif /* HEADER_MANAGER_H_ */
