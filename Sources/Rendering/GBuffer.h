#pragma once
#include <GL/gl3w.h>

class GBuffer
{
public:
	GBuffer(unsigned int width, unsigned int height);

	void Init();

	void SetFrameBuffer();

	void SetTexture();

private:
	unsigned int m_width;
	unsigned int m_height;

	unsigned int m_frameBuffer;

	unsigned int m_posData;
	unsigned int m_normalData;
	unsigned int m_albedoData;
	unsigned int m_depth;
};