from distutils.core import setup
from distutils.extension import Extension

noaho_module = Extension("noaho",
                         sources = ["noaho.cpp", "array-aho.cpp"],
                         depends = ["array-aho.h", "HashMap.h", "LinearHashTable.h"]
                         )

version = 0.9

setup(name = "Not-Quite Aho-Corasick",
      author="Jeff Donner",
      author_email="jeffrey.donner@gmail.com",
      maintainer="Jeff Donner",
      maintainer_email="jeffrey.donner@gmail.com",
      url="http://pypi.python.org/pypi/noaho",
      description="Non-overlapping simultaneous multi-key search",
      long_description = open('README.txt').read(),
      ext_modules = [noaho_module]
      )
