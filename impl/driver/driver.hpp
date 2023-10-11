#ifndef DRIVER_HPP
#define DRIVER_HPP
#define device_name "\\\\.\\{06be76d0-1c06-11ee-be56-0242ac120002}"
// source 
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <stdio.h>
#include <string_view>
#include <iostream>
#include <chrono>
#include <winternl.h>
#include <ntstatus.h>
#include <atomic>
#include <mutex>
#include <dwmapi.h>
#include <xmmintrin.h>
#include <wchar.h>

constexpr auto mm_copy_memory_physical = 0x1;
constexpr auto mm_copy_memory_virtual = 0x2;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
	KeyValueBasicInformation,
	KeyValueFullInformation,
	KeyValuePartialInformation,
	KeyValueFullInformationAlign64,
	KeyValuePartialInformationAlign64,
	KeyValueLayerInformation,
	MaxKeyValueInfoClass  // MaxKeyValueInfoClass should always be the last enum
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_VALUE_FULL_INFORMATION {
	ULONG   TitleIndex;
	ULONG   Type;
	ULONG   DataOffset;
	ULONG   DataLength;
	ULONG   NameLength;
	WCHAR   Name [ 1 ];            // Variable size
	//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, * PKEY_VALUE_FULL_INFORMATION;


#ifdef __cplusplus
extern "C++"
{
	char _RTL_CONSTANT_STRING_type_check( const char* s );
	char _RTL_CONSTANT_STRING_type_check( const WCHAR* s );
	// __typeof would be desirable here instead of sizeof.
	template <size_t N> class _RTL_CONSTANT_STRING_remove_const_template_class;
template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof( char )> { public: typedef  char T; };
template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof( WCHAR )> { public: typedef WCHAR T; };
#define _RTL_CONSTANT_STRING_remove_const_macro(s) \
    (const_cast<_RTL_CONSTANT_STRING_remove_const_template_class<sizeof((s)[0])>::T*>(s))
}
#else
char _RTL_CONSTANT_STRING_type_check( const void* s );
#define _RTL_CONSTANT_STRING_remove_const_macro(s) (s)
#endif
#define RTL_CONSTANT_STRING(s) \
{ \
    sizeof( s ) - sizeof( (s)[0] ), \
    sizeof( s ) / sizeof(_RTL_CONSTANT_STRING_type_check(s)), \
    _RTL_CONSTANT_STRING_remove_const_macro(s) \
}

extern "C" {
	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwQueryValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_writes_bytes_to_opt_( Length, *ResultLength ) PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwClose(
			_In_ HANDLE Handle
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwOpenKey(
			_Out_ PHANDLE KeyHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwQueryValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_writes_bytes_to_opt_( Length, *ResultLength ) PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwSetValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_opt_ ULONG TitleIndex,
			_In_ ULONG Type,
			_In_reads_bytes_opt_( DataSize ) PVOID Data,
			_In_ ULONG DataSize
		);

	NTSYSAPI NTSTATUS ZwCreateKey(
		PHANDLE            KeyHandle,
		ACCESS_MASK        DesiredAccess,
		POBJECT_ATTRIBUTES ObjectAttributes,
		ULONG              TitleIndex,
		PUNICODE_STRING    Class,
		ULONG              CreateOptions,
		PULONG             Disposition
	);
}

namespace registry
{
	inline ULONG get_key( HANDLE hKey, PUNICODE_STRING Key )
	{
		NTSTATUS Status;
		ULONG KeySize;

		Status = ( ZwQueryValueKey ) (hKey, Key, KeyValueFullInformation, 0, 0, &KeySize);

		if ( Status == STATUS_BUFFER_TOO_SMALL || Status == STATUS_BUFFER_OVERFLOW )
			return KeySize;

		return 0;
	}

	template <typename type>
	inline type read_object( UNICODE_STRING RegPath, UNICODE_STRING Key )
	{
		HANDLE hKey;
		OBJECT_ATTRIBUTES ObjAttr;
		NTSTATUS Status = STATUS_UNSUCCESSFUL;

		InitializeObjectAttributes( &ObjAttr, &RegPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );

		Status = ( ZwOpenKey ) (&hKey, KEY_ALL_ACCESS, &ObjAttr);

		if ( NT_SUCCESS( Status ) )
		{
			ULONG KeyInfoSize = get_key( hKey, &Key );
			ULONG KeyInfoSizeNeeded;

			if ( KeyInfoSize == NULL )
			{
				( ZwClose ) (hKey);
				return 0;
			}

			PKEY_VALUE_FULL_INFORMATION pKeyInfo = ( PKEY_VALUE_FULL_INFORMATION ) malloc( KeyInfoSize );
			RtlZeroMemory( pKeyInfo, KeyInfoSize );

			Status = ( ZwQueryValueKey ) (hKey, &Key, KeyValueFullInformation, pKeyInfo, KeyInfoSize, &KeyInfoSizeNeeded);

			if ( !NT_SUCCESS( Status ) || (KeyInfoSize != KeyInfoSizeNeeded) )
			{
				( ZwClose ) (hKey);
				( free ) (pKeyInfo);
				return 0;
			}

			( ZwClose ) (hKey);
			( free ) (pKeyInfo);

			return *( type* ) (( LONG64 ) pKeyInfo + pKeyInfo->DataOffset);
		}

		return 0;
	}

	inline bool write_object( UNICODE_STRING RegPath, UNICODE_STRING Key, PVOID Address, ULONG Type, ULONG Size )
	{
		bool Success = false;
		HANDLE hKey;
		OBJECT_ATTRIBUTES ObjAttr;
		NTSTATUS Status = STATUS_UNSUCCESSFUL;

		InitializeObjectAttributes( &ObjAttr, &RegPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );

		Status = ( ZwOpenKey ) (&hKey, KEY_ALL_ACCESS, &ObjAttr);

		if ( NT_SUCCESS( Status ) )
		{
			Status = ( ZwSetValueKey ) (hKey, &Key, NULL, Type, Address, Size);

			if ( NT_SUCCESS( Status ) )
				Success = true;

			( ZwClose ) (hKey);
		}
		else {
			Status = ( ZwCreateKey ) (&hKey, KEY_ALL_ACCESS, &ObjAttr, 0, &RegPath, 0, 0);

			if ( NT_SUCCESS( Status ) )
			{
				Status = ( ZwSetValueKey ) (hKey, &Key, NULL, Type, Address, Size);

				if ( NT_SUCCESS( Status ) )
					Success = true;
			}
			( ZwClose ) (hKey);
		}

		return Success;
	}
}

namespace driver
{
	class communicate_t
	{

		std::mutex m_mutex { };

		void* buffer = { nullptr };
		HANDLE m_handle;

		std::int32_t m_pid = 0;
		std::uintptr_t magic_code = 0x8749643124D9463;

		typedef enum _REQUEST_TYPE : UINT {
			WRITE,
			READ,
			PROTECT,
			ALLOC,
			FREE,
			MODULE,
			MAINBASE,
			THREADCALL,
			INIT = 99,
		} REQUEST_TYPE;

		typedef struct _REQUEST_DATA {
			ULONG64* MaggicCode;
			UINT Type;
			PVOID Arguments;
			NTSTATUS* Status;
		} REQUEST_DATA, * PREQUEST_DATA;

		typedef struct _REQUEST_WRITE {
			DWORD ProcessId;
			PVOID Dest;
			PVOID Src;
			DWORD Size;
			BOOL bPhysicalMem;
		} REQUEST_WRITE, * PREQUEST_WRITE;

		typedef struct _REQUEST_READ {
			DWORD ProcessId;
			void* Dest;
			uint64_t Src;
			uint32_t Size;
			BOOL bPhysicalMem;
		} REQUEST_READ, * PREQUEST_READ;

		typedef struct _REQUEST_PROTECT {
			DWORD ProcessId;
			PVOID Address;
			DWORD Size;
			PDWORD InOutProtect;
		} REQUEST_PROTECT, * PREQUEST_PROTECT;

		typedef struct _REQUEST_ALLOC {
			DWORD ProcessId;
			PVOID OutAddress;
			DWORD Size;
			DWORD Protect;
		} REQUEST_ALLOC, * PREQUEST_ALLOC;

		typedef struct _REQUEST_FREE {
			DWORD ProcessId;
			PVOID Address;
		} REQUEST_FREE, * PREQUEST_FREE;

		typedef struct _REQUEST_MODULE {
			DWORD ProcessId;
			WCHAR Module [ 0xFF ];
			PBYTE* OutAddress;
			DWORD* OutSize;
		} REQUEST_MODULE, * PREQUEST_MODULE;

		typedef struct _REQUEST_MAINBASE {
			DWORD ProcessId;
			PBYTE* OutAddress;
		} REQUEST_MAINBASE, * PREQUEST_MAINBASE;
		
	public:
		
		std::uintptr_t m_image_base = 0;

		[[nodiscard]] bool send_cmd( void* data, REQUEST_TYPE code );
		[[nodiscard]] uintptr_t get_peb();
		[[nodiscard]] bool attach( const std::wstring& proc_name );
		[[nodiscard]] const std::uintptr_t get_image_base( const char* module_name );
		[[nodiscard]] const std::uint32_t get_process_pid( const std::wstring& proc_name );

		[[nodiscard]] bool read_physical( const uintptr_t address, void* buffer, const size_t size );
		[[nodiscard]] bool write_physical( const uintptr_t address, void* buffer, const size_t size );

		template <typename t>
		[[nodiscard]] auto write( const uintptr_t address, t value ) -> bool
		{
			return write_physical( address, &value, sizeof( t ) );
		}

		template <typename t>
		[[nodiscard]] auto read( const uintptr_t address ) -> t
		{
			t response {};
			read_physical( address, &response, sizeof( t ) );
			return response;
		}

		template <typename t>
		[[nodiscard]] auto read_array( const uintptr_t address, t buffer, size_t size ) -> bool
		{
			if ( !address )
				return false;

			return read_physical( address, buffer, size );
		}

	};
}

inline driver::communicate_t* request = new driver::communicate_t( );

#endif // ! DRIVER_HPP