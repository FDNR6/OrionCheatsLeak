#pragma once

template <typename T>
__forceinline T safe_read(uint64_t Addr) {
	return (((Addr > 0x400000) && ((Addr + sizeof(T)) < 0x00007FFFFFFF0000)) ? *(T*)Addr : T{});
}

template <typename T>
__forceinline bool safe_write(uint64_t Addr, T Data) {
	if ((Addr > 0x400000) && ((Addr + sizeof(T)) < 0x00007FFFFFFF0000)) { *(T*)Addr = Data; return true; } return false;
}

template <typename T> 
__forceinline T read_chain(std::initializer_list<uint64_t> Offsets, bool ReadFirstOffset = true) {
	auto Start = Offsets.begin(); size_t ReadSize = Offsets.size();
	uint64_t LastPtr = safe_read<uint64_t>((ReadFirstOffset ? safe_read<uint64_t>(Start[0]) : Start[0]) + Start[1]);
	for (size_t i = 2; i < ReadSize - 1; i++) if (!(LastPtr = safe_read<uint64_t>(LastPtr + Start[i]))) return T{};
	return safe_read<T>(LastPtr + Start[ReadSize - 1]);
}

template <typename T>
__forceinline bool write_chain(std::initializer_list<uint64_t> Offsets, T Value, bool ReadFirstOffset = true) {
	auto Start = Offsets.begin(); size_t ReadSize = Offsets.size();
	uint64_t LastPtr = safe_read<uint64_t>((ReadFirstOffset ? safe_read<uint64_t>(Start[0]) : Start[0]) + Start[1]);
	for (size_t i = 2; i < ReadSize - 1; i++) if (!(LastPtr = safe_read<uint64_t>(LastPtr + Start[i]))) return FALSE;
	return safe_write(LastPtr + Start[ReadSize - 1], Value);
}

namespace Offsets {
	uint64_t GAME_MANAGER = 0x53C1670;
	uint64_t CAMERA_MANAGER = 0x52B20B0;
	uint64_t PROFILE_MANAGER = 0x53AE810;
	uint64_t STATUS_MANAGER = 0x5F4CFE8;

	uint64_t GAME_PROFILE_CHAIN1 = 0x78;
	uint64_t GAME_PROFILE_CHAIN2 = 0x0;
	uint64_t GAME_PROFILE_CHAIN3 = 0x130;

	uint64_t VIEWRIGHT = 0x1C0;
	uint64_t VIEWUP = 0x1D0;
	uint64_t VIEWFORWARD = 0x1E0;
	uint64_t VIEWTRANSLATION = 0x1F0;
	uint64_t VIEWFOV = 0x380;

	uint64_t ENTITY_LIST = 0x1C8;
	uint64_t ENTITY_COUNT = 0x1D0;
	uint64_t ENTITY_SKELETON = 0x20; 
	uint64_t ENTITY_BONE[] = { /*head*/ 0x670, /*neck*/ 0xF70, /*hand*/ 0x6A0, /*chest*/ 0xFB0,  /*stomach*/ 0xF90, /*pelvis*/ 0xFD0, /*feet*/ 0x6C0 };

	uint64_t ENTITYINFO = 0x28;
	uint64_t MAINCOMPONENT = 0xD8; 
	uint64_t CHILDCOMPONENT = 0x8; 
	uint64_t ENTITY_HEALTH = 0x148;

	uint64_t REPLICATION = 0xA8;
	uint64_t REPLICATION_NAME = 0x1C8;
	uint64_t REPLICATION_TEAM_ID = 0x1A8;

	uint64_t IS_IN_GAME = 0x870;

	uint64_t PROFILE_MANAGER_CONTAINER = 0x68;

	uint64_t WEAPON_CHAIN1 = 0x78;
	uint64_t WEAPON_CHAIN2 = 0xC8;

	uint64_t CURRENT_WEAPON = 0x208;

	uint64_t SPREAD = 0x50;

	uint64_t RECOIL = 0x168;
	uint64_t RECOIL_YAW = 0x14C;
	uint64_t RECOIL_PITCH = 0x15C;

	uint64_t VIEWANGLE_CHAIN2 = 0x11A0;
	uint64_t VIEWANGLE_BASIC = 0xC0;
	//uint64_t VIEWANGLE_SILENT = 0x134;
}
