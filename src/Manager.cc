/*
 * Manager.cpp
 *
 *  Created on: Feb 10, 2020
 *      Author: pony
 */


#include "Manager.h"
#include "ThostFtdcUserApiStruct.h"
#include <cstring>
#include <iostream>
#include <iomanip>
//#include <unistd.h>

void Manager::setAccountInfo(std::string brokerID, std::string userID, std::string password, std::string appID, std::string authCode)
{
	this->m_brokerID = brokerID;
	this->m_userID = userID;
	this->m_password = password;
	this->m_appID = appID;
	this->m_authCode = authCode;
}

void Manager::setMarketFrontAddr(std::string marketFrontAddr)
{
	strcpy(this->m_marketFrontAddr, marketFrontAddr.c_str());
}

void Manager::setTraderFrontAddr(std::string traderFrontAddr)
{
	strcpy(this->m_traderFrontAddr, traderFrontAddr.c_str());
}

void Manager::setInstruments(char * *instrumentIDs, int nInstruments)
{
	this->m_instrumentIDs = instrumentIDs;
	this->m_nInstruments = nInstruments;
}

void Manager::init()
{
	this->m_ask[0] = 0.0;
	this->m_ask[1] = 0.0;
	this->m_bid[0] = 0.0;
	this->m_bid[1] = 0.0;
	this->m_bReady[0] = false;
	this->m_bReady[1] = false;
	//this->m_logFile.open("log.txt", std::ios::app);
	this->m_pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("../log/");
	this->m_pMdSpi = new CustomMdSpi(this->m_pMdApi, &(this->m_msgQueue));
	this->m_pMdApi->RegisterSpi(this->m_pMdSpi);
	this->m_pMdApi->RegisterFront(this->m_marketFrontAddr);
	this->m_nRequestID = 0;
	this->m_count = 0;
	this->m_pMdApi->Init();
	//this->m_pMdApi->Join();
	//this->m_pMdApi->Release();

	this->m_pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("../log/");
	this->m_pTraderSpi = new CustomTraderSpi(this->m_pTraderApi, &(this->m_msgQueue));
	this->m_pTraderApi->RegisterSpi(this->m_pTraderSpi);
	this->m_pTraderApi->RegisterFront(this->m_traderFrontAddr);
	this->m_pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);
	this->m_pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);
}

void Manager::run()
{
	while(true)
	{
		//sleep(1);
		//std::cout << "msgQueue size: " << this->m_msgQueue.size() << std::endl;
		while(!this->m_msgQueue.empty())
		{
			Message msg = this->m_msgQueue.front();
			//std::cout << msg.msg_type << std::endl;
			switch (msg.msg_type)
			{
			case MARKET_FRONT_CONNECTED:
			{
				//this->m_logFile << "Market front connection successed." << std::endl;
				std::cout << "Market front connection successed." << std::endl;
				CThostFtdcReqUserLoginField loginReq;
				memset(&loginReq, 0, sizeof(loginReq));
				strcpy(loginReq.BrokerID, this->m_brokerID.c_str());
				strcpy(loginReq.UserID, this->m_userID.c_str());
				strcpy(loginReq.Password, this->m_password.c_str());
				this->m_pMdApi->ReqUserLogin(&loginReq, this->m_nRequestID);
				this->m_nRequestID ++;
			}
			break;

			case MARKET_FRONT_LOGIN:
			{
				CThostFtdcRspUserLoginField *pRspUserLogin = (CThostFtdcRspUserLoginField *) msg.data;
				std::cout << "Market front login successed." << "SessionID: " << pRspUserLogin->SessionID
						<< " TradingDay: " << pRspUserLogin->TradingDay << std::endl;
				std::string tradeDate = pRspUserLogin->TradingDay;
				this->m_signalFile.open(("../log/" + tradeDate + ".csv").c_str(), std::ios::app);
				this->m_signalFile << "time,milliSec,bid_f,ask_f,bid_s,ask_s,short,long" << std::endl;
				//char *InstrumentIDs[] = {"IF2002", "IF2003"};
				//char *InstrumentIDs[] = {"IF2002", "IF2003"};
				//int nInstruments = 2;
				//this->m_pMdApi->SubscribeMarketData(InstrumentIDs, nInstruments);
				//this->m_pMdApi->SubscribeMarketData(this->m_instrumentIDs, this->m_nInstruments);
				this->m_pTraderApi->Init();

			}
			break;

			case MARKET_FRONT_LOGOUT:
			{
				CThostFtdcUserLogoutField * pUserLogout = (CThostFtdcUserLogoutField *) msg.data;
				std::cout << "Market front logout successed. BrokerID: " << pUserLogout->BrokerID
									<< " UserID: " << pUserLogout->UserID << std::endl;
			}
			break;

			case TRADER_FRONT_CONNECTED:
			{
				std::cout << "Trader front connection successed." << std::endl;
				CThostFtdcReqAuthenticateField authReq;
				memset(&authReq, 0, sizeof(authReq));
				strcpy(authReq.BrokerID, this->m_brokerID.c_str());
				strcpy(authReq.UserID, this->m_userID.c_str());
				strcpy(authReq.AppID, this->m_appID.c_str());
				strcpy(authReq.AuthCode, this->m_authCode.c_str());
				this->m_pTraderApi->ReqAuthenticate(&authReq, this->m_nRequestID);
				this->m_nRequestID++;
			}
			break;

			case RSP_AUTHENTICATE:
			{
				std::cout << "Authentication successed." << std::endl;
				CThostFtdcReqUserLoginField loginReq;
				memset(&loginReq, 0, sizeof(loginReq));
				strcpy(loginReq.BrokerID, this->m_brokerID.c_str());
				strcpy(loginReq.UserID, this->m_userID.c_str());
				strcpy(loginReq.Password, this->m_password.c_str());
				this->m_pTraderApi->ReqUserLogin(&loginReq, this->m_nRequestID);
				this->m_nRequestID ++;
			}
			break;

			case TRADER_FRONT_LOGIN:
			{
				CThostFtdcRspUserLoginField *pRspUserLogin = (CThostFtdcRspUserLoginField *) msg.data;
				std::cout << "Trader front login successed. " << std::endl;
				std::cout << "SessionID: " << pRspUserLogin->SessionID << " TradingDay: " << pRspUserLogin->TradingDay << std::endl;
				std::cout << "MaxOrderRef: " << pRspUserLogin->MaxOrderRef << std::endl;
				strcpy(this->m_orderRef, pRspUserLogin->MaxOrderRef);
				CThostFtdcSettlementInfoConfirmField settlementConfirmReq;
				memset(&settlementConfirmReq, 0, sizeof(settlementConfirmReq));
				strcpy(settlementConfirmReq.BrokerID, this->m_brokerID.c_str());
				strcpy(settlementConfirmReq.InvestorID, this->m_userID.c_str());
				this->m_pTraderApi->ReqSettlementInfoConfirm(&settlementConfirmReq, this->m_nRequestID);
				this->m_nRequestID++;
			}
			break;

			case TRADER_FRONT_LOGOUT:
			{
				CThostFtdcUserLogoutField * pUserLogout = (CThostFtdcUserLogoutField *) msg.data;
				std::cout << "Trader front logout successed. UserID: " << pUserLogout->UserID << std::endl;
			}
			break;

			case RSP_SUB_MARKETDATA:
			{
				//std::cout << msg.msg_type << std::endl;
				//CThostFtdcSpecificInstrumentField * p = (CThostFtdcSpecificInstrumentField *) msg.data;
				//char * p = (char *) msg.data;
				//std::cout << "after after, instrumentID: " << p << std::endl;
				CThostFtdcSpecificInstrumentField * pSpecificInstrument = (CThostFtdcSpecificInstrumentField *) msg.data;
				std::cout << "SubMarketData successed. InstrumentID: " << pSpecificInstrument->InstrumentID << std::endl;
			}
			break;

			case RSP_SETTLEMENT_INFO_CONFIRM:
			{
				CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm = (CThostFtdcSettlementInfoConfirmField *) msg.data;
				std::cout << "Settlement Info Confirmed. " << std::endl;
				std::cout << "SettlementID: " << pSettlementInfoConfirm->SettlementID << std::endl;
				CThostFtdcQryInstrumentField instrumentReq;
				memset(&instrumentReq, 0, sizeof(instrumentReq));
				strcpy(instrumentReq.InstrumentID, this->m_instrumentIDs[0]);
				this->m_pTraderApi->ReqQryInstrument(&instrumentReq, this->m_nRequestID);
				this->m_nRequestID++;
			}
			break;

			case RSP_QRY_INSTRUMENT:
			{
				CThostFtdcInstrumentField *pInstrument = (CThostFtdcInstrumentField *) msg.data;
				std::cout << "Query instrument successed." << std::endl;
				std::cout << "InstrumentID: " << pInstrument->InstrumentID << " ExpireDate: " << pInstrument->ExpireDate << std::endl;
				if (!strcmp(pInstrument->InstrumentID, this->m_instrumentIDs[0]))
				{
					//std :: cout << InstrumentIDs[0] << std::endl;
					CThostFtdcQryInstrumentField instrumentReq;
					memset(&instrumentReq, 0, sizeof(instrumentReq));
					strcpy(instrumentReq.InstrumentID, this->m_instrumentIDs[1]);
					this->m_pTraderApi->ReqQryInstrument(&instrumentReq, this->m_nRequestID);
					this->m_nRequestID++;
				}
				else
				{
					CThostFtdcQryTradingAccountField tradingAccountReq;
					memset(&tradingAccountReq, 0, sizeof(tradingAccountReq));
					strcpy(tradingAccountReq.BrokerID, this->m_brokerID.c_str());
					strcpy(tradingAccountReq.InvestorID, this->m_userID.c_str());
					this->m_pTraderApi->ReqQryTradingAccount(&tradingAccountReq, this->m_nRequestID);
					this->m_nRequestID++;
				}

			}
			break;

			case RSP_QRY_TRADING_ACCOUNT:
			{
				CThostFtdcTradingAccountField *pTradingAccount = (CThostFtdcTradingAccountField *) msg.data;
				std::cout << "Query trading account successed." << std::endl;
				std::cout << "AccountID: " << pTradingAccount->AccountID << std::endl;
				std::cout << "Available: " << pTradingAccount->Available << std::endl;
				CThostFtdcQryInvestorPositionField positionReq;
				memset(&positionReq, 0, sizeof(positionReq));
				strcpy(positionReq.BrokerID, this->m_brokerID.c_str());
				strcpy(positionReq.InvestorID, this->m_userID.c_str());
				strcpy(positionReq.InstrumentID, this->m_instrumentIDs[0]);
				this->m_pTraderApi->ReqQryInvestorPosition(&positionReq, this->m_nRequestID);
				this->m_nRequestID++;

			}
			break;

			case RSP_QRY_INVESTOR_POSITION:
			{
				CThostFtdcInvestorPositionField *pInvestorPosition = (CThostFtdcInvestorPositionField *) msg.data;
				std::cout << "Query Investor position successed." << std::endl;
				if (pInvestorPosition)
				{
					std::cout << "InstrumentID: " << pInvestorPosition->InstrumentID << std::endl;
					std::cout << "Direction: " << pInvestorPosition->PosiDirection << std::endl;
					std::cout << "Position: " << pInvestorPosition->Position << std::endl;
					std::cout << "count: " << this->m_count << std::endl;
					this->m_count++;
				}
				else
				{
					std::cout << "Does not have position." << std::endl;
				}

				if (m_count == 1)
				{
					this->m_pMdApi->SubscribeMarketData(this->m_instrumentIDs, this->m_nInstruments);
				}
			}
			break;

			case RTN_DEPTH_MARKETDATA:
			{
				CThostFtdcDepthMarketDataField * pDepthMarketData = (CThostFtdcDepthMarketDataField *) msg.data;
				if (strcmp(pDepthMarketData->InstrumentID, this->m_instrumentIDs[0])){
					this->m_ask[0] = pDepthMarketData->AskPrice1;
					this->m_bid[0] = pDepthMarketData->BidPrice1;
					this->m_bReady[0] = true;
				}
				else{
					if (strcmp(pDepthMarketData->InstrumentID, this->m_instrumentIDs[1])){
						this->m_ask[1] = pDepthMarketData->AskPrice1;
						this->m_bid[1] = pDepthMarketData->BidPrice1;
						this->m_bReady[1] = true;
					}
				}

				if (this->m_bReady[0] && this->m_bReady[1]){
					this->m_signalFile << pDepthMarketData->UpdateTime << ',' << pDepthMarketData->UpdateMillisec << ','
						<< this->m_bid[0] << ',' << this->m_ask[0] << ',' << this->m_bid[1] << ',' << this->m_ask[1] << ','
						<< this->m_bid[0] - this->m_ask[1] << ',' << this->m_ask[0] - this->m_bid[1] << std::endl;
					std::cout << pDepthMarketData->InstrumentID << " " << pDepthMarketData->UpdateTime << '.' << std::setw(3) << std::setfill('0') << pDepthMarketData->UpdateMillisec << ','
							<< this->m_bid[0] - this->m_ask[1] << " " <<  this->m_ask[0] - this->m_bid[1] << std::endl;
				}
			}
			break;

			default:
				break;
			}
			this->m_msgQueue.pop();
		}
	}
}

