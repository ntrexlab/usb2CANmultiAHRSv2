#include "stdafx.h"
#include "CAN_DLL/CAN_Access.h"
#include <windows.h>

#define ACC 0x33 
#define GYR 0x34 
#define ANG 0x35 

//console Timer ���� : https://simplesamples.info/Windows/ConsoleTimer

int _tmain(int argc, _TCHAR* argv[])
{
	float DEG_Z[5] = { 0, };
	bool ret[5] = { false, };

	// PC�� ����� USB2CAN(FIFO) �����͸� ��� �˻��Ѵ�.
	// ���⼭ ��� ���� USB2CAN(FIFO) �����ʹ� �˻����� ���ܵȴ�.
	int noDevice = CAN_Fifo_ScanSerialNumber();
	if (noDevice <= 0) return -1;

	// �˻��� �ø��� �ѹ��� ȭ�鿡 ǥ���Ѵ�.
	for (int i = 0; i < noDevice; i++) {
		const char* serialNumber = CAN_Fifo_GetSerialNumber(i);
		printf("%s\n", serialNumber);
	}

	// ù ��° �ø��� �ѹ��� USB2CAN(FIFO) �ڵ��� ���´�.
	CAN_HANDLE h = CAN_OpenFifo(CAN_Fifo_GetSerialNumber(0));

	if (h < 0) {
		printf("USB2CAN open failed.\n");
		return -1;
	}

	// �޽����� ���ŵǾ��� �� �̺�Ʈ�� �ޱ����� Event �ڵ��� �����Ѵ�.
	HANDLE hEvent = CreateEvent(NULL, false, false, "");

	// USB2CAN(FIFO) �ڵ鿡 �̺�Ʈ �ڵ��� ����Ѵ�.
	CAN_Fifo_SetRxEventNotification(h, hEvent);

	// USB2CAN �����͸� �����ϸ� ���۸��� 0(���۾���)���� �����Ǿ� �ִ�.
	// �̸� 1(Binary) �Ǵ� 2(Text) ���� �����Ѵ�.
	CAN_SetTransferMode(h, 2);

	Sleep(1000);

	/*
	for (int i = 0; i < 1000; i++) {
		long sid = 0;
		unsigned char sdata[8] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11 };

		// CAN ��Ʈ��ũ�� �����͸� �����Ѵ�.
		int ret1 = CAN_Send(h, sid, 8, (char*)sdata, 0, 0);

		if (!ret1) {
			// ���� ���� 0�̸� ������ ������ ����.
			// ���⼭ ������ ����, USB2CAN �����Ͱ� CAN ��Ʈ��ũ�� ����Ǿ� ���� �ʰų�
			// CAN ��Ŷ�� �������� �������� �ʾƵ� ����(1)�� �����Ѵٴ� ���̴�.
			// ���� ���а� ���ϵǴ� ���� �߸��� �ڵ鿡 ���� ������ �õ��ϴ� ����̴�.
			printf("Send failed.\n");
		}
	}
	*/

	while (1)
	{
		char rdata[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		long rid;
		int rlen, ext, rtr;

		// �����͸� �����Ѵ�.
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

