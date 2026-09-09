{
  description = "C development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            gcc           
            gnumake
            gcc-arm-embedded         
            cmake
            qemu          
            
            bear    
            clang-tools     
            
            gdb           
            valgrind    
          ];

          shellHook = ''
            echo "Hi! You're in your C environment."
            echo "Compiler: $(gcc --version | head -n 1)"
          '';
        };
      });
}
