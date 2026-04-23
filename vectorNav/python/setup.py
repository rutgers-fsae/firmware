# VectorNav SDK (v1.0.0)
# Copyright (c) 2024 VectorNav Technologies, LLC
# 
# WARNING â€“ This software contains Controlled Technical Data, export of which
# is restricted by the Export Administration Regulations ("EAR") (ECCN 7D994). 
# Disclosure to foreign persons contrary to the EAR is prohibited. Violations
# of these export laws and regulations are subject to severe civil and criminal
# penalties.
# 
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from pybind11.setup_helpers import Pybind11Extension
from pathlib import Path
import platform
import subprocess
import sys 
regScan = Path('plugins/PyRegisterScan.cpp')
firUpdt = Path('plugins/PyFirmwareProgrammer.cpp')
logger = Path('plugins/PyLogger.cpp')
dataExp = Path('plugins/PyDataExport.cpp')
calibration = Path('plugins/PyCalibration.cpp')
math = Path('plugins/PyMath.cpp')

# Enable Python-specific features (e.g., GNSS SatInfo and RawMeas via Config.hpp)
macros = [('__PYTHON__', None)]
plugins = []
includes = ['../cpp/include/', '../cpp/plugins', '../cpp/libs', './include', '../cpp/plugins/Math']

if regScan.exists():
    print("Adding Register Scan Plugin")
    macros.append(('__REGSCAN__', None))
    plugins.append(str(regScan))
    plugins.extend([
        '../cpp/plugins/RegisterScan/src/RegisterScan.cpp',
        '../cpp/plugins/RegisterScan/src/ConfigReader.cpp',
        '../cpp/plugins/RegisterScan/src/ConfigWriter.cpp'
    ])
    includes.append('../cpp/plugins/RegisterScan/include')

if firUpdt.exists():
    print("Adding Firmware Programmer Plugin")
    macros.append(('__FIRMWARE_PROGRAMMER__', None))
    plugins.append(str(firUpdt))
    plugins.extend([
        '../cpp/plugins/FirmwareProgrammer/src/Bootloader.cpp',
        '../cpp/plugins/FirmwareProgrammer/src/FirmwareUpdater.cpp',   
        '../cpp/plugins/FirmwareProgrammer/src/VnXml.cpp',   
    ])
    includes.append('../cpp/plugins/FirmwareProgrammer/include')

if logger.exists():
    print("Adding Logger Plugin")
    macros.append(('__LOGGER__', None))
    plugins.append(str(logger))
    includes.append('../cpp/plugins/Logger')

if dataExp.exists():
    print("Adding Data Export Plugin")
    macros.append(('__DATAEXPORT__', None))
    plugins.append(str(dataExp))
    plugins.extend([
        '../cpp/plugins/DataExport/src/ExporterCsvUtils.cpp',   
    ])
    includes.append('../cpp/plugins/DataExport/include')

# if calibration.exists():
#     print("Adding Calibration Plugin")
#     macros.append(('__CALIBRATION__', None))
#     plugins.append(str(calibration))
#     plugins.extend([
#         '../cpp/plugins/Calibration/src/DynamicAccelBias.cpp',
#         '../cpp/plugins/Calibration/src/DynamicAccelBiasAuto.cpp',
#         '../cpp/plugins/Calibration/src/Hsi.cpp',
#         '../cpp/plugins/Calibration/src/HsiDipole.cpp',
#         '../cpp/plugins/Calibration/src/HsiPatch.cpp',
#         '../cpp/plugins/Calibration/src/StaticRfrAndAccelBias.cpp',
#     ])
#     includes.append('../cpp/plugins/Calibration/include')

if math.exists():
    print("Adding Math Plugin")
    macros.append(('__MATH__', None))
    plugins.append(str(math))

ext_libs = []
if platform.system() == 'Windows':
    ext_libs.append('winmm')

ext_modules = [
    Pybind11Extension(
        "vectornav",
        [
            # Pybind Files
            'src/vectornav.cpp',
            'src/PySensor.cpp',
            'src/PyRegisters.cpp',
            'src/PyCompositeData.cpp',
            'src/PyCommands.cpp',
            'src/PyVnTypes.cpp',
            'src/PyErrors.cpp',
            "src/PyPackets.cpp",
            "src/PyByteBuffer.cpp",
            "src/PyUtils.cpp",

            # Implemenation
            '../cpp/src/Implementation/AsciiPacketDispatcher.cpp',
            '../cpp/src/Implementation/AsciiPacketProtocol.cpp',
            '../cpp/src/Implementation/BinaryHeader.cpp',
            '../cpp/src/Implementation/CommandProcessor.cpp',
            '../cpp/src/Implementation/FaPacketDispatcher.cpp',
            '../cpp/src/Implementation/FaPacketProtocol.cpp',
            '../cpp/src/Implementation/FbPacketDispatcher.cpp',
            '../cpp/src/Implementation/FbPacketProtocol.cpp',
            '../cpp/src/Implementation/PacketSynchronizer.cpp',

            # Interface
            '../cpp/src/Interface/GenericCommand.cpp',
            '../cpp/src/Interface/Sensor.cpp',
            '../cpp/src/Interface/Registers.cpp',

            # plugins
            *plugins,
        ],
        include_dirs=includes,
        language="c++",
        cxx_std=17,
        define_macros=macros,
        libraries=ext_libs
    ),
]


class BuildExtWithStubs(build_ext):
    """Custom build_ext command that generates stub files after building the extension."""

    def run(self):
        # Build the extension first
        super().run()

        # Get the output directory where the extension was built
        if self.inplace:
            # Development install (pip install -e .)
            output_dir = Path.cwd()
        else:
            # Regular install - get the build lib directory
            output_dir = Path(self.build_lib)

        self.announce(f"Generating stub files with pybind11-stubgen to {output_dir}...", level=3)

        # Set up environment to find the built module
        import os
        env = os.environ.copy()
        pythonpath = str(output_dir)
        if 'PYTHONPATH' in env:
            pythonpath = pythonpath + os.pathsep + env['PYTHONPATH']
        env['PYTHONPATH'] = pythonpath

        try:
            # Run pybind11-stubgen with the build directory in PYTHONPATH
            subprocess.check_call([
                sys.executable, "-m", "pybind11_stubgen",
                "vectornav",
                "-o", str(output_dir),
                "--ignore-all-errors"
            ], env=env)

            self.announce("Stub files generated successfully", level=3)
        except subprocess.CalledProcessError as e:
            self.announce(f"Failed to generate stubs: {e}", level=2)
            raise
        except Exception as e:
            self.announce(f"Unexpected error generating stubs: {e}", level=2)
            raise


setup(
    name='vectornav',
    version='1.0.0',
    ext_modules=ext_modules,
    cmdclass={'build_ext': BuildExtWithStubs},
)
