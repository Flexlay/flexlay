{
  description = "SuperTux level editor (C++/Qt6 default; Python path retained)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        pythonPackages = pkgs.python3Packages;
      in rec {
        packages = rec {

          PyQt5-stubs = pythonPackages.buildPythonPackage rec {
            pname = "PyQt5-stubs";
            version = "5.15.6.0";
            src = pythonPackages.fetchPypi {
              inherit pname version;
              sha256 = "sha256-kScKwj6/OKHcBM2XqoUs0Ir4Lcg5EA5Tla8UR+Pplwc=";
            };
            pyproject = true;
            build-system = [ pythonPackages.setuptools ];
          };

          # Python/PyQt5 editor (legacy path — kept for transition)
          flexlay-python = pythonPackages.buildPythonPackage rec {
            pname = "flexlay-python";
            version = "0.2.0";
            src = ./.;
            pyproject = true;
            build-system = [ pythonPackages.setuptools ];
            nativeBuildInputs = [
              pkgs.qt5.wrapQtAppsHook
            ];

            nativeCheckInputs = [
              pythonPackages.flake8  # FIXME: not sure why this is needed here
            ];

            makeWrapperArgs = [
              "\${qtWrapperArgs[@]}"
            ];
            preCheck = ''
              export QT_QPA_PLATFORM_PLUGIN_PATH="${pkgs.qt5.qtbase.bin}/lib/qt-${pkgs.qt5.qtbase.version}/plugins";
              # export QT_QPA_PLATFORM=offscreen
            '';
            checkPhase = ''
              runHook preCheck
              flake8 flexlay supertux tests
              # pyright flexlay supertux tests
              # mypy flexlay supertux tests
              # pylint flexlay supertux tests
              # HOME=$TMP python3 -m unittest discover -v -s tests/
              runHook postCheck
            '';
            propagatedBuildInputs = [
              pkgs.xorg.libxcb
              pythonPackages.setuptools
              pythonPackages.numpy
              pythonPackages.pyqt5
              pythonPackages.pyqt5-sip
              pythonPackages.pyxdg
            ];
            checkInputs = (with pkgs; [
              pyright
            ]) ++ (with pythonPackages; [
              flake8
              mypy
              pylint
              types-setuptools
              pyannotate
            ]) ++ [
              PyQt5-stubs
            ];
          };

          # Prefer overridePythonAttrs: .override is not always present on
          # buildPythonPackage results (nixpkgs changes / pure drv path).
          flexlay-python-nocheck = flexlay-python.overridePythonAttrs (_old: {
            doCheck = false;
          });

          # Back-compat alias for scripts that still expect packages.flexlay
          # to be the Python package. Prefer packages.default / flexlay-cpp.
          flexlay = flexlay-python;

          /**
           * C++ / Qt6 port of the editor core + simpleed demo.
           * Build: nix build .#flexlay-cpp
           * Run:   nix run .#flexlay-simpleed
           */
          flexlay-cpp = pkgs.stdenv.mkDerivation {
            pname = "flexlay-cpp";
            version = "0.2.0";
            src = ./.;

            nativeBuildInputs = [
              pkgs.cmake
              pkgs.pkg-config
              pkgs.qt6.wrapQtAppsHook
            ];

            buildInputs = [
              pkgs.qt6.qtbase
              pkgs.libGL
              pkgs.libglvnd
            ];

            cmakeFlags = [
              "-S"
              "cpp"
              "-DFLEXLAY_BUILD_SIMPLEED=ON"
              "-DFLEXLAY_BUILD_TESTS=ON"
            ];

            # cmake configure is run from build dir with source root; point at cpp/
            configurePhase = ''
              runHook preConfigure
              cmake -S "$src/cpp" -B build \
                -DCMAKE_BUILD_TYPE=Release \
                -DCMAKE_INSTALL_PREFIX="$out" \
                -DFLEXLAY_BUILD_SIMPLEED=ON \
                -DFLEXLAY_BUILD_TESTS=ON
              runHook postConfigure
            '';

            buildPhase = ''
              runHook preBuild
              cmake --build build -j"$NIX_BUILD_CORES"
              runHook postBuild
            '';

            doCheck = true;
            checkPhase = ''
              runHook preCheck
              ctest --test-dir build --output-on-failure
              runHook postCheck
            '';

            installPhase = ''
              runHook preInstall
              cmake --install build
              # Primary SuperTux editor name (same binary as flexlay-simpleed)
              ln -s flexlay-simpleed "$out/bin/supertux-editor"
              runHook postInstall
            '';

            meta = with pkgs.lib; {
              description = "Flexlay C++/Qt6 SuperTux level editor";
              license = licenses.gpl3Plus;
              platforms = platforms.linux;
              mainProgram = "supertux-editor";
            };
          };

          # Aliases
          flexlay-simpleed = flexlay-cpp;
          # C++ is the default package path
          default = flexlay-cpp;
        };

        apps = rec {
          /**
           * Default: C++ SuperTux editor (`supertux-editor` / `flexlay-simpleed`).
           *   nix run
           *   nix run .#supertux-editor
           */
          default = {
            type = "app";
            program = "${packages.flexlay-cpp}/bin/supertux-editor";
          };

          supertux-editor = default;

          flexlay-simpleed = {
            type = "app";
            program = "${packages.flexlay-cpp}/bin/flexlay-simpleed";
          };

          /**
           * Legacy Python/PyQt5 SuperTux editor.
           *   nix run .#supertux-editor-python
           */
          supertux-editor-python = {
            type = "app";
            program = "${packages.flexlay-python}/bin/supertux-editor";
          };
        };

        devShells = rec {
          flexlay-dev = pkgs.mkShell {
            inputsFrom = [ packages.flexlay-python ];
            shellHook = packages.flexlay-python.preCheck + ''
              # runHook setuptoolsShellHook
            '';
          };

          # C++ port development shell
          flexlay-cpp-dev = pkgs.mkShell {
            packages = [
              pkgs.cmake
              pkgs.pkg-config
              pkgs.gcc
              pkgs.gdb
              pkgs.qt6.qtbase
              pkgs.qt6.qttools
              pkgs.libGL
              pkgs.libglvnd
            ];
            inputsFrom = [ packages.flexlay-cpp ];
            shellHook = ''
              echo "flexlay C++ dev shell — build with:"
              echo "  cmake -S cpp -B build -DCMAKE_BUILD_TYPE=Debug"
              echo "  cmake --build build && ctest --test-dir build"
            '';
          };

          # C++ port is the default development environment
          default = flexlay-cpp-dev;
        };
      }
    );
}
