/************************************************
*              		 Texture.cpp                  *
*************************************************
*          This file is a part of:              *
*               COLUMBUS ENGINE                 *
*************************************************
*             Nikolay(Columbus) Red             *
*                   20.07.2017                  *
*************************************************/

#include <Graphics/Texture.h>

namespace C
{

	//////////////////////////////////////////////////////////////////////////////
	//Load image from file
	char* C_LoadImage(const char* aPath, int* aWidth, int* aHeight)
	{
		FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(aPath, 0);
		if (formato == FIF_UNKNOWN) { C_Error("Can't get type of File: %s", aPath); return NULL; }
		FIBITMAP* imagen = FreeImage_Load(formato, aPath);
		if (!imagen) { C_Error("Can't load Image File: %s", aPath); return NULL; }
		FIBITMAP* temp = FreeImage_ConvertTo32Bits(imagen);
		if (!imagen) { C_Error("Can't convert image to 32 Bits: %s", aPath); return NULL; }
		FreeImage_Unload(imagen);
		imagen = temp;

		char* bits = (char*)FreeImage_GetBits(imagen);
		*aWidth = FreeImage_GetWidth(imagen);
		*aHeight = FreeImage_GetHeight(imagen);

		C_Success("Image loaded: %s", aPath);

		return bits;
	}
	//////////////////////////////////////////////////////////////////////////////
	//Constructor
	C_Texture::C_Texture()
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	//Constructor 2
	C_Texture::C_Texture(const char* aPath, bool aSmooth)
	{
		load(aPath, aSmooth);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Constructor 3
	C_Texture::C_Texture(const char* aData, const int aW, const int aH, bool aSmooth)
	{
		load(aData, aW, aH, aSmooth);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Load texture from file
	void C_Texture::load(const char* aPath, bool aSmooth)
	{
		if (mBuffer != NULL)
			FreeImage_Unload(mBuffer);
		FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(aPath, 0);
		if (formato == FIF_UNKNOWN) { C_Error("Can't get type of File: %s", aPath); return; }
		FIBITMAP* imagen = FreeImage_Load(formato, aPath);
		if (!imagen) { C_Error("Can't load Image File: %s", aPath); return; }
		FIBITMAP* temp = FreeImage_ConvertTo32Bits(imagen);
		if (!imagen) { C_Error("Can't convert image to 32 Bits: %s", aPath); return; }
		FreeImage_Unload(imagen);
		imagen = temp;

		char* bits = (char*)FreeImage_GetBits(imagen);
		int nWidth = FreeImage_GetWidth(imagen);
		int nHeight = FreeImage_GetHeight(imagen);
		int nBPP = FreeImage_GetBPP(imagen);

		glGenTextures(1, &mID);
		glBindTexture(GL_TEXTURE_2D, mID);

		switch (mWrapX)
		{
			case C_TEXTURE_CLAMP:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				break;
			case C_TEXTURE_REPEAT:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				break;
			case C_TEXTURE_MIRROR_REPEAT:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				break;
		}

		switch (mWrapY)
		{
			case C_TEXTURE_CLAMP:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case C_TEXTURE_REPEAT:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			case C_TEXTURE_MIRROR_REPEAT:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
		if (mMipmaps == true)
			glGenerateMipmap(GL_TEXTURE_2D);

		mBuffer = imagen;

		if (mSmooth == true)
		{
			if (mMipmaps == true)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, mAnisotropy);

		glBindTexture(GL_TEXTURE_2D, 0);

		mFile = (char*)aPath;
		mWidth = nWidth;
		mHeight = nHeight;
		mBPP = nBPP;

		C_Success("Texture loaded: %s", aPath);

	}

	/*void C_Texture::loadNoMessage(const char* aPath, bool aSmooth)
	{
		if(mBuffer != NULL && mBuffer != nullptr)
			FreeImage_Unload(mBuffer);

		FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(aPath, 0), aPath);
		if(bitmap == NULL)
		{
			return;
		}
		FIBITMAP *pImage = FreeImage_ConvertTo32Bits(bitmap);
		int nWidth = FreeImage_GetWidth(pImage);
		int nHeight = FreeImage_GetHeight(pImage);

		glGenTextures(1, &mID);
		glBindTexture(GL_TEXTURE_2D, mID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight,
        0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
	    glGenerateMipmap(GL_TEXTURE_2D);
		if(aSmooth == true)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    }
      glBindTexture(GL_TEXTURE_2D, 0);
      mBuffer = bitmap;
      FreeImage_Unload(pImage);
      FreeImage_Unload(bitmap);
	}*/

	//////////////////////////////////////////////////////////////////////////////
	//Load texture from raw data
	void C_Texture::load(const char* aData, const int aW, const int aH, bool aSmooth)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &mID);
		glBindTexture(GL_TEXTURE_2D, mID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if(aSmooth == true)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, /*GL_RGBA*/GL_RED, aW, aH,
		0, /*GL_ALPHA*/GL_RED, GL_UNSIGNED_BYTE, (void*)aData);
		glBindTexture(GL_TEXTURE_2D, 0);
		//printf("\x1b[32;1mTexture successfuly loaded from buffer\x1b[0m\n");
	}
	//////////////////////////////////////////////////////////////////////////////
	//Load texture from memory
	void C_Texture::loadFromMemory(const char* aData, size_t aSize, bool aSmooth)
	{
		glGenTextures(1, &mID);
		/*
		size_t size = C_FileSize(aPath);

		char* buf = (char*)malloc(aSize);

		FILE *file = fopen(aPath, "rb");
		fread(buf, size, 1, file);
		fclose(file);*/

		BYTE* mem_buffer = (BYTE*)malloc(aSize);

		memcpy(mem_buffer, aData, aSize);

		FIMEMORY* hmem = FreeImage_OpenMemory(mem_buffer, aSize);

		FIBITMAP* bitmap = FreeImage_LoadFromMemory(FreeImage_GetFileTypeFromMemory(hmem, 0), hmem, 0);
		FIBITMAP *pImage = FreeImage_ConvertTo32Bits(bitmap);

		glBindTexture(GL_TEXTURE_2D, mID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth(pImage), FreeImage_GetHeight(pImage),
	    0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
		glGenerateMipmap(GL_TEXTURE_2D);
		if(aSmooth)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	    glBindTexture(GL_TEXTURE_2D, 0);

		FreeImage_CloseMemory(hmem);
		FreeImage_Unload(bitmap);
		FreeImage_Unload(pImage);

		free((void*)aData);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Reload texture
	void C_Texture::reload()
	{
		if (mBuffer == nullptr)
			return;

		char* bits = (char*)FreeImage_GetBits(mBuffer);
		int nWidth = FreeImage_GetWidth(mBuffer);
		int nHeight = FreeImage_GetHeight(mBuffer);

		if (mID != 0)
			glDeleteTextures(1, &mID);

		glGenTextures(1, &mID);
		glBindTexture(GL_TEXTURE_2D, mID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
		if (mMipmaps == true)
			glGenerateMipmap(GL_TEXTURE_2D);

		if (mSmooth == true)
		{
			if (mMipmaps == true)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, mAnisotropy);

		glBindTexture(GL_TEXTURE_2D, 0);

		C_Success("Texture reloaded");
	}
	//////////////////////////////////////////////////////////////////////////////
	//Set texture config
	void C_Texture::setConfig(C_TextureConfig aConfig)
	{
		mSmooth = aConfig.smooth;
		mMipmaps = aConfig.mipmaps;
		mAnisotropy = aConfig.anisotropy;
		mWrapX = aConfig.wrapX;
		mWrapY = aConfig.wrapY;
	}
	//////////////////////////////////////////////////////////////////////////////
	//Return texture config
	C_TextureConfig C_Texture::getConfig()
	{
		C_TextureConfig c;
		c.smooth = mSmooth;
		c.mipmaps = mMipmaps;
		c.anisotropy = mAnisotropy;
		c.wrapX = mWrapX;
		c.wrapY = mWrapY;
		return c;
	}
	//////////////////////////////////////////////////////////////////////////////
	//Return texture size
	size_t C_Texture::getSize()
	{
		if (mBuffer == nullptr)
			return 0;

		return mWidth * mHeight * (mBPP / 8);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Save image to file
	void C_Texture::save(const char* aFile)
	{
		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'j' || aFile[strlen(aFile) - 3] == 'J')
				if(aFile[strlen(aFile) - 2] == 'p' || aFile[strlen(aFile) - 2] == 'P')
					if(aFile[strlen(aFile) - 1] == 'g' || aFile[strlen(aFile) - 1] == 'G')
					{
						FreeImage_Save(FIF_JPEG, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'j' || aFile[strlen(aFile) - 3] == 'J')
				if(aFile[strlen(aFile) - 2] == 'i' || aFile[strlen(aFile) - 2] == 'I')
					if(aFile[strlen(aFile) - 1] == 'l' || aFile[strlen(aFile) - 1] == 'L')
					{
						FreeImage_Save(FIF_JPEG, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'j' || aFile[strlen(aFile) - 3] == 'J')
				if(aFile[strlen(aFile) - 2] == 'p' || aFile[strlen(aFile) - 2] == 'P')
					if(aFile[strlen(aFile) - 1] == 'e' || aFile[strlen(aFile) - 1] == 'E')
					{
						FreeImage_Save(FIF_JPEG, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'p' || aFile[strlen(aFile) - 3] == 'P')
				if(aFile[strlen(aFile) - 2] == 'b' || aFile[strlen(aFile) - 2] == 'B')
					if(aFile[strlen(aFile) - 1] == 'm' || aFile[strlen(aFile) - 1] == 'M')
					{
						FreeImage_Save(FIF_PBM, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'p' || aFile[strlen(aFile) - 3] == 'P')
				if(aFile[strlen(aFile) - 2] == 'g' || aFile[strlen(aFile) - 2] == 'G')
					if(aFile[strlen(aFile) - 1] == 'm' || aFile[strlen(aFile) - 1] == 'M')
					{
						FreeImage_Save(FIF_PGM, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'p' || aFile[strlen(aFile) - 3] == 'P')
				if(aFile[strlen(aFile) - 2] == 'p' || aFile[strlen(aFile) - 2] == 'P')
					if(aFile[strlen(aFile) - 1] == 'm' || aFile[strlen(aFile) - 1] == 'M')
					{
						FreeImage_Save(FIF_PPM, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 't' || aFile[strlen(aFile) - 3] == 'T')
				if(aFile[strlen(aFile) - 2] == 'g' || aFile[strlen(aFile) - 2] == 'G')
					if(aFile[strlen(aFile) - 1] == 'a' || aFile[strlen(aFile) - 1] == 'A')
					{
						FreeImage_Save(FIF_TARGA, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 6] == '.')
			if(aFile[strlen(aFile) - 5] == 't' || aFile[strlen(aFile) - 5] == 'T')
				if(aFile[strlen(aFile) - 4] == 'a' || aFile[strlen(aFile) - 4] == 'A')
					if(aFile[strlen(aFile) - 3] == 'r' || aFile[strlen(aFile) - 3] == 'R')
						if(aFile[strlen(aFile) - 2] == 'g' || aFile[strlen(aFile) - 2] == 'G')
							if(aFile[strlen(aFile) - 1] == 'a' || aFile[strlen(aFile) - 1] == 'A')
							{
								FreeImage_Save(FIF_TARGA, mBuffer, aFile, 0);
							}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 't' || aFile[strlen(aFile) - 3] == 'T')
				if(aFile[strlen(aFile) - 2] == 'i' || aFile[strlen(aFile) - 2] == 'I')
					if(aFile[strlen(aFile) - 1] == 'f' || aFile[strlen(aFile) - 1] == 'F')
					{
						FreeImage_Save(FIF_TIFF, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 5] == '.')
			if(aFile[strlen(aFile) - 4] == 't' || aFile[strlen(aFile) - 4] == 'T')
				if(aFile[strlen(aFile) - 3] == 'i' || aFile[strlen(aFile) - 3] == 'I')
					if(aFile[strlen(aFile) - 2] == 'f' || aFile[strlen(aFile) - 2] == 'F')
						if(aFile[strlen(aFile) - 1] == 'f' || aFile[strlen(aFile) - 1] == 'F')
						{
							FreeImage_Save(FIF_TIFF, mBuffer, aFile, 0);
						}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'x' || aFile[strlen(aFile) - 3] == 'X')
				if(aFile[strlen(aFile) - 2] == 'p' || aFile[strlen(aFile) - 2] == 'P')
					if(aFile[strlen(aFile) - 1] == 'm' || aFile[strlen(aFile) - 1] == 'M')
					{
						FreeImage_Save(FIF_XPM, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'p' || aFile[strlen(aFile) - 3] == 'P')
				if(aFile[strlen(aFile) - 2] == 'n' || aFile[strlen(aFile) - 2] == 'N')
					if(aFile[strlen(aFile) - 1] == 'g' || aFile[strlen(aFile) - 1] == 'G')
					{
						FreeImage_Save(FIF_PNG, mBuffer, aFile, 0);
					}
		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'e' || aFile[strlen(aFile) - 3] == 'E')
				if(aFile[strlen(aFile) - 2] == 'x' || aFile[strlen(aFile) - 2] == 'X')
					if(aFile[strlen(aFile) - 1] == 'r' || aFile[strlen(aFile) - 1] == 'R')
					{
						FreeImage_Save(FIF_EXR, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'g' || aFile[strlen(aFile) - 3] == 'G')
				if(aFile[strlen(aFile) - 2] == 'i' || aFile[strlen(aFile) - 2] == 'I')
					if(aFile[strlen(aFile) - 1] == 'f' || aFile[strlen(aFile) - 1] == 'F')
					{
						FreeImage_Save(FIF_GIF, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'h' || aFile[strlen(aFile) - 3] == 'H')
				if(aFile[strlen(aFile) - 2] == 'd' || aFile[strlen(aFile) - 2] == 'D')
					if(aFile[strlen(aFile) - 1] == 'r' || aFile[strlen(aFile) - 1] == 'R')
					{
						FreeImage_Save(FIF_HDR, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'i' || aFile[strlen(aFile) - 3] == 'I')
				if(aFile[strlen(aFile) - 2] == 'c' || aFile[strlen(aFile) - 2] == 'C')
					if(aFile[strlen(aFile) - 1] == 'o' || aFile[strlen(aFile) - 1] == 'O')
					{
						FreeImage_Save(FIF_ICO, mBuffer, aFile, 0);
					}

		if(aFile[strlen(aFile) - 5] == '.')
			if(aFile[strlen(aFile) - 4] == 'j' || aFile[strlen(aFile) - 4] == 'J')
				if(aFile[strlen(aFile) - 3] == 'p' || aFile[strlen(aFile) - 3] == 'P')
					if(aFile[strlen(aFile) - 2] == 'e' || aFile[strlen(aFile) - 2] == 'E')
						if(aFile[strlen(aFile) - 1] == 'g' || aFile[strlen(aFile) - 1] == 'G')
						{
							FreeImage_Save(FIF_JPEG, mBuffer, aFile, 0);
						}

		if(aFile[strlen(aFile) - 4] == '.')
			if(aFile[strlen(aFile) - 3] == 'b' || aFile[strlen(aFile) - 3] == 'B')
				if(aFile[strlen(aFile) - 2] == 'm' || aFile[strlen(aFile) - 2] == 'M')
					if(aFile[strlen(aFile) - 1] == 'p' || aFile[strlen(aFile) - 3] == 'P')
					{
						FreeImage_Save(FIF_BMP, mBuffer, aFile, 0);
					}

		C_Success("Image saved: %s", aFile);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Bind texture
	void C_Texture::bind()
	{
		if(mID != 0)
			glBindTexture(GL_TEXTURE_2D, mID);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Unbind texture
	void C_Texture::unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Create sampler and bind texture
	void C_Texture::sampler2D(int a)
	{
		glActiveTexture(GL_TEXTURE0 + a);
		bind();
	}
	//////////////////////////////////////////////////////////////////////////////
	//Destructor
	C_Texture::~C_Texture()
	{
		glDeleteTextures(1, &mID);
		if(mBuffer != NULL && mBuffer != nullptr)
			FreeImage_Unload(mBuffer);
	}

}
