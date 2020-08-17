/*
 * CustomTraderSpi.h
 *
 *  Created on: Feb 11, 2020
 *      Author: pony
 */

#ifndef HEADER_CUSTOMTRADERSPI_H_
#define HEADER_CUSTOMTRADERSPI_H_

#include "ThostFtdcTraderApi.h"
#include "CustomUserStruct.h"
#include <fstream>
#include <queue>

class CustomTraderSpi : public CThostFtdcTraderSpi
{
private:

	CThostFtdcTraderApi *m_pTraderApi;
	std::queue<Message> *m_msgQueue;
	//int m_nRequestID;
	//TThostFtdcOrderRefType	m_orderRef;

public:

	CustomTraderSpi(CThostFtdcTraderApi *pTraderApi, std::queue<Message> *msgQueue);

	virtual ~CustomTraderSpi();

	bool isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(int nReason);

	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};



#endif /* HEADER_CUSTOMTRADERSPI_H_ */
