# Hitman Absolution Editor

A powerful C++23 desktop editor for modifying **Hitman: Absolution** game resources. This tool provides a UI for inspecting, extracting, and modifying in-game assets directly from the engine's `.pc_reslib` and `.pc_headerlib` files. 

*Language:* [English](README.md) | [Українська](README_ua.md)

## Screenshots

![Hierarchy](docs/images/image5.png)
![Audio Patching](docs/images/image4.png)
![Localization Editing](docs/images/image3.png)
![Text List Editor](docs/images/image2.png)
![Fonts SWF Editing](docs/images/image1.png)

## Features & Improvements

This fork introduces significant improvements over the original release, resolving many critical errors and optimizing the core architecture.

**Key Additions & Fixes:**
* **Crash Fixes**: Resolved critical crashes when previewing audio files directly in the editor, allowing seamless playback and inspection.
* **Texture Viewer**: Restored and fixed the ability to view `.dds` textures natively within the editor.
* **App Icon**: Added a proper application icon.
* **Optimized Rebuilding**: Safely patches resource libraries using a full rebuild approach to prevent file corruption.
* **Automated UI/UX Fixes**: Console output has been refined and UI copy/paste issues fixed.

## Patching Notes & Recommendations

When patching resources back into the game, please follow these strict formatting rules to prevent game crashes:

### 🔊 Audio Files (`.wavb` / `.fsbs`)
* When preparing custom sounds to patch into the game, ensure your audio file is encoded as **`.ogg`**.
* The file **must** have the following parameters: **48000 Hz** sample rate, **Mono** channel.

### 🔤 Fonts (`.swf`)
* Font resources are stored as Flash Movie (`.swf`) files.
* **Recommendation:** Export the original font resource to disk, open it using [JPEXS Free Flash Decompiler](https://github.com/jindrapetrik/jpexs-decompiler), make your typography changes, save the new `.swf`, and then patch it back to the game.

### 🌐 Localization & Text Lists (`.locm` / `.locr` / `.teli`)
* Subtitles, UI strings, and localization texts can be modified in two ways:
  1. **Directly in the Editor:** Double-click the text entries in the UI to change them and hit "Patch Back To Game".
  2. **JSON Export/Import:** Right-click the `.textlist` file, choose "Export Resource" to save a `.json` file, edit it externally, and use "Import JSON (Replace Localization)" to apply it back.

## Notice & Fork Rights

**Disclaimer**: This project is a modified fork of the original Hitman Absolution Editor. The original author did not provide a software license (e.g., GPL, MIT) in the base repository. All original rights belong to the initial creator. The modifications made in this fork are provided as-is, aimed at continuing the tool's operability and improving stability for the modding community.
