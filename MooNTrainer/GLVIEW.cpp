#include "stdafx.h"
#include "GLVIEW.h"
#include "MooNTrainerView.h"

#define MAX_CAM_HIGHTLEVEL 400
#define MIN_CAM_HIGHTLEVEL 2
#define MAX_CAM_FOV 20.0f
#define MASTER_LAYER_SIZE 600
#define SUB_LAYER_SIZE 190

enum TIMEREVNT { _RENDER = 100 };

CGLVIEW::CGLVIEW()
{
}


CGLVIEW::~CGLVIEW()
{
}

BEGIN_MESSAGE_MAP(CGLVIEW, COGLWnd)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


void CGLVIEW::InitGLview(int _nWidth, int _nHeight)
{
	mtSetPoint3D(&m_lookAt, 0, 0, 0);
	m_cameraPri.SetInitLevelHeight(MAX_CAM_HIGHTLEVEL * 2);
	m_cameraPri.InitializeCamera(MAX_CAM_FOV, 0, 0, m_lookAt, _nWidth, _nHeight);
	SetTimer(_RENDER, 30, NULL);

	//	SINGLETON_DataMng::GetInstance()->Test();
	glInitNames();
	m_renderlist = glGenLists(1);
//	m_bgTexId = SetMatTexture(bgimg);
}

void CGLVIEW::Render()
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	Render2D();
//	glEnable(GL_TEXTURE_2D);
	glCallList(m_renderlist);		
	glClear(GL_DEPTH_BUFFER_BIT);

	m_cameraPri.SetModelViewMatrix(m_lookAt, 0, 0);
	m_masterImage.Draw();
	for (auto i = 0; i < m_vecSubLayer.size(); i++) {
		m_vecSubLayer[i].Draw();
	}


	// Draw Valication Connection//
	if (m_masterImage.IsSelRect()) {
		POINT3D v1 = m_masterImage.GetGlobalSelectPos();
		glColor3f(0.0f, 1.0f, 0.0f);
		
		for (auto i = 0; i < m_vecSubLayer.size(); i++) {
			if (m_vecSubLayer[i].IsSelRect()) {
				glBegin(GL_LINES);
				POINT3D v2 = m_vecSubLayer[i].GetGlobalSelectPos();
				glVertex3f(v1.x, v1.y, v1.z);
				glVertex3f(v2.x, v2.y, v2.z);
				glEnd();
			}
		}		
	}


	SwapBuffers(m_CDCPtr->GetSafeHdc());
}

void CGLVIEW::Render2D()
{
	//wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	glNewList(m_renderlist, GL_COMPILE);
	gl_PushOrtho(0, m_rectWidth, 0, m_rectHeight);	
	//glBindTexture(GL_TEXTURE_2D, m_bgTexId);

	glBegin(GL_QUADS);
	glColor3f(0.1f, 0.3f, 0.5f);
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(0.0f, 0.0f, 0.0f);

	glColor3f(0.1f, 0.3f, 0.5f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(m_rectWidth, 0.0f, 0.0f);

	glColor3f(0.1f, 0.1f, 0.2f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(m_rectWidth, m_rectHeight, 0.0f);

	glColor3f(0.1f, 0.1f, 0.2f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(0.0f, m_rectHeight, 0.0f);
	glEnd();
	//glDisable(GL_TEXTURE_2D);	
	gl_PopOrtho();
	glEndList();
}

void CGLVIEW::MouseWheel(short zDelta)
{
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	float fLevelHeight = m_cameraPri.GetLevelHeight();
	float zoomValue = fLevelHeight*0.1f + MIN_CAM_HIGHTLEVEL;
	if (zDelta > 0) { zoomValue = -zoomValue; }
	fLevelHeight += zoomValue;

	if (fLevelHeight < MIN_CAM_HIGHTLEVEL) { fLevelHeight = MIN_CAM_HIGHTLEVEL; }

	m_cameraPri.SetInitLevelHeight(fLevelHeight);
//	m_cameraPri.SetModelViewMatrix(m_lookAt, 0, 0);
	Render();
}

int CGLVIEW::SetMatTexture(cv::Mat& img)
{
	GLuint tid;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, img.cols, img.rows, GL_RGB, GL_UNSIGNED_BYTE, img.ptr());

	return tid;
}

void CGLVIEW::SetMasterImage(cv::Mat& img)
{
//	cv::Mat srcimg = img.clone();

//	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	//int w = img.cols;
	//int h = img.rows;
//	cv::resize(img, img, cv::Size(2048, 2048));
	GLuint tid = SetMatTexture(img);
	POINT3D pos;
	mtSetPoint3D(&pos, -100.0f, 0.0f, 0.0f);
	m_masterImage.SetObjImage(img.cols, img.rows, MASTER_LAYER_SIZE, pos, tid);
}

void CGLVIEW::SetSubLayer(cv::Mat& img, int xid, int yid)
{
	GLuint tid = SetMatTexture(img);

//	int xpos = ((MASTER_LAYER_SIZE / 2) + SUB_LAYER_SIZE / 2) + xid*(SUB_LAYER_SIZE + 10)+10;
	int xpos = (MASTER_LAYER_SIZE / 2) + xid*(SUB_LAYER_SIZE + 10);
	int ypos = (MASTER_LAYER_SIZE / 2) - yid*(SUB_LAYER_SIZE + 10);


	CObjImage layerimg;
	POINT3D pos;
	pos.x = xpos;
	pos.y = ypos - (SUB_LAYER_SIZE + 10)/2;
	pos.z = 0;

	layerimg.SetObjImage(img.cols, img.rows, SUB_LAYER_SIZE, pos, tid);
	m_vecSubLayer.push_back(layerimg);	
}
void CGLVIEW::SetMasterImageSelection(int _wid, int wnum, int hnum, int _cellSize)
{
//	m_masterImage.SetSelectionRect(rect.x, rect.y, rect.width, rect.height);
	int xpos = (_wid % wnum)*_cellSize;
	int ypos = (_wid / hnum)*_cellSize;
	m_masterImage.SetSelectionRect(xpos, ypos, _cellSize, _cellSize);
}

void CGLVIEW::ReleaseSelections()
{
	//	m_masterImage.SetSelectionRect(rect.x, rect.y, rect.width, rect.height);
	m_masterImage.SetSelRect(false);
	for(auto i=0; i<m_vecSubLayer.size(); i++)
		m_vecSubLayer[i].SetSelRect(false);
}


void CGLVIEW::SetSubImageSelection(int _wid, int wnum, int hnum)
{
	int totalnum = wnum*hnum;
	int imgidx = _wid / totalnum;
	if (imgidx < m_vecSubLayer.size()) {
		int wordId = _wid - (_wid / totalnum)*(totalnum);
		int xpos = (wordId % wnum)*_UNIT_RESOLOTION_W;
		int ypos = (wordId / hnum)*_UNIT_RESOLOTION_W;
		m_vecSubLayer[imgidx].SetSelectionRect(xpos, ypos, _UNIT_RESOLOTION_W, _UNIT_RESOLOTION_W);
		if (imgidx > 0)
			m_vecSubLayer[imgidx - 1].SetSelRect(false);
	}	
}


void CGLVIEW::OnSize(UINT nType, int cx, int cy)
{
	COGLWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);
	m_rectWidth = cx;
	m_rectHeight = cy;
	m_cameraPri.SetProjectionMatrix(45.0f, 0.0f, 0.0f, cx, cy);
	m_cameraPri.SetModelViewMatrix(m_lookAt, 0.0f, 0.0f);

	Render2D();
}

void CGLVIEW::IDragMap(int x, int y, short sFlag)
{
	POINT3D curPos, prePos, transPos, ptLookAt;
	BOOL res = FALSE;
	int dx = 0, dy = 0;


	switch (sFlag) {
	case 0:		// DOWN
		m_dragOper.init();
		m_dragOper.IsDrag = true;
		m_dragOper.px = x;
		m_dragOper.py = y;
		break;

	case 1:		// MOVE
		if (m_dragOper.IsDrag) {

			curPos = m_cameraPri.ScreenToWorld(x, y);
			prePos = m_cameraPri.ScreenToWorld(m_dragOper.px, m_dragOper.py);
			transPos = prePos - curPos;
			ptLookAt = m_cameraPri.GetLookAt();

			//ptLookAt.x += transPos.x*m_moveVec.x;
			//ptLookAt.y += transPos.y*m_moveVec.y;

			ptLookAt.x += transPos.x;
			ptLookAt.y += transPos.y;


			m_dragOper.px = x;
			m_dragOper.py = y;

			//	m_DemProj->ProjectPoint(&ptLookAt, &m_cameraPri.m_currentBlockid);
			//m_cameraPri.SetModelViewMatrix(ptLookAt, 0, 0);
			m_lookAt = ptLookAt;
			//	UpdateCamera(ptLookAt, 0,0);
		}
		break;

	case 2:		// UP
		ReleaseCapture();
		m_dragOper.init();
		break;
	}
}

void CGLVIEW::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	wglMakeCurrent(m_CDCPtr->GetSafeHdc(), m_hRC);

	SelectObject3D(point.x, point.y, 2, 2, 0);

	m_mouseMode = 2;
	IDragMap(point.x, point.y, 0);
	SetCapture();

	COGLWnd::OnLButtonDown(nFlags, point);
}


void CGLVIEW::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_mouseMode = 0;
	IDragMap(point.x, point.y, 2);
	ReleaseCapture();

	COGLWnd::OnLButtonUp(nFlags, point);
}


void CGLVIEW::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetCapture()) {
		if ((point.x > 0) && (point.x < m_rectWidth) && (point.y > 0) && (point.y < m_rectHeight)) {
			IDragMap(point.x, point.y, 1);
		}
		Render();
	}
	COGLWnd::OnMouseMove(nFlags, point);
}



void CGLVIEW::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == _RENDER) {
		Render();
	}
	COGLWnd::OnTimer(nIDEvent);
}

int CGLVIEW::SelectObject3D(int x, int y, int rect_width, int rect_height, int selmode)
{
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();

	GLuint selectBuff[1024];
	memset(&selectBuff, 0, sizeof(GLuint) * 1024);

	GLint hits, viewport[4];
	hits = 0;

	glSelectBuffer(1024, selectBuff);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glRenderMode(GL_SELECT);


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix(x, viewport[3] - y, rect_width, rect_height, viewport);
	gluPerspective(m_cameraPri.m_Cntfovy, (float)viewport[2] / (float)viewport[3], m_cameraPri.GetNearPlane(), m_cameraPri.GetFarPlane());

	//m_cameraPri.SetProjectionMatrix(45.0f, 0.0f, 0.0f, cx, cy);
	glMatrixMode(GL_MODELVIEW);

	// Draw for picking==============================//
	glPushName(0);
	m_masterImage.DrawForPicking();
	glPopName();


	//=============================================//
	hits = glRenderMode(GL_RENDER);
	int lineid = -1, textid = -1;
	int uuid = -1;


	if (hits > 0) {
		int selid = selectBuff[3];
		if (selid == 0) {
			CPoint pos = m_masterImage.GetImagePos(m_cameraPri.ScreenToWorld(x, y));
			pView->SelectCellMaster(pos, true);

		}
		
	}


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	return hits;
}