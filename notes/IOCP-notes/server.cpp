#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")//Socket编程需用的动态链接库

#define DEFAULTPORT 6000
#define DEFAULTSIZE 1024

/** 
 * 结构体名称：PER_IO_DATA 
 * 结构体功能：重叠I/O需要用到的结构体，临时记录IO数据
 **/  
typedef struct  
{  
    OVERLAPPED overlapped;
    char buffer[DEFAULTSIZE];  
    int BufferLen;  
    bool readflag;  
}PER_IO_DATA, *LPPER_IO_DATA;  
  
/** 
 * 结构体名称：PER_HANDLE_DATA 
 * 结构体存储：记录单个套接字的数据，包括了套接字的变量及套接字的对应的客户端的地址。 
 * 结构体作用：当服务器连接上客户端时，信息存储到该结构体中，知道客户端的地址以便于回访。 
 **/  
typedef struct  
{  
    SOCKET socket;  
    SOCKADDR_IN ClientAddr;  
}PER_SOCKET_DATA, *LPPER_SOCKET_DATA; 

DWORD WINAPI ServerWorkThread(LPVOID CompletionPortID)
{
	HANDLE CompletionPort = (HANDLE)CompletionPortID;  
	DWORD BytesTransferred; 
	LPPER_SOCKET_DATA PerSocketData = NULL;  
	LPPER_IO_DATA PerIoData = NULL;  
	DWORD RecvBytes;  
	DWORD Flags = 0;  
	BOOL bRet = false;
	while(true)
	{   
		bRet = GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&PerSocketData, (LPOVERLAPPED*)&PerIoData, INFINITE);  
		if(bRet == FALSE)
		{  
			printf("GetQueuedCompletionStatus Error:%d\n", GetLastError());
			if(PerSocketData != NULL)
			{
				if(PerSocketData->socket != NULL)
					closesocket(PerSocketData->socket);
				free(PerSocketData);
			}
			if(PerIoData != NULL)
				free(PerIoData);
			continue;
		}  
		//PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(IpOverlapped, PER_IO_DATA, overlapped);  

		//http://blog.csdn.net/mylovepan/article/details/8204126
		//http://bbs.csdn.net/topics/390719212
		//1.关闭窗口或者停止调试windows会释放句柄的，socket会被关闭并向对方发送断开连接
		//2.对方关闭socket后，WSARecv操作的GetQueuedCompletionStatus返回值是真，不过收到的字节数BytesTransferred是0，这里判断。
		//GetQueuedCompletionStatus返回TRUE并且读取到的数据长度为0时,关闭套接字
		if(BytesTransferred == 0)
		{  
			printf("BytesTransferred==0:%d\n", GetLastError());
			if(PerSocketData != NULL)
			{
				if(PerSocketData->socket != NULL)
					closesocket(PerSocketData->socket);
				free(PerSocketData);
			}
			if(PerIoData != NULL)
				free(PerIoData);
			continue;
		}  

		int iRes = 0;
		Flags = 0;
		if(PerIoData->readflag == true)
		{
		// 开始数据处理，接收来自客户端的数据
		printf("%s says: %s\n", inet_ntoa(PerSocketData->ClientAddr.sin_addr), PerIoData->buffer);
		//send(PerSocketData->socket, PerIoData->buffer, strlen(PerIoData->buffer)+1, 0);
		memset(&PerIoData->overlapped, 0, sizeof(PerIoData->overlapped));
		PerIoData->readflag = false;
		WSABUF wsabuf;
		wsabuf.buf = PerIoData->buffer;
		wsabuf.len = sizeof(PerIoData->buffer);
		iRes = WSASend(PerSocketData->socket, &wsabuf, 1, &RecvBytes, Flags, &PerIoData->overlapped, NULL);
		}
		else
		{
		//为下一个重叠调用建立单I/O操作数据
		memset(PerIoData, 0, sizeof(PER_IO_DATA)); // 清空内存
		PerIoData->readflag = true;
		WSABUF wsabuf;
		wsabuf.buf = PerIoData->buffer;
		wsabuf.len = sizeof(PerIoData->buffer);
		iRes = WSARecv(PerSocketData->socket, &wsabuf, 1, &RecvBytes, &Flags, &PerIoData->overlapped, NULL);
		}
	}

	return 0;
}

DWORD WINAPI ServerSendThread(LPVOID IpParam);  

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

	//创建IOCP的内核对象  
    /** 
     * 需要用到的函数的原型： 
     * HANDLE WINAPI CreateIoCompletionPort( 
     *    __in   HANDLE FileHandle,     // 已经打开的文件句柄或者空句柄，一般是客户端的句柄 
     *    __in   HANDLE ExistingCompletionPort, // 已经存在的IOCP句柄 
     *    __in   ULONG_PTR CompletionKey,   // 完成键，包含了指定I/O完成包的指定文件
     *    __in   DWORD NumberOfConcurrentThreads // 真正并发同时执行最大线程数，一般推介是CPU核心数*2 
     * ); 
     **/  
    HANDLE completionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);  
    if (completionPort == NULL)//创建IO内核对象失败  
	{    
        printf("CreateIoCompletionPort failed. Error:%d\n", GetLastError());  
        system("pause");  
        return -1;  
    }

	// 创建IOCP线程--线程里面创建线程池  
	// 确定处理器的核心数量  
	SYSTEM_INFO mySysInfo;  
	GetSystemInfo(&mySysInfo);  
	// 基于处理器的核心数量创建线程  
	for(DWORD i = 0; i < (mySysInfo.dwNumberOfProcessors * 2); ++i)
	{  
		//创建服务器工作器线程，并将完成端口传递到该线程  
		HANDLE ThreadHandle = CreateThread(NULL, 0, ServerWorkThread, completionPort, 0, NULL);  
		if(ThreadHandle == NULL){  
			printf("Create Thread Handle failed. Error:%d\n", GetLastError());  
			system("pause");  
			return -1;  
		}  
		CloseHandle(ThreadHandle);  
	}

	// 建立流式套接字  
	SOCKET srvSocket = socket(AF_INET, SOCK_STREAM, 0);  
	// 绑定SOCKET到本机  
	SOCKADDR_IN srvAddr;  
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);  
	srvAddr.sin_family = AF_INET;  
	srvAddr.sin_port = htons(DEFAULTPORT);
	int bindResult = bind(srvSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR));  
	if(bindResult == SOCKET_ERROR)
	{  
		printf("Bind failed. Error:", GetLastError());  
		system("pause");  
		return -1;  
	}  
	// 将SOCKET设置为监听模式  
	int listenResult = listen(srvSocket, 10);  
	if(listenResult == SOCKET_ERROR)
	{  
		printf("Listen failed. Error:%d\n", GetLastError());  
		system("pause");  
		return -1;  
	}

	//开始处理IO数据  
	printf("服务器已准备就绪，正在等待客户端的接入...\n");
	while(true)
	{
		SOCKET acceptSocket;  
		//接收连接，并分配完成端，这儿可以用AcceptEx()  
		SOCKADDR_IN saRemote;  
		int RemoteLen = sizeof(saRemote);  
		acceptSocket = accept(srvSocket, (SOCKADDR*)&saRemote, &RemoteLen);  
		if(acceptSocket == SOCKET_ERROR)// 接收客户端失败  
		{   
			printf("Accept Socket Error:%d\n", GetLastError());  
			system("pause");  
			return -1;  
		}
		//创建用来和套接字关联的单句柄数据信息结构  
		LPPER_SOCKET_DATA PerSocketData = (LPPER_SOCKET_DATA)malloc(sizeof(PER_SOCKET_DATA));  //在堆中为这个PerHandleData申请指定大小的内存  
		PerSocketData->socket = acceptSocket;
		memcpy(&PerSocketData->ClientAddr, &saRemote, RemoteLen);
		// 将接受套接字和完成端口关联  
		CreateIoCompletionPort((HANDLE)(PerSocketData->socket), completionPort, (DWORD)PerSocketData, 0); 

		// 开始在接受套接字上处理I/O使用重叠I/O机制  
		// 在新建的套接字上投递一个或多个异步  
		// WSARecv或WSASend请求，这些I/O请求完成后，工作者线程会为I/O请求提供服务
		// 单I/O操作数据(I/O重叠)
		LPPER_IO_DATA PerIoData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));  
		memset(PerIoData, 0, sizeof(PER_IO_DATA));  
		PerIoData->readflag = true;    // read  

		DWORD RecvBytes;  
		DWORD Flags = 0;
		WSABUF wsabuf;
		wsabuf.buf = PerIoData->buffer;
		wsabuf.len = sizeof(PerIoData->buffer);
		WSARecv(PerSocketData->socket, &wsabuf, 1, &RecvBytes, &Flags, &PerIoData->overlapped, NULL);
	}

	system("pause");
	return 0;
}