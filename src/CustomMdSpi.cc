/*
 * CustomMdSpi.cpp
 *
 *  Created on: Feb 5, 2020
 *      Author: pony
 */

#include "CustomMdSpi.h"
#include "ThostFtdcUserApiStruct.h"
#include <cstring>
#include <iostream>

CustomMdSpi::CustomMdSpi(CThostFtdcMdApi *pMdApi, std::queue<Message> *msgQueue, std::mutex *queMutex)
{
	this->m_pMdApi = pMdApi;
	//this->logFile = logFile;
	this->m_msgQueue = msgQueue;
	this->m_queMutex = queMutex;
	std::cout << "Market Data Api version: " << CThostFtdcMdApi::GetApiVersion() << std::endl;
}

CustomMdSpi::~CustomMdSpi(){}

void CustomMdSpi::OnFrontConnected(){
	Message msg;
	msg.msg_type = MARKET_FRONT_CONNECTED;
	//this->m_msgQueue->push(msg);
	//std::cout << "MARKET FRONT CONNECTED" << std::endl;
	std::cout << "Market front connection successed." << std::endl;
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, "9999");
	strcpy(loginReq.UserID, "102757");
	strcpy(loginReq.Password, "1qaz@WSX");
	this->m_pMdApi->ReqUserLogin(&loginReq, 1);
}

void CustomMdSpi::OnFrontDisconnected(int nReason){
	std::cout << "Market front disconnected. Reason: " << nReason << std::endl;
	//this->logFile << "Market front disconnected. Reason: " << std::endl;
}

void CustomMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	//std::cout << "Market front UserLogin successed." << std::endl;
	//std::cout << pRspInfo->ErrorID << pRspInfo->ErrorMsg << std::endl;
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult){
		Message msg;
		msg.msg_type = MARKET_FRONT_LOGIN;
		msg.data = pRspUserLogin;
		this->m_queMutex->lock();
		this->m_msgQueue->push(msg);
		this->m_queMutex->unlock();
	}
	else{
		std::cout << "Market front loginError. ErrorID: " << pRspInfo->ErrorID
				<< " ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}
}

void CustomMdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
		if (!bResult){
			/*
			Message msg;
			msg.msg_type = MARKET_FRONT_LOGOUT;
			msg.data = pUserLogout;
			this->m_msgQueue->push(msg);
			*/
			std::cout << "Market front logout. UserID: " << pUserLogout->UserID << std::endl;
		}
		else{
			std::cout << "Market front logoutError. ErrorID: " << pRspInfo->ErrorID 
			<< " ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
		}
}

void CustomMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	std::cout << "OnRspError: ErrorID: " << pRspInfo->ErrorID << ", Error message: " << pRspInfo->ErrorMsg << std::endl;
	//this->logFile << "OnRspError, ErrorID: " << pRspInfo->ErrorID
			//<< " ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
}

void CustomMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult){
		//std::cout << "Sub market data successed. InstrumentID: " << pSpecificInstrument->InstrumentID << std::endl;
		Message msg;
		msg.msg_type = RSP_SUB_MARKETDATA;
		CThostFtdcSpecificInstrumentField *tem = new CThostFtdcSpecificInstrumentField;
		strcpy(tem->InstrumentID, pSpecificInstrument->InstrumentID);
		msg.data = tem;
		this->m_queMutex->lock();
		this->m_msgQueue->push(msg);
		this->m_queMutex->unlock();
		//std::cout << "push onRspSubMarketData " << pSpecificInstrument->InstrumentID << std::endl;
		//CThostFtdcSpecificInstrumentField * p = (CThostFtdcSpecificInstrumentField *) msg.data;
		//char * p = (char *) msg.data;
		//std::cout << "after, instrumentID: " << p << std::endl;
	}
	else{
		//this->logFile << "SubMarketDataError, ErrorID: " << pRspInfo->ErrorID
				//<< " ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}
	//std::cout << "SubMarketData: " << pSpecificInstrument->InstrumentID << " successed." << std::endl;
}

void CustomMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	Message msg;
	msg.msg_type = RTN_DEPTH_MARKETDATA;
	msg.data = pDepthMarketData;
	this->m_queMutex->lock();
	this->m_msgQueue->push(msg);
	this->m_queMutex->unlock();
	//std::cout << "push depth market data" << std::endl;
	/*
	std::cout << pDepthMarketData->ActionDay << ", "
	<< pDepthMarketData->InstrumentID << ", "
	<< pDepthMarketData->LastPrice << std::endl;
	*/
}
