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

CustomMdSpi::CustomMdSpi(CThostFtdcMdApi *pMdApi, std::queue<Message> *msgQueue)
{
	this->m_pMdApi = pMdApi;
	//this->logFile = logFile;
	this->m_msgQueue = msgQueue;
	std::cout << "Market Data Api version: " << CThostFtdcMdApi::GetApiVersion() << std::endl;
}

CustomMdSpi::~CustomMdSpi(){}

void CustomMdSpi::OnFrontConnected(){
	Message msg;
	msg.msg_type = MARKET_FRONT_CONNECTED;
	this->m_msgQueue->push(msg);
	//std::cout << "MARKET FRONT CONNECTED" << std::endl;
}

void CustomMdSpi::OnFrontDisconnected(int nReason){
	std::cout << "Market front disconnected. Reason: " << nReason << std::endl;
	//this->logFile << "Market front disconnected. Reason: " << std::endl;
}

void CustomMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult){
		Message msg;
		msg.msg_type = MARKET_FRONT_LOGIN;
		msg.data = pRspUserLogin;
		this->m_msgQueue->push(msg);
	}
	/*else{
		this->logFile << "Market front loginError. ErrorID: " << pRspInfo->ErrorID
				<< " ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}*/
}

void CustomMdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
		if (!bResult){
			Message msg;
			msg.msg_type = MARKET_FRONT_LOGOUT;
			msg.data = pUserLogout;
			this->m_msgQueue->push(msg);
		}
		/*else{
			//this->logFile << "Market front logoutError. ErrorID: " << pRspInfo->ErrorID
					//<< " ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
		}*/
}

void CustomMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	std::cout << "OnRspError, ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	//this->logFile << "OnRspError, ErrorID: " << pRspInfo->ErrorID
			//<< " ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
}

void CustomMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult){
		//std::cout << "sub market data successed. InstrumentID: " << pSpecificInstrument->InstrumentID << std::endl;
		Message msg;
		msg.msg_type = RSP_SUB_MARKETDATA;
		//char * a = pSpecificInstrument->InstrumentID;
		//msg.data = a;
		msg.data = pSpecificInstrument;
		this->m_msgQueue->push(msg);
		//std::cout << "push onRspSubMarketData " << pSpecificInstrument->InstrumentID << std::endl;
		//CThostFtdcSpecificInstrumentField * p = (CThostFtdcSpecificInstrumentField *) msg.data;
		//char * p = (char *) msg.data;
		//std::cout << "after, instrumentID: " << p << std::endl;
	}
	/*else{
		//this->logFile << "SubMarketDataError, ErrorID: " << pRspInfo->ErrorID
				//<< " ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}*/
}

void CustomMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	Message msg;
	msg.msg_type = RTN_DEPTH_MARKETDATA;
	msg.data = pDepthMarketData;
	this->m_msgQueue->push(msg);
	//std::cout << "push depth market data" << std::endl;
}
