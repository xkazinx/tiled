# Tileship: Strategy Turn-Based Tile-Engine Game

1. Download [Visual Studio Community 2019](https://visualstudio.microsoft.com/es/downloads/)
  
2. Create folder `C:\LIBPATH`
> All dependencies will be stored here to share between various projects
- Create environment variable for the created folder
  - Press Windows key
  - Search for `edit the system environment variables`
  - Click on `Environment Variables`
  - Click `New`
  - Set name as `LIBPATH` and value as `C:\LIBPATH`
  
3. Download [SFML](https://www.sfml-dev.org/files/SFML-2.5.1-windows-vc15-32-bit.zip)
- Extract in `LIBPATH` folder

4. Download and Compile [ENet](http://enet.bespin.org/Downloads.html)
- Extract in `LIBPATH` folder
- Run project `enet.dsp`
- In `Solution Explorer`, right click the `enet` project, press on `Properties` and in `C/C++`->`General` change `Debug Information Format` from `Program Database for Edit and Continue (/ZI)` to `Program Database (/Zi)`
- Build Solution (Ctrl+Shift+B) both in `Debug` and `Release` (the dropdown to toggle between them can be found at the top)
> This will generate `enet.lib` to use it both in `Debug` and `Release` projects

5. Download and setup latest `MySQL Server` from [MySQL Community Installer](https://dev.mysql.com/downloads/windows/installer/8.0.html)

6. Download [MySQL C Connector 6.1 - Windows (x86, 32-bit), MSI Installer](https://downloads.mysql.com/archives/c-c/)
> Should be automatically be installed in %ProgramFiles(x86)%\MySQL\
> This provides the `include` files and `.dll` files for the `server` project

7. Clone `tileship` repository

8. Run `symlinks.bat` as Administrator
> This will link `.dll` files from libraries placed at LIBPATH folder into the `deploy` project's directories

9. Open `src\tileship.sln`
- Build Solution (Ctrl+Shift+B)

10. Setup Debugging and opening with Ctrl+F5
> Client
- In `Solution Explorer`, right click the `client` project, press on `Properties`, and in `Debugging` set:
  - Make sure to first set `Configuration` at top as `All configurations` so that it applies both for Debug and Release
- Command: `$(SolutionDir)..\deploy\client\$(TargetName)$(TargetExt)`
- Working Directory: `$(SolutionDir)..\deploy\client\`
> Server
- In `Solution Explorer`, right click the `server` project, press on `Properties`, and in `Debugging` set:
  - Make sure to first set `Configuration` at top as `All configurations` so that it applies both for Debug and Release
- Command: `$(SolutionDir)..\deploy\server\$(TargetName)$(TargetExt)`
- Working Directory: `$(SolutionDir)..\deploy\server\`

##Workflow

- Use a MySQL Manager like [HeidiSQL](https://www.heidisql.com/)

- Use [Virtual Assist X](https://www.wholetomato.com/downloads) (recommended)
  > Visual Studio extension that provides tools for faster code browsing and development
  - To reset trial every 30 days:
    - Remove all `*.TMP` files in `C:\Users\[UserName]\AppData\Local\Temp`
    - Remove registry folder at "HKEY_CURRENT_USER\SOFTWARE\Licenses"
  
- Use Visual Studio's `Live Share` (download from Visual Studio 2019 itself)

- Use [Google Drive](https://www.google.com/intl/es/drive/download/) to sync the compiled `client.exe` and `server.exe`
  > So when using `Live Share` and host's computer compiles the projects, it could be set with `symlinks` so that the compiled files are shared with the host and the participants

- Use [AnyDesk](https://anydesk.com/es) in case we both need remote access
  > Like, to see something in the same executable, code synchronously, or debug at the same time
