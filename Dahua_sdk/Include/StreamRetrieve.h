#include "Infra/Thread.h"
#include "GenICam/StreamSource.h"

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

/// \~chinese
/// \brief 流获取线程（示例头文件）
/// \~english
/// \brief get stream thread (sample include)

class StreamRetrieve : public CThread
{
public:
	/// \~chinese
	/// \brief 构造函数
	/// \~english
	/// \brief construct function 
	StreamRetrieve(IStreamSourcePtr& streamSptr);

	/// \~chinese
	/// \brief 开始取流线程
	/// \~english
	/// \brief Start get frame thread
	bool start();

	/// \~chinese
	/// \brief 停止取流线程
	/// \~english
	/// \brief stop get frame thread
	void stop();
private:
	void threadProc();
	bool m_isLoop;
	IStreamSourcePtr m_streamSptr;
};