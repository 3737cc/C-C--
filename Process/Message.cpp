#include "Message.h"
#include <iostream>
#include <vector>
#include <string>
#include <boost/interprocess/ipc/message_queue.hpp>

MessageQueue::MessageQueue(const std::string& queueName)
	: m_szQueueName(".\\" + queueName), m_queueHandle(NULL) {
	m_szFormatName = getFormatName(m_szQueueName);
}

MessageQueue::~MessageQueue() {
	close();
}

bool MessageQueue::createQueue() {
	// 创建一个 MQQUEUEPROPS 结构体来存储队列属性
	MQQUEUEPROPS queueProps;
	QUEUEPROPID aQueuePropId[2] = { PROPID_Q_PATHNAME, PROPID_Q_JOURNAL };
	MQPROPVARIANT aQueuePropVar[2];

	// 设置队列路径
	std::wstring wQueueName(m_szQueueName.begin(), m_szQueueName.end());
	aQueuePropVar[0].vt = VT_LPWSTR;
	aQueuePropVar[0].pwszVal = const_cast<LPWSTR>(wQueueName.c_str());

	// 设置日志选项（可选）
	aQueuePropVar[1].vt = VT_UI1;
	aQueuePropVar[1].bVal = MQ_JOURNAL_NONE;

	// 设置 MQQUEUEPROPS 结构体的内容
	queueProps.aPropID = aQueuePropId;
	queueProps.aPropVar = aQueuePropVar;
	queueProps.cProp = 2;  // 现在有两个属性

	// 初始化格式名称缓冲区
	WCHAR formatNameBuffer[MAX_PATH];
	DWORD formatNameLength = MAX_PATH;

	// 创建安全描述符（可选）
	PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;

	SC_HANDLE schService = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schService == NULL) {
		std::cerr << "Failed to open Service Control Manager" << std::endl;
		return false;
	}
	SC_HANDLE msmqService = OpenService(schService, L"MSMQ", SERVICE_QUERY_STATUS);
	if (msmqService == NULL) {
		std::cerr << "Failed to open MSMQ service" << std::endl;
		CloseServiceHandle(schService);
		return false;
	}
	// 调用 MQCreateQueue 创建队列
	HRESULT hr = MQCreateQueue(
		pSecurityDescriptor,
		&queueProps,
		formatNameBuffer,
		&formatNameLength
	);

	if (FAILED(hr)) {
		if (hr == MQ_ERROR_QUEUE_EXISTS) {
			std::cout << "Queue already exists. Trying to open it..." << std::endl;
			// 如果队列已存在，尝试打开它
			return true;
		}
		else {
			std::cerr << "Failed to create queue. Error code: 0x"
				<< std::hex << hr << std::dec << std::endl;
			// 打印更详细的错误信息
			printErrorDetails(hr);
			return false;
		}
	}

	// 保存格式名称
	std::wstring wFormatName(formatNameBuffer);
	m_szFormatName = std::string(wFormatName.begin(), wFormatName.end());

	return true;
}

// 添加一个辅助函数来打印详细的错误信息
void MessageQueue::printErrorDetails(HRESULT hr) {
	switch (hr) {
	case MQ_ERROR_INVALID_PARAMETER:
		std::cerr << "Invalid parameter provided" << std::endl;
		break;
	case MQ_ERROR_INVALID_HANDLE:
		std::cerr << "Invalid handle" << std::endl;
		break;
	case MQ_ERROR_SERVICE_NOT_AVAILABLE:
		std::cerr << "MSMQ service is not running" << std::endl;
		break;
	case MQ_ERROR_INSUFFICIENT_RESOURCES:
		std::cerr << "Insufficient resources" << std::endl;
		break;
	case MQ_ERROR_ACCESS_DENIED:
		std::cerr << "Access denied. Check permissions" << std::endl;
		break;
	default:
		std::cerr << "Unknown error occurred" << std::endl;
	}
}

bool MessageQueue::openQueue(bool isReceive) {
	DWORD access = isReceive ? MQ_RECEIVE_ACCESS : MQ_SEND_ACCESS;

	// 将格式名称转换为 LPCWSTR 类型
	std::wstring wFormatName(m_szFormatName.begin(), m_szFormatName.end());
	HRESULT hr = MQOpenQueue(
		wFormatName.c_str(), // Format name
		access,              // Access mode
		MQ_DENY_NONE,        // Share mode
		&m_queueHandle       // Queue handle
	);

	if (hr != MQ_OK) {
		std::cerr << "Failed to open queue. Error: " << hr << std::endl;
		return false;
	}
	return true;
}

bool MessageQueue::send(const Message& msg) {
	if (m_queueHandle == NULL) {
		std::cerr << "Queue not opened for sending" << std::endl;
		return false;
	}

	// 序列化消息
	std::string serialized = std::to_string(msg.id) + ":" + msg.body;

	// 创建并填充 MQMSGPROPS 结构体
	MQMSGPROPS msgProps;
	MSGPROPID aPropId[1] = { PROPID_M_BODY };
	MQPROPVARIANT aPropVar[1];
	aPropVar[0].vt = VT_VECTOR | VT_UI1;
	aPropVar[0].caub.cElems = static_cast<ULONG>(serialized.size());
	aPropVar[0].caub.pElems = reinterpret_cast<LPBYTE>(const_cast<char*>(serialized.c_str()));

	msgProps.aPropID = aPropId;          // 属性 ID 数组
	msgProps.aPropVar = aPropVar;        // 属性值数组
	msgProps.cProp = 1;                   // 属性数量

	HRESULT hr = MQSendMessage(
		m_queueHandle,   // Queue handle
		&msgProps,       // Message properties
		NULL             // No transaction
	);

	if (hr != MQ_OK) {
		std::cerr << "Failed to send message. Error: " << hr << std::endl;
		return false;
	}
	return true;
}

bool MessageQueue::receive(Message& msg) {
	if (m_queueHandle == NULL) {
		std::cerr << "Queue not opened for receiving" << std::endl;
		return false;
	}

	// 创建并填充 MQMSGPROPS 结构体
	MQMSGPROPS msgProps;
	MSGPROPID aPropId[1] = { PROPID_M_BODY };
	MQPROPVARIANT aPropVar[1];

	// 设置消息属性
	msgProps.aPropID = aPropId;         // 属性 ID 数组
	msgProps.aPropVar = aPropVar;       // 属性值数组
	msgProps.cProp = 1;                  // 属性数量

	// 分配缓冲区并接收消息
	std::vector<BYTE> buffer;

	HRESULT hr = MQReceiveMessage(
		m_queueHandle,      // Queue handle
		INFINITE,           // Timeout
		MQ_ACTION_RECEIVE,  // Action
		&msgProps,         // Message properties
		NULL,              // Overlapped (not used)
		NULL,              // Callback (not used)
		NULL,              // Cursor (not used)
		NULL               // No transaction
	);

	if (hr != MQ_OK) {
		std::cerr << "Failed to receive message. Error: " << hr << std::endl;
		return false;
	}

	// 获取消息体的大小
	ULONG msgLen = aPropVar[0].caub.cElems;
	buffer.resize(msgLen);
	aPropVar[0].vt = VT_VECTOR | VT_UI1;
	aPropVar[0].caub.cElems = msgLen;
	aPropVar[0].caub.pElems = buffer.data();

	// 反序列化消息
	std::string serialized(reinterpret_cast<char*>(buffer.data()), msgLen);
	size_t pos = serialized.find(':');
	if (pos != std::string::npos) {
		msg.id = std::stoi(serialized.substr(0, pos));
		msg.body = serialized.substr(pos + 1);
	}

	return true;
}

bool MessageQueue::empty() const {
	return m_messages.empty();
}

size_t MessageQueue::size() const {
	return m_messages.size();
}

void MessageQueue::close() {
	if (m_queueHandle != NULL) {
		MQCloseQueue(m_queueHandle);
		m_queueHandle = NULL;
	}
}

std::string MessageQueue::getFormatName(const std::string& queueName) {
	DWORD formatNameLength = 0;
	WCHAR formatNameBuffer[MAX_PATH];
	std::wstring wQueueName(queueName.begin(), queueName.end());

	HRESULT hr = MQPathNameToFormatName(
		wQueueName.c_str(),
		formatNameBuffer,
		&formatNameLength
	);

	if (hr != MQ_OK) {
		std::cerr << "Failed to convert path name to format name. Error: " << hr << std::endl;
		return "";
	}

	// 使用 wcslen 计算字符串长度并进行转换
	std::wstring wFormatName(formatNameBuffer, formatNameLength / sizeof(WCHAR));
	return std::string(wFormatName.begin(), wFormatName.end());
}
