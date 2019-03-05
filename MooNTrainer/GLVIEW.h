#pragma once
#include "OGLWnd.h"
#include "MNDBLayer.h"
#include "ObjImage.h"

class CGLVIEW :
	public COGLWnd
{
public:
	CGLVIEW();
	~CGLVIEW();

	void InitGLview(int _nWidth, int _nHeight);
	void MouseWheel(short zDelta);

	void Render();
	void Render2D();
	void IDragMap(int x, int y, short sFlag);

	int SetMatTexture(cv::Mat& img);
	void SetMasterImage(cv::Mat& img);
	void SetSubLayer(cv::Mat& img, int xid, int yid);
//	void SetMasterImageSelection(cv::Rect rect);
	void SetMasterImageSelection(int _wid, int wnum, int hnum, int _cellSize);
	void SetSubImageSelection(int _wid, int wnum, int hnum);
	int SelectObject3D(int x, int y, int rect_width, int rect_height, int selmode);
	void ReleaseSelections();
private:
	CPoint m_mousedown;
	CPoint m_preMmousedown;
	short m_mouseMode;

	POINT3D m_lookAt;
	unsigned short m_rectWidth, m_rectHeight;
	VA_DRAG_OPER m_dragOper;
	LOGFONT		m_LogFont;
	BITMAPINFO* m_pBmpInfo;

	int m_bgTexId;
	GLuint m_renderlist;


	//================//
	CObjImage m_masterImage;
	std::vector<CObjImage> m_vecSubLayer;
	

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

