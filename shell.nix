{ pkgs ? import <nixpkgs> { }
, cross_64 ? import <nixpkgs> {
    crossSystem = {
      config = "x86_64-elf";
    };
  }
}:

with pkgs;
mkShell {
  buildInputs = [
    cross_64.buildPackages.gcc
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
    limine
  ];
}

