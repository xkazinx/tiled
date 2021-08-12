# Tileship: Strategy Turn-Based Tile-Engine Game

1. Download [Visual Studio Community 2019](https://visualstudio.microsoft.com/es/downloads/)

2. Download [Virtual Assist X](https://www.wholetomato.com/downloads) (recommended)

> Visual Studio extension that provides tools for faster development
- To reset trial:
  - Remove all `*.TMP` files in `C:\Users\[UserName]\AppData\Local\Temp`
  - Remove registry folder at "HKEY_CURRENT_USER\SOFTWARE\Licenses"
  
4. Create folder `C:\LIBPATH`
> All dependencies will be stored here to share between various projects
- Create environment variable for the created folder
  - Press Windows key
  - Search for `edit the system environment variables`
  - Click on `Environment Variables`
  - Click `New`
  - Set name as `LIBPATH` and value as `C:\LIBPATH`
  
5. Download [SFML](https://www.sfml-dev.org/files/SFML-2.5.1-windows-vc15-32-bit.zip)
- Extract in `LIBPATH` folder

6. Download [ENet](http://enet.bespin.org/Downloads.html)
- Extract in `LIBPATH` folder

7. Clone repository

8. Run `symlinks.bat` as Administrator
> This will link `.dll` files from libraries placed at LIBPATH folder into the `deploy` project's directories

9. Open `tileship.sln`

10. Build Solution (Ctrl+Shift+B)

