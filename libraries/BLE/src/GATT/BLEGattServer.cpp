/*
  Copyright (c) 2016 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
*/

#include "BLEGattServer.h"

BLEGattServer::BLEGattServer(){
	//
}
	
void BLEGattServer::addService(BLEService &service){
	serviceList.add(&service);
}

void BLEGattServer::forwardGattsEventWriteToServices(ble_gatts_evt_write_t *ble_gatts_evt_write){
    for(BLEService *service = serviceList.getFirstElement(); service != 0; service = service->getNextElement())
        service->onGattsEventWrite(ble_gatts_evt_write);
}

void BLEGattServer::setConHandleInServices(uint16_t conHandle){
    for(BLEService *service = serviceList.getFirstElement(); service != 0; service = service->getNextElement())
        service->setConHandle(conHandle);
}