## Tools

### Unpacker
**Input** for Sly 1: `WAC` + `WAL` file pair
**Input** for Sly 2/3: `WAL`

**Output**: Files contained within the archive

### Packer
**Input**: Directory with files to archive

**Output** for Sly 1: `WAC` + `WAL` file pair
**Output** for Sly 2/3: `WAL`

### Compressor
Compresses files to the LZSS-derived format that the official game expects for Sly level files.

## Templates
The `templates/` directory contains various [010 Editor](https://www.sweetscape.com/010editor/) templates for game files, in-memory data structures, memory dumps, etc that were developed during research.
This is a dump of everything that I have, some of them might have been superseeded and not useful anymore.

## Instructions
### Extracting assets
1) Download the tools from the Releases panel on the right (or build them)1)
1) If you're extracting PS2 files, move to step 7. For PS3 continue.
1) Download the [psarc]([http://www.mediafire.com/file/aegbgpkm3xfot80/psarc.zip/file](https://cdn.discordapp.com/attachments/439459490702557195/990547521858322462/psarc.exe)) tool
1) Go to your installed game's USRDIR directory (example: `C:/rpcs3/dev_hdd0/game/NPUA80663/USRDIR`).
   We'll now refer to this as `$USRDIR`, replace it where appropriate.
1) Copy `psarc.exe` there (or use the full path to it in the next command)
1) Run this to extract the contents.
   Replace `Sly1` with `Sly2` or `Sly3` if appropriate:
    ```cmd
    psarc extract --input=Sly1.psarc --to=.
    rename Sly1.psarc Sly1.orig.psarc
    ```
1) Run this to unpack the archive:
   For Sly 1:
   ```cmd
   sly_unpacker_1.exe $USRDIR/Sly1/SLY.WAC .
   ```
   For Sly 2 and 3 (change paths as needed):
   ```cmd
   sly_unpacker_2_3.exe $USRDIR/Sly2/SLY2.WAL .
   ```
   After a while you should see an `extracted/` dir in there, with all the games' files.

### Repackaging assets
No detailed instructions yet, see `sly_bake.bat` and modify the paths in it accordingly.

### Modifying / re-compressing levels
No detailed instructions yet, see `sly_bake.bat` and modify the paths in it accordingly.

### Asset List
A Google Sheet with a list of all the official game's assets [is maintained here](https://docs.google.com/spreadsheets/d/1bdhTl2IvXVWOjnjhpgUTH0kg6e-RcioezIYrsi-_mso/edit?usp=sharing).

## Contact
I'm `velocity` on the [Sly Cooper Modding](https://discord.gg/gh5xwfj) Discord server.
