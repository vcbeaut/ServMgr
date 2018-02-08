
// ServMgrView.h : CServMgrView 类的接口
//

#pragma once


class CServMgrView : public CListView
{
protected: // 仅从序列化创建
	CServMgrView();
	DECLARE_DYNCREATE(CServMgrView)

// 特性
public:
	CServMgrDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CServMgrView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ServMgrView.cpp 中的调试版本
inline CServMgrDoc* CServMgrView::GetDocument() const
   { return reinterpret_cast<CServMgrDoc*>(m_pDocument); }
#endif

