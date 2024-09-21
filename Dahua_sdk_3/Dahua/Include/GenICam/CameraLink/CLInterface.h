#ifndef __DAHUA_GENICAM_ICLINTERFACE_H__
#define __DAHUA_GENICAM_ICLINTERFACE_H__

#include "GenICam/Defs.h"
#include "GenICam/Camera.h"
#include "Memory/SharedPtr.h"

GENICAM_NAMESPACE_BEGIN

class ICLInterface;
typedef Memory::TSharedPtr<ICLInterface> ICLInterfacePtr;

/// \~chinese
/// \brief 相机接口类
/// \defgroup ICLInterface  连接CameraLink相机的接口类
/// \~english
/// \brief camera interface class
/// \defgroup ICLInterface API for CameraLink interface Only
/// @{

/// \~chinese
/// \brief Class ICLInterface 连接CameraLink相机的接口类
/// \~english
/// \brief Class ICLInterface interface class used to connect CL Camera 
class GENICAM_API ICLInterface
{
public:

	enum EInterfaceType
	{
		Euresys_CL_GRABLINK		= 0,		///< \~chinese Euresys CameraLink采集卡		\~english Euresys board
		Euresys_CL_Undefined1	= 1,		///< \~chinese Euresys CameraLink采集卡		\~english Euresys board
		Euresys_CL_Undefined2	= 2,		///< \~chinese Euresys CameraLink采集卡		\~english Euresys board
		Euresys_CL_Undefined3	= 3,		///< \~chinese Euresys CameraLink采集卡		\~english Euresys board
		Euresys_CL_Undefined4	= 4,		///< \~chinese Euresys CameraLink采集卡		\~english Euresys board

		Matrox_CL_RADIENTEVCL	= 5,		///< \~chinese Matrox CameraLink采集卡		\~english Matrox board
		Matrox_CL_SOLIOS		= 6,		///< \~chinese Matrox CameraLink采集卡		\~english Matrox board
		Matrox_CL_Undefined1	= 7,		///< \~chinese Matrox CameraLink采集卡		\~english Matrox board
		Matrox_CL_Undefined2	= 8,		///< \~chinese Matrox CameraLink采集卡		\~english Matrox board
		Matrox_CL_Undefined3	= 9,		///< \~chinese Matrox CameraLink采集卡		\~english Matrox board

		Dalsa_CL_Xtium_CL		= 10,		///< \~chinese Dalsa CameraLink采集卡		\~english Dalsa board
		Dalsa_CL_Undefined1		= 11,		///< \~chinese Dalsa CameraLink采集卡		\~english Dalsa board
		Dalsa_CL_Undefined2		= 12,		///< \~chinese Dalsa CameraLink采集卡		\~english Dalsa board
		Dalsa_CL_Undefined3		= 13,		///< \~chinese Dalsa CameraLink采集卡		\~english Dalsa board
		Dalsa_CL_Undefined4		= 14,		///< \~chinese Dalsa CameraLink采集卡		\~english Dalsa board
		
		Silicon_CL	= 15,					///< \~chinese Silicon采集卡	\~english Dalsa board

		InterfaceTypeUndefined	= 50			///< \~chinese 未定义采集卡		\~english Undefined board
	};

	enum ETriggerMode
	{
		Off			= 0,		///< \~chinese 触发模式关闭（连续模式）	\~english TriggerMode Off（Continuous Acquisition Mode）
		Software	= 1,		///< \~chinese 软触发模式				\~english Soft TriggerMode
		External	= 2,		///< \~chinese 外触发模式				\~english External TriggerMode

		Undefined	= 5,		///< \~chinese 未定义模式				\~english Undefined Mode
	};

	enum EParamName
	{
		Width = 1,		///< \~chinese 宽属性		\~english Width
		Heigh = 2,		///< \~chinese 高属性		\~english Heigh
		TriggerMode		= 3,	///< \~chinese 触发模式		\~english TriggerMode
		SoftwareExecute = 4,	///< \~chinese 软触发执行	\~english SoftwareExecute

		ParamNameUndefined = 255,	///< \~chinese 未定义属性名	\~english Undefined param
	};

	/// \~chinese
	/// \brief Struct Euresys卡信息结构体
	/// \~english
	/// \brief Struct Euresys board information structure
	typedef struct _EuresysInfo
	{
		uint16_t	index;				   ///< \~chinese 卡号					  \~english  Board index
		uint16_t	port;		           ///< \~chinese 端口号				  \~english  port
		char*		pConfigFile;		   ///< \~chinese 配置文件				  \~english  config file

		uint16_t	reserved[4];		   ///< \~chinese 预留					  \~english  Reserved field
	}EuresysInfo;

	/// \~chinese
	/// \brief Struct Dalsa卡信息结构体
	/// \~english
	/// \brief Struct Dalsa board information structure
	typedef struct _DalsaInfo
	{
		uint16_t	index;				    ///< \~chinese 卡号					  \~english  Board index
		uint16_t	templateNum;		    ///< \~chinese 模板号				  \~english  TemplateNumber
		char*		pConfigFile;			///< \~chinese 配置文件				  \~english  config file

		uint16_t	reserved[4];		    ///< \~chinese 预留					  \~english  Reserved field

	}DalsaInfo;

	/// \~chinese
	/// \brief Struct Matrox卡信息结构体
	/// \~english
	/// \brief Struct Matrox board information structure
	typedef struct _MatroxInfo
	{
		uint16_t	index;				    ///< \~chinese M_DEV			      \~english  Board Number
		char*		pConfigFile;			///< \~chinese 配置文件				  \~english  config file
		
		uint16_t	reserved[5];		    ///< \~chinese 预留					  \~english  Reserved field
	}MatroxInfo;

	/// \~chinese
	/// \brief Struct 采集卡参数结构体
	/// \~english
	/// \brief Struct Gige device stream statistics information structure
	typedef struct _InterfaceInfo
	{
		union
		{
			EuresysInfo euresysInfo;        ///< \~chinese Euresys采集卡信息		\~english  Euresys Information Object
			DalsaInfo	dalsaInfo;          ///< \~chinese Dalsa采集卡信息			\~english  Dalsa Information Object
			MatroxInfo	matroxInfo;			///< \~chinese Matrox采集卡信息			\~english  Matrox Information Object			                                
		};
	}InterfaceInfo;

protected:
	/// \~chinese
	/// \brief 析构函数
	/// \~english
	/// \brief destruct
    virtual ~ICLInterface(){}

public:
    /// \~chinese
    /// \brief 连接CameraLink相机接口对象获取函数，同一个cameraPtr对应的是同一个接口对象
    /// \param [in] cameraPtr CameraLink相机智能指针对象
    /// \~english
    /// \brief CL camera object access interface, same cameraPtr is correspond to same CL camera object
    /// \param [in] cameraPtr CL camera smart pointer object
    static ICLInterfacePtr getInstance(const ICameraPtr &cameraPtr);

    /// \~chinese
    /// \brief  获取接口描述信息
    /// \return 返回接口描述信息，失败返回NULL
    /// \~english
    /// \brief get interface description information
    /// \return success:return interface description information, fail:return NULL
	virtual const char * getDescription() = 0;

    /// \~chinese
    /// \brief  获取接口的供应商名
    /// \return 返回接口的供应商名，失败返回NULL
    /// \~english
    /// \brief get interface's VendorName
    /// \return success:return interface's VendorName, fail:return NULL
    virtual const char * getVendorName() = 0;

	/// \~chinese
	/// \brief  获取接口的端口
	/// \return 返回接口的端口，失败返回NULL
	/// \~english
	/// \brief get interface's PortId
	/// \return success:return interface's PortId, fail:return NULL
	virtual const char * getPortId() = 0;

	/// \~chinese
	/// \brief  获取接口(采集卡)类型
	/// \return 返回接口(采集卡)类型
	/// \~english
	/// \brief get interface's Type
	/// \return interface's Type
	virtual EInterfaceType getInterfaceType() const = 0;

	/// \~chinese
	/// \brief  设置指定采集卡的配置文件
	/// \param [in] index 采集卡编号
	/// \param [in] port 采集卡的端口
	/// \param [in] pConfigFile 采集卡配置文件
	/// \return 设置是否成功
	/// \~english
	/// \brief get interface's PortId
	/// \param [in] index the property's value
	/// \param [in] port the property's value
	/// \param [in] pConfigFile 采集卡配置文件
	/// \retval false set board config fail
	/// \retval true  set board config success
	virtual bool setBoardConfig(uint16_t index, uint16_t port, const char *pConfigFile) = 0;

	/// \~chinese
	/// \brief 获取属性值
	/// \param [in] paramName 属性名
	/// \param [out] val 获取到的属性值
	/// \return 成功返回true，否则false
	/// \~english
	/// \brief get property's value
	/// \param [in] paramName the property's name
	/// \param [out] val the property's value
	/// \return success:true, fail:false
	virtual bool getParamValue(EParamName paramName, uint32_t& val) const = 0;

	/// \~chinese
	/// \brief 设置属性值
	/// \param [in] paramName 属性名
	/// \param [in] val 待设置的属性值
	/// \return 成功则返回true，否则返回false
	/// \~english
	/// \brief set the property's value
	/// \param [in] paramName the property's name
	/// \param [in] val the value to be set for this property
	/// \return success:true, fail:false
	virtual bool setParamValue(EParamName paramName, uint32_t val) = 0;

	/// \~chinese
	/// \brief  设置接口(采集卡)类型
	/// \param [in] interfaceType (采集卡)类型
	/// \return 成功则返回true，否则返回false
	/// \~english
	/// \brief set interface's Type
	/// \param [in] interfaceType interface's Type
	/// \retval false set board config fail
	/// \retval true  set board config success
	virtual bool setInterfaceType(EInterfaceType interfaceType) = 0;

	/// \~chinese
	/// \brief  设置指定采集卡信息
	/// \param [in] interfaceInfo 采集卡信息结构体
	/// \return 成功则返回true，否则返回false
	/// \~english
	/// \brief get interface's PortId
	/// \param [in] interfaceInfo the cameralink info
	/// \retval false set board config fail
	/// \retval true  set board config success
	virtual bool setBoardConfigEx(InterfaceInfo &interfaceInfo) = 0;

};

/// @}

GENICAM_NAMESPACE_END

#endif//__DAHUA_GENICAM_ICLINTERFACE_H__