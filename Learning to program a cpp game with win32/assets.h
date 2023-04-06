#pragma once

#include <map>
#include <string>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

std::map<std::string, LoadedSprite> resources;

void loadResources(HDC hdc) {
	for (const fs::directory_entry& dirEntry : fs::recursive_directory_iterator("resources")) {
		std::cout << dirEntry.path();
		LoadedSprite sprite = loadSprite(hdc, dirEntry.path().string().c_str());
		resources.insert(std::pair<std::string, LoadedSprite>(dirEntry.path().filename().string(), sprite));
		MessageBox(NULL, "Asset loaded.", "Asset Loaded", MB_ICONINFORMATION);
	}
}

void initResources(HDC hdc) {
	if (fs::exists("resources")) {
		loadResources(hdc);
	} else {
		fs::create_directory(fs::path{"resources"});
	}
}




