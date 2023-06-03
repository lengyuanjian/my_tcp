#include<iostream>
#include<windows.h>
#include<winsock.h>   // windows平台的网络库头文件
#pragma comment(lib,"ws2_32.lib")   // 库文件
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
			cout << "客户端信息:" << recv_buf << endl;
			std::string st = "lgz:";
			st += recv_buf;
			if (send(client, st.c_str(), st.length() + 1, 0) < 0)
			{
				cout << "连接异常！" << endl;
			}
		}
		else 
		{
			cout << "连接异常！" << endl;
			break;
		}
	}
	delete[] recv_buf;
	printf("pid = %d Exit\n", GetCurrentThreadId());
	return 0;
}


void initialization() {
	//初始化套接字库
	// WSA  windows socket async  windows异步套接字     WSAStartup启动套接字
	// parm1:请求的socket版本 2.2 2.1 1.0     parm2:传出参数    参数形式：WORD  WSADATA
	WORD w_req = MAKEWORD(2, 2);//版本号  
	WSADATA wsadata;
	// 成功：WSAStartup函数返回零
	if (WSAStartup(w_req, &wsadata) != 0) {
		cout << "初始化套接字库失败！" << endl;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}
}

SOCKET createServeSocket(const char* ip)
{
	//1.创建空的Socket					
		//parm1:af 地址协议族 ipv4 ipv6
		//parm2:type 传输协议类型 流式套接字(SOCK_STREAM) 数据报
		//parm3：protocl 使用具体的某个传输协议
	// AF_INET6;
	SOCKET s_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s_server == INVALID_SOCKET)
	{
		cout << "套接字创建失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字创建成功！" << endl;
	}
	//2.给socket绑定ip地址和端口号
	struct sockaddr_in server_addr;   // sockaddr_in, sockaddr  老版本和新版的区别
	server_addr.sin_family = AF_INET;  // 和创建socket时必须一样
	server_addr.sin_port = htons(PORT);       // 端口号  大端（高位）存储(本地)和小端（低位）存储(网络），两个存储顺序是反着的  htons 将本地字节序转为网络字节序
	server_addr.sin_addr.S_un.S_addr = inet_addr(ip); //inet_addr将点分十进制的ip地址转为二进制
	if (bind(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "套接字绑定失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字绑定成功！" << endl;
	}

	//3.设置套接字为监听状态  SOMAXCONN 监听的端口数 右键转到定义为5
	if (listen(s_server, SOMAXCONN) < 0) {
		cout << "设置监听状态失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "设置监听状态成功！" << endl;
	}
	return s_server;
}
#include <map>
int main() {
	//定义发送缓冲区和接受缓冲区长度
	char send_buf[BUFSIZ];
	char recv_buf[BUFSIZ];
	//定义服务端套接字，接受请求套接字
	SOCKET s_server;
	SOCKET s_accept;

	initialization(); // 初始化启动套接字
	s_server = createServeSocket("10.3.71.52");
	cout << "10.3.71.52:9527wait client connect..." << endl;

	std::map<SOCKET, HANDLE> s_h;
	
	// 如果有客户端请求连接
	while (b_runing)
	{
		s_accept = accept(s_server, NULL, NULL);
		if (s_accept == INVALID_SOCKET)
		{
			cout << "连接失败！" << endl;
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
	
	// 可以和客户端进行通信了
	//while (true) {
	//	// recv从指定的socket接受消息
	//	memset(recv_buf, 0, BUFSIZ);
	//	if (recv(s_accept, recv_buf, BUFSIZ, 0) > 0) {
	//		cout << "客户端信息:" << recv_buf << endl;
	//	}
	//	else {
	//		cout << "接受失败！" << endl;
	//		//break;
	//	}
	//	cout << "请输入回复信息:";
	//	//cin >> send_buf;
	//	std::string st = recv_buf;
	//	st += ":sb";
	//	if (send(s_accept, st.c_str(), st.length() + 1, 0) < 0) {
	//		cout << "发送失败！" << endl;
	//		//break;
	//	}
	//}
	//关闭套接字
	closesocket(s_server);
	closesocket(s_accept);
	//释放DLL资源
	WSACleanup();
	return 0;
}
