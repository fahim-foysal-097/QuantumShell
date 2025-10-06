# Quantum Shell (qsh)

Quantum Shell (`qsh`) is a simple custom command-line shell.

<img src="qsh_ss.png" alt="screenshot" height=720>

## Built-ins

- cd
- pwd
- alias
- echo
- exit
- help

## Installation

qsh looks for config file `(.qshrc)` in `$HOME` directory.

##### First clone this repo :

```sh
git clone --depth 1 https://github.com/fahim-foysal-097/QuantumShell.git

```

##### Then :

```sh
cd QuantumnShell
```

### In Archlinux

##### Install :

```sh
make clean
make
makepkg -si
```

##### Uninstall :

```sh
sudo pacman -R qsh qsh-debug
```

### In other systems

This installs in `/usr/local/bin`

##### Build :

```sh
make
```

##### Install :

```sh
sudo make install
```

##### Unistall :

```sh
sudo make uninstall
```

### Build yourself

qsh looks for config file `(.qshrc)` in `$HOME` directory.

##### Build :

```sh
make
```

##### Run :

```sh
./qsh
```
