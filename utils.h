typedef struct LDR_DATA_TABLE_ENTRY_FIX {
	PVOID Reserved1[2];
	LIST_ENTRY InMemoryOrderLinks;
	PVOID Reserved2[2];
	PVOID DllBase;
	PVOID Reserved3[2];
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	PVOID Reserved5[2];
#pragma warning(push)
#pragma warning(disable: 4201)
	union {
		ULONG CheckSum;
		PVOID Reserved6;
	} DUMMYUNIONNAME;
#pragma warning(pop)
	ULONG TimeDateStamp;
};

//FACE Utils v4
namespace FC
{
#define RVA(Instr, InstrSize) ((DWORD64)Instr + InstrSize + *(LONG*)((DWORD64)Instr + (InstrSize - sizeof(LONG))))
#define ConstStrLen(Str) ((sizeof(Str) - sizeof(Str[0])) / sizeof(Str[0]))
#define ToLower(Char) ((Char >= 'A' && Char <= 'Z') ? (Char + 32) : Char)

//StrCompare (with StrInStrI(Two = false))
	template <typename StrType, typename StrType2>
	bool StrCmp(StrType Str, StrType2 InStr, bool Two) {
		if (!Str || !InStr) return false;
		wchar_t c1, c2; do {
			c1 = *Str++; c2 = *InStr++;
			c1 = ToLower(c1); c2 = ToLower(c2);
			if (!c1 && (Two ? !c2 : 1)) return true;
		} while (c1 == c2); return false;
	}

	//CRC16 StrHash	
	template <typename StrType> __declspec(noinline) constexpr unsigned short HashStr(StrType Data, int Len) {
		unsigned short CRC = 0xFFFF; while (Len--) {
			auto CurChar = *Data++; if (!CurChar) break;
			CRC ^= ToLower(CurChar) << 8; for (int i = 0; i < 8; i++)
				CRC = CRC & 0x8000 ? (CRC << 1) ^ 0x8408 : CRC << 1;
		} return CRC;
	}
#define ConstHashStr(Str) [](){ constexpr unsigned short CRC = FC::HashStr(Str, ConstStrLen(Str)); return CRC; }()

	//EncryptDecryptPointer
	template <typename PtrType>
	__forceinline PtrType EPtr(PtrType Ptr) {
		typedef union { struct { USHORT Key1; USHORT Key2; USHORT Key3; USHORT Key4; }; ULONG64 Key; } CryptData;
		CryptData Key{ ConstHashStr(__TIME__), ConstHashStr(__DATE__), ConstHashStr(__TIMESTAMP__), ConstHashStr(__TIMESTAMP__) };
		return (PtrType)((DWORD64)Ptr ^ Key.Key);
	}
#define EPtr(Ptr) FC::EPtr(Ptr)

	//GetModuleBase
	template <typename StrType>
	PBYTE GetModuleBase_Wrapper(StrType ModuleName) {
		PPEB_LDR_DATA Ldr = ((PTEB)__readgsqword(FIELD_OFFSET(NT_TIB, Self)))->ProcessEnvironmentBlock->Ldr; void* ModBase = nullptr;
		for (PLIST_ENTRY CurEnt = Ldr->InMemoryOrderModuleList.Flink; CurEnt != &Ldr->InMemoryOrderModuleList; CurEnt = CurEnt->Flink) {
			LDR_DATA_TABLE_ENTRY_FIX* pEntry = CONTAINING_RECORD(CurEnt, LDR_DATA_TABLE_ENTRY_FIX, InMemoryOrderLinks);
			if (!ModuleName || StrCmp(ModuleName, pEntry->BaseDllName.Buffer, false)) return (PBYTE)pEntry->DllBase;
		} return nullptr;
	}
#define GetModuleBase FC::GetModuleBase_Wrapper

	//Signature Scan
	PBYTE FindPattern_Wrapper(const char* Pattern, const char* Module = nullptr)
	{
		//find pattern utils
#define InRange(x, a, b) (x >= a && x <= b) 
#define GetBits(x) (InRange(x, '0', '9') ? (x - '0') : ((x - 'A') + 0xA))
#define GetByte(x) ((BYTE)(GetBits(x[0]) << 4 | GetBits(x[1])))

//get module range
		PBYTE ModuleStart = (PBYTE)GetModuleBase_Wrapper(Module); if (!ModuleStart) return nullptr;
		PIMAGE_NT_HEADERS NtHeader = ((PIMAGE_NT_HEADERS)(ModuleStart + ((PIMAGE_DOS_HEADER)ModuleStart)->e_lfanew));
		PBYTE ModuleEnd = (PBYTE)(ModuleStart + NtHeader->OptionalHeader.SizeOfImage - 0x1000); ModuleStart += 0x1000;

		//scan pattern main
		PBYTE FirstMatch = nullptr;
		const char* CurPatt = Pattern;
		for (; ModuleStart < ModuleEnd; ++ModuleStart)
		{
			bool SkipByte = (*CurPatt == '\?');
			if (SkipByte || *ModuleStart == GetByte(CurPatt)) {
				if (!FirstMatch) FirstMatch = ModuleStart;
				SkipByte ? CurPatt += 2 : CurPatt += 3;
				if (CurPatt[-1] == 0) return FirstMatch;
			}

			else if (FirstMatch) {
				ModuleStart = FirstMatch;
				FirstMatch = nullptr;
				CurPatt = Pattern;
			}
		}

		return nullptr;
	}
#define FindPattern FC::FindPattern_Wrapper

	//Spoof Return Address
#pragma section(".text") 
	PVOID ReturnAddressStubEnc = 0;
	__declspec(allocate(".text")) const unsigned char SpooferRetBytes[] = {
		0x41, 0x5B, 0x90, 0x49, 0x0F, 0xA3, 0xDA, 0x45, 0x0F, 0x42, 0xD5, 0x48,
		0x83, 0xC4, 0x08, 0x48, 0x90, 0x48, 0x8B, 0x44, 0x24, 0x18, 0x4D, 0x63,
		0xD5, 0x4C, 0x8B, 0x10, 0x4D, 0x87, 0xD2, 0x4C, 0x89, 0x14, 0x24, 0x4C,
		0x8B, 0x50, 0x08, 0x4C, 0x89, 0x58, 0x08, 0x4D, 0x87, 0xDB, 0x4C, 0x89,
		0x78, 0x10, 0x66, 0x41, 0xF7, 0xD7, 0x66, 0x44, 0x0F, 0xBE, 0xF9, 0x45,
		0x0F, 0xBF, 0xF8, 0x4C, 0x8D, 0x3D, 0x15, 0x00, 0x00, 0x00, 0x4C, 0x89,
		0x38, 0x49, 0xF7, 0xD7, 0x66, 0x41, 0xBF, 0x73, 0x51, 0x4C, 0x8B, 0xF8,
		0x4D, 0x87, 0xFF, 0x90, 0x41, 0xFF, 0xE2, 0x48, 0x83, 0xEC, 0x10, 0xF6,
		0xD1, 0x49, 0x8B, 0xCF, 0x4C, 0x0F, 0xB7, 0xFF, 0x41, 0xBF, 0xF5, 0x55,
		0x46, 0x7E, 0x4C, 0x8B, 0x79, 0x10, 0x90, 0xFF, 0x71, 0x08, 0xC3
	};
	template<typename Ret = void, typename First = void*, typename Second = void*, typename Third = void*, typename Fourth = void*, typename... Stack>
	__forceinline Ret SpoofCall(void* Func, First a1 = First{}, Second a2 = Second{}, Third a3 = Third{}, Fourth a4 = Fourth{}, Stack... args) {
		struct shell_params { const void* v0; void* v1; void* v2; };
		shell_params ShellData = { (PVOID)EPtr(ReturnAddressStubEnc), Func }; typedef Ret(*ShellFn)(...);
		return ((ShellFn)&SpooferRetBytes)(a1, a2, a3, a4, &ShellData, nullptr, args...);
	}
	__forceinline void SetSpoofStub(PVOID R15_Stub) { ReturnAddressStubEnc = EPtr(R15_Stub); }
#define SpoofCall FC::SpoofCall

	//Spoof VFTable Return Address
	template<typename Ret = void, typename... Args>
	__forceinline Ret VCall(PVOID Class, int Index, Args... AddArgs) {
		return SpoofCall<Ret>((*(PVOID**)Class)[Index], Class, AddArgs...);
	}
#define VCall FC::VCall

	//Spoof Call Export Return Address
	PVOID GetExportAddress(PBYTE hDll, const char* Name)
	{
		//process image data
		PIMAGE_NT_HEADERS NT_Head = (PIMAGE_NT_HEADERS)(hDll + ((PIMAGE_DOS_HEADER)hDll)->e_lfanew);
		PIMAGE_EXPORT_DIRECTORY ExportDir = (PIMAGE_EXPORT_DIRECTORY)(hDll + NT_Head->OptionalHeader.DataDirectory[0].VirtualAddress);

		//process list
		for (DWORD i = 0; i < ExportDir->NumberOfNames; i++)
		{
			USHORT Ordinal = ((USHORT*)(hDll + ExportDir->AddressOfNameOrdinals))[i];
			const char* ExpName = (const char*)hDll + ((DWORD*)(hDll + ExportDir->AddressOfNames))[i];
			if (StrCmp(Name, ExpName, true))
				return (PVOID)(hDll + ((DWORD*)(hDll + ExportDir->AddressOfFunctions))[Ordinal]);
		} return nullptr;
	}
#define FC(Mod, Name, ...) [&](){ static PVOID FAddr = nullptr; \
		if (!FAddr) FAddr = EPtr(FC::GetExportAddress(FC::GetModuleBase_Wrapper(E(#Mod)), E(#Name))); \
		return SpoofCall<decltype(Name(__VA_ARGS__))>(EPtr(FAddr), __VA_ARGS__); \
	}()
}


#define FC_FORMAT_A(...) [&](){wchar_t Str[64];FC(ntdll, _swprintf, Str, __VA_ARGS__);return Str;}()
#define FC_FORMAT_W(...) [&](){wchar_t Str[64];FC(ntdll, _swprintf, Str, __VA_ARGS__);return Str;}()
#define FC_WCHAR(a) [&](){wchar_t Str[64];for(int i=0;;i++){if(!(Str[i]=a[i]))break;}return Str;}()
#define FC_CHAR(a) [&](){char Str[64];for(int i=0;;i++){if(!(Str[i]=a[i]))break;}return Str;}()

#ifdef DEBUG
template<typename... Args>
void Print(const char* Format, Args... args)
{
	static bool Once = false;
	if (!Once) {
		FC(kernel32, AllocConsole);
		Once = true;
	}

	char Buff[128];
	FC(ntdll, sprintf, Buff, Format, args...);
	int StrLengthOut = FC(ntdll, strlen, Buff);
	HANDLE Out = FC(kernel32, GetStdHandle, STD_OUTPUT_HANDLE);
	FC(kernel32, WriteConsoleA, Out, Buff, StrLengthOut, NULL, NULL);
}
#define dp(a) Print(E("DEC (%d): %d\n"), __LINE__, (a))
#define hp(a) Print(E("HEX (%d): %p\n"), __LINE__, (a))
#define sp(a) Print(E("STR (%d): %s\n"), __LINE__, (a))
#define wsp(a) Print(E("STR (%d): %ws\n"), __LINE__, (a))
#define fp(a) Print(E("FLOAT (%d): %d.%d\n"), __LINE__, (int)(a), (int)(((a) - (int)(a)) * 1000000));
#define v2p(a) { fp(a.x); fp(a.y); }
#define v3p(a) { fp(a.x); fp(a.y); fp(a.z); }
#endif

//FACE VMT Hook v4
int VMT_FilledRecords = 0;
#pragma comment(linker,"/SECTION:.hkdata,RW")
#pragma bss_seg(push, ".hkdata")
PVOID VMTHookData[10000];
#pragma bss_seg(pop)
class VMT_Hook
{
private:
	PVOID* TblEnc = nullptr; DWORD Size = 0;
	__forceinline bool InExecMemory(PVOID Addr, DWORD64 ModBase, PIMAGE_NT_HEADERS NtHeader) {
		PIMAGE_SECTION_HEADER Sect = IMAGE_FIRST_SECTION(NtHeader);
		for (DWORD Cnt = 0; Cnt < NtHeader->FileHeader.NumberOfSections; Cnt++, Sect++) {
			if ((Sect->Characteristics & IMAGE_SCN_MEM_EXECUTE) &&
				((DWORD64)Addr >= (ModBase + Sect->VirtualAddress)) &&
				((DWORD64)Addr < (ModBase + Sect->VirtualAddress + Sect->Misc.VirtualSize)))
				return true;
		} return false;
	}

public:
	template <typename T>
	VMT_Hook(const char* Module, T Class) {
		DWORD64 ModBase = (DWORD64)GetModuleBase(Module); Size = 0; PVOID* VFTable = *(PVOID**)Class;
		PIMAGE_NT_HEADERS NtHeader = (PIMAGE_NT_HEADERS)(ModBase + ((PIMAGE_DOS_HEADER)ModBase)->e_lfanew);
		while (InExecMemory(VFTable[Size], ModBase, NtHeader)) Size++;
		PVOID* VFT_New = (PVOID*)&VMTHookData[VMT_FilledRecords];
		__movsq((PDWORD64)VFT_New, (const PDWORD64)VFTable, Size);
		*(PVOID**)Class = VFT_New; TblEnc = (PVOID*)EPtr(VFT_New);
		VMT_FilledRecords += Size;
	}

	template <typename T>
	__forceinline void SetupHook(T* Orginal, PVOID Hook, int Index) {
		PVOID* Tbl = (PVOID*)EPtr(TblEnc);
		*Orginal = (T)Tbl[Index];
		Tbl[Index] = Hook;
	}
};
