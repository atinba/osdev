{ pkgs ? import <nixpkgs> { }
, cross_i686 ? import <nixpkgs> {
    crossSystem = {
      config = "i686-elf";
    };
  }
}:

with pkgs;
mkShell {
  buildInputs = [
    cross_i686.buildPackages.gcc
    #cross_i686.buildPackages.clang
    gdb
    gnumake
    valgrind
    grub2
    xorriso
    qemu
    ripgrep
    nasm
    clang
    cppcheck
    splint
    cloc
    nasmfmt
    bear
  ];
}

