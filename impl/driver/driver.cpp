#ifndef DRIVER_CPP
#define DRIVER_CPP
#include "driver.hpp"

constexpr bool debug = false;

typedef struct _COPY_MEMORY {
	void* buffer;
	ULONG64		address;
	ULONG		size;
	HANDLE		pid;
	bool		get_pid;
	bool		base;
	bool		peb;
	bool		read;
	bool		write;
	const char* module_name;
	const char* process_name;
}COPY_MEMORY;


auto driver::communicate_t::get_process_pid(
	const std::wstring& proc_name ) -> const std::uint32_t
{
	PROCESSENTRY32 proc_info;
	proc_info.dwSize = sizeof( proc_info );

	HANDLE proc_snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
	if ( proc_snapshot == INVALID_HANDLE_VALUE ) {
		return 0;
	}

	Process32First( proc_snapshot, &proc_info );
	if ( !wcscmp( proc_info.szExeFile, proc_name.c_str( ) ) )
	{
		CloseHandle( proc_snapshot );
		return proc_info.th32ProcessID;
	}

	while ( Process32Next( proc_snapshot, &proc_info ) )
	{
		if ( !wcscmp( proc_info.szExeFile, proc_name.c_str( ) ) )
		{
			CloseHandle( proc_snapshot );
			return proc_info.th32ProcessID;
		}
	}

	CloseHandle( proc_snapshot );
	return 0;
}

auto driver::communicate_t::send_cmd(
	void* data,
	REQUEST_TYPE code ) -> bool
{
	//if ( !data || !code )
	//	return false;

	std::scoped_lock lock( m_mutex );
	REQUEST_DATA req;
	NTSTATUS result;

	req.MaggicCode = &this->magic_code;
	req.Type = code;
	req.Arguments = data;
	req.Status = &result;
	( memcpy ) (this->buffer, &req, sizeof( REQUEST_DATA ));
	( FlushFileBuffers ) (this->m_handle);

	return result;
}

auto driver::communicate_t::attach(
	const std::wstring& proc_name ) -> bool
{

	this->m_handle = ( CreateFileA ) (("\\\\.\\\\vdrvroot"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	if (this->m_handle == INVALID_HANDLE_VALUE) {
		std::printf( " [log] -> invalid handle.\n ");
		return false;
	}
		
	this->buffer = (VirtualAlloc)(0, sizeof(REQUEST_DATA), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	if ( !this->buffer) {
		std::printf(" [log] -> invalid pages.\n ");
		return false;
	}

	UNICODE_STRING RegPath = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\tiktok_live");
	if (!registry::write_object(RegPath, RTL_CONSTANT_STRING(L"TikTok Files"), &this->buffer, REG_QWORD, 8)) {
		std::printf(" [log] -> invalid object.\n ");
		return false;
	}

	DWORD current_pid = (GetCurrentProcessId)();
	if (!registry::write_object(RegPath, RTL_CONSTANT_STRING(L"TikTok Registre"), &current_pid, REG_QWORD, 8)) {
		std::printf(" [log] -> invalid object(2).\n ");
		return false;
	}

	auto old_magic = this->magic_code;
	send_cmd(0, REQUEST_TYPE::INIT);

	if (this->magic_code == old_magic) {

		this->magic_code = (ULONG64)registry::read_object<LONG64>(
			RegPath,
			RTL_CONSTANT_STRING(L"allah")
		);
	}

	auto pid = this->get_process_pid( proc_name.c_str() );
	if ( !pid ) {
		std::printf( "\n [log] -> failed to get process id.\n" );
		std::cin.get( );
	}

	this->m_pid = std::move( pid );

	auto image_base = this->get_image_base( nullptr );
	if ( !image_base ) {
		std::printf( " [log] -> failed to get base address.\n" );
		std::cin.get( );
	}

	this->m_image_base = std::move( image_base );

	std::printf( " [m_pid] -> %i\n", this->m_pid );
	std::printf( " [m_image_base] -> %I64d\n", this->m_image_base );

	return true;
}

auto driver::communicate_t::get_image_base(
	const char* module_name ) -> const std::uintptr_t
{
	REQUEST_MAINBASE req;
	uint64_t base = NULL;
	req.ProcessId = this->m_pid;
	req.OutAddress = ( PBYTE* ) &base;

	send_cmd(
		&req,
		REQUEST_TYPE::MAINBASE 
	);

	return base;
}

auto driver::communicate_t::get_peb() -> uintptr_t
{
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);
	COPY_MEMORY m{};
	m.peb = true;
	m.pid = processHandle;
	send_cmd(&m, REQUEST_TYPE::MAINBASE); // i prob messed this up horribly lmao
	return (uintptr_t)m.buffer;
}
auto driver::communicate_t::write_physical(
	const std::uintptr_t address, 
	void* buffer,
	const std::size_t size ) -> bool
{
	REQUEST_WRITE args;
	args.ProcessId = this->m_pid;

	args.Src = reinterpret_cast<void**>(address);
	args.Dest = buffer;
	args.Size = size;
	args.bPhysicalMem = true;

	auto result = send_cmd( 
		&args,
		REQUEST_TYPE::WRITE 
	);

	return result;
}

auto driver::communicate_t::read_physical(
	const std::uintptr_t address, 
	void* buffer,
	const std::size_t size ) -> bool
{
	REQUEST_READ args { };
	args.ProcessId = this->m_pid;

	args.Src = address;
	args.Dest = buffer;
	args.Size = size;
	args.bPhysicalMem = true;

	auto result = send_cmd(
		&args,
		REQUEST_TYPE::READ
	);

	return result;
}
	 
#endif // ! DRIVER_CPP
