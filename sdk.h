#pragma once

namespace Globals {
	Vector2 WndSize;
	uint64_t BaseAddress;
	IDXGISwapChain* SwapChain;
	HWND hWnd;
}

enum BONE_ID
{
	BONE_HEAD,
	BONE_NECK,
	BONE_HAND,
	BONE_CHEST,
	BONE_STOMACH,
	BONE_PELVIS,
	BONE_FEET
};

#pragma pack(push, 1)
	template<class T> struct TArray
	{
	public:
		T* Data;
		uint64_t Size;

		const T& operator [](uint64_t i)
		{
			return this->Data[i];
		}
	};

class Entity {
public:
	int GetHealth();
	int GetMaxHealth();
	Vector3 GetBoneLocationByID(int id);
	uintptr_t GetWeapon();
	int GetTeamNumber();
	void SetViewAngle(Vector4 Angle);
	Vector4 GetViewAngle();
	void NoRecoil();
	void NoSpread();
	void NoFlash();
};

class Camera {
public:
	Vector3 GetViewRight();
	Vector3 GetViewUp();
	Vector3 GetViewForward();
	Vector3 GetViewTranslation();
	Vector2 GetViewFov();
};

class Engine {
public:
	bool IsInGame();
	uintptr_t GetGameStatus();
	uintptr_t GetGameManager();
	uintptr_t GetProfileManager();
	Entity* GetLocalPlayer();
	TArray<Entity*> GetEntities();
	uint16_t GetEntitiesCount();
	Camera* GetCamera();
	bool WorldToScreen(const Vector3& origin, Vector2& screen);
	float FOV(const Vector3& origin);
};
#pragma pack(pop)

extern Engine* GameEngine;
extern Entity* LocalPlayer;

Vector3 Entity::GetBoneLocationByID(int id)
{
	if (!this) return Vector3();
	return read_chain<Vector3>({ (uint64_t)this, Offsets::ENTITY_SKELETON, Offsets::ENTITY_BONE[id] }, false);
}
int Entity::GetHealth() {
	if (!this) return 0.0f;
	return read_chain<int>({ (uint64_t)this, Offsets::ENTITYINFO, Offsets::MAINCOMPONENT, Offsets::CHILDCOMPONENT, Offsets::ENTITY_HEALTH }, false);
}

int Entity::GetMaxHealth() {
	if (!this) return 0.0f;
	int MaxHealth = read_chain<int>({ (uint64_t)this, Offsets::ENTITYINFO, Offsets::MAINCOMPONENT, Offsets::CHILDCOMPONENT, 0x90, 0x0, 0x48 }, false);
	int ExtraMaxHealth = read_chain<int>({ (uint64_t)this, Offsets::ENTITYINFO, Offsets::MAINCOMPONENT, Offsets::CHILDCOMPONENT, 0x90, 0x0, 0x60 }, false);
	return MaxHealth + (ExtraMaxHealth > 20 ? 0 : 20);
}
uint64_t Entity::GetWeapon()
{
	return read_chain<uint64_t>({ (uint64_t)this, Offsets::WEAPON_CHAIN1, Offsets::WEAPON_CHAIN2 }, false);
}

int Entity::GetTeamNumber()
{
	return read_chain<int>({ (uint64_t)this, Offsets::REPLICATION, Offsets::REPLICATION_TEAM_ID }, false);
}

Vector4 Entity::GetViewAngle() {
	return read_chain<Vector4>({ (uint64_t)this,  Offsets::ENTITY_SKELETON, Offsets::VIEWANGLE_CHAIN2, Offsets::VIEWANGLE_BASIC }, false);
}

void Entity::SetViewAngle(Vector4 Angle) {
	write_chain<Vector4>({ (uint64_t)this,  Offsets::ENTITY_SKELETON, Offsets::VIEWANGLE_CHAIN2, Offsets::VIEWANGLE_BASIC }, Angle, false);
}

void Entity::NoRecoil()
{
	uint64_t Weapon = this->GetWeapon();
	if (!Weapon)
		return;
	write_chain<bool>({ (uint64_t)Weapon, Offsets::CURRENT_WEAPON, Offsets::RECOIL }, 0, false);
	write_chain<float>({ (uint64_t)Weapon, Offsets::CURRENT_WEAPON, Offsets::RECOIL_PITCH }, 0.f, false);
	write_chain<float>({ (uint64_t)Weapon, Offsets::CURRENT_WEAPON, Offsets::RECOIL_YAW }, 0.f, false);

}
void Entity::NoSpread()
{
	uint64_t Weapon = this->GetWeapon();
	if (!Weapon)
		return;

	write_chain<float>({ (uint64_t)Weapon, Offsets::CURRENT_WEAPON, Offsets::SPREAD }, 0.f, false);

}

void Entity::NoFlash()
{
	write_chain<float>({ (uint64_t)LocalPlayer, 0x30, 0x30, 0x28, 0x40 }, 0.f, false);
}

Vector3 Camera::GetViewRight()
{
	if (!this) return Vector3();
	return safe_read<Vector3>((uint64_t)this + Offsets::VIEWRIGHT);
}
Vector3 Camera::GetViewUp()
{
	if (!this) return Vector3();
	return safe_read<Vector3>((uint64_t)this + Offsets::VIEWUP);
}
Vector3 Camera::GetViewForward()
{
	if (!this) return Vector3();
	return safe_read<Vector3>((uint64_t)this + Offsets::VIEWFORWARD);
}
Vector3 Camera::GetViewTranslation()
{
	if (!this) return Vector3();
	return safe_read<Vector3>((uintptr_t)this + Offsets::VIEWTRANSLATION);
}
Vector2 Camera::GetViewFov()
{
	if (!this) return Vector2();
	Vector2 fov = safe_read<Vector2>((uintptr_t)this + Offsets::VIEWFOV);
	return { abs(fov.x), abs(fov.y) };
}

uint64_t Engine::GetGameManager()
{
	return safe_read<uint64_t>(Globals::BaseAddress + Offsets::GAME_MANAGER);
}

uint64_t Engine::GetGameStatus()
{
	return safe_read<uint64_t>(Globals::BaseAddress + Offsets::STATUS_MANAGER);
}

uint64_t Engine::GetProfileManager()
{
	return safe_read<uint64_t>(Globals::BaseAddress + Offsets::PROFILE_MANAGER);
}

bool Engine::IsInGame()
{
	return safe_read<bool>(this->GetGameStatus() + Offsets::IS_IN_GAME);
}

Entity* Engine::GetLocalPlayer()
{
	return read_chain<Entity*>({ this->GetProfileManager(), Offsets::PROFILE_MANAGER_CONTAINER, 0x0, Offsets::ENTITYINFO }, false);
}

TArray<Entity*> Engine::GetEntities()
{
	return safe_read<TArray<Entity*>>(this->GetGameManager() + Offsets::ENTITY_LIST);
}

uint16_t Engine::GetEntitiesCount()
{
	return safe_read<uint16_t>(this->GetGameManager() + Offsets::ENTITY_COUNT);
}

Camera* Engine::GetCamera()
{
	return read_chain<Camera*>({ this->GetProfileManager(), Offsets::GAME_PROFILE_CHAIN1, Offsets::GAME_PROFILE_CHAIN2, Offsets::GAME_PROFILE_CHAIN3 }, false);
}

bool Engine::WorldToScreen(const Vector3& origin, Vector2& screen)
{
	auto Camera = GameEngine->GetCamera();

	if (!Camera)
		return false;

	Vector3 Temp = origin - Camera->GetViewTranslation();
	float x = Math::Dot(Temp, Camera->GetViewRight());
	float y = Math::Dot(Temp, Camera->GetViewUp());
	float z = Math::Dot(Temp, Camera->GetViewForward() * -1);

	screen.x = (Globals::WndSize.x / 2) * (1 + x / Camera->GetViewFov().x / z);
	screen.y = (Globals::WndSize.y / 2) * (1 - y / Camera->GetViewFov().y / z);
	return z >= 1.0f;
}

float Engine::FOV(const Vector3& Src) {
	Vector2 Out; this->WorldToScreen(Src, Out);
	return (abs(Out.x - (Globals::WndSize.x / 2)) + abs(Out.y - (Globals::WndSize.y / 2)));
}






