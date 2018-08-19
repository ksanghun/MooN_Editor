#pragma once

#include "math_tool.h"

class CObjImage
{
public:
	CObjImage();
	~CObjImage();

	void SetObjImage(int _w, int _h, int size, POINT3D pos, GLuint _texid);
	void Draw();
	void DrawForPicking();
	int SetSelectionRect(int x, int y, int width, int height);
	int SetSelectionRectByID(int _id);
	CPoint GetImagePos(POINT3D hitPnt);

	POINT3D GetGlobalSelectPos();
	bool IsSelRect() { return m_bIsSelRect; }

private:
	GLuint m_texId;

	POINT3D m_vertex[4];
	POINT2D m_texcoord[4];
	POINT3D m_vSelRect[4];

	int m_nImgWidth;
	int m_nImgHeight;
	int m_nRectWidth;
	int m_nRectHeight;
	float m_fXScale;
	float m_fYScale;

	POINT3D m_pos;
	bool m_bIsSelRect;
};

