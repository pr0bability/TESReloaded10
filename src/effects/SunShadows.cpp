void SunShadowsEffect::SetCT() {
	EffectRecord::SetCT();

	// Change shadow atlas sampler state if anisotropy is supposed to be used.
	int anisotropy = TheShaderManager->Effects.ShadowsExteriors->Settings.ShadowMaps.Anisotropy;
	if (anisotropy) {
		TheRenderManager->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		TheRenderManager->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, anisotropy);
	}
}