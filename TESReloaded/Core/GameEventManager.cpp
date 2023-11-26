void GameEventManager::Initialize() {

	Logger::Log("Starting the event manager...");
	TheGameEventManager = new GameEventManager();

}

void GameEventManager::OnHitByPlayer() {

	float RandomPercent = 0.0f;

	if (TheShaderManager->Effects.BloodLens->Constants.Percent == 0) {
		RandomPercent = (double)rand() / (RAND_MAX + 1) * (100 - 1) + 1;
		if (RandomPercent <= TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "Chance")) TheShaderManager->Effects.BloodLens->Constants.Percent = 1;
	}

}