/*
 * CustomMdSpi.h
 *
 *  Created on: Feb 5, 2020
 *      Author: pony
 */

#ifndef HEADER_CUSTOMMDSPI_H_
#define HEADER_CUSTOMMDSPI_H_

#include "ThostFtdcMdApi.h"
#include "CustomUserStruct.h"
#include <fstream>
#include <queue>
#include <mutex>

class CustomMdSpi : public CThostFtdcMdSpi
{
private:
	CThostFtdcMdApi *m_pMdApi;
	//std::ofstream logFile;
	std::queue<Message> *m_msgQueue;
	std::mutex *m_queMutex;

public:

	CustomMdSpi(CThostFtdcMdApi *pMdApi, std::queue<Message> *msgQueue, std::mutex *queMutex);

	virtual ~CustomMdSpi();

	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(int nReason);

	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
};

#endif /* HEADER_CUSTOMMDSPI_H_ */
