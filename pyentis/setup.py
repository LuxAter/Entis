from distutils.core import setup, Extension

entis_module = Extension('entis', sources=['entis.c'], libraries=['xcb','m'])

setup(name="pyentis",
        ext_modules=[entis_module])
