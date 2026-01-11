#pragma once

class ImageSpaceEffectAfterTonemapping : public ImageSpaceEffect {
private:
	static void* vtable[8];

public:
	static ImageSpaceEffectAfterTonemapping* CreateObject();

	void RenderEx(NiTriShape* apScreenShape, NiDX9Renderer* apRenderer, ImageSpaceEffectParam* apParam, bool bEndFrame);
	bool IsActiveEx();
};
