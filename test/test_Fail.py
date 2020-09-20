import pytest
import jclass


def test_jclass():
    jcls = jclass.load('java/HelloWorld/HelloWorld.class')
    assert isinstance(jcls, jclass.JavaClass)


def test_jclass_badfile():
    with pytest.raises(ValueError):
        jcls = jclass.load('java/Fail/DeadBeef.class')


def test_jclass_corrupt_pool():
    with pytest.raises(ValueError):
        jcls = jclass.load('java/Fail/CorruptPool.class')


def test_jclass_corrupt_length():
    with pytest.raises(ValueError):
        jcls = jclass.load('java/Fail/CorruptLength.class')


def test_jclass_bad_path():
    with pytest.raises(OSError):
        jcls = jclass.load('java/Fail/bad-path')


def test_jclass_BadVersion():
    with pytest.raises(ValueError):
        jcls = jclass.load('java/Fail/BadVersion.class')

