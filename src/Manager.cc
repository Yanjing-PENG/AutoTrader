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
#include <thread>
#include <chrono>
//#include <unistd.h>

/*
Manager::Manager()
{
	this->m_signalFile.open("../data/signal.csv", std::ios::out|std::ios::app);
}
*/
/*
Manager::~Manager()
{
	this->m_signalFile.close();
	this->m_quoteFile.close();
}
*/

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
	this->m_pMdSpi = new CustomMdSpi(this->m_pMdApi, &(this->m_msgQueue), &(this->m_queMutex));
	this->m_pMdApi->RegisterSpi(this->m_pMdSpi);
	this->m_pMdApi->RegisterFront(this->m_marketFrontAddr);
	this->m_nRequestID = 10;
	this->m_count = 0;
	//this->m_pMdApi->Init();
	//this->m_pMdApi->Join();
	//this->m_pMdApi->Release();

	this->m_pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("../log/");
	this->m_pTraderSpi = new CustomTraderSpi(this->m_pTraderApi, &(this->m_msgQueue), &(this->m_queMutex));
	this->m_pTraderApi->RegisterSpi(this->m_pTraderSpi);
	this->m_pTraderApi->RegisterFront(this->m_traderFrontAddr);
	this->m_pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);
	this->m_pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);
	this->m_pTraderApi->Init();
}

void Manager::run()
{
	while(true)
	{
		//sleep(1);
		//std::cout << "msgQueue size: " << this->m_msgQueue.size() << std::endl;
		Message msg;
		this->m_queMutex.lock();
		if (this->m_msgQueue.empty()) 
		{
			this->m_queMutex.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		} else {
			msg = this->m_msgQueue.front();
			this->m_msgQueue.pop();
			this->m_queMutex.unlock();

			switch (msg.msg_type)
			{
			case RSP_SETTLEMENT_INFO_CONFIRM:
			{
				CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm = (CThostFtdcSettlementInfoConfirmField *) msg.data;
				std::cout << std::endl;
				std::cout << "Settlement Info Confirmed. " << std::endl;
				std::cout << "SettlementID: " << pSettlementInfoConfirm->SettlementID << std::endl;
				std::cout << std::endl;
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
				std::cout << std::endl;
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
				std::cout << "Available: " << std::to_string(pTradingAccount->Available) << std::endl;
				std::cout << std::endl;
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
					std::cout << std::endl;
					this->m_count++;
				}
				else
				{
					std::cout << "Does not have position." << std::endl;
					std::cout << std::endl;
					this->m_pMdApi->Init();

				}

				if (m_count == 1)
				{
					//this->m_pMdApi->SubscribeMarketData(this->m_instrumentIDs, this->m_nInstruments);
					this->m_pMdApi->Init();
				}
			}
			break;

			case MARKET_FRONT_LOGIN:
			{
				std::cout << "Market front UserLogin successed." << std::endl;
				std::cout << std::endl;
				CThostFtdcRspUserLoginField *pRspUserLogin = (CThostFtdcRspUserLoginField *) msg.data;
				char signal_file_path[128];
				sprintf(signal_file_path, "../data/signal_%s.csv", pRspUserLogin->TradingDay);
				strcpy(this->m_trading_day, pRspUserLogin->TradingDay);
				this->m_signalFile.open(signal_file_path, std::ios::out|std::ios::app);
				this->m_pMdApi->SubscribeMarketData(this->m_instrumentIDs, this->m_nInstruments);
			}
			break;

			case RSP_SUB_MARKETDATA:
			{
				CThostFtdcSpecificInstrumentField * pSpecificInstrument = (CThostFtdcSpecificInstrumentField *) msg.data;
				std::cout << "SubMarketData: " << pSpecificInstrument->InstrumentID << " successed." << std::endl;
				if (! strcmp(pSpecificInstrument->InstrumentID, this->m_instrumentIDs[1]))
				{
					char quote_file_path[128];
					sprintf(quote_file_path, "../data/quote_%s.csv", this->m_trading_day);
					this->m_quoteFile.open(quote_file_path, std::ios::out|std::ios::app);
				}
				delete pSpecificInstrument;
			}
			break;

			case RTN_DEPTH_MARKETDATA:
			{
				
				CThostFtdcDepthMarketDataField * pDepthMarketData = (CThostFtdcDepthMarketDataField *) msg.data;
				/*
				std::cout << pDepthMarketData->InstrumentID << "," << pDepthMarketData->UpdateTime << '.' << std::setw(3) << std::setfill('0') << pDepthMarketData->UpdateMillisec << ','
						<< pDepthMarketData->LastPrice << std::endl;
				*/
				if (!strcmp(pDepthMarketData->InstrumentID, this->m_instrumentIDs[0]))
				{
					this->m_ask[0] = pDepthMarketData->AskPrice1;
					this->m_bid[0] = pDepthMarketData->BidPrice1;
					this->m_bReady[0] = true;
				}
				else
				{
					this->m_ask[1] = pDepthMarketData->AskPrice1;
					this->m_bid[1] = pDepthMarketData->BidPrice1;
					this->m_bReady[1] = true;
				}

				if (this->m_bReady[0] && this->m_bReady[1] && !strcmp(pDepthMarketData->InstrumentID, this->m_instrumentIDs[0]))
				{
					
					this->m_signalFile << pDepthMarketData->UpdateTime << ',' << pDepthMarketData->UpdateMillisec << ','
						<< this->m_bid[0] << ',' << this->m_ask[0] << ',' << this->m_bid[1] << ',' << this->m_ask[1] << ','
						<< this->m_bid[0] - this->m_ask[1] << ',' << this->m_ask[0] - this->m_bid[1] << std::endl;
					/*
					std::cout << pDepthMarketData->InstrumentID << "," << pDepthMarketData->UpdateTime << '.' << std::setw(3) << std::setfill('0') << pDepthMarketData->UpdateMillisec << ','
							<< this->m_bid[0] - this->m_ask[1] << "," <<  this->m_ask[0] - this->m_bid[1] << std::endl;
					*/
				}
				this->m_quoteFile << pDepthMarketData->InstrumentID << ',' << pDepthMarketData->UpdateTime << ',' << pDepthMarketData->UpdateMillisec 
				<< ',' << pDepthMarketData->LastPrice << ',' << pDepthMarketData->Volume << ',' << std::to_string(pDepthMarketData->Turnover) << ','
				<< pDepthMarketData->BidPrice1 << ',' << pDepthMarketData->BidVolume1 << ',' << pDepthMarketData->AskPrice1 << ',' << pDepthMarketData->AskVolume1 << std::endl;
			}
			break;

			default:
				break;
			}
		}
	}
}

