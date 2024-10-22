<div align="center">
    <a href="https://dlpnd.github.io/nvr-wiki/"><img src="https://i.imgur.com/SUr8ORH.png" width="1024" alt="EOP" /></a>
</div>

<div align="center">

## What is New Vegas Reloaded?

New Vegas Reloaded is a custom graphical extender for Obsidian's "Fallout: New Vegas". It overrides the rendering pipeline to inject various effects that can be completely configured.

Started originally as part of the [TESReloaded](https://github.com/llde/TESReloaded10) project, it has now branched off into it's own project.
</div>

## Features
For a full list of features and screenshots, please consult the [wiki](https://dlpnd.github.io/nvr-wiki/)

## Contributions
This project wouldn't be possible without all the amazing contributions from the Fallout New Vegas and Bethesda modding community. If you want to contribute, please reach out on the [Discord](https://discord.com/invite/QgN6mR6eTK)!  Thank you to all of our contributors so far! 
<div align=center>
  <a href="https://github.com/pr0bability/TESReloaded10/graphs/contributors">
    <img src="https://contrib.rocks/image?repo=pr0bability/TESReloaded10" />
  </a>
  </p>
</div>

----
## Installation

[Check the Wiki page for the most up-do-date instructions on how to install](https://dlpnd.github.io/nvr-wiki/docs/Installation)

----
## Configuration

To activate/Deactivate each effect, press <kbd>O</kbd> in game to bring up the menu. Navigate the menu with the arrow keys, and activate/deactivate effects and increase/decrease effect values with the numpad <kbd>+</kbd>/<kbd>-</kbd> keys.

The configuration is stored in `NewVegasReloaded/nvse/Plugins/NewVegasReloaded.dll.toml` and can be edited directly with a text editor.

----
## Building from source
To build, with Visual Studio 2019 (Community Edition) with Desktop Development libraries, Windows SDK and DirectX9 SDK installed, run the `build.bat` file in command line, with the following command:

```bat
build.bat "ProjectName" "C:/DeployFolder/"
```

 * `"Project Name"` can be either `OblivionReloaded` or `NewVegasReloaded`. 
 * The second argument is optional, and can be used to copy the built library to the game folder or (preferably) to a mod folder managed by [Mod Organizer 2](https://vivanewvegas.github.io/mo2.html). Otherwise, the built files can be found in the `/build/` folder at the root, to be copied manually.

----
## Troubleshooting

If the mod loaded properly, a message will appear at the bottom of the title screen with the version number. If not, try reinstalling directX or the directX SDK.

### TESReloaded is not compatible with the following mods:
* **ENB**

If you want extra control over the look of your game, use Reshade which is compatible.

* **Improved Lighting Shaders**

Problem: Big areas covered in broken shaders results in bright blue surfaces. 

Solution: Remove the TESReloaded Shaders with "SLS" in the name, as they are the source of the conflict. Only post process shadows will be working.

* **Enhanced Camera**

Problem: Crash when loading a game.

Solution: Disable "First Person Shadow" setting in Enhanced Camera .ini. 

----
## License
Check [License.md](License.md) for the licensing terms.