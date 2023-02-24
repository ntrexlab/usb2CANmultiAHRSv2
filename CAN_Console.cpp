#include "stdafx.h"
#include "CAN_DLL/CAN_Access.h"
#include <windows.h>

#define ACC 0x33 
#define GYR 0x34 
#define ANG 0x35 

//console Timer 참조 : https://simplesamples.info/Windows/ConsoleTimer

int _tmain(int argc, _TCHAR* argv[])
{
	float DEG_Z[5] = { 0, };
	bool ret[5] = { false, };

	// PC에 연결된 USB2CAN(FIFO) 컨버터를 모두 검색한다.
	// 여기서 사용 중인 USB2CAN(FIFO) 컨버터는 검색에서 제외된다.
	int noDevice = CAN_Fifo_ScanSerialNumber();
	if (noDevice <= 0) return -1;

	// 검색된 시리얼 넘버를 화면에 표시한다.
	for (int i = 0; i < noDevice; i++) {
		const char* serialNumber = CAN_Fifo_GetSerialNumber(i);
		printf("%s\n", serialNumber);
	}

	// 첫 번째 시리얼 넘버로 USB2CAN(FIFO) 핸들을 얻어온다.
	CAN_HANDLE h = CAN_OpenFifo(CAN_Fifo_GetSerialNumber(0));

	if (h < 0) {
		printf("USB2CAN open failed.\n");
		return -1;
	}

	// 메시지가 수신되었을 때 이벤트를 받기위한 Event 핸들을 생성한다.
	HANDLE hEvent = CreateEvent(NULL, false, false, "");

	// USB2CAN(FIFO) 핸들에 이벤트 핸들을 등록한다.
	CAN_Fifo_SetRxEventNotification(h, hEvent);

	// USB2CAN 컨버터를 오픈하면 전송모드는 0(전송안함)으로 설정되어 있다.
	// 이를 1(Binary) 또는 2(Text) 모드로 설정한다.
	CAN_SetTransferMode(h, 2);

	Sleep(1000);

	/*
	for (int i = 0; i < 1000; i++) {
		long sid = 0;
		unsigned char sdata[8] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11 };

		// CAN 네트워크로 데이터를 전송한다.
		int ret1 = CAN_Send(h, sid, 8, (char*)sdata, 0, 0);

		if (!ret1) {
			// 리턴 값이 0이면 전송을 실패한 경우다.
			// 여기서 주의할 것은, USB2CAN 컨버터가 CAN 네트워크에 연결되어 있지 않거나
			// CAN 패킷이 목적지에 도달하지 않아도 성공(1)을 리턴한다는 것이다.
			// 전송 실패가 리턴되는 경우는 잘못된 핸들에 대해 전송을 시도하는 경우이다.
			printf("Send failed.\n");
		}
	}
	*/

	while (1)
	{
		char rdata[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		long rid;
		int rlen, ext, rtr;

		// 데이터를 수신한다.
		int ret2 = CAN_Recv(h, &rid, &rlen, rdata, &ext, &rtr);

		if (ret2)
		{
			switch (rdata[1])
			{
				case ANG:
					DEG_Z[rid - 1] = (float)(((int)(unsigned char)rdata[6] | (int)(unsigned char)rdata[7] << 8)) / 100.0;;
					ret[rid - 1] = true;
				break;
			}
		}

		if(ret[0] && ret[1] && ret[2]  && ret[3]  && ret[4])
		{
			for (int i = 0; i < 5; i++)
			{
				printf("%.2f ", DEG_Z[i]);
			}
			printf("\n");

			for (int i = 0; i < 5; i++)
			{
				ret[i] = false;
			}
		}
	}

	CAN_Close(h);

	CloseHandle(hEvent);

	getchar();

	return 0;
}

