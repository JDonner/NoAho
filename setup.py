from distutils.core import setup
from distutils.extension import Extension

noaho_module = Extension(
    'noaho',
    sources=[
      # Cython generated
      'src/noaho.cpp',
      # original
      'src/array-aho.cpp'],
    depends=[
        'src/array-aho.h',
        'src/HashMap.h',
        'src/LinearHashTable.h',
        'src/noaho.pyx']
)

version = '0.9.5'

setup(
    name='NoAho',
    version=version,
    author='Jeff Donner',
    author_email='jeffrey.donner@gmail.com',
    maintainer='Jeff Donner',
    maintainer_email='jeffrey.donner@gmail.com',
    url='https://github.com/JDonner/NoAho',
    download_url='http://pypi.python.org/packages/source/N/NoAho/NoAho-%s.tar.gz' % version,
    description='Fast, non-overlapping simultaneous multiple keyword search',
    long_description=open('README.txt').read(),
    ext_modules=[noaho_module],
    classifiers=[
        'Intended Audience :: Developers',
        'Intended Audience :: Information Technology',
        'License :: OSI Approved :: MIT License',
        'Development Status :: 4 - Beta',
        'Programming Language :: C++',
        'Programming Language :: Cython',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.0',
        'Programming Language :: Python :: 3.1',
        'Programming Language :: Python :: 3.2',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Operating System :: OS Independent',
        'Environment :: Console',
        'Topic :: Text Processing',
    ]
)
