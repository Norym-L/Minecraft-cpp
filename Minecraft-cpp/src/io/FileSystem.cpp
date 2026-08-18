#include "FileSystem.h"

#include <iostream>

// TODO 文件路径 debug用
#define ASSETS_ROOT "C:/dev/Minecraft-cpp/Minecraft-cpp/assets"

#if defined _WIN32
#include <Windows.h>
#elif defined (__APPLE__)
#include <mach-o/dyld.h>
#elif defined (__Linux__)
#include <unistd.h>
#include <limits.h>
#endif

#include <fstream>

namespace mymc
{
	std::filesystem::path FileSystem::GetExecutableFolder()
	{
#if defined _WIN32
		wchar_t buf[MAX_PATH];
		GetModuleFileNameW(nullptr, buf, MAX_PATH);
		return std::filesystem::path(buf).remove_filename();
#elif defined (__APPLE__)
		uint32_t size = 0;
		_NSGetExecutablePath(nullptr, &size);
		std::string tmp(size, '\0');
		_NSGetExecutablePath(tmp.data(), &size);
		return std::filesystem::weakly_canonical(std::filesystem::path(tmp)).remove_filename();
#elif defined (__Linux__)
		return std::filesystem::weakly_canonical(std::filesystem::read_symlink("/proc/self/exe")).remove_filename();
#else
		return std::filesystem::current_path();
#endif

	}

	std::filesystem::path FileSystem::GetAssetsFolder()
	{
#if defined (ASSETS_ROOT)
		auto path = std::filesystem::path(std::string(ASSETS_ROOT));
		if (std::filesystem::exists(path))
		{
			return path;
		}
#else
		return std::filesystem::weakly_canonical(GetExecutableFolder() / "assets");
#endif
		std::cerr << "IO: error file_path" << std::endl;
		return std::filesystem::path(nullptr);
	}

	std::vector<char> FileSystem::LoadFile(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			return {};
		}

		const auto size = file.tellg();
		file.seekg(0);

		std::vector<char> buffer(size);

		if (!file.read(buffer.data(), size))
		{
			return {};
		}

		return buffer;
	}

	std::vector<char> FileSystem::LoadAssetFile(const std::string& relative_path)
	{
		return LoadFile(GetAssetsFolder() / relative_path);
	}

	std::string FileSystem::LoadAssetFileText(const std::string& relative_path)
	{
		auto buffer = LoadAssetFile(relative_path);
		return {buffer.begin(), buffer.end()};
	}
}