#include "global.h"

Vector2 MenuPos = { 200.f, 200.f };
PVOID PresentOrg = nullptr;

Engine* GameEngine;
Entity* LocalPlayer = NULL;
Entity* BestTarget = NULL;
Entity* NewTarget = NULL;
Entity* AimbotTarget = NULL;
float BestDistance, Distance;

void esp(Entity* pEntity) {
	Vector2 ScreenFeet, ScreenHead;
	Vector3 Feet = pEntity->GetBoneLocationByID(BONE_FEET), Head = pEntity->GetBoneLocationByID(BONE_HEAD);

	if (GameEngine->WorldToScreen(Feet, ScreenFeet) && GameEngine->WorldToScreen(Head, ScreenHead)) {
		if (Visuals::ESP){
			float Height = (ScreenHead.y - ScreenFeet.y), Width = Height / 2.f;
			Render::Rectangle({ (ScreenFeet.x - (Width / 2.f)), ScreenFeet.y }, { Width, Height }, Render::Color(0, 0, 0));
			Render::Rectangle({ (ScreenFeet.x - (Width / 2.f)), ScreenFeet.y }, { Width, Height }, Render::Color(255, 255, 255));
			float HealthSize = (Height * pEntity->GetHealth()) / pEntity->GetMaxHealth();
			Render::Line({ (ScreenFeet.x - (Width / 2.f) + 4.f), ScreenFeet.y }, { (ScreenFeet.x - (Width / 2.f) + 4.f), ScreenHead.y }, Render::Color(0, 0, 0), 5.f);
			Render::Line({ (ScreenFeet.x - (Width / 2.f) + 4.f), ScreenFeet.y + HealthSize }, { (ScreenFeet.x - (Width / 2.f) + 4.f), ScreenHead.y }, Render::Color(255, 0, 0), 2.f);
			Render::Line({ (ScreenFeet.x - (Width / 2.f) + 4.f), ScreenFeet.y }, { (ScreenFeet.x - (Width / 2.f) + 4.f), ScreenFeet.y + HealthSize }, Render::Color(0, 255, 0), 2.f);
		}
	
		if (Visuals::Head)
			Render::Circle(ScreenHead, Render::Color(255, 255, 255), 5.f);
		
		if (Visuals::Skeleton) {
			Vector3 BoneLocation[7];
			Vector2 ScreenSpace[7];
			for (int i = 0; i < 7; i++)
			{
				BoneLocation[i] = pEntity->GetBoneLocationByID(i);
				GameEngine->WorldToScreen(BoneLocation[i], ScreenSpace[i]);
			}
			Render::Line(ScreenSpace[BONE_HEAD], ScreenSpace[BONE_NECK], Render::Color(255, 255, 255));
			Render::Line(ScreenSpace[BONE_NECK], ScreenSpace[BONE_HAND], Render::Color(255, 255, 255));
			Render::Line(ScreenSpace[BONE_NECK], ScreenSpace[BONE_STOMACH], Render::Color(255, 255, 255));
			Render::Line(ScreenSpace[BONE_STOMACH], ScreenSpace[BONE_PELVIS], Render::Color(255, 255, 255));
			Render::Line(ScreenSpace[BONE_PELVIS], ScreenSpace[BONE_FEET], Render::Color(255, 255, 255));
		}

		if (Visuals::Distance) {
			float Distanse = Math::FastDist3D(GameEngine->GetCamera()->GetViewTranslation(), pEntity->GetBoneLocationByID(BONE_HEAD));
			auto DistanseFormater = xorstr(L"%d");
			Render::String({ ScreenFeet.x, ScreenFeet.y + 10 }, FC_FORMAT_W(DistanseFormater.crypt_get(), (int)Distanse), true, Render::Color(255, 255, 255));
		}
	}
}

void find_new_target(Entity* pEntity) {

	Vector3 BoneLocation = pEntity->GetBoneLocationByID(BONE_HEAD);

	if (GameEngine->FOV(BoneLocation) > AimBot::FOV)
		return;

	if (AimBot::BestDistance)
		Distance = Math::FastDist3D(GameEngine->GetCamera()->GetViewTranslation(), BoneLocation);

	if (AimBot::CloseToCrosshair) {
		Vector2 ScreenSpace; GameEngine->WorldToScreen(BoneLocation, ScreenSpace);
		Distance = Math::FastDist2D(Vector2(ScreenSpace.x, 0.f), Vector2(Globals::WndSize.x / 2, Globals::WndSize.y / 2));
	}
	
	if (Distance > BestDistance)
		return;

	BestDistance = Distance;
	BestTarget = pEntity;
}

void aimbot() {
	if (!AimBot::Active || !LocalPlayer->GetWeapon())
		return;

	bool KeyPressed = GUI::IO.KeysDown[VK_RBUTTON];

	NewTarget = BestTarget;

	if (AimBot::AutoSwitch || !KeyPressed)
		AimbotTarget = NULL;

	if (AimbotTarget == NULL)
		AimbotTarget = NewTarget;

	if (AimbotTarget && AimbotTarget->GetHealth() > 0) {

		Vector3 AimAngles = Math::CalcAngle(AimbotTarget->GetBoneLocationByID(BONE_HEAD), GameEngine->GetCamera()->GetViewTranslation());
		Math::ClampAngle(AimAngles);
		Vector3 LocalViewAngle = Math::ToEuler(LocalPlayer->GetViewAngle());
		Math::SmoothAngle(LocalViewAngle, AimAngles, AimBot::Smooth);

		if (KeyPressed)
			LocalPlayer->SetViewAngle(Math::ToQuaternion(AimAngles));
	}
}

void hook() {
	TArray<Entity*> EntityList = GameEngine->GetEntities();

	int LocalPlayerTeam = LocalPlayer->GetTeamNumber();

	BestTarget = NULL; BestDistance = 10000.f;
	
	for (uint32_t i = 0; i < GameEngine->GetEntitiesCount(); i++) {
		auto pEntity = EntityList[i];

		if (pEntity == NULL || pEntity == LocalPlayer || LocalPlayerTeam == pEntity->GetTeamNumber() || pEntity->GetHealth() <= 0 )
			continue;

		esp(pEntity);
		
		find_new_target(pEntity);
	}

	aimbot();

	if (Misc::NoRecoil)
		LocalPlayer->NoRecoil();
	if (Misc::NoSpread)
		LocalPlayer->NoSpread();
	if (Misc::NoSpread)
		LocalPlayer->NoFlash();
}

void __fastcall Present(PVOID a1, PVOID a2)
{
	if (Render::NewFrame(Globals::SwapChain, Globals::WndSize)) {
		if (GameEngine->IsInGame()) {
			if (!LocalPlayer)
				LocalPlayer = GameEngine->GetLocalPlayer();
			hook();
		}

		if(Visuals::Crosshair)
			Render::FillCircle(Globals::WndSize / 2.f, Render::Color(255, 255, 0), 2.f);

		if (Visuals::FOV)
			Render::Circle(Globals::WndSize / 2.f, Render::Color(255, 255, 0), AimBot::FOV);

		if (GUI::Visible)
		{
			GUI::Begin(E(L"ORION"), MenuPos, { 300.f, 500.f }, { 0.f, 0.f, 0.f, .75f });
			GUI::Spacing(E(L"Visuals"));
			GUI::CheckBox(E(L"ESP"), Visuals::ESP);
			GUI::CheckBox(E(L"Distance"), Visuals::Distance);
			GUI::CheckBox(E(L"Skeleton"), Visuals::Skeleton);
			GUI::CheckBox(E(L"Head Circle"), Visuals::Head);
			GUI::CheckBox(E(L"Crosshair"), Visuals::Crosshair);
			GUI::CheckBox(E(L"FOV"), Visuals::FOV);

			GUI::Spacing(E(L"Aimbot"));
			GUI::CheckBox(E(L"Active"), AimBot::Active);
			if (GUI::CheckBox(E(L"Best Distance"), AimBot::BestDistance)) 
				AimBot::CloseToCrosshair = false;
			if (GUI::CheckBox(E(L"Close To Crosshair"), AimBot::CloseToCrosshair))
				AimBot::BestDistance = false;

			GUI::CheckBox(E(L"Auto Switch"), AimBot::AutoSwitch);

			GUI::SliderFloat(E(L"Smooth"), &AimBot::Smooth, 1.f, 25.f);
			GUI::SliderFloat(E(L"FOV"), &AimBot::FOV, 5.f, 90.f);

			GUI::Spacing(E(L"Misc"));
			GUI::CheckBox(E(L"No Recoil"), Misc::NoRecoil);
			GUI::CheckBox(E(L"No Spread"), Misc::NoSpread);
			GUI::CheckBox(E(L"No Flash"), Misc::NoFlash);
			
		
			GUI::End();
		}

		Render::EndFrame(Globals::SwapChain);
	}

	static bool init;
	if (!init) {
		Globals::hWnd = *(HWND*)RVA(FindPattern(E("48 89 3D ? ? ? ? 48 89 7B 20")), 7); if (!Globals::hWnd) return;
		DWORD ThID = FC(user32, GetWindowThreadProcessId, Globals::hWnd, NULL);
		FC(user32, SetWindowsHookExA, WH_GETMESSAGE, GUI::WndHook, (HINSTANCE)Globals::BaseAddress, ThID);
		RECT WndRealSize; FC(user32, GetClientRect, Globals::hWnd, &WndRealSize);
		Globals::WndSize = Vector2((float)(WndRealSize.right - WndRealSize.left), (float)(WndRealSize.bottom - WndRealSize.top));
		init = true;
	}


	//ret:
	SpoofCall(PresentOrg, a1, a2);
}

bool DllMain(HMODULE ModBase, DWORD CallReason, PVOID Reserv)
{
	if (CallReason == DLL_PROCESS_ATTACH)
	{
		FC::SetSpoofStub(FindPattern(E("41 FF 27"), E("combase")));
		Globals::BaseAddress = (uint64_t)GetModuleBase(nullptr);
		DWORD64 SwapChain = *(DWORD64*)RVA(FindPattern(E("4C 8B 35 ? ? ? ? 0F 28 7C")), 7);
		SwapChain = *(DWORD64*)(SwapChain + 0x790);
		Globals::SwapChain = *(IDXGISwapChain**)(SwapChain + 0x28);

		BYTE ClassOffset = *(BYTE*)(FindPattern(E("8B ? ? ? ? ? 48 8B 01 48 8D 55"), E("dxgi")) + 2);
		VMT_Hook D3D(E("d3d11"), *(DWORD64*)((DWORD64)Globals::SwapChain + ClassOffset));
		D3D.SetupHook(&PresentOrg, Present, 13);
	} return true;
}
