#include<iostream>
#include<windows.h>
#include<winsock.h>   // windowsƽ̨�������ͷ�ļ�
#pragma comment(lib,"ws2_32.lib")   // ���ļ�
using namespace std;

#define IP "10.3.71.52"
#define PORT 9527

#define BUFSIZ 512

bool b_runing = true;

DWORD WINAPI thread(LPVOID p_socket)
{
	SOCKET  client = *(SOCKET *)p_socket;
	char *recv_buf = new char[BUFSIZ];
	printf("pid = %d Enter\n", GetCurrentThreadId());
	send(client, "begin:", 7, 0);
	while (b_runing)
	{
		memset(recv_buf, 0, BUFSIZ);
		if (recv(client, recv_buf, BUFSIZ, 0) > 0)
		{
			cout << "�ͻ�����Ϣ:" << recv_buf << endl;
			std::string st = "lgz:";
			st += recv_buf;
			if (send(client, st.c_str(), st.length() + 1, 0) < 0)
			{
				cout << "�����쳣��" << endl;
			}
		}
		else 
		{
			cout << "�����쳣��" << endl;
			break;
		}
	}
	delete[] recv_buf;
	printf("pid = %d Exit\n", GetCurrentThreadId());
	return 0;
}


void initialization() {
	//��ʼ���׽��ֿ�
	// WSA  windows socket async  windows�첽�׽���     WSAStartup�����׽���
	// parm1:�����socket�汾 2.2 2.1 1.0     parm2:��������    ������ʽ��WORD  WSADATA
	WORD w_req = MAKEWORD(2, 2);//�汾��  
	WSADATA wsadata;
	// �ɹ���WSAStartup����������
	if (WSAStartup(w_req, &wsadata) != 0) {
		cout << "��ʼ���׽��ֿ�ʧ�ܣ�" << endl;
	}
	else {
		cout << "��ʼ���׽��ֿ�ɹ���" << endl;
	}
}

SOCKET createServeSocket(const char* ip)
{
	//1.�����յ�Socket					
		//parm1:af ��ַЭ���� ipv4 ipv6
		//parm2:type ����Э������ ��ʽ�׽���(SOCK_STREAM) ���ݱ�
		//parm3��protocl ʹ�þ����ĳ������Э��
	// AF_INET6;
	SOCKET s_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s_server == INVALID_SOCKET)
	{
		cout << "�׽��ִ���ʧ�ܣ�" << endl;
		WSACleanup();
	}
	else {
		cout << "�׽��ִ����ɹ���" << endl;
	}
	//2.��socket��ip��ַ�Ͷ˿ں�
	struct sockaddr_in server_addr;   // sockaddr_in, sockaddr  �ϰ汾���°������
	server_addr.sin_family = AF_INET;  // �ʹ���socketʱ����һ��
	server_addr.sin_port = htons(PORT);       // �˿ں�  ��ˣ���λ���洢(����)��С�ˣ���λ���洢(���磩�������洢˳���Ƿ��ŵ�  htons �������ֽ���תΪ�����ֽ���
	server_addr.sin_addr.S_un.S_addr = inet_addr(ip); //inet_addr�����ʮ���Ƶ�ip��ַתΪ������
	if (bind(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "�׽��ְ�ʧ�ܣ�" << endl;
		WSACleanup();
	}
	else {
		cout << "�׽��ְ󶨳ɹ���" << endl;
	}

	//3.�����׽���Ϊ����״̬  SOMAXCONN �����Ķ˿��� �Ҽ�ת������Ϊ5
	if (listen(s_server, SOMAXCONN) < 0) {
		cout << "���ü���״̬ʧ�ܣ�" << endl;
		WSACleanup();
	}
	else {
		cout << "���ü���״̬�ɹ���" << endl;
	}
	return s_server;
}
#include <map>
int main() {
	//���巢�ͻ������ͽ��ܻ���������
	char send_buf[BUFSIZ];
	char recv_buf[BUFSIZ];
	//���������׽��֣����������׽���
	SOCKET s_server;
	SOCKET s_accept;

	initialization(); // ��ʼ�������׽���
	s_server = createServeSocket("10.3.71.52");
	cout << "10.3.71.52:9527wait client connect..." << endl;

	std::map<SOCKET, HANDLE> s_h;
	
	// ����пͻ�����������
	while (b_runing)
	{
		s_accept = accept(s_server, NULL, NULL);
		if (s_accept == INVALID_SOCKET)
		{
			cout << "����ʧ�ܣ�" << endl;
			//WSACleanup();
			//return 0;
		}
		else
		{
			SOCKET clinet = s_accept;
			s_h[clinet] = 0;
			std::map<SOCKET, HANDLE>::iterator it = s_h.find(clinet);
			HANDLE id = CreateThread(NULL, 0, thread, (void *)(&(it->first)), 0, 0);
			s_h[clinet] = id;
			 
		}
	}
	
	// ���ԺͿͻ��˽���ͨ����
	//while (true) {
	//	// recv��ָ����socket������Ϣ
	//	memset(recv_buf, 0, BUFSIZ);
	//	if (recv(s_accept, recv_buf, BUFSIZ, 0) > 0) {
	//		cout << "�ͻ�����Ϣ:" << recv_buf << endl;
	//	}
	//	else {
	//		cout << "����ʧ�ܣ�" << endl;
	//		//break;
	//	}
	//	cout << "������ظ���Ϣ:";
	//	//cin >> send_buf;
	//	std::string st = recv_buf;
	//	st += ":sb";
	//	if (send(s_accept, st.c_str(), st.length() + 1, 0) < 0) {
	//		cout << "����ʧ�ܣ�" << endl;
	//		//break;
	//	}
	//}
	//�ر��׽���
	closesocket(s_server);
	closesocket(s_accept);
	//�ͷ�DLL��Դ
	WSACleanup();
	return 0;
}
