/*
 * main.cpp
 *
 *  Created on: Feb 5, 2020
 *      Author: pony
 */

#include "Manager.h"
#include <fstream>
#include <cstring>

int main(){

	// Trade Front：180.168.146.187:10100，Market Front：180.168.146.187:10110
	// Trade Front：180.168.146.187:10101，Market Front：180.168.146.187:10111
	// Trade Front：218.202.237.33:10102，Market Front：218.202.237.33:10112
	// 交易前置：180.168.146.187:10130 行情前置：180.168.146.187:10131 [7*24]
	std::string TraderFrontAddr = "tcp://180.168.146.187:10130";
	std::string MdFrontAddr = "tcp://180.168.146.187:10131";

	const char *InstrumentIDs[2] = {"IF2010", "IF2011"};
	int nInstruments = 2;

	Manager manager;
	manager.setAccountInfo("9999", "102757", "1qaz@WSX", "simnow_client_test", "0000000000000000");
	manager.setMarketFrontAddr(MdFrontAddr);
	manager.setTraderFrontAddr(TraderFrontAddr);
	manager.setInstruments((char **)InstrumentIDs, nInstruments);
	manager.init();
	manager.run();
}
