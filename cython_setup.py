from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

setup(ext_modules=[Extension(
            "noaho",                        # name of extension
            ["noaho.pyx", "array-aho.cpp"], # our Cython source
            language="c++")],               # causes Cython to create C++ source
      cmdclass={'build_ext': build_ext})
