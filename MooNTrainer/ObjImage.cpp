#include "stdafx.h"
#include "ObjImage.h"


CObjImage::CObjImage()
{
	m_texId = 0;

	for (int i = 0; i < 4; i++)	{
		mtSetPoint3D(&m_vSelRect[i], 0, 0, 0);
		mtSetPoint3D(&m_vertex[i], 0, 0, 0);
	}

	m_bIsSelRect = false;
}


CObjImage::~CObjImage()
{
}

void CObjImage::SetObjImage(int _w, int _h, int _size, POINT3D pos, GLuint _texid)
{
	if (m_texId != 0) {
		glDeleteTextures(1, &m_texId);
	}

	m_texId = _texid;
	m_pos = pos;
	m_nImgWidth = _w;
	m_nImgHeight = _h;

	m_nRectWidth = _size;
	m_nRectHeight = _size;

	m_fXScale = (float)m_nImgWidth / (float)m_nRectWidth;
	m_fYScale = (float)m_nImgHeight / (float)m_nRectHeight;

	mtSetPoint2D(&m_texcoord[0], 0.0f, 1.0f);
	mtSetPoint2D(&m_texcoord[1], 1.0f, 1.0f);
	mtSetPoint2D(&m_texcoord[2], 1.0f, 0.0f);
	mtSetPoint2D(&m_texcoord[3], 0.0f, 0.0f);


	mtSetPoint3D(&m_vertex[0], -_size*0.5f, -_size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertex[1], _size*0.5f, -_size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertex[2], _size*0.5f, _size*0.5f, 0.0f);
	mtSetPoint3D(&m_vertex[3], -_size*0.5f, _size*0.5f, 0.0f);
}

void CObjImage::Draw()
{
	if (m_texId == 0)
		return;

	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texId);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(m_texcoord[0].x, m_texcoord[0].y);
	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
	glTexCoord2f(m_texcoord[1].x, m_texcoord[1].y);
	glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);
	glTexCoord2f(m_texcoord[2].x, m_texcoord[2].y);
	glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);
	glTexCoord2f(m_texcoord[3].x, m_texcoord[3].y);
	glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor4f(0.5f, 0.7f, 1.0f, 1.0f);
	glBegin(GL_LINE_STRIP);
	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
	glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);
	glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);
	glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
	glEnd();

	// Draw Sel Rect //
	glLineWidth(2);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glBegin(GL_LINE_STRIP);
	glVertex3f(m_vSelRect[0].x, m_vSelRect[0].y, m_vSelRect[0].z);
	glVertex3f(m_vSelRect[1].x, m_vSelRect[1].y, m_vSelRect[1].z);
	glVertex3f(m_vSelRect[2].x, m_vSelRect[2].y, m_vSelRect[2].z);
	glVertex3f(m_vSelRect[3].x, m_vSelRect[3].y, m_vSelRect[3].z);
	glVertex3f(m_vSelRect[0].x, m_vSelRect[0].y, m_vSelRect[0].z);
	glEnd();
	glLineWidth(1);


	glPopMatrix();

}

void CObjImage::DrawForPicking()
{
	glPushMatrix();
	glTranslatef(m_pos.x, m_pos.y, m_pos.z);

	glBegin(GL_QUADS);
	glVertex3f(m_vertex[0].x, m_vertex[0].y, m_vertex[0].z);
	glVertex3f(m_vertex[1].x, m_vertex[1].y, m_vertex[1].z);
	glVertex3f(m_vertex[2].x, m_vertex[2].y, m_vertex[2].z);
	glVertex3f(m_vertex[3].x, m_vertex[3].y, m_vertex[3].z);
	glEnd();

	glPopMatrix();
}

int CObjImage::SetSelectionRectByID(int _id)
{
	return 0;
}


int CObjImage::SetSelectionRect(int x, int y, int width, int height)
{
	x = ((float)x / m_fXScale) - m_nRectWidth*0.5f;
	y = (m_nRectHeight -((float)y / m_fXScale)) - m_nRectHeight*0.5f;
	width = (float)width / m_fXScale;
	height = (float)height / m_fYScale;

	mtSetPoint3D(&m_vSelRect[0], x, y, 0);
	mtSetPoint3D(&m_vSelRect[1], x+width, y, 0);
	mtSetPoint3D(&m_vSelRect[2], x+width, y-height, 0);
	mtSetPoint3D(&m_vSelRect[3], x, y-height, 0);

	m_bIsSelRect = true;
	return 0;
}

CPoint CObjImage::GetImagePos(POINT3D hitPnt)
{
	CPoint imgpos;
	imgpos.x = 0;
	imgpos.y = 0;
	

	hitPnt.x = (hitPnt.x - m_pos.x) + m_nRectWidth*0.5f;
	hitPnt.y = m_nRectHeight - ((hitPnt.y - m_pos.y) + m_nRectHeight*0.5f);
	
	if ((hitPnt.x > 0) && (hitPnt.x < m_nRectWidth) && (hitPnt.y > 0) && (hitPnt.y < m_nRectHeight)) {
		imgpos.x = hitPnt.x * m_fXScale;
		imgpos.y = hitPnt.y * m_fYScale;
	}
	return imgpos;
}

POINT3D CObjImage::GetGlobalSelectPos()
{
	POINT3D pos = m_vSelRect[0];
	pos.x += m_pos.x;
	pos.y += m_pos.y;
	pos.z = 0;
	return pos;
}