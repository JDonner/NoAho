from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

# run with:
# python cython-regenerate-noaho-setup.py build_ext --inplace

setup(ext_modules=[
  Extension('noaho',               # name of extension
            ['src/noaho.pyx',      # our Cython source
             'src/array-aho.cpp'],
            language='c++')],      # causes Cython to create C++ source
      cmdclass={'build_ext': build_ext})
