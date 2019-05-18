from distutils.core import setup, Extension

entis_module = Extension('pyentis', sources=['c/entis.c', 'c/color.c','c/event.c','c/key_codes.c','c/binding.c'], include_dirs=['./c/'], libraries=['xcb','m'])

setup(name="pyentis",
        ext_modules=[entis_module])
