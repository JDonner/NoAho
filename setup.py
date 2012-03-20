from distutils.core import setup
from distutils.extension import Extension

noaho_module = Extension("noaho",
                         sources = ["noaho/noaho.cpp", "noaho/array-aho.cpp"],
                         depends = ["noaho/array-aho.h", "noaho/HashMap.h", "noaho/LinearHashTable.h",
                                    'noaho/noaho.pyx']
                         )

version = "0.9.02"

setup(name = "NoAho",
      version=version,
      author="Jeff Donner",
      author_email="jeffrey.donner@gmail.com",
      maintainer="Jeff Donner",
      maintainer_email="jeffrey.donner@gmail.com",
      url="https://github.com/JDonner/NoAho",
      download_url="http://pypi.python.org/packages/source/N/NoAho/NoAho-%s.tar.gz" % version,
      description="Fast, non-overlapping simultaneous multiple keyword search",
      long_description = open('README.txt').read(),
      ext_modules = [noaho_module],
      packages = ['noaho'],

      classifiers = [
        'Intended Audience :: Developers',
        'Intended Audience :: Information Technology',
        'License :: OSI Approved :: MIT License',
        "Development Status :: 4 - Beta",
        'Programming Language :: C++',
        'Programming Language :: Cython',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.0',
        'Programming Language :: Python :: 3.1',
        'Programming Language :: Python :: 3.2',
        'Operating System :: OS Independent',
        "Environment :: Console",
        'Topic :: Text Processing',
        ]
      )
