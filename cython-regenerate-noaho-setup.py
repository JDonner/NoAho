from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

# run with:
# python cython_deep_build_setup.py build_ext --inplace

setup(ext_modules=[Extension(
            "noaho",                        # name of extension
            ["noaho.pyx", "array-aho.cpp"], # our Cython source
            language="c++")],               # causes Cython to create C++ source
      cmdclass={'build_ext': build_ext})
