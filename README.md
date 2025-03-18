# DoJaject
A program to quickly import a game in DoJa SDK.

## Download
Please, for downloading existing builds, check the releases page. There is already a release available.

## Build
First, clone the repository:

```git clone git@github.com:memory-hunter/DoJaject.git```

You need to have [Boost](https://www.boost.org).

For Windows XP support, place a version of [Boost](https://www.boost.org) that supports Windows XP in `/libs/` folder (you have to create a directory manually under the project root folder), and install v141_xp toolset.

## Usage

Use VS project for Windows XP and upwards.

***NOTE:*** **The `.jar/.jam` both should be in the same directory.**

It also imports `.sp` file, if provided in the same directory.

Initially, you will need to give the program the root directory of iDKDoJa.

## License
[MIT](https://choosealicense.com/licenses/mit/)
