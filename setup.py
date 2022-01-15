from setuptools import setup, Extension


setup(
    name='jclass',
    version='0.0.0',
    author='Andrew Palmer',
    author_email='atp@sdf.org',
    description='',
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/atpalmer/python-jclass',

    ext_modules=[
        Extension('jclass',
            extra_compile_args=['-Wall', '-Wextra', '-pedantic'],
            include_dirs=['libjclass/include'],
            sources=[
                'src/jclass.c',
                'src/javaclassobj.c',
                'src/conv.c',
                'src/error.c',
                'libjclass/src/error.c',
                'libjclass/src/javaclass.c',
                'libjclass/src/stream.c',
                'libjclass/src/constant_pool.c',
                'libjclass/src/interfaces.c',
                'libjclass/src/fields.c',
                'libjclass/src/methods.c',
                'libjclass/src/attributes.c',
            ],
        )
    ],

    classifiers=[
    ],

    python_requires=">=3.7",
)

