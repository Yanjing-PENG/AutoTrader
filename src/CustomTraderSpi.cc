/*
 * CustomTraderSpi.cpp
 *
 *  Created on: Feb 11, 2020
 *      Author: pony
 */

#include "CustomTraderSpi.h"
#include "ThostFtdcUserApiStruct.h"
#include <cstring>
#include <iostream>

//extern std::string BrokerID;
//extern std::string UserID;
//extern std::string Password;
//extern std::string AppID;
//extern std::string AuthCode;
//extern char *InstrumentIDs[];
//extern int nInstruments;

CustomTraderSpi::CustomTraderSpi(CThostFtdcTraderApi *pTraderApi, std::queue<Message> *msgQueue)
{
	this->m_pTraderApi = pTraderApi;
	this->m_msgQueue = msgQueue;
	std::cout << "Trader Api version: " << CThostFtdcTraderApi::GetApiVersion() << std::endl;
}

CustomTraderSpi::~CustomTraderSpi(){}

bool CustomTraderSpi::isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
	{
		//std::cerr << "Trader front return error, ErrorID: " << pRspInfo->ErrorID << std::endl;
		std::cout << "ErrorID: " << pRspInfo->ErrorID << " Error message: " << pRspInfo->ErrorMsg << std::endl;
	}
	return bResult;
}

void CustomTraderSpi::OnFrontConnected()
{
	Message msg;
	msg.msg_type = TRADER_FRONT_CONNECTED;
	this->m_msgQueue->push(msg);
	//std::cout << "Trader FRONT CONNECTED" << std::endl;
}

void CustomTraderSpi::OnFrontDisconnected(int nReason)
{
	std::cout << "Trader front disconnected. Reason: " << nReason << std::endl;
}

void CustomTraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_AUTHENTICATE;
		msg.data = pRspAuthenticateField;
		this->m_msgQueue->push(msg);
	}
}

void CustomTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = TRADER_FRONT_LOGIN;
		msg.data = pRspUserLogin;
		this->m_msgQueue->push(msg);
	}
}

void CustomTraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = TRADER_FRONT_LOGOUT;
		msg.data = pUserLogout;
		this->m_msgQueue->push(msg);
	}
}

void CustomTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_SETTLEMENT_INFO_CONFIRM;
		msg.data = pSettlementInfoConfirm;
		this->m_msgQueue->push(msg);
	}
}

void CustomTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_QRY_INSTRUMENT;
		msg.data = pInstrument;
		this->m_msgQueue->push(msg);
	}
}

void CustomTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_QRY_TRADING_ACCOUNT;
		msg.data = pTradingAccount;
		this->m_msgQueue->push(msg);
	}
}

void CustomTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_QRY_INVESTOR_POSITION;
		msg.data = pInvestorPosition;
		this->m_msgQueue->push(msg);
	}
}

void CustomTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_ORDER_INSERT;
		msg.data = pInputOrder;
		this->m_msgQueue->push(msg);
	}
}

void CustomTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_ORDER_ACTION;
		msg.data = pInputOrderAction;
		this->m_msgQueue->push(msg);
	}
}

void CustomTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	Message msg;
	msg.msg_type = RTN_ORDER;
	msg.data = pOrder;
	this->m_msgQueue->push(msg);
}

void CustomTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	Message msg;
	msg.msg_type = RTN_TRADE;
	msg.data = pTrade;
	this->m_msgQueue->push(msg);
}

void CustomTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cout << "Error message: " << pRspInfo->ErrorMsg << std::endl;
}

