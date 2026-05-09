from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'NG',
        ['src/NG.cpp'],
        include_dirs=[pybind11.get_include(), pybind11.get_include(user=True)], 
        language='c++',
        extra_compile_args=['-O3', '-Wall', '-fopenmp'],
        extra_link_args=['-fopenmp'],
    ),
]

setup(
    name='game_of_life',
    version='0.1',
    ext_modules=ext_modules,
    install_requires=['pybind11'],
)