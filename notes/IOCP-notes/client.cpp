#include <winsock2.h>  
#include <Windows.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")      // Socket编程需用的动态链接库  

#define DEFAULTPORT 6000
#define DEFAULTSIZE 1024 

int main(int argc, char* argv[])  
{  
	//加载socket动态链接库
	WORD wVersionRequested = MAKEWORD(2, 2); //请求2.2版本的WinSock库  
	WSADATA wsaData;    //接收Windows Socket的结构信息
	DWORD err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)//检查套接字库是否申请成功
	{  
		printf("Request Windows Socket Library Error!\n");  
		system("pause");
		return -1;
	}  
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)//检查是否申请了所需版本的套接字库
	{
		WSACleanup();  
		printf("Request Windows Socket Version 2.2 Error!\n");  
		system("pause");  
		return -1;  
	}

	// 创建socket操作，建立流式套接字，返回套接字号sockClient  
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);  
	if(sockClient == INVALID_SOCKET)
	{   
		printf("Error at socket():%ld\n", WSAGetLastError());   
		WSACleanup();   
		return -1;
	}   

	// 将套接字sockClient与远程主机相连  
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);  
	// 第一个参数：需要进行连接操作的套接字  
	// 第二个参数：设定所需要连接的地址信息  
	// 第三个参数：地址的长度  
	SOCKADDR_IN addrSrv;  
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");      // 本地回路地址是127.0.0.1;   
	addrSrv.sin_family = AF_INET;  
	addrSrv.sin_port = htons(DEFAULTPORT);  
	while(SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
	{  
		// 如果还没连接上服务器则要求重连  
		printf("服务器连接失败，是否重新连接？（Y/N):");  
		char choice;  
		while((choice = getchar()) && (!((choice != 'Y' && choice == 'N') || (choice == 'Y' && choice != 'N'))))
		{  
			printf("输入错误，请重新输入:");
		}  
		if (choice == 'Y'){  
			continue;  
		}  
		else
		{  
			printf("退出系统中...\n");  
			system("pause");  
			return 0;  
		}  
	}   
	printf("客户端已准备就绪，直接输入文字向服务器反馈信息。\n");  

	char buf[DEFAULTSIZE];
	int iRes = 0;
	while(true)
	{  
		gets(buf);
		if(buf[0] == 'q')
		{  
			break;  
		}  
		else
		{  
			printf("I Say:(\"quit\"to exit):%s\n",buf);  
			iRes = send(sockClient, buf, strlen(buf)+1, 0); // 发送信息
			memset(buf, 0, DEFAULTSIZE);
			iRes = recv(sockClient, buf, DEFAULTSIZE, 0);
			printf("Server Say:%s\n",buf);
		}  
	}  

	closesocket(sockClient);  
	WSACleanup();   // 终止对套接字库的使用  
	printf("End linking...\n");   
	system("pause");  
	return 0;
}