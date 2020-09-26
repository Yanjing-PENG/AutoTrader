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

CustomTraderSpi::CustomTraderSpi(CThostFtdcTraderApi *pTraderApi, std::queue<Message> *msgQueue, std::mutex *queMutex)
{
	this->m_pTraderApi = pTraderApi;
	this->m_msgQueue = msgQueue;
	this->m_queMutex = queMutex;
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
	/*Message msg;
	msg.msg_type = TRADER_FRONT_CONNECTED;
	this->m_msgQueue->push(msg);
	std::cout << "Trader FRONT CONNECTED" << std::endl;
	*/
	std::cout << "Trader front connection successed." << std::endl;
	CThostFtdcReqAuthenticateField authReq;
	memset(&authReq, 0, sizeof(authReq));
	strcpy(authReq.BrokerID, "9999");
	strcpy(authReq.UserID, "102757");
	strcpy(authReq.AppID, "simnow_client_test");
	strcpy(authReq.AuthCode, "0000000000000000");
	this->m_pTraderApi->ReqAuthenticate(&authReq, 1);
}

void CustomTraderSpi::OnFrontDisconnected(int nReason)
{
	std::cout << "Trader front disconnected. Reason: " << nReason << std::endl;
}

void CustomTraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	/*if (!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_AUTHENTICATE;
		msg.data = pRspAuthenticateField;
		this->m_msgQueue->push(msg);
	}*/
	std::cout << "Authentication successed." << std::endl;
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, "9999");
	strcpy(loginReq.UserID, "102757");
	strcpy(loginReq.Password, "1qaz@WSX");
	this->m_pTraderApi->ReqUserLogin(&loginReq, 2);
}

void CustomTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	/*if (!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = TRADER_FRONT_LOGIN;
		msg.data = pRspUserLogin;
		this->m_msgQueue->push(msg);
	}*/
	std::cout << "Trader front login successed." << std::endl;
	
	CThostFtdcSettlementInfoConfirmField settlementConfirmReq;
	memset(&settlementConfirmReq, 0, sizeof(settlementConfirmReq));
	strcpy(settlementConfirmReq.BrokerID, "9999");
	strcpy(settlementConfirmReq.InvestorID, "102757");
	this->m_pTraderApi->ReqSettlementInfoConfirm(&settlementConfirmReq, 3);
}

void CustomTraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!this->isErrorRspInfo(pRspInfo))
	{   /*
		Message msg;
		msg.msg_type = TRADER_FRONT_LOGOUT;
		msg.data = pUserLogout;
		this->m_msgQueue->push(msg);
		*/
		std::cout << "Trader front logout successed." << std::endl;
	}
}

void CustomTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_SETTLEMENT_INFO_CONFIRM;
		msg.data = pSettlementInfoConfirm;
		this->m_queMutex->lock();
		this->m_msgQueue->push(msg);
		this->m_queMutex->unlock();
	}
	/*
	std::cout << "Settlement Info Confirmed. " << std::endl;
	std::cout << "SettlementID: " << pSettlementInfoConfirm->SettlementID << std::endl;
	CThostFtdcQryInstrumentField instrumentReq;
	memset(&instrumentReq, 0, sizeof(instrumentReq));
	strcpy(instrumentReq.InstrumentID, "IF2010");
	this->m_pTraderApi->ReqQryInstrument(&instrumentReq, 4);
	*/
}

void CustomTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_QRY_INSTRUMENT;
		msg.data = pInstrument;
		this->m_queMutex->lock();
		this->m_msgQueue->push(msg);
		this->m_queMutex->unlock();
	}
	/*
	std::cout << "Query instrument successed." << std::endl;
	std::cout << "InstrumentID: " << pInstrument->InstrumentID << " ExpireDate: " << pInstrument->ExpireDate << std::endl;
	 
	//std :: cout << InstrumentIDs[0] << std::endl;
	CThostFtdcQryTradingAccountField tradingAccountReq;
	memset(&tradingAccountReq, 0, sizeof(tradingAccountReq));
	strcpy(tradingAccountReq.BrokerID, "9999");
	strcpy(tradingAccountReq.InvestorID, "102757");
	this->m_pTraderApi->ReqQryTradingAccount(&tradingAccountReq, 5);
	*/
 
}

void CustomTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_QRY_TRADING_ACCOUNT;
		msg.data = pTradingAccount;
		this->m_queMutex->lock();
		this->m_msgQueue->push(msg);
		this->m_queMutex->unlock();
	}
	/*
	std::cout << "Query trading account successed." << std::endl;
	std::cout << "AccountID: " << pTradingAccount->AccountID << std::endl;
	std::cout << "Available: " << pTradingAccount->Available << std::endl;
	CThostFtdcQryInvestorPositionField positionReq;
	memset(&positionReq, 0, sizeof(positionReq));
	strcpy(positionReq.BrokerID, "9999");
	strcpy(positionReq.InvestorID, "102757");
	strcpy(positionReq.InstrumentID, "IF2009");
	this->m_pTraderApi->ReqQryInvestorPosition(&positionReq, 6);
	*/
}

void CustomTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_QRY_INVESTOR_POSITION;
		msg.data = pInvestorPosition;
		this->m_queMutex->lock();
		this->m_msgQueue->push(msg);
		this->m_queMutex->unlock();
	}
	/*
	std::cout << "Query Investor position successed." << std::endl;
	if (pInvestorPosition)
	{
		std::cout << "InstrumentID: " << pInvestorPosition->InstrumentID << std::endl;
		std::cout << "Direction: " << pInvestorPosition->PosiDirection << std::endl;
		std::cout << "Position: " << pInvestorPosition->Position << std::endl;
	}
	else
	{
		std::cout << "Does not have position." << std::endl;
	}
	CThostFtdcInputOrderField orderReq;
	memset(&orderReq, 0, sizeof(orderReq));
	strcpy(orderReq.BrokerID, "9999");
	strcpy(orderReq.InvestorID, "102757");
	strcpy(orderReq.ExchangeID, "CFFEX");
	strcpy(orderReq.InstrumentID, "IF2009");
	strcpy(orderReq.UserID, "102757");
	orderReq.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	orderReq.Direction = THOST_FTDC_D_Buy;
	orderReq.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	orderReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	orderReq.LimitPrice = 4700.0;
	orderReq.VolumeTotalOriginal = 1;
	orderReq.TimeCondition = THOST_FTDC_TC_GFD;
	orderReq.VolumeCondition = THOST_FTDC_VC_AV;
	orderReq.MinVolume = 1;
	orderReq.ContingentCondition = THOST_FTDC_CC_Immediately;
	orderReq.StopPrice = 0;
	orderReq.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	orderReq.IsAutoSuspend = 0;
	this->m_pTraderApi->ReqOrderInsert(&orderReq, 7);
	*/
}

void CustomTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	/*if (!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_ORDER_INSERT;
		msg.data = pInputOrder;
		this->m_msgQueue->push(msg);
	}*/
	std::cout << "OnRspOrderInsert." << std::endl;
	std::cout << "ErrorID: " << pRspInfo->ErrorID << " Error message: " << pRspInfo->ErrorMsg << std::endl;
}

void CustomTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	std::cout << "OnErrRtnOrderInsert." << std::endl;
	std::cout << "ErrorID: " << pRspInfo->ErrorID << " Error message: " << pRspInfo->ErrorMsg << std::endl;
}

void CustomTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	/*Message msg;
	msg.msg_type = RTN_ORDER;
	msg.data = pOrder;
	this->m_msgQueue->push(msg);*/
	std::cout << "OnRtnOrder: " << pOrder->OrderStatus << ", " << pOrder->LimitPrice << std::endl;
}

void CustomTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	/*Message msg;
	msg.msg_type = RTN_TRADE;
	msg.data = pTrade;
	this->m_msgQueue->push(msg);*/
	std::cout << "OnRtnTrade: " << pTrade->Price << ", " << pTrade->Direction <<std::endl;
}

void CustomTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	/*if (!this->isErrorRspInfo(pRspInfo))
	{
		Message msg;
		msg.msg_type = RSP_ORDER_ACTION;
		msg.data = pInputOrderAction;
		this->m_msgQueue->push(msg);
	}*/
	std::cout << "OnRspOrderAction: " << pInputOrderAction->InstrumentID << ", " << pInputOrderAction->OrderSysID << std::endl;
}

void CustomTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cout << "ErrorID: " << pRspInfo->ErrorID << ", Error message: " << pRspInfo->ErrorMsg << std::endl;
}

