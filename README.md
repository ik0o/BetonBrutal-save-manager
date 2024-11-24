# bb_save

it's a simple save manager for Beton Brutal (and DLC's)
(this program allows you to quickly create and load saves in beton brutal)

!!! If you have any issues or suggestions please post them in issues, especially if the issue is that bb_save
is being complained about by an anti-virus, in which case I will rewrite the input capturing method. !!!

## usage

To simply run bb_save, run the bb_save.exe together with Beton Brutal

### when running without options:

> [!IMPORTANT]
> All functions work only when you open the settings (ESC) in beton brutal <br/>
> (because when you press esc in Beton Brutal your progress is saved in the internal files)

- quicksave : to create a quicksave press F5 and exit settings, your current progress saved.
- quickload : to load quicksave press F9 and exit settings, you load last quicksave.
- save      : to create noraml save press F1 and open console where the bb_save is running and type
              what you want to name save file with current progress.
- load      : to load normal save (or any other save) press F2 and open console where the bb_save is
              running and type the name of save file that you want to load.

For convenience, when you load or save, the list of existing save files for a given map is listed at the top. <br/>
All hotkeys for functions are configurable. <br/>
You can only use English letters, numbers, '-' and '_' to name your save files. <br/>
100_m, 200_m, ... - these are the save files I created with heights 100, 200, ... <br/>

### quicksave history:

when you make a quicksave, a quicksave backup is created just in case.
They're listed in the save list as: 
`quicksave_b<backup number>`
default maximum number of backups is 3, but you can configure it by opening config file and change the value
after max_qsave_history: (new value can be from 0 to 9, where 0 - don't make backup)

### running with options:

-h / --help   : print help text <br/>
-c / --config : configure keys <br/>
-i / --import : import stats .dat file from another person (specify path to file after this option) <br/>

There are two ways to configure hotkeys:
1. run bb_save with the -c or --config option and press the new hotkeys
2. open the configuration file in the bb_save directory and modify it

## Installation

you can install bb_save from Releases

## Build

```sh
& mingw32-make
```
or
```sh
& g++ -o bb_save main.cpp -std=c++14 -Wall -Wformat
```
(one file, so it's not very difficult)
