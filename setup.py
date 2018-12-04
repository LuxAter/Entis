"""A setuptools based setup module.
See:
https://packaging.python.org/en/latest/distributing.html
https://github.com/pypa/sampleproject
"""

from setuptools import setup, find_packages, Extension
from codecs import open
from os import path
import os


def package_files(directory):
    paths = []
    for (path, directories, filenames) in os.walk(directory):
        for filename in filenames:
            paths.append(os.path.join('..', path, filename))
    return paths


extra_files = []

here = path.abspath(path.dirname(__file__))

with open(path.join(here, 'README.rst'), encoding='utf-8') as f:
    long_description = f.read()

entismodule = Extension('entis', sources=['pyentis/entis.c'])

setup(
    name='pyentis',
    version='0.0.0',
    description='Python based graphics engine',
    long_description=long_description,
    license='MIT',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
    ],
    keywords='setuptools development',
    packages=find_packages(exclude=['docs', 'tests']),
    package_data={'': extra_files},
    ext_modules=[entismodule],
    entry_points={
        'console_scripts': ['pyentis= pyentis.pyentis:main']
    },)
