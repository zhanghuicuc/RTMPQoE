// 从类型库向导中用“添加类”创建的计算机生成的 IDispatch 包装类

#import "C:\\Program Files\\TeeChart Pro v5 ActiveX Control\\TeeChart5.ocx" no_namespace
// CPage 包装类

class CPage : public COleDispatchDriver
{
public:
	CPage(){} // 调用 COleDispatchDriver 默认构造函数
	CPage(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CPage(const CPage& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// 特性
public:

	// 操作
public:


	// IPage 方法
public:
	void Next()
	{
		InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Previous()
	{
		InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long get_MaxPointsPerPage()
	{
		long result;
		InvokeHelper(0x36, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_MaxPointsPerPage(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x36, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Current()
	{
		long result;
		InvokeHelper(0x38, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Current(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x38, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_ScaleLastPage()
	{
		BOOL result;
		InvokeHelper(0x39, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ScaleLastPage(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x39, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Count()
	{
		long result;
		InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}

	// IPage 属性
public:

};
