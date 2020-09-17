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
        Extension('jclass', sources=[
            'src/jclass.c',
            'src/membuff.c',
            'src/javaclassobj.c',
            'src/constant_pool.c',
            'src/interfaces.c',
            'src/fields.c',
            'src/methods.c',
            'src/attributes.c',
        ])
    ],

    classifiers=[
    ],

    python_requires=">=3.7",
)

