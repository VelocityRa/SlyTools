## Tools

### Unpacker
Input: `WAC` + `WAL` file pair
Output: Files contained within

### Packer
Input: Directory with files to pack
Output: `WAC` + `WAL` file pair

### Compressor
Compresses files to the LZSS-derived format that the official game expects for Sly level files.

## Templates
The `templates/` directory contains various [010 Editor](https://www.sweetscape.com/010editor/) templates for game files, in-memory data structures, memory dumps, etc that were developed during reserach.
This is a dump of everything that I have, some of them might have been superseeded and not useful anymore.

## Instructions (Sly 1 PS3)
### Extracting assets
1) Download the tools from the Releases panel on the right (or build them)
1) Download the [psarc](http://www.mediafire.com/file/aegbgpkm3xfot80/psarc.zip/file) tool
1) Go to your installed game's USRDIR directory (example: `rpcs3\dev_hdd0\game\NPUA80663\USRDIR`).
   We'll now refer to this as `$USRDIR`.
1) Copy `psarc.exe` there (or use the full path to it in the next command)
1) Run this to extract the Sly1 contents:
    ```cmd
    psarc extract --input=Sly1.psarc --to=.
    rename Sly1.psarc Sly1.orig.psarc
    ```
1) Run this to unpack the archive:
   ```cmd
   sly_unpacker.exe $USRDIR/Sly1/SLY.WAC
   ```
   After a while you should see an `extracted/` dir in there, with all the games' files.

### Repackaging assets
No detailed instructions yet, see `sly_bake.bat` and modify the paths in it accordingly.

### Modifying / re-compressing levels
No detailed instructions yet, see `sly_bake.bat` and modify the paths in it accordingly.

### Asset List
A Google Sheet with all the official game's assets [is maintaned here](https://docs.google.com/spreadsheets/d/1bdhTl2IvXVWOjnjhpgUTH0kg6e-RcioezIYrsi-_mso/edit?usp=sharing)

## Contact
I'm `velocity` on the [Sly Cooper Modding](https://discord.gg/gh5xwfj) Discord server.
