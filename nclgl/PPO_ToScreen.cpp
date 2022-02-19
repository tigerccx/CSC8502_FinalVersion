#include "PPO_ToScreen.h"

void PPO_ToScreen::RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast)
{
	renderTargetFinal = renderTargetLast;
	RenderToDefaultRenderTarget();
}
